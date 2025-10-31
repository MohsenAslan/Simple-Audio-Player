#pragma once
#include "JuceHeader.h"

class PlaylistComponent : public juce::Component,
    public juce::TableListBoxModel
{
public:
    PlaylistComponent();
    ~PlaylistComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    int getNumRows() override;
    void paintRowBackground(juce::Graphics&, int rowNumber, int width, int height, bool rowIsSelected) override;
    void paintCell(juce::Graphics&, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;

    void addFile(const juce::File& audioFile);
    juce::File getFile(int index) const;
    int getSelectedRow() const;

    // Theme API — PlayerGUI calls this so playlist matches the same colors
    void setTheme(const juce::Colour& deepViolet, const juce::Colour& accentYellow);

private:
    juce::TableListBox tableComponent;
    juce::Array<juce::File> playlistFiles;

    // theme defaults (will be overridden by setTheme)
    juce::Colour themeDeepViolet{ juce::Colour::fromRGB(100, 0, 160) };
    juce::Colour themeAccentYellow{ juce::Colour::fromRGB(255, 215, 0) };
};