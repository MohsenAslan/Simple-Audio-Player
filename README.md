# 🎵 M.M.E Audio Player  

![App Screenshot](https://github.com/MohsenAslan/Simple-Audio-Player/blob/main/Screenshot%202025-11-03%20214720.png)

A modern multi-track **Audio Player** built using **JUCE Framework**.  
This application allows users to load, play, pause, and mix multiple tracks, visualize audio waveforms, control playback speed, manage playlists, and save/load sessions.  

---

## 🧠 Overview

The **M.M.E Audio Player** provides a rich feature set for both casual and advanced users.  
It uses object-oriented programming principles to organize its structure into **independent, reusable, and maintainable components** such as `PlayerAudio`, `PlayerGUI`, and `MainComponent`.

---

## ✨ Main Features

### 🎧 Core Player Controls
- **Play / Pause / Stop / Restart** — Basic playback management.  
- **Load File** — Choose any audio file from your system.  
- **Go To Start / Go To End** — Jump instantly within the track.  
- **Mute** — Instantly mute/unmute audio output.  
- **Volume Slider** — Adjust the playback volume in real time.

### 🔁 Loop & Navigation
- **Loop Mode** — Toggle looping for a selected track.  
- **A-B Loop (StartLoop / EndLoop)** — Define custom looping sections.  
- **Mini Loop** — Short looping section for focused playback.  
- **Fast Forward / Rewind (±10s)** — Quickly move through the track.  

### 🧩 Extra Features
- **Waveform Display** — Real-time waveform visualization using `AudioThumbnail`.  
- **Bookmarks** — Save important positions inside tracks for easy access.  
- **Keyboard Shortcuts** — Quickly control playback without using the mouse.  
- **Playlist System** — Load and manage a list of tracks with “Play Selected”.  
- **Mixer** — Two separate players (A & B) play simultaneously and mix their outputs.  
- **Speed Slider** — Control playback rate (slow down or speed up).  
- **Session Save & Load** — Automatically saves the last opened tracks and their positions.

---

## 🧩 Components & Responsibilities

| Component | Responsibility |
|------------|----------------|
| **PlayerAudio** | Handles audio playback logic, file loading, position tracking, saving/loading sessions, and connecting to JUCE’s audio engine. |
| **PlayerGUI** | Manages all user interface elements: buttons, sliders, waveform, and the violet theme. Communicates user actions to `PlayerAudio`. |
| **MainComponent** | Combines two `PlayerGUI` + `PlayerAudio` pairs and handles the audio mixing of both players. |

---

## 🧠 Object-Oriented Design

The program is built using **OOP principles**:

### 1. Encapsulation
Each class hides its internal logic (e.g., audio state, slider values) and provides clear interfaces (`prepareToPlay`, `getNextAudioBlock`, etc.).

### 2. Abstraction
Users interact only with high-level GUI actions — like pressing Play or adjusting a slider — without worrying about the low-level audio logic handled inside `PlayerAudio`.

### 3. Inheritance
`PlayerGUI` inherits from `juce::Component`, `juce::Button::Listener`, and `juce::Slider::Listener`, allowing it to directly manage UI events.

### 4. Polymorphism
JUCE’s event system (e.g., `buttonClicked`, `sliderValueChanged`) uses polymorphism to call the correct function dynamically depending on which component triggered the event.

### 5. Composition
`MainComponent` **composes** two `PlayerAudio` and `PlayerGUI` objects to form the full dual-player mixer interface.

---

## 🧮 Mixer Logic

In `MainComponent.cpp`:

```cpp
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
```

🔹 `player1` and `player2` each produce audio data.  
🔹 Their buffers are mixed together using `addFrom()`.  
🔹 `applyGain(0.5f)` prevents clipping by lowering combined volume.  

This allows **two tracks to play simultaneously** — just like a real mixer!

---

## 🧰 Technologies Used
- **JUCE Framework** (C++)
- **Object-Oriented Programming**
- **Audio Programming**
- **GUI Design**

---

## 👨‍💻 Team Members

| Name | Contributions |
|------|----------------|
| **Eman Saad** | Play/Pause, GoToEnd, GoToStart, Metadata, Playlist, ±10s, Multiple Player |
| **Mariem Badawy** | Loop, Progress Bar, A-B Loop, Bookmarks, Keyboard Shortcuts |
| **Mohsen Aslan** | Mute, Speed Slider, Waveform Visualization, Mixer, Save & Load Sessions |

---

## 🚀 How to Use
1. **Open the app** — Two audio players (top & bottom) will appear.  
2. **Click "Load File"** to choose audio tracks for each player.  
3. Use the **Play**, **Pause**, **Loop**, and **Speed** controls freely.  
4. Mix both tracks together for creative effects!  
5. When you close the app, the session automatically saves.  
6. On next launch, your previous tracks and positions will reload automatically.  

---

## 📸 Screenshot
![M.M.E Audio Player Interface](ab8fa789-0c9f-44f1-aeaf-445ab40fc2a8.png)

---

## 🏁 License
This project was created for educational purposes under the **Faculty of Computers and Artificial Intelligence, Cairo University**, as part of **CS251 – Software Engineering I** coursework.

---

**Developed with ❤️ by M.M.E Team**
