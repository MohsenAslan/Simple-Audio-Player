
#include "PlaylistComponent.h"

PlaylistComponent::PlaylistComponent() {
	addAndMakeVisible(tableComponent);
	tableComponent.setModel(this);
	tableComponent.getHeader().addColumn("track", 1, 400);
}
PlaylistComponent::~PlaylistComponent() {}
void PlaylistComponent::paint(juce::Graphics& g)
{
	g.fillAll(juce::Colours::darkgrey);

}
void PlaylistComponent::resized() {
	tableComponent.setBounds(getLocalBounds());
}
int PlaylistComponent::getNumRows() {
	return playlistFiles.size();
}
void PlaylistComponent::paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected)

{
	if (rowIsSelected)
		g.fillAll(juce::Colours::lightblue);

}
void PlaylistComponent::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{

	if (rowNumber < playlistFiles.size()) {
		g.setColour(juce::Colours::white);
		g.drawText(playlistFiles[rowNumber].getFileName(),
			2, 0, width - 4, height, juce::Justification::centredLeft);
	}
}
void PlaylistComponent::addFile(const juce::File& audioFile)
{

	playlistFiles.add(audioFile);
	tableComponent.updateContent();
}
juce::File PlaylistComponent::getFile(int index)const {
	if (index >= 0 && index < playlistFiles.size())
		return playlistFiles[index];
	return juce::File();
}
int PlaylistComponent::getSelectedRow()const
{
	return tableComponent.getSelectedRow();
}