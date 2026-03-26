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
// text-to-speech synthesis requests and results inside VoiceEngine.
//
// Typical examples may include:
// - synthesis requests
// - synthesized audio results
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
// - Allow implementations to expose output-file metadata when synthesis
//   naturally produces a file as an intermediate or final artifact.
// - Avoid leaking backend-specific structures into the rest of the system.
// - This file should define clear input/output types usable by synthesizers
//   and higher-level orchestration layers.
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
    Completed = 0,
    Failed,
    InvalidInput
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

    // Optional target path for generated synthesis output.
    std::string outputFilePath{};
};

// ======================================================
// Synthesis result
// ======================================================

struct SynthesisResult
{
    core::AudioBuffer audio{};

    std::string inputText{};
    std::string outputFilePath{};

    SynthesisStatus status = SynthesisStatus::Failed;

    std::string errorMessage{};
};

} // namespace voice_engine::tts