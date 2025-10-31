#include "PlayerGUI.h"
#include "PlaylistComponent.h"

PlayerGUI::PlayerGUI(PlayerAudio& audioRef)
    : playerAudio(audioRef)
{
    // register formats for thumbnail and audio reading
    formatManager.registerBasicFormats();

    for (auto* btn : { &loadButton, &restartButton, &stopButton, &playButton, &pauseButton,
                       &goToStartButton, &goToEndButton, &loopButton, &beginButton, &endButton,
                       &loopABButton, &setBookMarkButton, &goToBookMarkButton,& forwardButton,& backwardButton })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }

    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);

    // ✅ Speed Slider
    speedSlider.setRange(0.5, 2.0, 0.01);
    speedSlider.setValue(1.0);
    speedSlider.addListener(this);
    addAndMakeVisible(speedSlider);

    speedLabel.setText("Speed", juce::dontSendNotification);
    speedLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    speedLabel.setFont(juce::Font(14.0f));
    addAndMakeVisible(speedLabel);

    titleLabel.setText("Title: ---", juce::dontSendNotification);
    artistLabel.setText("Artist: ---", juce::dontSendNotification);
    durationLabel.setText("Duration: ---", juce::dontSendNotification);

    for (auto* lbl : { &titleLabel, &artistLabel, &durationLabel })
    {
        lbl->setColour(juce::Label::textColourId, juce::Colours::white);
        lbl->setFont(juce::Font(16.0f));
        addAndMakeVisible(lbl);
    }

    // Setup playlist inside viewport so it can scroll
    playlistViewport.setViewedComponent(&playlist, false);
    addAndMakeVisible(playlistViewport);

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

    muteButton.setButtonText("Mute");
    muteButton.addListener(this);
    addAndMakeVisible(muteButton);

    // 🎨 === تلوين الثيم البنفسجي والأصفر ===
    auto accentYellow = juce::Colour::fromRGB(255, 215, 0);
    auto deepViolet = juce::Colour::fromRGB(100, 0, 160);

    // الأزرار
    for (auto* btn : { &loadButton, &restartButton, &stopButton, &playButton, &pauseButton,
                       &goToStartButton, &goToEndButton, &loopButton, &beginButton, &endButton,
                       &loopABButton, &setBookMarkButton, &goToBookMarkButton,
                       &loadPlaylistButton, &playSelectedButton,& muteButton ,
                   & forwardButton,& backwardButton })
    {
        btn->setColour(juce::TextButton::buttonColourId, deepViolet);
        btn->setColour(juce::TextButton::buttonOnColourId, accentYellow);
        btn->setColour(juce::TextButton::textColourOnId, juce::Colours::black);
        btn->setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    }
    // apply same theme to playlist rows and background
    playlist.setTheme(deepViolet, accentYellow);

    // Configure buttons that represent persistent toggle states so their color
    // will reflect on/off (deepViolet <-> accentYellow)
    muteButton.setClickingTogglesState(true);
    loopButton.setClickingTogglesState(true);
    loopABButton.setClickingTogglesState(true);

    // initialize toggle states to match PlayerAudio where a getter exists
    muteButton.setToggleState(playerAudio.getMuteState(), juce::dontSendNotification);
    loopABButton.setToggleState(playerAudio.isLoopABEnable(), juce::dontSendNotification);

    // Ensure those toggle buttons visually reflect their initial state
    auto applyToggleColour = [&](juce::TextButton& b)
        {
            bool on = b.getToggleState();
            b.setColour(juce::TextButton::buttonColourId, on ? accentYellow : deepViolet);
            b.setColour(juce::TextButton::textColourOffId, on ? juce::Colours::black : juce::Colours::white);
        };

    applyToggleColour(muteButton);
    applyToggleColour(loopButton);
    applyToggleColour(loopABButton);

    // السلايدر
    for (auto* slider : { &volumeSlider, &positionSlider, &speedSlider })
    {
        slider->setColour(juce::Slider::thumbColourId, accentYellow);
        slider->setColour(juce::Slider::trackColourId, deepViolet);
        slider->setColour(juce::Slider::backgroundColourId, juce::Colour::fromRGB(40, 0, 60));
    }

    // الليبلز
    for (auto* lbl : { &titleLabel, &artistLabel, &durationLabel, &speedLabel, &timeLabel })
    {
        lbl->setColour(juce::Label::textColourId, accentYellow);
    }

    startTimerHz(30);
}

PlayerGUI::~PlayerGUI() {}

void PlayerGUI::paint(juce::Graphics& g)
{
    // === خلفية بنفسجية متدرجة ===
    juce::ColourGradient backgroundGradient(
        juce::Colour::fromRGB(30, 0, 60), 0, 0,
        juce::Colour::fromRGB(50, 0, 80), 0, (float)getHeight(), false);
    g.setGradientFill(backgroundGradient);
    g.fillRect(getLocalBounds());

    // Draw waveform area at the bottom
    auto area = getLocalBounds();
    auto waveformArea = area.removeFromBottom(waveformHeight).reduced(10, 10);

    g.setColour(juce::Colours::black.withAlpha(0.25f));
    g.fillRect(waveformArea);

    if (thumbnail.getTotalLength() > 0.0)
    {
        g.setColour(juce::Colour::fromRGB(200, 180, 255)); // بنفسجي فاتح للموجة
        thumbnail.drawChannels(g, waveformArea.reduced(4), 0.0, thumbnail.getTotalLength(), 1.0f);

        // draw current position cursor
        double totalLength = thumbnail.getTotalLength();
        double currentTime = playerAudio.getPosition();
        double proportion = (totalLength > 0.0) ? (currentTime / totalLength) : 0.0;
        int cursorX = waveformArea.getX() + static_cast<int>(proportion * waveformArea.getWidth());

        g.setColour(juce::Colour::fromRGB(255, 215, 0)); // أصفر لمّاع
        g.drawLine((float)cursorX, (float)waveformArea.getY(), (float)cursorX, (float)waveformArea.getBottom(), 2.0f);
    }
    else
    {
        g.setColour(juce::Colours::white.withAlpha(0.6f));
        g.drawFittedText("No waveform loaded", waveformArea, juce::Justification::centred, 1);
    }

    // لمعة حول الإطار
    g.setColour(juce::Colour::fromRGB(255, 215, 0).withAlpha(0.3f));
    g.drawRect(getLocalBounds().reduced(4), 2.0f);
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
                       &playButton, &pauseButton, &goToStartButton, &goToEndButton,
                   & forwardButton,& backwardButton })
    {
        btn->setBounds(x, y, buttonWidth, buttonHeight);
        x += buttonWidth + spacing;
    }

    y += buttonHeight + 20;

    int slidersHeight = 20;
    int sliderSpacing = 8;
    int timeLabelWidth = 80;
    int availableWidth = getWidth() - (2 * margin) - timeLabelWidth - (sliderSpacing * 3);
    int oneSliderW = availableWidth / 3;
    int sx = margin;

    positionSlider.setBounds(sx, y, oneSliderW, slidersHeight);
    sx += oneSliderW + sliderSpacing;

    volumeSlider.setBounds(sx, y, oneSliderW, slidersHeight);
    sx += oneSliderW + sliderSpacing;

    speedSlider.setBounds(sx, y, oneSliderW, slidersHeight);
    speedLabel.setBounds(sx, y - 18, 60, 16);

    timeLabel.setBounds(getWidth() - margin - timeLabelWidth, y, timeLabelWidth, slidersHeight);

    y += slidersHeight + 10;

    int bottomReserved = waveformHeight + 2 * margin;
    int playlistTop = y + 40;
    int playlistHeight = getHeight() - playlistTop - bottomReserved;
    if (playlistHeight < 80) playlistHeight = 80;

    playlistViewport.setBounds(margin, playlistTop, getWidth() - 2 * margin, playlistHeight);
    playlist.setSize(getWidth() - 2 * margin, playlistHeight);
}

void PlayerGUI::buttonClicked(juce::Button* button)
{
    // Removed the global/static per-click color toggle (it caused cross-button state
    // and first-click inconsistencies). Toggle state is now handled per-button.

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

                    thumbnail.clear();
                    thumbnail.setSource(new juce::FileInputSource(file));
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
        // audio state toggles; button is configured as a toggle button so its visual
        // state is managed separately. Sync text / toggle state with underlying audio.
        playerAudio.toggleMute();
        muteButton.setButtonText(playerAudio.getMuteState() ? "Unmute" : "Mute");
        muteButton.setToggleState(playerAudio.getMuteState(), juce::dontSendNotification);

        // ensure color matches new state
        auto accentYellow = juce::Colour::fromRGB(255, 215, 0);
        auto deepViolet = juce::Colour::fromRGB(100, 0, 160);
        bool on = muteButton.getToggleState();
        muteButton.setColour(juce::TextButton::buttonColourId, on ? accentYellow : deepViolet);
        muteButton.setColour(juce::TextButton::textColourOffId, on ? juce::Colours::black : juce::Colours::white);
        muteButton.repaint();
    }
    else if (button == &loopButton)
    {
        if (!playerAudio.isFileLoaded())
            return;

        playerAudio.toggleLoop();
        // loopButton is a toggle button; JUCE toggles its state automatically when clicked.
        // update its visual colour to match its toggle state:
        auto accentYellow = juce::Colour::fromRGB(255, 215, 0);
        auto deepViolet = juce::Colour::fromRGB(100, 0, 160);
        bool on = loopButton.getToggleState();
        loopButton.setColour(juce::TextButton::buttonColourId, on ? accentYellow : deepViolet);
        loopButton.setColour(juce::TextButton::textColourOffId, on ? juce::Colours::black : juce::Colours::white);

        static bool loopOn2 = false;
        loopOn2 = !loopOn2;
        loopButton.setButtonText(loopOn2 ? "Loop: ON" : "Loop: OFF");
        loopButton.repaint();
    }
    else if (button == &beginButton)
    {
        stratLoop = true;
        playerAudio.setPointA(positionSlider.getValue());
    }
    else if (button == &endButton)
    {
        endLoop = true;
        playerAudio.setPointB(positionSlider.getValue());
    }
    else if (button == &loopABButton)
    {
        if (!playerAudio.isFileLoaded())
            return;
        else if (stratLoop && endLoop)
        {
            playerAudio.toggleLoopAB();
            loopABButton.setButtonText(playerAudio.isLoopABEnable() ? "MiniLoop: ON" : "MiniLoop: OFF");
            loopABButton.setToggleState(playerAudio.isLoopABEnable(), juce::dontSendNotification);

            auto accentYellow = juce::Colour::fromRGB(255, 215, 0);
            auto deepViolet = juce::Colour::fromRGB(100, 0, 160);
            bool on = loopABButton.getToggleState();
            loopABButton.setColour(juce::TextButton::buttonColourId, on ? accentYellow : deepViolet);
            loopABButton.setColour(juce::TextButton::textColourOffId, on ? juce::Colours::black : juce::Colours::white);
            loopABButton.repaint();
        }
    }
    else if (button == &setBookMarkButton)
        playerAudio.setBookmark(positionSlider.getValue());
    else if (button == &goToBookMarkButton)
        playerAudio.goToBookmark();
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

                thumbnail.clear();
                thumbnail.setSource(new juce::FileInputSource(selectedFile));
            }
        }
       

    }
    else if (button == &forwardButton) {
        playerAudio.skipForward(10.0);
        }
    else if (button == &backwardButton) {
        playerAudio.skipBackward(10.0);
            }
}

void PlayerGUI::timerCallback()
{
    if (playerAudio.isFileLoaded())
    {
        double currentTime = playerAudio.getPosition();
        int hours = static_cast<int>(currentTime) / 3600;
        int minutes = static_cast<int>(currentTime) / 60;
        int seconds = static_cast<int>(currentTime) % 60;

        juce::String timeText = juce::String::formatted("%02d:%02d:%02d", hours, minutes, seconds);
        timeLabel.setText(timeText, juce::dontSendNotification);
        positionSlider.setValue(currentTime, juce::dontSendNotification);

        playerAudio.loopBetweenTwoPoints();
    }

    repaint();
}

void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
        playerAudio.setGain((float)slider->getValue());
    else if (slider == &positionSlider)
    {
        playerAudio.setPosition((float)slider->getValue());
        int totalSeconds = (int)slider->getValue();
        int minutes = totalSeconds / 60;
        int seconds = totalSeconds % 60;
        timeLabel.setText(juce::String(minutes) + ":" + (seconds < 10 ? "0" : "") + juce::String(seconds),
            juce::dontSendNotification);
    }
    else if (slider == &speedSlider)
        playerAudio.setResamplingRatio(speedSlider.getValue());
}

void PlayerGUI::updateMetadataDisplay()
{
    titleLabel.setText("Title: " + playerAudio.getTitle(), juce::dontSendNotification);
    artistLabel.setText("Artist: " + playerAudio.getArtist(), juce::dontSendNotification);
    durationLabel.setText("Duration: " + playerAudio.getDurationString(), juce::dontSendNotification);
}

void PlayerGUI::mouseDown(const juce::MouseEvent& event)
{
    auto area = getLocalBounds().removeFromBottom(waveformHeight).reduced(10, 10);
    if (area.contains(event.getPosition()))
    {
        double totalLength = playerAudio.getLengthInSecond();
        if (totalLength > 0.0)
        {
            double clickX = event.x - area.getX();
            double proportion = clickX / (double)area.getWidth();
            proportion = juce::jlimit(0.0, 1.0, proportion);
            double newPos = proportion * totalLength;
            playerAudio.setPosition(newPos);
        }
    }
}