#pragma once

// include/voice_engine/core/AudioTypes.h
// ======================================
//
// AudioTypes
// ----------
//
// Shared audio domain types used across VoiceEngine.
//
// Architecture role
// -----------------
// Core layer.
//
// This header defines the foundational audio-related types that are
// shared by multiple layers of the system, including audio input/output,
// preprocessing, STT, and TTS modules.
//
// This module is responsible ONLY for:
// - defining common audio scalar and metadata types
// - centralizing basic audio-related aliases or structs
// - providing a shared vocabulary for audio buffers and formats
//
// Non-responsibilities
// --------------------
// This module MUST NOT:
// - implement audio capture or playback
// - perform audio preprocessing
// - contain STT or TTS logic
// - depend on concrete providers or backends
//
// Design notes
// ------------
// - Keep this file lightweight and dependency-minimal.
// - Prefer stable, reusable domain types over provider-specific details.
// - This file should act as a neutral foundation for the rest of the system.
//

#include <cstdint>

namespace voice_engine::core
{

// ======================================================
// Scalar types
// ======================================================

// Single audio sample (normalized -1.0f to 1.0f)
using Sample = float;

// Number of samples per second
using SampleRate = std::uint32_t;

// Number of channels (1 = mono, 2 = stereo, etc.)
using ChannelCount = std::uint16_t;

// Frame index (position in time)
using FrameIndex = std::uint64_t;

// Total number of frames
using FrameCount = std::uint64_t;


// ======================================================
// Audio format description
// ======================================================

enum class SampleFormat
{
    Unknown = 0,

    Float32,   // preferred internal format
    Int16,     // common for devices
    Int32
};


// ======================================================
// Audio metadata
// ======================================================

struct AudioFormat
{
    SampleRate sampleRate = 0;
    ChannelCount channels = 0;
    SampleFormat format = SampleFormat::Unknown;
};


// ======================================================
// Utility helpers (lightweight, no logic)
// ======================================================

// Returns true if format is valid
inline bool isValidFormat(const AudioFormat& fmt)
{
    return fmt.sampleRate > 0 &&
           fmt.channels > 0 &&
           fmt.format != SampleFormat::Unknown;
}

} // namespace voice_engine::core