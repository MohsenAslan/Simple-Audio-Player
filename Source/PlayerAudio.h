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

    // ✅ تعديل موحّد للصوت (Gain)
    void setGain(float gain);
    float getGain() const { return (float)currentVolume; }

    // ✅ ميوت / أنميوت
    void toggleMute();
    bool getMuteState() const { return isMuted; }

private:
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;

    double currentVolume = 1.0;     // الصوت الحالي
    double previousVolume = 1.0;    // الصوت قبل الكتم
    bool isMuted = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerAudio)
};