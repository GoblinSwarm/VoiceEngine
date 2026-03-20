// include/voice_engine/core/AudioTypes.h
// ======================================
//
// AudioTypes
// ----------
//
// Shared audio domain types used across VoiceEngine.
//
// Architecture role
// -----------------
// Core layer.
//
// This header defines the foundational audio-related types that are
// shared by multiple layers of the system, including audio input/output,
// preprocessing, STT, and TTS modules.
//
// This module is responsible ONLY for:
// - defining common audio scalar and metadata types
// - centralizing basic audio-related aliases or structs
// - providing a shared vocabulary for audio buffers and formats
//
// Non-responsibilities
// --------------------
// This module MUST NOT:
// - implement audio capture or playback
// - perform audio preprocessing
// - contain STT or TTS logic
// - depend on concrete providers or backends
//
// Design notes
// ------------
// - Keep this file lightweight and dependency-minimal.
// - Prefer stable, reusable domain types over provider-specific details.
// - This file should act as a neutral foundation for the rest of the system.