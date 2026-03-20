// src/core/AudioBuffer.cpp
// ========================
//
// AudioBuffer
// -----------
//
// Runtime implementation for the AudioBuffer core container.
//
// Architecture role
// -----------------
// Core layer.
//
// This module contains the concrete implementation of the AudioBuffer
// abstraction used across the VoiceEngine system.
//
// It provides the runtime behavior associated with storing, accessing,
// and managing audio sample data and its metadata, while preserving the
// lightweight and reusable nature of the buffer abstraction.
//
// Typical usage
// -------------
// Audio input / synthesis / processing
//        ↓
// AudioBuffer
//        ↓
// downstream consumers (preprocessor, STT, TTS, playback)
//
// This module is responsible ONLY for:
// - implementing the storage and access behavior of AudioBuffer
// - managing audio sample container state and metadata at runtime
// - providing stable buffer semantics for other modules
//
// Non-responsibilities
// --------------------
// This module MUST NOT:
// - capture audio from devices
// - perform preprocessing operations
// - perform STT or TTS logic
// - depend on external providers or backends
//
// Design notes
// ------------
// - Keep implementation lightweight and predictable.
// - Avoid turning AudioBuffer into a processing or orchestration module.
// - Preserve clear separation between data representation and transformation.
// - This implementation should remain a neutral foundation for the whole system.