#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"

class PlayerGUI : public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener
{
public:
    PlayerGUI(PlayerAudio& audioRef);
    ~PlayerGUI() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;

    void setGain(float gain);
    float getGain() const;
private:
    PlayerAudio& playerAudio;

    juce::TextButton loadButton{ "Load File" };
    juce::TextButton restartButton{ "Restart" };
    juce::TextButton stopButton{ "Stop" };
    juce::TextButton playButton{ "play" };
    juce::TextButton pauseButton{ "pause" };
    juce::TextButton goToStartButton{ "|< Go To Start" };
    juce::TextButton goToEndButton{ ">| Go To End" };
    juce::TextButton loopButton{ "Loop" };


    juce::Slider volumeSlider;

    juce::TextButton muteButton{ "Mute" };

    std::unique_ptr<juce::FileChooser> fileChooser;

    bool isMuted = false;
    float currentGain = 1.0f;
    float previousVolume = 1.0f;

    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)
};