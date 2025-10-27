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
    void goToEnd();

    bool isFileLoaded() const;

    // ✅ تعديل موحّد للصوت (Gain)
    void setGain(float gain);
    float getGain() const { return (float)currentVolume; }
    void toggleMute();
    bool getMuteState() const { return isMuted; }
    void toggleLoop();

    //setLooping(bool shouldLoop);
    //void setLooping(bool shouldLoop);
    void setPosition(double newPositionInSeconds);
    double getPosition() const;
    double getLengthInSecond() const;
    void setPoistionA(double position);
    double getCurrentPosition();
    double getTotalLength();

    void setPointA(double newPositionInSecond);
    void setPointB(double newPositionInSecond);
    void loopBetweenTwoPoints();
    void toggleLoopAB();
    bool isLoopABEnable() const { return loopABEnabled; }

    // bonus 
    void setBookmark(double newPositionInSecond);
    void goToBookmark();


private:
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioFormatReader* reader = nullptr;
    juce::AudioTransportSource transportSource;


    double currentVolume = 1.0;     // الصوت الحالي
    double previousVolume = 1.0;    // الصوت قبل الكتم
    bool isMuted = false;

    bool isLooping = false;
    double pointA = 0.0;
    double pointB = 0.0;
    bool loopABEnabled = false;

    // bonus 
    double BookmarkPosition = 0.0;


    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerAudio)

};
