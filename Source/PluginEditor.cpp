#include "PluginEditor.h"

namespace
{
    const juce::Colour kBg (0xff0a0a0b), kPanel (0xff151618), kMetal (0xff2b2d31), kRed (0xffd81f1f), kText (0xffc9ccd1);

    juce::Path poly (std::initializer_list<juce::Point<float>> pts)
    {
        juce::Path p; bool first = true;
        for (auto& q : pts) { if (first) { p.startNewSubPath (q); first = false; } else p.lineTo (q); }
        p.closeSubPath(); return p;
    }
    void rect (juce::Path& p, float x, float y, float w, float h) { p.addRectangle (x, y, w, h); }

    juce::Path makeWeapon (int i)
    {
        juce::Path p;
        switch (i)
        {
            case 0: // Desert Eagle
                rect (p, 5, 5, 92, 12); rect (p, 10, 17, 55, 5); rect (p, 88, 2, 5, 3); rect (p, 8, 2, 5, 3);
                p.addPath (poly ({ { 14, 17 }, { 36, 17 }, { 28, 46 }, { 6, 46 } }));
                p.addPath (poly ({ { 40, 17 }, { 62, 17 }, { 58, 27 }, { 44, 27 } })); break;
            case 3: // Glock
                rect (p, 5, 6, 85, 11); rect (p, 8, 3, 4, 3); rect (p, 82, 3, 4, 3);
                p.addPath (poly ({ { 10, 17 }, { 75, 17 }, { 70, 21 }, { 40, 21 }, { 36, 25 }, { 10, 21 } }));
                p.addPath (poly ({ { 10, 17 }, { 40, 17 }, { 33, 46 }, { 6, 46 } }));
                p.addPath (poly ({ { 42, 21 }, { 62, 21 }, { 58, 29 }, { 46, 29 } })); break;
            case 5: // Revolver
                rect (p, 40, 8, 58, 6); rect (p, 18, 6, 26, 18); p.addEllipse (22, 3, 22, 22); rect (p, 94, 4, 4, 4);
                p.addPath (poly ({ { 14, 6 }, { 22, 6 }, { 18, 0 }, { 10, 2 } }));
                p.addPath (poly ({ { 12, 22 }, { 28, 22 }, { 22, 52 }, { 2, 46 } }));
                p.addPath (poly ({ { 30, 24 }, { 46, 24 }, { 42, 32 }, { 33, 32 } })); break;
            case 2: // AK-47
                rect (p, 50, 12, 60, 10); rect (p, 52, 9, 46, 3); rect (p, 110, 14, 60, 3); rect (p, 110, 11, 32, 9);
                rect (p, 110, 8, 40, 3); rect (p, 166, 8, 3, 9); rect (p, 168, 13, 12, 5);
                p.addPath (poly ({ { 72, 22 }, { 90, 22 }, { 100, 44 }, { 86, 50 } }));
                p.addPath (poly ({ { 50, 12 }, { 50, 22 }, { 10, 34 }, { 2, 28 }, { 8, 12 } }));
                p.addPath (poly ({ { 54, 22 }, { 64, 22 }, { 60, 38 }, { 50, 38 } })); break;
            case 6: // MP5
                rect (p, 45, 12, 60, 10); rect (p, 90, 11, 38, 11); rect (p, 125, 14, 22, 4); rect (p, 143, 10, 3, 5);
                rect (p, 10, 14, 35, 3); rect (p, 47, 9, 8, 3);
                p.addPath (poly ({ { 68, 22 }, { 80, 22 }, { 88, 50 }, { 76, 50 } }));
                p.addPath (poly ({ { 45, 12 }, { 45, 18 }, { 8, 19 }, { 3, 15 }, { 8, 11 } }));
                p.addPath (poly ({ { 52, 22 }, { 62, 22 }, { 58, 38 }, { 49, 38 } })); break;
            case 4: // Shotgun
                rect (p, 60, 11, 30, 12); rect (p, 70, 12, 140, 5); rect (p, 70, 18, 120, 4); rect (p, 205, 9, 4, 4);
                p.addRoundedRectangle (120, 16, 32, 9, 3);
                p.addPath (poly ({ { 60, 12 }, { 60, 22 }, { 8, 38 }, { 2, 24 }, { 10, 10 } }));
                p.addPath (poly ({ { 62, 23 }, { 72, 23 }, { 66, 36 }, { 57, 36 } })); break;
            case 1: // Intervention
                rect (p, 100, 14, 128, 3); rect (p, 224, 11, 14, 9); rect (p, 60, 12, 45, 10);
                p.addRoundedRectangle (70, 1, 62, 9, 3); rect (p, 78, 9, 4, 3); rect (p, 118, 9, 4, 3);
                rect (p, 98, 16, 12, 3); p.addEllipse (108, 15, 6, 6); rect (p, 76, 22, 16, 11); rect (p, 20, 7, 38, 5);
                p.addPath (poly ({ { 60, 13 }, { 60, 22 }, { 10, 38 }, { 2, 28 }, { 10, 12 } }));
                p.addPath (poly ({ { 160, 17 }, { 163, 17 }, { 150, 36 }, { 146, 36 } })); break;
            default: // Sniper
                rect (p, 100, 15, 120, 3); rect (p, 218, 13, 8, 7); rect (p, 60, 12, 45, 10);
                p.addRoundedRectangle (76, 3, 44, 6, 2); rect (p, 80, 9, 3, 3); rect (p, 112, 9, 3, 3);
                rect (p, 98, 16, 10, 3); rect (p, 76, 22, 14, 9); rect (p, 22, 8, 33, 5);
                p.addPath (poly ({ { 60, 13 }, { 60, 22 }, { 10, 36 }, { 2, 26 }, { 10, 12 } })); break;
        }
        return p;
    }
}

void GunshotLook::drawRotarySlider (juce::Graphics& g, int x, int y, int w, int h, float pos,
                                    float a0, float a1, juce::Slider&)
{
    auto b = juce::Rectangle<int> (x, y, w, h).toFloat().reduced (6.f);
    const float r = juce::jmin (b.getWidth(), b.getHeight()) / 2.f;
    const auto c = b.getCentre();
    const float ang = a0 + pos * (a1 - a0);

    g.setColour (juce::Colours::black);
    g.fillEllipse (c.x - r, c.y - r, r * 2, r * 2);

    juce::Path track; track.addCentredArc (c.x, c.y, r - 3, r - 3, 0, a0, a1, true);
    g.setColour (kMetal.darker()); g.strokePath (track, juce::PathStrokeType (3.f));
    juce::Path val; val.addCentredArc (c.x, c.y, r - 3, r - 3, 0, a0, ang, true);
    g.setColour (kRed); g.strokePath (val, juce::PathStrokeType (3.f));

    const float kr = r - 10.f;
    g.setGradientFill (juce::ColourGradient (kMetal.brighter (0.3f), c.x, c.y - kr, kPanel, c.x, c.y + kr, false));
    g.fillEllipse (c.x - kr, c.y - kr, kr * 2, kr * 2);
    g.setColour (juce::Colours::black.withAlpha (0.6f)); g.drawEllipse (c.x - kr, c.y - kr, kr * 2, kr * 2, 1.f);

    juce::Path ptr; ptr.addRectangle (-1.5f, -kr + 2.f, 3.f, kr * 0.45f);
    g.setColour (kRed); g.fillPath (ptr, juce::AffineTransform::rotation (ang).translated (c.x, c.y));
}

GunshotAudioProcessorEditor::GunshotAudioProcessorEditor (GunshotAudioProcessor& p)
    : AudioProcessorEditor (&p), proc (p)
{
    setLookAndFeel (&look);
    setSize (900, 660);

    for (int i = 0; i < kNumWeapons; ++i) paths.push_back (makeWeapon (i));

    auto setup = [this] (juce::Slider& s, const char* id, std::unique_ptr<SA>& a, int decimals, const char* suffix = "")
    {
        s.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        s.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 64, 16);
        s.setColour (juce::Slider::textBoxTextColourId, kText);
        s.setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
        s.setRotaryParameters (juce::degreesToRadians (225.f), juce::degreesToRadians (495.f), true);
        s.setNumDecimalPlacesToDisplay (decimals);
        s.setTextValueSuffix (suffix);
        addAndMakeVisible (s);
        a = std::make_unique<SA> (proc.apvts, id, s);
    };
    setup (drive, "drive", aDrive, 2);  setup (mix, "mix", aMix, 2);
    setup (inGain, "in", aIn, 1, " dB"); setup (outGain, "out", aOut, 1, " dB");
    setup (sens, "sens", aSens, 2);     setup (depth, "depth", aDepth, 2);  setup (speed, "speed", aSpeed, 2);

    const char* modes[] = { "HARD", "NORMAL", "BIT" };
    for (int i = 0; i < 3; ++i)
    {
        modeBtn[i].setButtonText (modes[i]);
        modeBtn[i].setColour (juce::TextButton::buttonColourId, kPanel);
        modeBtn[i].setColour (juce::TextButton::buttonOnColourId, kRed.darker (0.3f));
        modeBtn[i].setColour (juce::TextButton::textColourOffId, kText);
        modeBtn[i].setColour (juce::TextButton::textColourOnId, juce::Colours::white);
        modeBtn[i].onClick = [this, i] { setChoice ("mode", i); };
        addAndMakeVisible (modeBtn[i]);
    }
    for (auto* b : { &prevBtn, &nextBtn })
    {
        b->setColour (juce::TextButton::buttonColourId, kPanel);
        b->setColour (juce::TextButton::textColourOffId, kRed);
        addAndMakeVisible (b);
    }
    prevBtn.onClick = [this] { stepWeapon (-1); };
    nextBtn.onClick = [this] { stepWeapon (+1); };

    curWeapon = prevWeapon = (int) *proc.apvts.getRawParameterValue ("weapon");
    curMode = (int) *proc.apvts.getRawParameterValue ("mode");
    startTimerHz (30);
}

GunshotAudioProcessorEditor::~GunshotAudioProcessorEditor() { setLookAndFeel (nullptr); }

void GunshotAudioProcessorEditor::setChoice (const juce::String& id, int index)
{
    if (auto* p = dynamic_cast<juce::AudioParameterChoice*> (proc.apvts.getParameter (id)))
    { p->beginChangeGesture(); *p = index; p->endChangeGesture(); }
}

void GunshotAudioProcessorEditor::stepWeapon (int d)
{
    setChoice ("weapon", (curWeapon + d + kNumWeapons) % kNumWeapons);
}

void GunshotAudioProcessorEditor::mouseWheelMove (const juce::MouseEvent& e, const juce::MouseWheelDetails& w)
{
    if (artArea.expanded (0, 80).contains (e.getPosition()) && std::abs (w.deltaY) > 0.f)
        stepWeapon (w.deltaY > 0 ? -1 : 1);
}

void GunshotAudioProcessorEditor::timerCallback()
{
    const int w = (int) *proc.apvts.getRawParameterValue ("weapon");
    if (w != curWeapon) { prevWeapon = curWeapon; curWeapon = w; fade = 0.f; }
    fade = juce::jmin (1.f, fade + 0.07f);

    const int m = (int) *proc.apvts.getRawParameterValue ("mode");
    curMode = m;
    for (int i = 0; i < 3; ++i) modeBtn[i].setToggleState (i == m, juce::dontSendNotification);
    for (auto* s : { &sens, &depth, &speed }) s->setAlpha (m == 2 ? 1.f : 0.3f);

    auto decay = [] (float& l, float in) { l = juce::jmax (in, l * 0.86f); };
    decay (inLvl, proc.inPeak.exchange (0.f));
    decay (outLvl, proc.outPeak.exchange (0.f));
    act += (proc.bitActivity.load() - act) * 0.4f;
    repaint();
}

void GunshotAudioProcessorEditor::place (juce::Slider& s, const juce::String& label, int x, int y, int w, int h)
{
    labels.push_back ({ label, { x, y, w, 14 } });
    s.setBounds (x, y + 14, w, h - 14);
}

void GunshotAudioProcessorEditor::resized()
{
    labels.clear();
    artArea = { 110, 70, 680, 200 };
    nameArea = { 250, 274, 400, 46 };
    tagArea = { 250, 320, 400, 18 };
    prevBtn.setBounds (190, 278, 44, 38);
    nextBtn.setBounds (666, 278, 44, 38);
    for (int i = 0; i < 3; ++i) modeBtn[i].setBounds (275 + i * 120, 352, 110, 34);

    place (sens,  "SENSITIVITY", 292, 398, 76, 92);
    place (depth, "DEPTH",       412, 398, 76, 92);
    place (speed, "SPEED",       532, 398, 76, 92);

    place (drive,  "DRIVE",  365, 496, 170, 160);
    place (inGain, "INPUT",  255, 530, 90, 120);
    place (mix,    "MIX",    155, 530, 90, 120);
    place (outGain, "OUTPUT", 555, 530, 90, 120);
    actGauge = { 695, 540, 90, 90 };

    inMeter = { 28, 80, 14, 540 };
    outMeter = { 858, 80, 14, 540 };
}

void GunshotAudioProcessorEditor::drawMeter (juce::Graphics& g, juce::Rectangle<float> r, float level)
{
    g.setColour (juce::Colours::black); g.fillRoundedRectangle (r, 3.f);
    const float db = juce::Decibels::gainToDecibels (level, -60.f);
    const float v = juce::jlimit (0.f, 1.f, (db + 60.f) / 60.f);
    auto fill = r.reduced (2.f);
    fill = fill.removeFromBottom (fill.getHeight() * v);
    g.setGradientFill (juce::ColourGradient (kRed, 0, r.getY(), kRed.darker (0.8f), 0, r.getBottom(), false));
    g.fillRoundedRectangle (fill, 2.f);
    g.setColour (kMetal); g.drawRoundedRectangle (r, 3.f, 1.f);
}

void GunshotAudioProcessorEditor::drawWeapon (juce::Graphics& g, int idx, juce::Rectangle<float> area, float alpha, float xOff)
{
    if (alpha <= 0.01f) return;
    auto p = paths[(size_t) idx];
    p.applyTransform (p.getTransformToScaleToFit (area.reduced (30.f, 10.f).translated (xOff, 0.f), true));
    g.setColour (kRed.withAlpha (0.10f * alpha)); g.strokePath (p, juce::PathStrokeType (12.f));
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xff62666d).withAlpha (alpha), 0, area.getY(),
                                             juce::Colour (0xff1a1b1e).withAlpha (alpha), 0, area.getBottom(), false));
    g.fillPath (p);
    g.setColour (kRed.withAlpha (0.6f * alpha)); g.strokePath (p, juce::PathStrokeType (1.2f));
}

void GunshotAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (kBg);
    // brushed-metal texture
    for (int y = 0; y < getHeight(); y += 3)
    {
        const float a = 0.015f + 0.02f * (float) ((y * 37) % 11) / 11.f;
        g.setColour (juce::Colours::white.withAlpha (a)); g.drawHorizontalLine (y, 0.f, (float) getWidth());
    }
    g.setGradientFill (juce::ColourGradient (juce::Colours::transparentBlack, 0, 0, juce::Colours::black.withAlpha (0.55f), 450, 330, true));
    g.fillAll();

    // title
    g.setColour (juce::Colours::white); g.setFont (juce::Font (28.f, juce::Font::bold));
    g.drawText ("GUNSHOT", 70, 18, 300, 36, juce::Justification::centredLeft);
    g.setColour (kRed); g.fillRect (70, 54, 120, 2);
    g.setColour (kText.withAlpha (0.5f)); g.setFont (12.f);
    g.drawText ("DISTORTION  //  VST3", 600, 24, 230, 20, juce::Justification::centredRight);

    // weapon panel
    auto art = artArea.toFloat();
    g.setColour (kPanel); g.fillRoundedRectangle (art, 8.f);
    g.setColour (kMetal); g.drawRoundedRectangle (art, 8.f, 1.f);
    g.setColour (kRed.withAlpha (0.35f));
    for (auto* c : { &art })
    {   // corner brackets
        const float L = 18.f;
        g.drawLine (c->getX() + 8, c->getY() + 8, c->getX() + 8 + L, c->getY() + 8, 1.5f);
        g.drawLine (c->getX() + 8, c->getY() + 8, c->getX() + 8, c->getY() + 8 + L, 1.5f);
        g.drawLine (c->getRight() - 8, c->getBottom() - 8, c->getRight() - 8 - L, c->getBottom() - 8, 1.5f);
        g.drawLine (c->getRight() - 8, c->getBottom() - 8, c->getRight() - 8, c->getBottom() - 8 - L, 1.5f);
    }
    const float e = 1.f - std::pow (1.f - fade, 3.f);
    drawWeapon (g, prevWeapon, art, 1.f - e, -40.f * e);
    drawWeapon (g, curWeapon, art, e, 40.f * (1.f - e));

    // name
    g.setFont (juce::Font (36.f, juce::Font::bold));
    g.setColour (juce::Colours::white.withAlpha (1.f - e)); g.drawText (kWeapons[prevWeapon].name, nameArea, juce::Justification::centred);
    g.setColour (juce::Colours::white.withAlpha (e));       g.drawText (kWeapons[curWeapon].name, nameArea, juce::Justification::centred);
    g.setFont (13.f);
    g.setColour (kText.withAlpha (0.6f * (1.f - e))); g.drawText (kWeapons[prevWeapon].tag, tagArea, juce::Justification::centred);
    g.setColour (kText.withAlpha (0.6f * e));         g.drawText (kWeapons[curWeapon].tag, tagArea, juce::Justification::centred);

    // labels
    g.setFont (juce::Font (11.f, juce::Font::bold)); g.setColour (kText.withAlpha (0.75f));
    for (auto& l : labels) g.drawText (l.first, l.second, juce::Justification::centred);

    // meters
    drawMeter (g, inMeter, inLvl);  drawMeter (g, outMeter, outLvl);
    g.setColour (kText.withAlpha (0.6f)); g.setFont (10.f);
    g.drawText ("IN", (int) inMeter.getX() - 10, (int) inMeter.getBottom() + 4, 34, 14, juce::Justification::centred);
    g.drawText ("OUT", (int) outMeter.getX() - 10, (int) outMeter.getBottom() + 4, 34, 14, juce::Justification::centred);

    // bit activity gauge
    auto gr = actGauge.reduced (8.f);
    const float v = curMode == 2 ? juce::jlimit (0.f, 1.f, act) : 0.f;
    g.setColour (juce::Colours::black); g.fillEllipse (gr);
    g.setColour (kRed.withAlpha (0.08f + 0.8f * v)); g.fillEllipse (gr.reduced (12.f));
    g.setColour (kMetal); g.drawEllipse (gr, 2.f);
    g.setColour (kText.withAlpha (0.75f)); g.setFont (juce::Font (11.f, juce::Font::bold));
    g.drawText ("BIT ACTIVITY", (int) actGauge.getX() - 10, (int) actGauge.getBottom() + 2, (int) actGauge.getWidth() + 20, 14, juce::Justification::centred);
}
