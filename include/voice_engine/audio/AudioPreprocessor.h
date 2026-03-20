// include/voice_engine/audio/AudioPreprocessor.h
// ==============================================
//
// AudioPreprocessor
// -----------------
//
// Audio transformation utilities used to prepare buffers for downstream processing.
//
// Architecture role
// -----------------
// Audio layer.
//
// This module encapsulates the preprocessing steps applied to audio data
// before it is consumed by other parts of the system, especially STT pipelines.
//
// Typical preprocessing responsibilities may include:
// - normalization
// - silence trimming
// - resampling
// - basic format adaptation
//
// This module is responsible ONLY for:
// - transforming audio buffers into a cleaner or expected format
// - centralizing reusable preprocessing steps
// - preparing audio for downstream consumers without coupling them to preprocessing logic
//
// Non-responsibilities
// --------------------
// This module MUST NOT:
// - capture audio from devices
// - play audio to output devices
// - perform speech-to-text inference
// - perform text-to-speech synthesis
// - decide orchestration or interaction flow
//
// Design notes
// ------------
// - Keep preprocessing logic deterministic and reusable.
// - Avoid coupling this module to a specific STT or TTS provider.
// - Prefer explicit, composable operations over hidden automatic behavior.
// - This module should improve audio readiness, not own the audio lifecycle.