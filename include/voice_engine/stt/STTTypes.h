// include/voice_engine/stt/STTTypes.h
// ===================================
//
// STTTypes
// --------
//
// Shared speech-to-text domain types used across the STT pipeline.
//
// Architecture role
// -----------------
// STT layer.
//
// This module defines the core data structures used to represent
// speech-to-text results and related metadata inside VoiceEngine.
//
// Typical examples may include:
// - transcription results
// - confidence values
// - segment metadata
// - recognition status information
//
// This module is responsible ONLY for:
// - defining shared STT-related domain types
// - providing a stable representation of transcription results
// - separating STT data structures from engine implementation details
//
// Non-responsibilities
// --------------------
// This module MUST NOT:
// - implement transcription logic
// - capture or preprocess audio
// - depend on a concrete STT backend
// - decide how recognized text should be interpreted
//
// Design notes
// ------------
// - Keep these types provider-agnostic whenever possible.
// - Prefer domain-oriented structures over backend-specific payloads.
// - This file should model STT output clearly without leaking engine internals.
// - Favor stable result types that can be used by recognizers, orchestrators, and tests.