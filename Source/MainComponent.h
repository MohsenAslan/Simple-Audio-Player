#pragma once
#include <JuceHeader.h>
#include "PlayerGUI.h"
#include "PlayerAudio.h"

class MainComponent : public juce::AudioAppComponent
{
public:
    MainComponent();
    ~MainComponent() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    PlayerAudio player1;
    PlayerGUI gui1{ player1 };


    PlayerAudio player2;
    PlayerGUI gui2{ player2 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};