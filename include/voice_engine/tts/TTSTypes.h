#pragma once

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
//

#include <string>

#include "voice_engine/core/AudioBuffer.h"

namespace voice_engine::tts
{

// ======================================================
// Synthesis status
// ======================================================

enum class SynthesisStatus
{
    Idle = 0,
    Processing,
    Completed,
    Failed
};


// ======================================================
// Synthesis request
// ======================================================

struct SynthesisRequest
{
    std::string text{};

    std::string voiceName{};
    float speechRate = 1.0f;
    float volume = 1.0f;
};


// ======================================================
// Synthesis result
// ======================================================

struct SynthesisResult
{
    core::AudioBuffer audio{};

    std::string originalText{};

    SynthesisStatus status = SynthesisStatus::Idle;

    double durationSeconds = 0.0;
};

} // namespace voice_engine::tts