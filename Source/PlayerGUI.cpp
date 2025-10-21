#include "PlayerGUI.h"

PlayerGUI::PlayerGUI(PlayerAudio& audioRef)
    : playerAudio(audioRef)
{
    for (auto* btn : { &loadButton, &restartButton, &stopButton, &playButton, &pauseButton, &goToStartButton, &goToEndButton })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }

    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);

    muteButton.setButtonText("Mute");
    muteButton.addListener(this);
    addAndMakeVisible(muteButton);
}

PlayerGUI::~PlayerGUI() {}

void PlayerGUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void PlayerGUI::resized()
{
    int y = 20;
    loadButton.setBounds(20, y, 100, 40);
    restartButton.setBounds(140, y, 80, 40);
    stopButton.setBounds(240, y, 80, 40);
    muteButton.setBounds(335, y, 70, 40);

    volumeSlider.setBounds(20, 100, getWidth() - 40, 30);

    int y2 = 160;
    int buttonWidth = 100;
    playButton.setBounds(20, y2, buttonWidth, 40);
    pauseButton.setBounds(130, y2, buttonWidth, 40);
    goToStartButton.setBounds(240, y2, buttonWidth + 20, 40);
    goToEndButton.setBounds(370, y2, buttonWidth + 20, 40);
}

void PlayerGUI::buttonClicked(juce::Button* button)
{
    if (button == &loadButton)
    {
        fileChooser = std::make_unique<juce::FileChooser>(
            "Select an audio file...", juce::File{}, "*.wav;*.mp3");

        fileChooser->launchAsync(
            juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser& fc)
            {
                auto file = fc.getResult();
                if (file.existsAsFile())
                    playerAudio.loadFile(file);
            });
    }
    else if (button == &restartButton)
        playerAudio.restart();
    else if (button == &stopButton)
        playerAudio.stop();
    else if (button == &playButton)
        playerAudio.play();
    else if (button == &pauseButton)
        playerAudio.pause();
    else if (button == &goToStartButton)
        playerAudio.goToStart();
    else if (button == &goToEndButton)
        playerAudio.goToEnd();
    else if (button == &muteButton)
    {
        playerAudio.toggleMute();
        muteButton.setButtonText(playerAudio.getMuteState() ? "Unmute" : "Mute");
    }
}

void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
        playerAudio.setGain((float)slider->getValue());
}
