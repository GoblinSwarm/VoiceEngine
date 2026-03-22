// src/stt/SpeechRecognizer.cpp
// ============================
//
// SpeechRecognizer
// ----------------
//
// Runtime implementation for the high-level speech recognition pipeline.
//
// Architecture role
// -----------------
// STT layer.
//
// This module contains the concrete implementation of the VoiceEngine-side
// speech recognition flow. It coordinates the processing path from incoming
// audio buffers to stable transcription results by delegating actual
// recognition work to an ISTTEngine implementation.
//
// It serves as the system-level recognition component that connects prepared
// audio data with provider-backed transcription engines while preserving
// VoiceEngine domain boundaries.
//
// Typical flow
// ------------
// AudioBuffer / prepared audio
//        ↓
// SpeechRecognizer
//        ↓
// ISTTEngine
//        ↓
// provider-backed transcription
//        ↓
// TranscriptionResult
//
// This module is responsible ONLY for:
// - implementing the runtime coordination of the STT pipeline
// - receiving audio intended for transcription
// - delegating transcription to the configured ISTTEngine
// - returning stable VoiceEngine STT domain results
//
// Non-responsibilities
// --------------------
// This module MUST NOT:
// - implement provider-specific STT logic directly
// - capture audio from hardware devices
// - interpret the meaning of recognized text
// - orchestrate full application or interaction flow
//
// Design notes
// ------------
// - Keep this module focused on recognition coordination, not engine internals.
// - Provider-specific details must remain inside concrete ISTTEngine implementations.
// - Return VoiceEngine domain types instead of raw provider payloads.
// - This module should remain a clean boundary between audio preparation and text interpretation.

#include "voice_engine/stt/SpeechRecognizer.h"

namespace voice_engine::stt
{

SpeechRecognizer::SpeechRecognizer(
    audio::AudioPreprocessor& preprocessor,
    ISTTEngine& engine)
    : m_preprocessor(preprocessor),
      m_engine(engine)
{
}

TranscriptionResult SpeechRecognizer::recognize(
    const core::AudioBuffer& input,
    const TranscriptionOptions& options)
{
    core::AudioBuffer preparedBuffer = m_preprocessor.prepareForSTT(input);

    TranscriptionResult result = m_engine.transcribe(preparedBuffer, options);
    m_lastError = m_engine.lastError();

    return result;
}

core::Error SpeechRecognizer::lastError() const
{
    return m_lastError;
}

} // namespace voice_engine::stt