#include "PlaylistComponent.h"

PlaylistComponent::PlaylistComponent()
{
    addAndMakeVisible(tableComponent);
    tableComponent.setModel(this);
    tableComponent.getHeader().addColumn("track", 1, 400);

    // Apply default theme (PlayerGUI will call setTheme(...) to override)
    setTheme(themeDeepViolet, themeAccentYellow);
}

PlaylistComponent::~PlaylistComponent() {}

void PlaylistComponent::setTheme(const juce::Colour& deepViolet, const juce::Colour& accentYellow)
{
    themeDeepViolet   = deepViolet;
    themeAccentYellow = accentYellow;

    // make the list background match theme
    tableComponent.setColour(juce::ListBox::backgroundColourId, themeDeepViolet.darker(0.45f));

    tableComponent.updateContent();
    repaint();
}

void PlaylistComponent::paint(juce::Graphics& g)
{
    juce::Colour top    = themeDeepViolet.darker(0.45f);
    juce::Colour bottom = themeDeepViolet.darker(0.15f);
    juce::ColourGradient grad(top, 0, 0, bottom, 0, (float)getHeight(), false);
    g.setGradientFill(grad);
    g.fillRect(getLocalBounds());
}

void PlaylistComponent::resized()
{
    tableComponent.setBounds(getLocalBounds());
}

int PlaylistComponent::getNumRows()
{
    return playlistFiles.size();
}

void PlaylistComponent::paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected)
{
    if (rowIsSelected)
        g.fillRect(0, 0, width, height); // clipped to row
    else
    {
        auto rowShade = (rowNumber & 1) ? themeDeepViolet.darker(0.33f) : themeDeepViolet.darker(0.38f);
        g.fillRect(0, 0, width, height);
        g.setColour(rowShade);
        g.fillRect(0, 0, width, height);
    }
}

void PlaylistComponent::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{
    if (rowNumber < playlistFiles.size())
    {
        g.setColour(rowIsSelected ? juce::Colours::black : themeAccentYellow);
        g.setFont(14.0f);
        g.drawText(playlistFiles[rowNumber].getFileName(),
                   4, 0, width - 8, height, juce::Justification::centredLeft);
    }
}

void PlaylistComponent::addFile(const juce::File& audioFile)
{
    playlistFiles.add(audioFile);
    tableComponent.updateContent();
}

juce::File PlaylistComponent::getFile(int index) const
{
    if (index >= 0 && index < playlistFiles.size())
        return playlistFiles[index];
    return juce::File();
}

int PlaylistComponent::getSelectedRow() const
{
    return tableComponent.getSelectedRow();
}