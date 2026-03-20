// include/voice_engine/providers/tts/PiperEngine.h
// =================================================
//
// PiperEngine
// -----------
//
// Concrete TTS provider implementation based on Piper.
//
// Architecture role
// -----------------
// Provider layer (TTS).
//
// This module implements the ITTSEngine interface using the Piper backend.
// It is responsible for translating VoiceEngine text input into Piper-compatible
// synthesis requests, invoking the model, and converting the result into
// VoiceEngine audio-domain types.
//
// Typical flow
// ------------
// text input
//        ↓
// PiperEngine
//        ↓
// Piper synthesis
//        ↓
// SynthesisResult / AudioBuffer
//
// This module is responsible ONLY for:
// - implementing the ITTSEngine contract using Piper
// - adapting VoiceEngine text requests to Piper-compatible synthesis input
// - mapping Piper output into VoiceEngine TTS domain types
//
// Non-responsibilities
// --------------------
// This module MUST NOT:
// - decide what text should be spoken
// - play audio through output devices
// - perform high-level orchestration
// - depend on STT subsystems
//
// Design notes
// ------------
// - This module acts as a boundary between VoiceEngine and an external library.
// - Keep Piper-specific logic isolated here.
// - Avoid leaking Piper-specific structures outside this module.
// - All outputs should be mapped to VoiceEngine domain types (e.g. AudioBuffer or SynthesisResult).