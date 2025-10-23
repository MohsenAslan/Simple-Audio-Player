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
    void setGain(float gain);
    float getGain() const { return (float)currentVolume; }
    void toggleMute();
    bool getMuteState() const { return isMuted; }
    void toggleLoop();

 
private:
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;


    double currentVolume = 1.0;     
    double previousVolume = 1.0; 
    bool isMuted = false;

    bool isLooping = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerAudio)

};
