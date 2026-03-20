// src/providers/tts/PiperEngine.cpp
// =================================
//
// PiperEngine
// -----------
//
// Runtime implementation for the Piper-based TTS provider.
//
// Architecture role
// -----------------
// Provider layer (TTS).
//
// This module contains the concrete implementation of the ITTSEngine
// contract using Piper as the underlying text-to-speech backend.
//
// It acts as the runtime integration boundary between VoiceEngine and
// the external Piper provider, adapting internal text-domain requests
// into Piper-compatible synthesis input and translating provider output
// back into VoiceEngine audio-domain types.
//
// Typical flow
// ------------
// text input
//        ↓
// PiperEngine
//        ↓
// Piper runtime invocation
//        ↓
// raw Piper output
//        ↓
// SynthesisResult / AudioBuffer
//
// This module is responsible ONLY for:
// - implementing the ITTSEngine contract using Piper
// - adapting VoiceEngine synthesis requests to Piper-compatible input
// - invoking the provider runtime and collecting synthesis output
// - mapping Piper-specific output into stable VoiceEngine TTS domain types
//
// Non-responsibilities
// --------------------
// This module MUST NOT:
// - decide what text should be spoken
// - coordinate the full TTS pipeline
// - play audio through output devices
// - leak Piper-specific types outside the provider boundary
//
// Design notes
// ------------
// - Keep all Piper-specific logic isolated in this module.
// - This file should behave as an adapter, not as a high-level synthesizer.
// - Provider output must be normalized into VoiceEngine domain structures.
// - Avoid spreading provider assumptions into upper layers.