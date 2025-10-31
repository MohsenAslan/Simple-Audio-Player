#include "MainComponent.h"

MainComponent::MainComponent()
{
    addAndMakeVisible(gui1);
    addAndMakeVisible(gui2);

    setAudioChannels(0, 2);
    setSize(1500, 1200);

    // ✅ تحميل الجلسة السابقة
    player1.loadLastSession();
    player2.loadLastSession();
}

MainComponent::~MainComponent()
{
    // ✅ حفظ الجلسة قبل الإغلاق
    player1.saveLastSession();
    player2.saveLastSession();

    shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    player1.prepareToPlay(samplesPerBlockExpected, sampleRate);
    player2.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    juce::AudioBuffer<float> tempBuffer;
    tempBuffer.setSize(bufferToFill.buffer->getNumChannels(), bufferToFill.numSamples);
    juce::AudioSourceChannelInfo tempInfo(&tempBuffer, 0, bufferToFill.numSamples);

    player1.getNextAudioBlock(bufferToFill);
    player2.getNextAudioBlock(tempInfo);

    for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
        bufferToFill.buffer->addFrom(channel, bufferToFill.startSample, tempBuffer, channel, 0, bufferToFill.numSamples);

    bufferToFill.buffer->applyGain(0.5f);
}

void MainComponent::releaseResources()
{
    player1.releaseResources();
    player2.releaseResources();
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void MainComponent::resized()
{
    auto area = getLocalBounds().reduced(10);
    auto top = area.removeFromTop(area.getHeight() / 2);
    gui1.setBounds(top);
    gui2.setBounds(area);
}
