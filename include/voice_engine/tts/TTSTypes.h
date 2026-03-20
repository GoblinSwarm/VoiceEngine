// include/voice_engine/tts/TTSTypes.h
// ===================================
//
// TTSTypes
// --------
//
// Shared text-to-speech domain types used across the TTS pipeline.
//
// Architecture role
// -----------------
// TTS layer.
//
// This module defines the core data structures used to represent
// text-to-speech synthesis results and related metadata inside VoiceEngine.
//
// Typical examples may include:
// - synthesis results
// - audio output metadata
// - synthesis status information
//
// This module is responsible ONLY for:
// - defining shared TTS-related domain types
// - providing a stable representation of synthesized audio results
// - separating TTS data structures from engine implementation details
//
// Non-responsibilities
// --------------------
// This module MUST NOT:
// - implement synthesis logic
// - play audio to output devices
// - depend on a concrete TTS backend
// - decide what text should be synthesized
//
// Design notes
// ------------
// - Keep these types provider-agnostic whenever possible.
// - Prefer returning engine-neutral audio representations (e.g. AudioBuffer).
// - Avoid leaking backend-specific structures into the rest of the system.
// - This file should define clear output types usable by synthesizers and orchestrators.