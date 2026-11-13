#include "PluginProcessor.h"
#include "PluginEditor.h"

//                         name             tag                                  drv   bias shp  low  mid  high midHz  trans hard
const Weapon kWeapons[kNumWeapons] = {
    { "DESERT EAGLE", "Heavy clipping  -  thick, punchy",            1.30f, 0.00f, 1,  3.f,  0.f,  1.f, 1500.f, 0.00f, 0.80f },
    { "INTERVENTION", "Explosive transients  -  gritty top end",     1.10f, 0.00f, 3, -2.f,  1.f,  5.f, 3500.f, 0.90f, 0.50f },
    { "AK-47",        "Raw crunch  -  strong mids",                  1.00f, 0.10f, 2,  1.f,  5.f,  0.f, 1200.f, 0.20f, 0.40f },
    { "GLOCK",        "Tight, controlled drive",                     0.80f, 0.00f, 4,  0.f,  1.f, -2.f, 2000.f, 0.30f, 0.30f },
    { "SHOTGUN",      "Wide, heavy, aggressive",                     1.50f, 0.15f, 2,  4.f, -1.f,  2.f,  800.f, 0.10f, 0.90f },
    { "REVOLVER",     "Warm, vintage saturation",                    0.70f, 0.20f, 2,  2.f,  2.f, -3.f,  700.f, 0.00f, 0.10f },
    { "MP5",          "Smooth, consistent drive",                    0.85f, 0.00f, 0,  0.f,  1.f,  0.f, 1800.f, 0.10f, 0.20f },
    { "SNIPER",       "Clean, razor-sharp transients",               0.50f, 0.00f, 4, -1.f,  0.f,  4.f, 4500.f, 1.00f, 0.10f },
};

static float shapeFn (int s, float x, float bias)
{
    switch (s)
    {
        case 0:  return std::tanh (x);
        case 1:  return juce::jlimit (-1.f, 1.f, x);
        case 2:  return std::tanh (x + bias * 3.f) - std::tanh (bias * 3.f);
        case 3:  return std::sin (juce::MathConstants<float>::halfPi * juce::jlimit (-3.f, 3.f, x));
        default: return std::atan (x) * (2.f / juce::MathConstants<float>::pi);
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout GunshotAudioProcessor::createLayout()
{
    using namespace juce;
    AudioProcessorValueTreeState::ParameterLayout l;
    StringArray names;
    for (auto& w : kWeapons) names.add (w.name);

    auto f = [&] (const char* id, const char* n, float lo, float hi, float def)
    { l.add (std::make_unique<AudioParameterFloat> (ParameterID { id, 1 }, n, NormalisableRange<float> (lo, hi), def)); };

    f ("drive", "Drive", 0.f, 1.f, 0.5f);
    f ("mix",   "Mix",   0.f, 1.f, 1.f);
    f ("in",    "Input",  -24.f, 24.f, 0.f);
    f ("out",   "Output", -24.f, 24.f, 0.f);
    f ("sens",  "Bit Sensitivity", 0.f, 1.f, 0.5f);
    f ("depth", "Bit Depth",       0.f, 1.f, 0.7f);
    f ("speed", "Bit Speed",       0.f, 1.f, 0.5f);
    l.add (std::make_unique<AudioParameterChoice> (ParameterID { "mode", 1 },   "Mode",   StringArray { "HARD", "NORMAL", "BIT" }, 0));
    l.add (std::make_unique<AudioParameterChoice> (ParameterID { "weapon", 1 }, "Weapon", names, 0));
    return l;
}

GunshotAudioProcessor::GunshotAudioProcessor()
    : AudioProcessor (BusesProperties().withInput ("Input", juce::AudioChannelSet::stereo(), true)
                                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "STATE", createLayout()),
      oversampling (2, 1, juce::dsp::Oversampling<float>::filterHalfBandFIREquiripple, true, true)
{
    pDrive = apvts.getRawParameterValue ("drive");  pMix = apvts.getRawParameterValue ("mix");
    pIn = apvts.getRawParameterValue ("in");        pOut = apvts.getRawParameterValue ("out");
    pMode = apvts.getRawParameterValue ("mode");    pWeapon = apvts.getRawParameterValue ("weapon");
    pSens = apvts.getRawParameterValue ("sens");    pDepth = apvts.getRawParameterValue ("depth");
    pSpeed = apvts.getRawParameterValue ("speed");
}

bool GunshotAudioProcessor::isBusesLayoutSupported (const BusesLayout& l) const
{
    return l.getMainInputChannelSet() == juce::AudioChannelSet::stereo()
        && l.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void GunshotAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    sr = sampleRate;
    maxBlock = juce::jmax (1, samplesPerBlock);
    juce::dsp::ProcessSpec spec { sampleRate, (juce::uint32) maxBlock, 2 };

    oversampling.initProcessing ((size_t) maxBlock);
    oversampling.reset();
    const int latency = (int) std::round (oversampling.getLatencyInSamples());
    setLatencySamples (latency);

    dryDelay.prepare (spec);
    dryDelay.setDelay ((float) latency);

    dryBuf.setSize (2, maxBlock);
    preBuf.assign ((size_t) maxBlock, 1.f);
    makeBuf.assign ((size_t) maxBlock, 1.f);
    blendBuf.assign ((size_t) maxBlock, 1.f);

    driveSmooth.reset (sampleRate, 0.02);
    driveSmooth.setCurrentAndTargetValue (*pDrive);

    for (int c = 0; c < 2; ++c) { lowF[c].reset(); midF[c].reset(); highF[c].reset(); dcX[c] = dcY[c] = tLp[c] = 0.f; }
    envFast = envSlow = modS = 0.f;
    prevIn = juce::Decibels::decibelsToGain ((float) *pIn);
    prevOut = juce::Decibels::decibelsToGain ((float) *pOut);
    prevMix = *pMix;
    lastMode = lastWeapon = -1;
}

void GunshotAudioProcessor::updateTone (int mode, int wi)
{
    const auto& w = kWeapons[wi];
    using C = juce::dsp::IIR::Coefficients<float>;
    auto lo = C::makeLowShelf  (sr, 120.f, 0.7f, juce::Decibels::decibelsToGain (w.lowDb + (mode == 0 ? 2.f : 0.f)));
    auto mi = C::makePeakFilter (sr, w.midHz, 0.8f, juce::Decibels::decibelsToGain (w.midDb));
    auto hi = C::makeHighShelf (sr, juce::jmin (6000.0, sr * 0.45), 0.7f, juce::Decibels::decibelsToGain (w.highDb));
    for (int c = 0; c < 2; ++c) { lowF[c].coefficients = lo; midF[c].coefficients = mi; highF[c].coefficients = hi; }
    lastMode = mode; lastWeapon = wi;
}

void GunshotAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals nd;
    const int total = buffer.getNumSamples();
    const int nch = buffer.getNumChannels();
    if (nch < 2) return;
    for (int c = 2; c < nch; ++c) buffer.clear (c, 0, total);

    for (int start = 0; start < total; start += maxBlock)
    {
        const int len = juce::jmin (maxBlock, total - start);
        juce::AudioBuffer<float> sub (buffer.getArrayOfWritePointers(), 2, start, len);
        processChunk (sub);
    }
}

void GunshotAudioProcessor::processChunk (juce::AudioBuffer<float>& buf)
{
    const int n = buf.getNumSamples();
    const int mode = (int) *pMode;
    const int wi = juce::jlimit (0, kNumWeapons - 1, (int) *pWeapon);
    const auto& w = kWeapons[wi];
    if (mode != lastMode || wi != lastWeapon) updateTone (mode, wi);

    const float inG = juce::Decibels::decibelsToGain ((float) *pIn);
    const float outG = juce::Decibels::decibelsToGain ((float) *pOut);
    const float mixT = *pMix;
    const float sens = *pSens, depth = *pDepth, speed = *pSpeed;

    // input gain + input meter
    buf.applyGainRamp (0, n, prevIn, inG);
    prevIn = inG;
    float ip = 0.f;
    for (int c = 0; c < 2; ++c) ip = juce::jmax (ip, buf.getMagnitude (c, 0, n));
    if (ip > inPeak.load()) inPeak.store (ip);

    // dry path, delayed to match oversampling latency
    for (int c = 0; c < 2; ++c)
    {
        const float* x = buf.getReadPointer (c);
        float* d = dryBuf.getWritePointer (c);
        for (int i = 0; i < n; ++i) { dryDelay.pushSample (c, x[i]); d[i] = dryDelay.popSample (c); }
    }

    // BIT-mode analysis coefficients
    auto coef = [this] (float ms) { return std::exp (-1.f / (0.001f * ms * (float) sr)); };
    const float atkFast = coef (0.5f), relFast = coef (20.f);
    const float atkSlow = coef (30.f), relSlow = coef (juce::jmap (speed, 0.f, 1.f, 400.f, 60.f));
    const float modAtk = coef (5.f + (1.f - speed) * 60.f), modRel = coef (juce::jmap (speed, 0.f, 1.f, 350.f, 40.f));

    driveSmooth.setTargetValue (*pDrive);
    const float* L = buf.getReadPointer (0);
    const float* R = buf.getReadPointer (1);

    for (int i = 0; i < n; ++i)
    {
        float d = driveSmooth.getNextValue();
        if (mode == 2)
        {
            const float a = juce::jmax (std::abs (L[i]), std::abs (R[i]));
            envFast = a + (a > envFast ? atkFast : relFast) * (envFast - a);
            envSlow = a + (a > envSlow ? atkSlow : relSlow) * (envSlow - a);
            const float energy = juce::jlimit (0.f, 1.f, envSlow * (1.f + sens * 6.f));
            const float trans  = juce::jlimit (0.f, 1.f, (envFast - envSlow) * (2.f + sens * 10.f));
            const float target = juce::jlimit (0.f, 1.f, 0.7f * energy + 0.6f * trans);
            modS = target + (target > modS ? modAtk : modRel) * (modS - target);
            const float dyn = 0.2f + 1.6f * modS;
            d *= (1.f - depth) + depth * dyn;
        }
        float pre, blend = 1.f;
        if (mode == 0)      pre = 1.f + 45.f * d * std::sqrt (juce::jmax (0.f, d));
        else if (mode == 1) { pre = 1.f + 6.f * d; blend = juce::jlimit (0.f, 1.f, 0.45f + 0.55f * d); }
        else                pre = 1.f + 30.f * d;
        pre *= w.driveScale;
        preBuf[(size_t) i] = pre;
        makeBuf[(size_t) i] = std::pow (pre, mode == 1 ? -0.2f : -0.3f);
        blendBuf[(size_t) i] = blend;
    }
    bitActivity.store (mode == 2 ? modS : 0.f);

    // transient emphasis (pre-shaper)
    if (w.transient > 0.f)
    {
        const float k = 1.f - std::exp (-juce::MathConstants<float>::twoPi * 2500.f / (float) sr);
        const float amt = w.transient * (mode == 0 ? 0.8f : 1.2f);
        for (int c = 0; c < 2; ++c)
        {
            float* x = buf.getWritePointer (c);
            for (int i = 0; i < n; ++i) { tLp[c] += k * (x[i] - tLp[c]); x[i] += amt * (x[i] - tLp[c]); }
        }
    }

    // shaping at 2x oversampling
    juce::dsp::AudioBlock<float> block (buf);
    auto osBlock = oversampling.processSamplesUp (block);
    const size_t osN = osBlock.getNumSamples();
    const float hMix = 0.35f + 0.6f * w.hardness;
    for (size_t c = 0; c < 2; ++c)
    {
        float* p = osBlock.getChannelPointer (c);
        for (size_t i = 0; i < osN; ++i)
        {
            const size_t bi = juce::jmin ((size_t) (n - 1), i >> 1);
            const float x = p[i], xs = x * preBuf[bi];
            float y = shapeFn (w.shape, xs, w.bias);
            if (mode == 0) y += (juce::jlimit (-1.f, 1.f, xs * 1.4f) - y) * hMix;
            y *= makeBuf[bi];
            p[i] = mode == 1 ? x + (y - x) * blendBuf[bi] : y;
        }
    }
    oversampling.processSamplesDown (block);

    // tone, DC block, mix, output
    float op = 0.f;
    for (int c = 0; c < 2; ++c)
    {
        float* y = buf.getWritePointer (c);
        const float* dry = dryBuf.getReadPointer (c);
        for (int i = 0; i < n; ++i)
        {
            const float t = (float) i / (float) n;
            float v = highF[c].processSample (midF[c].processSample (lowF[c].processSample (y[i])));
            const float o = v - dcX[c] + 0.995f * dcY[c];
            dcX[c] = v; dcY[c] = o;
            const float m = prevMix + (mixT - prevMix) * t;
            const float g = prevOut + (outG - prevOut) * t;
            y[i] = (dry[i] * (1.f - m) + o * m) * g;
            op = juce::jmax (op, std::abs (y[i]));
        }
    }
    prevMix = mixT; prevOut = outG;
    if (op > outPeak.load()) outPeak.store (op);
}

juce::AudioProcessorEditor* GunshotAudioProcessor::createEditor() { return new GunshotAudioProcessorEditor (*this); }

void GunshotAudioProcessor::getStateInformation (juce::MemoryBlock& dest)
{
    if (auto xml = apvts.copyState().createXml()) copyXmlToBinary (*xml, dest);
}

void GunshotAudioProcessor::setStateInformation (const void* data, int size)
{
    if (auto xml = getXmlFromBinary (data, size))
        if (xml->hasTagName (apvts.state.getType())) apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new GunshotAudioProcessor(); }
