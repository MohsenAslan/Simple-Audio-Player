
#pragma once
#include"JuceHeader.h"
class PlaylistComponent :public juce::Component,
	public juce::TableListBoxModel
{
public:
	PlaylistComponent();
	~PlaylistComponent() override;
	void paint(juce::Graphics&)override;
	void resized()override;

	int getNumRows()override;
	void paintRowBackground(juce::Graphics&, int rowNumber, int width, int height, bool rowIsSelected)override;
	void paintCell(juce::Graphics&, int rowNumber, int columnId, int width, int height, bool rowIsSelected)override;

	void addFile(const juce::File& audioFile);
	juce::File getFile(int index)const;
	int getSelectedRow()const;

private:
	juce::TableListBox tableComponent;
	juce::Array<juce::File>playlistFiles;
};



