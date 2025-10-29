#include <JuceHeader.h>
#include "MainComponent.h"
MainComponent::MainComponent()
{
    //player لكل track
    addAndMakeVisible(gui1);
    addAndMakeVisible(gui2);

    setAudioChannels(0, 2); // مخرج صوت ستيريو
    setSize(1500, 1200);
}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    player1.prepareToPlay(samplesPerBlockExpected, sampleRate);
    player2.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    // بافر مؤقت لتراك 2
    juce::AudioBuffer<float> tempBuffer;
    tempBuffer.setSize(bufferToFill.buffer->getNumChannels(), bufferToFill.numSamples);
    juce::AudioSourceChannelInfo tempInfo(&tempBuffer, 0, bufferToFill.numSamples);

    // تراك 1
    player1.getNextAudioBlock(bufferToFill);

    // تراك 2 في البافر المؤقت
    player2.getNextAudioBlock(tempInfo);

    // دمج الصوتين
    for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
    {
        bufferToFill.buffer->addFrom(
            channel,
            bufferToFill.startSample,
            tempBuffer,
            channel,
            0,
            bufferToFill.numSamples
        );
    }

    // تقليل الصوت العام لتفادي التشويش
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
