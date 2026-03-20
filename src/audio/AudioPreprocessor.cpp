// src/audio/AudioPreprocessor.cpp
// ===============================
//
// AudioPreprocessor
// -----------------
//
// Runtime implementation for reusable audio preprocessing operations.
//
// Architecture role
// -----------------
// Audio layer.
//
// This module contains the concrete implementation of the preprocessing
// behavior applied to audio buffers before they are consumed by other
// subsystems, especially speech recognition pipelines.
//
// It centralizes reusable audio transformation steps such as normalization,
// silence trimming, resampling, and other low-level preparation tasks needed
// to improve downstream compatibility and consistency.
//
// Typical flow
// ------------
// captured or generated audio
//        ↓
// AudioPreprocessor
//        ↓
// cleaned / adapted audio buffer
//        ↓
// downstream consumer (STT, playback, analysis)
//
// This module is responsible ONLY for:
// - implementing reusable audio preprocessing operations
// - transforming audio buffers into formats expected by downstream modules
// - centralizing low-level audio preparation behavior
//
// Non-responsibilities
// --------------------
// This module MUST NOT:
// - capture audio from devices
// - play audio to output devices
// - perform STT inference
// - perform TTS synthesis
// - orchestrate system behavior
//
// Design notes
// ------------
// - Keep transformations deterministic and explicit.
// - Avoid embedding provider-specific assumptions unless explicitly modeled.
// - Prefer composable operations over hidden, monolithic preprocessing behavior.
// - This module should prepare audio, not own the audio lifecycle.