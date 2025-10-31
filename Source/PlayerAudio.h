#pragma once
#include <JuceHeader.h>

class PlayerAudio
{
public:
    PlayerAudio();
    ~PlayerAudio();

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();
    void loadFile(const juce::File& file);

    void play();
    void stop();
    void restart();
    void pause();
    void goToStart();

    bool isFileLoaded() const;

    void goToEnd();

    void setGain(float gain);
    float getGain() const { return (float)currentVolume; }
    void toggleMute();
    bool getMuteState() const { return isMuted; }

    void toggleLoop();

    void setPosition(double newPositionInSeconds);
    double getPosition() const;
    double getLengthInSecond() const;

    double getTotalLength();

    void setPointA(double newPositionInSecond);
    void setPointB(double newPositionInSecond);
    void toggleLoopAB();
    bool isLoopABEnable() const { return loopABEnabled; }
    void loopBetweenTwoPoints();

    void setResamplingRatio(double spede);

    void setBookmark(double newPositionInSecond);
    void goToBookmark();

    // === Persistence (task) ===
    void saveLastSession(); // CHANGED: now uses PropertiesFile with unique key prefix
    void loadLastSession(); // CHANGED: reads the same keys
    juce::String settingsKeyPrefix = "player_main_"; // ثابت دايمًا


    juce::String getTitle() const;
    juce::String getArtist() const;
    juce::String getDurationString() const;

private:
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    juce::ResamplingAudioSource resamplingAudioSource{ &transportSource, false, 2 };

    // file + metadata
    juce::File lastLoadedFile;
    juce::String title = "---";
    juce::String artist = "Unknown Artist";

    double durationInSeconds = 0.0;
    double currentVolume = 1.0;
    double previousVolume = 1.0;

    bool isMuted = false;
    bool isLooping = false;

    double pointA = 0.0;
    double pointB = 0.0;
    bool loopABEnabled = false;

    double BookmarkPosition = 0.0;

    // CHANGED: prefix key used to store per-player values in the common PropertiesFile
   // e.g. "player_7ffdf1234_"

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerAudio)
};
