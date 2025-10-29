#include "PlayerGUI.h"

PlayerGUI::PlayerGUI(PlayerAudio& audioRef)
    : playerAudio(audioRef)
{
    for (auto* btn : { &loadButton, &restartButton, &stopButton, &playButton, &pauseButton,
                       &goToStartButton, &goToEndButton, &loopButton, &beginButton, &endButton,
                       &loopABButton, &setBookMarkButton, &goToBookMarkButton })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }

    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);

    titleLabel.setText("Title: ---", juce::dontSendNotification);
    artistLabel.setText("Artist: ---", juce::dontSendNotification);
    durationLabel.setText("Duration: ---", juce::dontSendNotification);

    for (auto* lbl : { &titleLabel, &artistLabel, &durationLabel })
    {
        lbl->setColour(juce::Label::textColourId, juce::Colours::white);
        lbl->setFont(juce::Font(16.0f));
        addAndMakeVisible(lbl);
    }

    addAndMakeVisible(playlist);
    addAndMakeVisible(loadPlaylistButton);
    addAndMakeVisible(playSelectedButton);

    loadPlaylistButton.addListener(this);
    playSelectedButton.addListener(this);

    positionSlider.setRange(0.0, 1.0, 0.01);
    positionSlider.addListener(this);
    addAndMakeVisible(positionSlider);

    addAndMakeVisible(timeLabel);
    timeLabel.setText("00:00:00", juce::dontSendNotification);
    timeLabel.setJustificationType(juce::Justification::centred);

    startTimerHz(30);

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
    int margin = 10;
    int buttonHeight = 30;
    int buttonWidth = 80;
    int spacing = 5;


    int y = margin;
    int x = margin;

    for (auto* btn : { &loadButton, &restartButton, &stopButton, &muteButton,
                       &loopButton, &beginButton, &endButton, &loopABButton,
                       &setBookMarkButton, &goToBookMarkButton,
                       &playButton, &pauseButton, &goToStartButton, &goToEndButton })
    {
        btn->setBounds(x, y, buttonWidth, buttonHeight);
        x += buttonWidth + spacing;
    }


    y += buttonHeight + 20;
    int sliderWidth = getWidth() - (2 * margin + 100);
    volumeSlider.setBounds(margin, y, sliderWidth, 20);

    y += 40;
    positionSlider.setBounds(margin, y, sliderWidth, 20);
    timeLabel.setBounds(positionSlider.getRight() + 10, y, 80, 20);


    y += 40;
    titleLabel.setBounds(margin, y, getWidth() - 40, 20);
    artistLabel.setBounds(margin, y + 25, getWidth() - 40, 20);
    durationLabel.setBounds(margin, y + 50, getWidth() - 40, 20);


    y += 90;
    loadPlaylistButton.setBounds(margin, y, 120, 30);
    playSelectedButton.setBounds(margin + 130, y, 120, 30);
    playlist.setBounds(margin, y + 40, getWidth() - 2 * margin, 120);
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
                {
                    playerAudio.loadFile(file);
                    updateMetadataDisplay();

                    positionSlider.setRange(0.0, playerAudio.getLengthInSecond(), 0.01);
                }
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
    
    else if (button == &loopButton)
    {
        if (!playerAudio.isFileLoaded()) { // ->
            return;
        }
        playerAudio.toggleLoop();

        static bool loopOn2 = false;
        loopOn2 = !loopOn2;

        loopButton.setButtonText(loopOn2 ? "Loop: ON" : "Loop: OFF");
    }
    else if (button == &beginButton){
        stratLoop = true;
        playerAudio.setPointA(positionSlider.getValue());

    }
    else if (button == &endButton) {
        endLoop = true;
        playerAudio.setPointB(positionSlider.getValue());
        
    }
    else if (button == &loopABButton) {
        if (!playerAudio.isFileLoaded()) {//->
            return;
        }
        else if (stratLoop && endLoop) {
            playerAudio.toggleLoopAB();
            loopABButton.setButtonText(playerAudio.isLoopABEnable() ? "MiniLoop: ON" : "MiniLoop: OFF");
        }
    }
    else if (button == &setBookMarkButton) {

        playerAudio.setBookmark(positionSlider.getValue());

    }
    else if (button == &goToBookMarkButton) {

        playerAudio.goToBookmark();
    }
    else if (button == &loadPlaylistButton)
    {
        fileChooser = std::make_unique<juce::FileChooser>(
            "Select multiple audio files...", juce::File{}, "*.wav;*.mp3");

        fileChooser->launchAsync(
            juce::FileBrowserComponent::openMode |
            juce::FileBrowserComponent::canSelectFiles |
            juce::FileBrowserComponent::canSelectMultipleItems,
            [this](const juce::FileChooser& fc)
            {
                auto results = fc.getResults();
                for (auto file : results)
                    playlist.addFile(file);
            });
    }
    else if (button == &playSelectedButton)
    {
        int selected = playlist.getSelectedRow();
        if (selected >= 0)
        {
            juce::File selectedFile = playlist.getFile(selected);
            if (selectedFile.existsAsFile())
            {
                playerAudio.loadFile(selectedFile);
                playerAudio.play();
                updateMetadataDisplay();
            }
        }
    }





}

void PlayerGUI::timerCallback(){
    if (playerAudio.isFileLoaded()) {
       
        double currentTime = playerAudio.getPosition(); // الوقت الحالي بالثواني
        int hours = static_cast<int>(currentTime) / 3600;
        int minutes = static_cast<int>(currentTime) / 60;
        int seconds = static_cast<int>(currentTime) % 60;

        juce::String timeText = juce::String::formatted("%02d:%02d:%02d",hours, minutes, seconds);
        timeLabel.setText(timeText, juce::dontSendNotification);

        // تحديث السلايدر عشان يتحرك مع الصوت
        positionSlider.setValue(currentTime, juce::dontSendNotification);

        playerAudio.loopBetweenTwoPoints();
    }

}



void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
        playerAudio.setGain((float)slider->getValue());

    else if (slider == &positionSlider)
    {
        playerAudio.setPosition((float)slider->getValue());

        // تحويل الثواني إلى دقائق:ثواني
        int totalSeconds = (int)slider->getValue();
        int minutes = totalSeconds / 60;
        int seconds = totalSeconds % 60;

        timeLabel.setText(juce::String(minutes) + ":" + (seconds < 10 ? "0" : "") + juce::String(seconds),
            juce::dontSendNotification);
    }
};
void PlayerGUI::updateMetadataDisplay()
{
    titleLabel.setText("Title: " + playerAudio.getTitle(), juce::dontSendNotification);
    artistLabel.setText("Artist: " + playerAudio.getArtist(), juce::dontSendNotification);
    durationLabel.setText("Duration: " + playerAudio.getDurationString(), juce::dontSendNotification);
}
