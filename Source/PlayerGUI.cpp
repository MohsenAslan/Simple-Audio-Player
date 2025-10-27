#include "PlayerGUI.h"

PlayerGUI::PlayerGUI(PlayerAudio& audioRef)
    : playerAudio(audioRef)
{
    for (auto* btn : { &loadButton, &restartButton, &stopButton, &playButton, &pauseButton, &goToStartButton, &goToEndButton , &loopButton , &beginButton , &endButton
        , &loopABButton , & setBookMarkButton ,& goToBookMarkButton })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }

    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);


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
    int y = 20;
    loadButton.setBounds(20, y, 100, 40);
    restartButton.setBounds(140, y, 80, 40);
    stopButton.setBounds(240, y, 80, 40);
    muteButton.setBounds(335, y, 70, 40);
    loopButton.setBounds(420, y, 80, 40);
    beginButton.setBounds(525, y, 80, 40);
    endButton.setBounds(625, y, 80, 40);
    loopABButton.setBounds(725, y, 120, 40);
    setBookMarkButton.setBounds(865, y, 80, 40);
    goToBookMarkButton.setBounds(965, y, 80, 40);

    

    int labelWidth = 90;
    int spacing = 10;
    int sliderWidth = getWidth() - (40 + labelWidth + spacing);
    positionSlider.setBounds(20, 500 ,sliderWidth, 30);
    timeLabel.setBounds(positionSlider.getRight()+spacing, 500, labelWidth, 30);

    volumeSlider.setBounds(20, 400, sliderWidth, 30);


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
                if (file.existsAsFile()) {
                    playerAudio.loadFile(file);


                    positionSlider.setRange(0.0, playerAudio.getLengthInSecond(), 0.01); //-------->Silder position 
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
}﻿
