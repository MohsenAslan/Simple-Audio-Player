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

    if (!isLooping && transportSource.getCurrentPosition() >= transportSource.getLengthInSeconds() - 0.05)
    {
        transportSource.stop();
        transportSource.setPosition(0.0);
    }

}

void PlayerAudio::releaseResources()
{
    transportSource.releaseResources();
}

void PlayerAudio::loadFile(const juce::File& file)
{
    if (auto* reader = formatManager.createReaderFor(file))
    {
       double sampleRate = reader->sampleRate;
        transportSource.stop();
        transportSource.setSource(nullptr);
        readerSource.reset(new juce::AudioFormatReaderSource(reader, true));
        transportSource.setSource(readerSource.get(), 0, nullptr, sampleRate);
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

void PlayerAudio::pause()
{
    transportSource.stop();
}

void PlayerAudio::goToStart()
{
    transportSource.setPosition(0.0);
}

bool PlayerAudio::isFileLoaded() const {
    return transportSource.getLengthInSeconds() > 0;
}

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
    if (!readerSource)
        return;

    isLooping = !isLooping;


    readerSource->setLooping(true);

}

double PlayerAudio::getTotalLength(){
    return transportSource.getTotalLength();
}

void PlayerAudio::setPosition(double newPositionInSecond) {
    transportSource.setPosition(newPositionInSecond);

    
}
double PlayerAudio::getPosition() const{
    return transportSource.getCurrentPosition() ;
}
double PlayerAudio::getLengthInSecond() const {
   
    return transportSource.getLengthInSeconds();
}

void PlayerAudio::setPointA(double newPositionInSecond) {

    pointA = newPositionInSecond;
}

void PlayerAudio::setPointB(double newPositionInSecond) {

    pointB = newPositionInSecond;
}

void PlayerAudio::toggleLoopAB() {

    loopABEnabled = !loopABEnabled;
}
void PlayerAudio::loopBetweenTwoPoints() {
    if (loopABEnabled &&transportSource.isPlaying()&& pointB> pointA && (pointB - pointA)>0.1) {
         
        double currentPosition = transportSource.getCurrentPosition();

        if (currentPosition >= pointB) {
            transportSource.setPosition(pointA);
        }

    }
}

void PlayerAudio::setBookmark(double newPositionInSecond) {
    BookmarkPosition = newPositionInSecond;
    
}

void PlayerAudio::goToBookmark() {
    if (BookmarkPosition > 0.0) {
        transportSource.setPosition(BookmarkPosition);
    }
    
}








