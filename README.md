# 🎙️ VoiceEngine

A modular C++ voice processing system designed for **local-first execution**, focused on clean architecture, extensibility, and real-time interaction pipelines.

---

## 🧠 Overview

VoiceEngine is a layered voice processing framework that connects:

- Audio input/output
- Speech-to-Text (STT)
- Text-to-Speech (TTS)
- Command interpretation

All components are **decoupled, replaceable, and provider-agnostic**, enabling seamless integration of different backends such as Whisper.cpp or Piper.

---

## 🏗️ Architecture

The system is structured into clear layers:

Audio Layer
├── IAudioInput
├── IAudioOutput
└── AudioPreprocessor

STT Layer
├── ISTTEngine
└── SpeechRecognizer

TTS Layer
├── ITTSEngine
└── SpeechSynthesizer

Orchestration Layer
├── VoiceEngine (facade)
└── CommandRouter


### 🔁 Pipeline Flow

Audio Input
↓
AudioPreprocessor
↓
SpeechRecognizer → ISTTEngine
↓
CommandRouter
↓
SpeechSynthesizer → ITTSEngine
↓
Audio Output


---

## ⚙️ Current Status

✅ End-to-end pipeline working (mock mode)  
✅ Clean layered architecture  
✅ Provider-agnostic interfaces (STT / TTS)  
✅ Fully local execution (no external services required)  

⬜ Real STT integration (Whisper.cpp)  
⬜ Real TTS integration (Piper)  
⬜ Real audio input/output  

---

## 🚀 Build Instructions (Windows)

```bash
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Debug

▶️ Run
.\Debug\VoiceEngine.exe

🧪 Example Output
Running VoiceEngine (mock mode)...
[TTS OUTPUT] Hello. How can I help you?
[AUDIO OUTPUT] Playing 4 samples at 22050 Hz, channels=1
Done.

🎯 Design Goals
Modularity first — every subsystem is replaceable
Local-first execution — no dependency on cloud APIs
Clear boundaries — strict separation of concerns
Testability — mock-driven development supported
Extensibility — easy integration of new engines and behaviors

🔮 Roadmap
Integrate Whisper.cpp for STT
Integrate Piper for TTS
Real-time audio capture and playback
Expand CommandRouter into a richer semantic system
Multi-language support

📌 Notes
This project is part of a broader exploration into:

real-time systems
audio processing pipelines
AI-assisted local applications
modular system design in C++

👤 Author
Daniel "Goblin" Perdomo
GitHub: https://github.com/GoblinSwarm