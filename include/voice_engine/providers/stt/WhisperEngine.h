// include/voice_engine/providers/stt/WhisperEngine.h
// ==================================================
//
// WhisperEngine
// -------------
//
// Concrete STT provider implementation based on whisper.cpp.
//
// Architecture role
// -----------------
// Provider layer (STT).
//
// This module implements the ISTTEngine interface using the whisper.cpp backend.
// It is responsible for translating VoiceEngine audio data into the format expected
// by Whisper, invoking the model, and converting the result into VoiceEngine domain types.
//
// Typical flow
// ------------
// AudioBuffer / processed audio
//        ↓
// WhisperEngine
//        ↓
// whisper.cpp inference
//        ↓
// TranscriptionResult
//
// This module is responsible ONLY for:
// - implementing the ISTTEngine contract using whisper.cpp
// - adapting internal audio buffers to Whisper-compatible input
// - mapping Whisper output into VoiceEngine STT domain types
//
// Non-responsibilities
// --------------------
// This module MUST NOT:
// - capture audio from devices
// - perform high-level pipeline orchestration
// - interpret recognized text
// - depend on TTS or output systems
//
// Design notes
// ------------
// - This module acts as a boundary between VoiceEngine and an external library.
// - Keep Whisper-specific logic isolated here.
// - Avoid leaking whisper.cpp types or structures outside this module.
// - All outputs should be mapped to VoiceEngine domain types (e.g. TranscriptionResult).