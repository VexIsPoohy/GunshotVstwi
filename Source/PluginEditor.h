#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
#include "PluginProcessor.h"

class GunshotLook : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider (juce::Graphics&, int x, int y, int w, int h, float pos,
                           float startAngle, float endAngle, juce::Slider&) override;
};

class GunshotAudioProcessorEditor : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    explicit GunshotAudioProcessorEditor (GunshotAudioProcessor&);
    ~GunshotAudioProcessorEditor() override;
    void paint (juce::Graphics&) override;
    void resized() override;
    void mouseWheelMove (const juce::MouseEvent&, const juce::MouseWheelDetails&) override;

private:
    using SA = juce::AudioProcessorValueTreeState::SliderAttachment;
    void timerCallback() override;
    void setChoice (const juce::String& id, int index);
    void stepWeapon (int delta);
    void drawWeapon (juce::Graphics&, int idx, juce::Rectangle<float> area, float alpha, float xOff);
    void drawMeter (juce::Graphics&, juce::Rectangle<float> r, float level);
    void place (juce::Slider&, const juce::String& label, int x, int y, int w, int h);

    GunshotAudioProcessor& proc;
    GunshotLook look;

    juce::Slider drive, mix, inGain, outGain, sens, depth, speed;
    std::unique_ptr<SA> aDrive, aMix, aIn, aOut, aSens, aDepth, aSpeed;
    juce::TextButton modeBtn[3], prevBtn { "<" }, nextBtn { ">" };

    std::vector<juce::Path> paths;
    std::vector<std::pair<juce::String, juce::Rectangle<int>>> labels;
    juce::Rectangle<int> artArea, nameArea, tagArea;
    juce::Rectangle<float> inMeter, outMeter, actGauge;

    int curWeapon = 0, prevWeapon = 0, curMode = 0;
    float fade = 1.f, inLvl = 0.f, outLvl = 0.f, act = 0.f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GunshotAudioProcessorEditor)
};
