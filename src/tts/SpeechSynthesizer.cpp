// src/tts/SpeechSynthesizer.cpp
// =============================
//
// SpeechSynthesizer
// -----------------
//
// Runtime implementation for the high-level speech synthesis pipeline.
//
// Architecture role
// -----------------
// TTS layer.
//
// This module contains the concrete implementation of the VoiceEngine-side
// speech synthesis flow. It coordinates the path from input text to stable
// audio-domain output by delegating actual synthesis work to an ITTSEngine
// implementation.
//
// It serves as the system-level synthesis component that connects text
// requests with provider-backed TTS engines while preserving VoiceEngine
// domain boundaries.
//
// Typical flow
// ------------
// text input
//        ↓
// SpeechSynthesizer
//        ↓
// ITTSEngine
//        ↓
// provider-backed synthesis
//        ↓
// SynthesisResult / AudioBuffer
//
// This module is responsible ONLY for:
// - implementing the runtime coordination of the TTS pipeline
// - receiving text intended for voice synthesis
// - delegating synthesis to the configured ITTSEngine
// - returning stable VoiceEngine TTS domain results
//
// Non-responsibilities
// --------------------
// This module MUST NOT:
// - implement provider-specific TTS logic directly
// - play audio through hardware devices
// - decide what text should be spoken
// - orchestrate full application or interaction flow
//
// Design notes
// ------------
// - Keep this module focused on synthesis coordination, not engine internals.
// - Provider-specific details must remain inside concrete ITTSEngine implementations.
// - Return VoiceEngine domain types instead of raw provider payloads.
// - Playback concerns should remain outside this module unless explicitly modeled elsewhere.

#include "voice_engine/tts/SpeechSynthesizer.h"

namespace voice_engine::tts
{

SpeechSynthesizer::SpeechSynthesizer(ITTSEngine& engine)
    : m_engine(engine)
{
}

SynthesisResult SpeechSynthesizer::synthesize(const SynthesisRequest& request)
{
    SynthesisResult result = m_engine.synthesize(request);
    m_lastError = m_engine.lastError();

    return result;
}

core::Error SpeechSynthesizer::lastError() const
{
    return m_lastError;
}

} // namespace voice_engine::tts