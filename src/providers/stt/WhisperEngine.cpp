// src/providers/stt/WhisperEngine.cpp
// ===================================
//
// WhisperEngine
// -------------
//
// Runtime implementation for the Whisper-based STT provider.
//
// Architecture role
// -----------------
// Provider layer (STT).
//
// This module contains the concrete implementation of the ISTTEngine
// contract using whisper.cpp as the underlying speech-to-text backend.
//
// It acts as the runtime integration boundary between VoiceEngine and
// the external Whisper provider, adapting internal audio-domain data
// into Whisper-compatible input and translating provider output back
// into VoiceEngine STT domain types.
//
// Typical flow
// ------------
// AudioBuffer / prepared audio
//        ↓
// WhisperEngine
//        ↓
// whisper.cpp runtime invocation
//        ↓
// raw Whisper output
//        ↓
// TranscriptionResult
//
// This module is responsible ONLY for:
// - implementing the ISTTEngine contract using whisper.cpp
// - adapting VoiceEngine audio buffers to Whisper-compatible input
// - invoking the provider runtime and collecting inference results
// - mapping Whisper-specific output into stable VoiceEngine STT domain types
//
// Non-responsibilities
// --------------------
// This module MUST NOT:
// - capture audio from input devices
// - coordinate the full STT pipeline
// - interpret recognized text semantically
// - leak whisper.cpp-specific types outside the provider boundary
//
// Design notes
// ------------
// - Keep all whisper.cpp-specific logic isolated in this module.
// - This file should behave as an adapter, not as a high-level recognizer.
// - Provider output must be normalized into VoiceEngine domain structures.
// - Avoid spreading provider assumptions into upper layers.