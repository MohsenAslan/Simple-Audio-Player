#include "PlayerAudio.h"
#include <taglib/fileref.h>           //  لقراءة الميتاداتا
#include <taglib/tag.h>               //  للوصول إلى البيانات (title, artist, album)
#include <taglib/audioproperties.h>   //  لقراءة خصائص الصوت (المدة، إلخ)


// CHANGED: global ApplicationProperties so JUCE manages single settings file for the app.
// We use per-player key prefixes to avoid collisions when multiple PlayerAudio instances exist.
static juce::ApplicationProperties appProperties;

PlayerAudio::PlayerAudio()
{
    formatManager.registerBasicFormats();

    // ✅ إعداد التخزين مرة واحدة فقط
    static bool propsInitialized = false;
    if (!propsInitialized)
    {
        juce::PropertiesFile::Options options;
        options.applicationName = "SimpleAudioPlayer";
        options.filenameSuffix = "settings";
        options.folderName = "SimpleAudioPlayer";
        options.osxLibrarySubFolder = "Application Support";
        options.storageFormat = juce::PropertiesFile::storeAsXML;

        appProperties.setStorageParameters(options);
        propsInitialized = true;
    }

    // ✅ اجعل المفتاح ثابتًا، مش بيعتمد على pointer
    settingsKeyPrefix = "player_main_";

    if (auto* userSettings = appProperties.getUserSettings())
        DBG("PlayerAudio constructed. Settings file: " << userSettings->getFile().getFullPathName()
            << "  keyPrefix=" << settingsKeyPrefix);

    loadLastSession();
}

PlayerAudio::~PlayerAudio()
{
    // Save session on destruction
    saveLastSession();

    // Close files to avoid leak warnings (safe to call multiple times)
    appProperties.closeFiles();

    transportSource.releaseResources();
}

void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    resamplingAudioSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    resamplingAudioSource.getNextAudioBlock(bufferToFill);

    if (!isLooping && transportSource.getCurrentPosition() >= transportSource.getLengthInSeconds() - 0.05)
    {
        transportSource.stop();
        transportSource.setPosition(0.0);
    }

    
    loopBetweenTwoPoints();
}

void PlayerAudio::releaseResources()
{
    resamplingAudioSource.releaseResources();
}

// ✅ تحميل ملف صوت وقراءة الميتاداتا باستخدام TagLib
// =====================================================
void PlayerAudio::loadFile(const juce::File& file)
{
    if (auto* reader = formatManager.createReaderFor(file))
    {
        transportSource.stop();
        transportSource.setSource(nullptr);
        readerSource.reset(new juce::AudioFormatReaderSource(reader, true));
        transportSource.setSource(readerSource.get(), 0, nullptr, reader->sampleRate);

        durationInSeconds = static_cast<double>(reader->lengthInSamples) / reader->sampleRate;
        lastLoadedFile = file;

        // 🔹 قراءة الميتاداتا من TagLib بأمان
        TagLib::FileRef f(file.getFullPathName().toRawUTF8());
        if (!f.isNull() && f.tag())
        {
            TagLib::Tag* tag = f.tag();
            TagLib::AudioProperties* props = f.audioProperties();

            title = juce::String::fromUTF8(tag->title().toCString(true));
            artist = juce::String::fromUTF8(tag->artist().toCString(true));
            album = juce::String::fromUTF8(tag->album().toCString(true));

            if (title.isEmpty())  title = file.getFileNameWithoutExtension();
            if (artist.isEmpty()) artist = "Unknown Artist";
            if (album.isEmpty())  album = "Unknown Album";

            if (props)
                durationInSeconds = props->length();
        }
        else
        {
            title = file.getFileNameWithoutExtension();
            artist = "Unknown Artist";
            album = "Unknown Album";
        }

        play();
    }
    else
    {
        title = "Invalid File";
        artist = "";
        album = "";
        durationInSeconds = 0.0;
    }
}
void PlayerAudio::play() { transportSource.start(); }
void PlayerAudio::stop() { transportSource.stop(); transportSource.setPosition(0.0); }
void PlayerAudio::restart() { transportSource.setPosition(0.0); transportSource.start(); }
void PlayerAudio::pause() { transportSource.stop(); }
void PlayerAudio::goToStart() { transportSource.setPosition(0.0); }

bool PlayerAudio::isFileLoaded() const { return transportSource.getLengthInSeconds() > 0; }

void PlayerAudio::goToEnd()
{
    double length = transportSource.getLengthInSeconds();
    if (length > 0.1)
        transportSource.setPosition(length - 0.1);
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
        setGain((float)previousVolume);
        isMuted = false;
    }
    else
    {
        previousVolume = currentVolume;
        setGain(0.0f);
        isMuted = true;
    }
}

void PlayerAudio::toggleLoop()
{
    if (!readerSource) return;
    isLooping = !isLooping;

    readerSource->setLooping(true);
}

double PlayerAudio::getTotalLength() { return transportSource.getTotalLength(); }

void PlayerAudio::setPosition(double newPositionInSecond)
{
    transportSource.setPosition(newPositionInSecond);
}

double PlayerAudio::getPosition() const { return transportSource.getCurrentPosition(); }
double PlayerAudio::getLengthInSecond() const { return transportSource.getLengthInSeconds(); }

void PlayerAudio::setPointA(double newPositionInSecond) { pointA = newPositionInSecond; }
void PlayerAudio::setPointB(double newPositionInSecond) { pointB = newPositionInSecond; }

void PlayerAudio::toggleLoopAB() { loopABEnabled = !loopABEnabled; }

void PlayerAudio::loopBetweenTwoPoints()
{
    if (loopABEnabled && transportSource.isPlaying() && pointB > pointA && (pointB - pointA) > 0.1)
    {
        double currentPosition = transportSource.getCurrentPosition();
        if (currentPosition >= pointB)
        {
            transportSource.setPosition(pointA);
        }
    }
}

void PlayerAudio::setBookmark(double pos) {
    bookmarks.push_back(pos);
}


void PlayerAudio::goToBookmark()
{
    if (bookmarks.empty()) return;

    juce::PopupMenu menu;
    for (int i = 0; i < bookmarks.size(); ++i) {
        double totalSeconds = bookmarks[i];
        int hours = static_cast<int>(totalSeconds / 3600);
        int minutes = static_cast<int>((totalSeconds - hours * 3600) / 60);
        int seconds = static_cast<int>(totalSeconds) % 60;
        juce::String timeFormatted = juce::String::formatted("%02d:%02d:%02d", hours, minutes, seconds);

        menu.addItem(i + 1, "Bookmark " + juce::String(i + 1) + " (" + timeFormatted + ")");
    }

    menu.showMenuAsync(juce::PopupMenu::Options(),
        [this](int result)
        {
            if (result > 0 && result <= bookmarks.size()) {
                setPosition(bookmarks[result - 1]);
            }
        });
}

void PlayerAudio::setResamplingRatio(double spede)
{
    resamplingAudioSource.setResamplingRatio(spede);
}

// =====================================================
// CHANGED: Save & Load Last Session using ApplicationProperties
// keys are stored as: settingsKeyPrefix + "lastFile" / "lastPosition"
// =====================================================

void PlayerAudio::saveLastSession()
{
    if (auto* settings = appProperties.getUserSettings())
    {
        // 🔹 احفظ باستخدام key ثابت
        if (lastLoadedFile.existsAsFile())
            settings->setValue(settingsKeyPrefix + "lastFile", lastLoadedFile.getFullPathName());

        settings->setValue(settingsKeyPrefix + "lastPosition", transportSource.getCurrentPosition());
        settings->saveIfNeeded();

        DBG("💾 Session saved: " << lastLoadedFile.getFileName()
            << " @ " << transportSource.getCurrentPosition());
    }
}

void PlayerAudio::loadLastSession()
{
    juce::PropertiesFile::Options options;
    options.applicationName = "SimpleAudioPlayer";
    options.filenameSuffix = "settings";
    options.osxLibrarySubFolder = "Application Support";
    options.folderName = "SimpleAudioPlayer";

    juce::ApplicationProperties appProperties;
    appProperties.setStorageParameters(options);

    auto* props = appProperties.getUserSettings();

    juce::String lastFilePath = props->getValue(settingsKeyPrefix + "lastFile");
    double lastPosition = props->getDoubleValue(settingsKeyPrefix + "lastPosition", 0.0);

    if (lastFilePath.isNotEmpty())
    {
        juce::File file(lastFilePath);
        if (file.existsAsFile())
        {
            loadFile(file);

            // ✋ تأكد إن التشغيل متوقف
            transportSource.stop();

            // ✅ أعد الضبط للموضع الأخير بدون تشغيل
            transportSource.setPosition(lastPosition);

            // ⚙ جهّز الصوت لكن ما تشغلوش
            transportSource.prepareToPlay(512, 44100);

            // 🔇 mute & loop states reset for safety
            isMuted = false;
            transportSource.setGain((float)currentVolume);
            isLooping = false;
        }
    }
}
void PlayerAudio::skipForward(double seconds)
{
    double current = transportSource.getCurrentPosition();
    double length = transportSource.getLengthInSeconds();
    double newPos = current + seconds;

    if (newPos < length)
        transportSource.setPosition(newPos);
    else
        transportSource.setPosition(length);
}

void PlayerAudio::skipBackward(double seconds)
{
    double current = transportSource.getCurrentPosition();
    double newPos = current - seconds;

    if (newPos > 0)
        transportSource.setPosition(newPos);
    else
        transportSource.setPosition(0.0);
}





// =====================================================

juce::String PlayerAudio::getTitle() const { return title; }
juce::String PlayerAudio::getArtist() const { return artist; }


juce::String PlayerAudio::getDurationString() const
{
    int totalSeconds = static_cast<int>(durationInSeconds);
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;
    return juce::String::formatted("%d:%02d", minutes, seconds);
}


// bonus 2
void PlayerAudio::togglePlayPause() {
    if (transportSource.isPlaying()) {
        transportSource.stop();
    }
    else {
        transportSource.start();
    }
}