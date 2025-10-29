#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"
#include"PlaylistComponent.h"

class PlayerGUI : public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener,
    public::juce::Timer
{
public:
    PlayerGUI(PlayerAudio& audioRef);
    ~PlayerGUI() override;


    void paint(juce::Graphics& g) override;
    void resized() override;

    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;

    void timerCallback() override;
    void setGain(float gain);
    float getGain() const;
    void updateMetadataDisplay();
    
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

    juce::TextButton beginButton{ "StartLoop" };
    juce::TextButton endButton{ "EndLoop" };
    juce::TextButton loopABButton{ "Mini Loop" };

    // Bonus
    juce::TextButton setBookMarkButton{ "Set mark" };
    juce::TextButton goToBookMarkButton{ "Go To mark" };
    


    juce::Slider volumeSlider;
    juce::Slider positionSlider;
    juce::Label timeLabel;
    

    juce::TextButton muteButton{ "Mute" };

    std::unique_ptr<juce::FileChooser> fileChooser;

    bool isMuted = false;
    float currentGain = 1.0f;
    float previousVolume = 1.0f;

    bool stratLoop = false;
    bool endLoop = false;


    PlaylistComponent playlist;
    juce::TextButton loadPlaylistButton{ "Load Playlist" };
    juce::TextButton playSelectedButton{ "Play Selected" };

    juce::Label titleLabel;
    juce::Label artistLabel;
    juce::Label durationLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)
};

