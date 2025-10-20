#include "PlayerAudio.h"

PlayerAudio::PlayerAudio()
{
    formatManager.registerBasicFormats();
}

PlayerAudio::~PlayerAudio()
{
    transportSource.releaseResources();
}

void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    transportSource.getNextAudioBlock(bufferToFill);
}

void PlayerAudio::releaseResources()
{
    transportSource.releaseResources();
}

void PlayerAudio::loadFile(const juce::File& file)
{
    if (auto* reader = formatManager.createReaderFor(file))
    {
        transportSource.stop();
        transportSource.setSource(nullptr);
        readerSource.reset(new juce::AudioFormatReaderSource(reader, true));

        transportSource.setSource(readerSource.get(), 0, nullptr, reader->sampleRate);
        play();
    }
}

void PlayerAudio::play()
{
    transportSource.start();
}

void PlayerAudio::stop()
{
    transportSource.stop();
    transportSource.setPosition(0.0);
}

void PlayerAudio::restart()
{
    transportSource.setPosition(0.0);
    transportSource.start();
}

void PlayerAudio::setGain(float gain)
{
    currentVolume = gain;
    transportSource.setGain(gain);
}

void PlayerAudio::toggleMute()
{
    if (isMuted)
    {
        // Unmute
        setGain((float)previousVolume);
        isMuted = false;
    }
    else
    {
        // Mute
        previousVolume = currentVolume;
        setGain(0.0f);
        isMuted = true;
    }
}