// include/voice_engine/core/AudioBuffer.h
// =======================================
//
// AudioBuffer
// -----------
//
// Lightweight container for audio sample data and metadata.
//
// Architecture role
// -----------------
// Core layer.
//
// This module provides a reusable abstraction for storing and passing
// audio data across the VoiceEngine system. It is used as the primary
// data carrier between audio input, preprocessing, STT, and TTS modules.
//
// This module is responsible ONLY for:
// - encapsulating raw audio sample data (e.g. float buffers)
// - storing basic audio metadata (sample rate, channels, length)
// - providing safe and simple access to audio contents
//
// Non-responsibilities
// --------------------
// This module MUST NOT:
// - capture audio from devices
// - perform preprocessing (normalize, trim, resample)
// - perform STT or TTS operations
// - depend on any external audio backend or provider
//
// Design notes
// ------------
// - Keep this class simple and efficient (no heavy logic).
// - Prefer std::vector<float> (or equivalent) for sample storage.
// - Avoid embedding processing logic; this is a data container, not a processor.
// - This type is expected to flow through multiple layers, so stability is critical.