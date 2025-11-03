#include "PlayerGUI.h"
#include "PlaylistComponent.h"
#include <array> // added for safe range-based loops that use fixed-size arrays

// small vertical offset to raise the waveform slightly
static constexpr int kWaveformVerticalOffset = 40;

PlayerGUI::PlayerGUI(PlayerAudio& audioRef)
    : playerAudio(audioRef)
{
    // register formats for thumbnail and audio reading
    formatManager.registerBasicFormats();

    // Put volume/speed as vertical sliders (they will sit to the sides of the waveform)
    volumeSlider.setSliderStyle(juce::Slider::LinearVertical);
    volumeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 48, 18);

    speedSlider.setSliderStyle(juce::Slider::LinearVertical);
    speedSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 48, 18);

    // position slider is horizontal and we hide its textbox (timeLabel shows the time)
    positionSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    positionSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

    for (auto* btn : { &loadButton, &restartButton, &stopButton, &playButton, &pauseButton, &goToStartButton, &goToEndButton, &loopButton, &beginButton, &endButton, &loopABButton, &setBookMarkButton, &goToBookMarkButton, &forwardButton, &backwardButton })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }

    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);

    // volume label
    volumeLabel.setText("Volume", juce::dontSendNotification);
    volumeLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    volumeLabel.setFont(juce::Font(14.0f));
    addAndMakeVisible(volumeLabel);

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

    // bonus 2
    setWantsKeyboardFocus(true);

    // Use std::array to avoid braced-init-list deduction issues on some compilers
    {
        std::array<juce::Label*, 3> lbls = { &titleLabel, &artistLabel, &durationLabel };
        for (auto* lbl : lbls)
        {
            lbl->setColour(juce::Label::textColourId, juce::Colours::white);
            lbl->setFont(juce::Font(16.0f));
            addAndMakeVisible(lbl);
        }
    }

    // Setup playlist inside viewport so it can scroll
    playlistViewport.setViewedComponent(&playlist, false);
    addAndMakeVisible(playlistViewport);

    // We'll move these two controls above the playlist panel in resized()
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
    // Move theme colours into members so paint() can use them.
    // They already have sensible defaults in the header; assign again to keep existing behaviour.
    themeAccentYellow = juce::Colour::fromRGB(255, 215, 0);
    themeDeepViolet  = juce::Colour::fromRGB(100, 0, 160);

    // الأزرار
    for (auto* btn : { &loadButton, &restartButton, &stopButton, &playButton, &pauseButton, &goToStartButton, &goToEndButton, &loopButton, &beginButton, &endButton, &loopABButton, &setBookMarkButton, &goToBookMarkButton, &loadPlaylistButton, &playSelectedButton, &muteButton, &forwardButton, &backwardButton })
    {
        btn->setColour(juce::TextButton::buttonColourId, themeDeepViolet);
        btn->setColour(juce::TextButton::buttonOnColourId, themeAccentYellow);
        btn->setColour(juce::TextButton::textColourOnId, juce::Colours::black);
        btn->setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    }

    // apply same theme to playlist rows and background
    playlist.setTheme(themeDeepViolet, themeAccentYellow);

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
            b.setColour(juce::TextButton::buttonColourId, on ? themeAccentYellow : themeDeepViolet);
            b.setColour(juce::TextButton::textColourOffId, on ? juce::Colours::black : juce::Colours::white);
        };

    applyToggleColour(muteButton);
    applyToggleColour(loopButton);
    applyToggleColour(loopABButton);

    // السلايدر (colors only — styles set above)
    for (auto* slider : { &volumeSlider, &positionSlider, &speedSlider })
    {
        slider->setColour(juce::Slider::thumbColourId, themeAccentYellow);
        slider->setColour(juce::Slider::trackColourId, themeDeepViolet);
        slider->setColour(juce::Slider::backgroundColourId, juce::Colour::fromRGB(40, 0, 60));
    }

    // الليبلز
    {
        std::array<juce::Label*, 6> lbls = { &titleLabel, &artistLabel, &durationLabel, &speedLabel, &timeLabel, &volumeLabel };
        for (auto* lbl : lbls)
        {
            lbl->setColour(juce::Label::textColourId, themeAccentYellow);
        }
    }

    // make waveform taller by default (will be clamped in resized)
    waveformHeight = 180;

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

    // layout: reserve a vertical playlist on the right, draw waveform in left/main area
    int margin = 10;
    int rightPanelWidth = std::max(220, getWidth() / 5); // playlist panel width (small on the right)

    // left/main area dimensions (matches resized())
    int leftAreaX = margin;
    int leftAreaW = getWidth() - (2 * margin) - rightPanelWidth;
    int leftAreaRight = leftAreaX + leftAreaW;

    // Waveform full rect (aligned to bottom of main/left area, then raised by offset)
    juce::Rectangle<int> waveformFullRect(leftAreaX, getHeight() - margin - waveformHeight - kWaveformVerticalOffset, leftAreaW, waveformHeight);

    // make waveform narrower (70% of the available main width) and center it horizontally
    int targetWidth = static_cast<int>(waveformFullRect.getWidth() * 0.7f);
    int horizPad = (waveformFullRect.getWidth() - targetWidth) / 2;
    juce::Rectangle<int> waveformArea = waveformFullRect.reduced(horizPad, 4);

    // subtle background for waveform
    g.setColour(juce::Colours::black.withAlpha(0.25f));
    g.fillRoundedRectangle((float)waveformArea.getX() - 4.0f, (float)waveformArea.getY() - 4.0f,
                           (float)waveformArea.getWidth() + 8.0f, (float)waveformArea.getHeight() + 8.0f, 6.0f);

    if (thumbnail.getTotalLength() > 0.0)
    {
        // Swap theme: waveform -> accent yellow, cursor -> deep violet
        g.setColour(themeAccentYellow.withAlpha(0.95f)); // bright accent for waveform
        // drawChannels into the smaller, centered waveform area
        thumbnail.drawChannels(g, waveformArea.reduced(4), 0.0, thumbnail.getTotalLength(), 1.0f);

        // draw current position cursor relative to the centered waveformArea
        double totalLength = thumbnail.getTotalLength();
        double currentTime = playerAudio.getPosition();
        double proportion = (totalLength > 0.0) ? (currentTime / totalLength) : 0.0;
        int cursorX = waveformArea.getX() + static_cast<int>(proportion * waveformArea.getWidth());
        g.setColour(themeDeepViolet); // use violet for cursor
        g.drawLine((float)cursorX, (float)waveformArea.getY(), (float)cursorX, (float)waveformArea.getBottom(), 2.0f);
    }
    else
    {
        g.setColour(juce::Colours::white.withAlpha(0.6f));
        g.drawFittedText("No waveform loaded", waveformArea, juce::Justification::centred, 1);
    }

    // لمعة حول الإطار
    g.setColour(themeAccentYellow.withAlpha(0.3f));
    g.drawRect(getLocalBounds().reduced(4), 2.0f);
}

void PlayerGUI::resized()
{
    int margin = 10;
    int smallBtnH = 28;
    int smallBtnW = 80;
    int spacing = 8;
    int y = margin;
    int x = margin;

    // reserve a vertical playlist panel on the right
    int rightPanelWidth = std::max(220, getWidth() / 5);
    int leftAreaWidth = getWidth() - (2 * margin) - rightPanelWidth;

    // ========== top small buttons (exclude the main large controls) ==========
    // Layout small buttons in centered rows to make the window symmetric
    std::vector<juce::Button*> smallButtons = {
        &loadButton,
        &goToStartButton,
        &goToEndButton,
        &loopButton,
        &muteButton,
        &beginButton,
        &endButton,
        &loopABButton,
        &setBookMarkButton,
        &goToBookMarkButton,
        &forwardButton,
        &backwardButton
    };

    int maxPerRow = std::max(1, (leftAreaWidth + spacing) / (smallBtnW + spacing));
    int idx = 0;
    int rowY = y;
    while (idx < (int)smallButtons.size())
    {
        int remain = (int)smallButtons.size() - idx;
        int countInRow = std::min(maxPerRow, remain);
        int rowWidth = countInRow * smallBtnW + (countInRow - 1) * spacing;
        int startX = margin + std::max(0, (leftAreaWidth - rowWidth) / 2);

        int curX = startX;
        for (int c = 0; c < countInRow; ++c)
        {
            smallButtons[idx]->setBounds(curX, rowY, smallBtnW, smallBtnH);
            curX += smallBtnW + spacing;
            ++idx;
        }

        rowY += smallBtnH + spacing;
    }

    // compute y after placing small buttons
    y = rowY;

    // ===== make the very important buttons larger and centered in the main area =====
    // important buttons: play, pause, stop, restart
    const int bigW = 96;
    const int bigH = 48;
    const int bigSpacing = 16;

    // Make the Play button slightly narrower so it remains fully visible.
    const int playW = bigW - 18;   // adjust this value to taste
    const int otherW = bigW;

    int totalBigW = playW + otherW * 3 + bigSpacing * 3;
    int bigStartX = margin + std::max(0, (leftAreaWidth - totalBigW) / 2);
    int bigY = y + 12; // place them a bit below the small buttons

    playButton.setBounds(bigStartX, bigY, playW, bigH);
    pauseButton.setBounds(bigStartX + playW + bigSpacing, bigY, otherW, bigH);
    stopButton.setBounds(bigStartX + playW + bigSpacing + otherW + bigSpacing, bigY, otherW, bigH);
    restartButton.setBounds(bigStartX + playW + bigSpacing + otherW + bigSpacing + otherW + bigSpacing, bigY, otherW, bigH);

    // ensure buttons are visible on top and get default look
    for (auto* b : { &playButton, &pauseButton, &stopButton, &restartButton })
    {
        b->setSize(b->getWidth(), b->getHeight()); // Ensure button size is set
        b->setLookAndFeel(nullptr); // Use default look and feel
    }

    // advance y so other elements go below the main controls
    y = bigY + bigH + 16;

    // ===== labels area (still in left/main area) =====
    titleLabel.setBounds(margin, y, leftAreaWidth / 3, 20);
    artistLabel.setBounds(margin + leftAreaWidth / 3, y, leftAreaWidth / 3, 20);
    durationLabel.setBounds(margin + 2 * (leftAreaWidth / 3), y, leftAreaWidth / 3 - 2 * margin, 20);
    y += 30;

    // ===== playlist buttons (keep them at the top of the right panel) =====
    int rpX = getWidth() - rightPanelWidth - margin;
    int rpInnerPad = 8;
    int rpBtnW = rightPanelWidth - rpInnerPad * 2;

    // place the two playlist control buttons near the top margin inside the right panel
    int rpTop = margin;
    loadPlaylistButton.setBounds(rpX + rpInnerPad, rpTop, rpBtnW, 26);
    playSelectedButton.setBounds(rpX + rpInnerPad, rpTop + 30, rpBtnW, 26);

    // make playlist occupy the remaining height of the right panel (below the two buttons)
    int playlistX = rpX + rpInnerPad;
    int playlistY = margin + 62; // push content a bit below the top buttons visually (buttons drawn on top)
    int playlistH = getHeight() - 2 * margin - 62;
    if (playlistH < 100) playlistH = 100;

    // Use the computed playlistY/playlistH so the buttons do NOT overlap/hide the playlist
    playlistViewport.setBounds(playlistX, playlistY, rpBtnW, playlistH);
    playlist.setSize(rpBtnW, playlistH);

    // ----------> labels (keep in left area) - compute label positions relative to left area
    int playlistBtnW = 120;
    int labelY = y - 35;
    int labelH = 24;
    int gap = 8;
    int labelsStartX = margin + (playlistBtnW * 2) + (gap * 3);
    int labelW = (leftAreaWidth - labelsStartX - margin) / 3;
    if (labelW < 80) labelW = 80;

    titleLabel.setBounds(labelsStartX, labelY, labelW, labelH);
    artistLabel.setBounds(labelsStartX + labelW + gap, labelY, labelW, labelH);
    durationLabel.setBounds(labelsStartX + 2 * (labelW + gap), labelY, labelW, labelH);

    // ===== Waveform area and sliders =====
    // Waveform full rect (aligned to bottom of main/left area and raised by the offset)
    int waveformFullX = margin;
    int waveformFullW = leftAreaWidth;
    int waveformFullY = getHeight() - margin - waveformHeight - kWaveformVerticalOffset;

    // make waveform narrower (70% of the available main width) and center it horizontally
    int waveformTargetW = static_cast<int>(waveformFullW * 0.7f);
    int waveformHorizPad = (waveformFullW - waveformTargetW) / 2;
    int waveformX = waveformFullX + waveformHorizPad;
    int waveformY = waveformFullY;

    // position slider should sit directly above the waveform and match its width
    int posSliderH = 28;
    positionSlider.setBounds(waveformX, waveformY - (posSliderH + 8), waveformTargetW, posSliderH);

    // volume (left) and speed (right) vertical sliders beside waveform
    int sideSliderW = 48;
    int sidePad = 12;
    int volX = waveformX - sideSliderW - sidePad;
    int speedX = waveformX + waveformTargetW + sidePad;
    int sideY = waveformY;
    int sideH = waveformHeight;

    volumeSlider.setBounds(volX, sideY, sideSliderW, sideH);
    speedSlider.setBounds(speedX, sideY, sideSliderW, sideH);

    // volume label above left-side slider
    volumeLabel.setBounds(volX, sideY - 20, sideSliderW, 16);

    // speed label above right-side slider
    speedLabel.setBounds(speedX, sideY - 20, sideSliderW, 16);

    // time label near left area but not overlapping playlist
    int timeLabelW = 100;
    timeLabel.setBounds(margin + leftAreaWidth - timeLabelW, waveformY - (posSliderH + 8), timeLabelW, posSliderH);

    // ensure waveformHeight is not too large for small windows
    waveformHeight = std::min(getHeight() / 3, 220);
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
        playerAudio.toggleMute();
        muteButton.setButtonText(playerAudio.getMuteState() ? "Unmute" : "Mute");
        muteButton.setToggleState(playerAudio.getMuteState(), juce::dontSendNotification);

        bool on = muteButton.getToggleState();
        muteButton.setColour(juce::TextButton::buttonColourId, on ? themeAccentYellow : themeDeepViolet);
        muteButton.setColour(juce::TextButton::textColourOffId, on ? juce::Colours::black : juce::Colours::white);
        muteButton.repaint();
    }
    else if (button == &loopButton)
    {
        if (!playerAudio.isFileLoaded())
            return;

        playerAudio.toggleLoop();

        bool on = loopButton.getToggleState();
        loopButton.setColour(juce::TextButton::buttonColourId, on ? themeAccentYellow : themeDeepViolet);
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

            bool on = loopABButton.getToggleState();
            loopABButton.setColour(juce::TextButton::buttonColourId, on ? themeAccentYellow : themeDeepViolet);
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
            juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::canSelectMultipleItems,
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
    else if (button == &forwardButton)
    {
        playerAudio.skipForward(10.0);
    }
    else if (button == &backwardButton)
    {
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
        int hours = totalSeconds / 3600;
        int minutes = totalSeconds / 60;
        int seconds = totalSeconds % 60;

        timeLabel.setText(
            juce::String::formatted("%02d:%02d:%02d", hours, minutes, seconds),
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
    // compute the same waveform area as in paint() so seeking matches the drawn waveform
    int margin = 10;
    int rightPanelWidth = std::max(220, getWidth() / 5);
    int leftAreaX = margin;
    int leftAreaW = getWidth() - (2 * margin) - rightPanelWidth;

    juce::Rectangle<int> waveformFullRect(leftAreaX, getHeight() - margin - waveformHeight - kWaveformVerticalOffset, leftAreaW, waveformHeight);
    int targetWidth = static_cast<int>(waveformFullRect.getWidth() * 0.7f);
    int horizPad = (waveformFullRect.getWidth() - targetWidth) / 2;
    juce::Rectangle<int> waveformArea = waveformFullRect.reduced(horizPad, 4);

    if (waveformArea.contains(event.getPosition()))
    {
        double totalLength = playerAudio.getLengthInSecond();
        if (totalLength > 0.0)
        {
            double clickX = event.x - waveformArea.getX();
            double proportion = clickX / (double)waveformArea.getWidth();
            proportion = juce::jlimit(0.0, 1.0, proportion);
            double newPos = proportion * totalLength;
            playerAudio.setPosition(newPos);
        }
    }
}

// bonus 2
bool PlayerGUI::keyPressed(const juce::KeyPress& key)
{
    if (key == juce::KeyPress::spaceKey)
        playerAudio.togglePlayPause();
    else if (key == juce::KeyPress('l'))
        playerAudio.toggleLoop();
    else if (key == juce::KeyPress('m'))
        playerAudio.toggleMute();
    else if (key == juce::KeyPress('r'))
        playerAudio.restart();
    else if (key == juce::KeyPress::leftKey)
        playerAudio.skipBackward(5.0);
    else if (key == juce::KeyPress::rightKey)
		playerAudio.skipForward(5.0);

    return true;
}