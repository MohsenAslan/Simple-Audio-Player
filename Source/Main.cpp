#include <JuceHeader.h>
#include "MainComponent.h"

// Our application class
class SimpleAudioPlayer : public juce::JUCEApplication
{
public:
    const juce::String getApplicationName() override { return "Simple Audio Player"; }
    const juce::String getApplicationVersion() override { return "1.0"; }

    void initialise(const juce::String&) override
    {
         // Create and show the main window
        mainWindow = std::make_unique<MainWindow>(getApplicationName());
    }

    void shutdown() override
    {
        // Close the main window
        mainWindow = nullptr;

        // Save settings to disk
        juce::PropertiesFile::Options options;
        options.applicationName = "SimpleAudioPlayer";
        options.filenameSuffix = "settings";
        options.osxLibrarySubFolder = "Application Support";
        options.folderName = "SimpleAudioPlayer";

        juce::ApplicationProperties props;
        props.setStorageParameters(options);
        props.saveIfNeeded();
    }

private:

    // The main window of the app
    class MainWindow : public juce::DocumentWindow
    {
    public:
        MainWindow(juce::String name)
            : DocumentWindow(name,
                juce::Colours::lightgrey,
                DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar(true);

            setContentOwned(new MainComponent(), true); // MainComponent = our UI + logic

            // Make the window full-screen:
            setFullScreen(true);

            // If you still want it resizable while full-screen is off, you can call:
            // setResizable(true, false);

            setVisible(true);
        }

        void closeButtonPressed() override
        {
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
        }
    };

    std::unique_ptr<MainWindow> mainWindow;
};

// This macro starts the app
START_JUCE_APPLICATION(SimpleAudioPlayer)
