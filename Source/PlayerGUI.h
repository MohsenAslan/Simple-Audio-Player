#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"
#include "PlaylistComponent.h"

class PlayerGUI : public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener,
    public juce::Timer
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

    void mouseDown(const juce::MouseEvent& event) override; // to seek in waveforma

    // bonus 2
    bool keyPressed(const juce::KeyPress& key) override;

private:
    PlayerAudio& playerAudio;

    juce::TextButton loadButton{ "Load File" };
    juce::TextButton restartButton{ "Restart" };
    juce::TextButton stopButton{ "Stop" };
    juce::TextButton playButton{ "Play" };
    juce::TextButton pauseButton{ "Pause" };
    juce::TextButton goToStartButton{ "|< Go To Start" };
    juce::TextButton goToEndButton{ ">| Go To End" };
    juce::TextButton loopButton{ "Loop" };

    juce::TextButton beginButton{ "StartLoop" };
    juce::TextButton endButton{ "EndLoop" };
    juce::TextButton loopABButton{ "Mini Loop" };

    // Bonus
    juce::TextButton setBookMarkButton{ "Set mark" };
    juce::TextButton goToBookMarkButton{ "Go To Book mark" };


    // Bonus
  //---------------------------------------------------
    juce::TextButton forwardButton{ ">> +10s" };
    juce::TextButton backwardButton{ "<< -10s" };
    //----------------------------------------------------

    juce::Slider volumeSlider;
    juce::Label volumeLabel;               // <--- added (was referenced from cpp)
    juce::Slider positionSlider;
    juce::Slider speedSlider;       // ? ????
    juce::Label speedLabel;         // ? ????
    juce::Label timeLabel;

    juce::TextButton muteButton{ "Mute" };

    std::unique_ptr<juce::FileChooser> fileChooser;

    bool isMuted = false;
    float currentGain = 1.0f;
    float previousVolume = 1.0f;

    bool stratLoop = false;
    bool endLoop = false;



    // Playlist inside a viewport to allow scrolling
    PlaylistComponent playlist;
    juce::Viewport playlistViewport;
    juce::TextButton loadPlaylistButton{ "Load Playlist" };
    juce::TextButton playSelectedButton{ "Play Selected" };

    juce::Label titleLabel;
    juce::Label artistLabel;
    juce::Label durationLabel;

    // Waveform / thumbnail
    juce::AudioFormatManager formatManager;
    juce::AudioThumbnailCache thumbnailCache{ 5 };
    juce::AudioThumbnail thumbnail{ 512, formatManager, thumbnailCache };
    int waveformHeight = 120; // height of waveform area

    // Theme colours used by PlayerGUI.cpp (declare here so cpp can reference them)
    juce::Colour themeAccentYellow { juce::Colour::fromRGB(255, 215, 0) };
    juce::Colour themeDeepViolet  { juce::Colour::fromRGB(100, 0, 160) };


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)
};