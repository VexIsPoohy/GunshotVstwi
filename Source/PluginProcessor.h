#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

constexpr int kNumWeapons = 8;

struct Weapon
{
    const char* name;
    const char* tag;
    float driveScale;   // how hard this weapon pushes the drive
    float bias;         // asymmetry (even harmonics)
    int   shape;        // 0 tanh, 1 hardclip, 2 tube, 3 sine-fold, 4 atan
    float lowDb, midDb, highDb, midHz;
    float transient;    // transient emphasis before the shaper
    float hardness;     // how much hard clipping is blended in (HARD mode)
};
extern const Weapon kWeapons[kNumWeapons];

class GunshotAudioProcessor : public juce::AudioProcessor
{
public:
    GunshotAudioProcessor();
    ~GunshotAudioProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout&) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "GUNSHOT"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}
    void getStateInformation (juce::MemoryBlock&) override;
    void setStateInformation (const void*, int) override;

    juce::AudioProcessorValueTreeState apvts;
    std::atomic<float> inPeak { 0.f }, outPeak { 0.f }, bitActivity { 0.f };

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createLayout();
    void processChunk (juce::AudioBuffer<float>&);
    void updateTone (int mode, int weapon);

    std::atomic<float> *pDrive, *pMix, *pIn, *pOut, *pMode, *pWeapon, *pSens, *pDepth, *pSpeed;

    juce::dsp::Oversampling<float> oversampling;
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::None> dryDelay { 1024 };
    juce::AudioBuffer<float> dryBuf;
    std::vector<float> preBuf, makeBuf, blendBuf;
    juce::SmoothedValue<float> driveSmooth;

    juce::dsp::IIR::Filter<float> lowF[2], midF[2], highF[2];
    float dcX[2] {}, dcY[2] {}, tLp[2] {};
    float envFast = 0, envSlow = 0, modS = 0;
    float prevIn = 1, prevOut = 1, prevMix = 1;
    int lastMode = -1, lastWeapon = -1, maxBlock = 512;
    double sr = 44100.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GunshotAudioProcessor)
};
