#pragma once

// include/voice_engine/audio/IAudioFileInput.h
// ============================================
//
// IAudioFileInput
// ---------------
//
// Contract for file-based audio input sources used by VoiceEngine.
//
// Architecture role
// -----------------
// Audio layer.
//
// This module defines the abstract contract that any file-based audio
// source must satisfy in order to load audio data from disk into the system.
//
// Typical implementations may include:
// - WAV file readers
// - future MP3/FLAC decoders
// - mock or test file-based audio sources
//
// This module is responsible ONLY for:
// - defining the contract for loading audio from files
// - abstracting file-based audio ingestion
// - allowing higher-level modules to depend on file loading capability
//   instead of concrete file parsers
//
// Non-responsibilities
// --------------------
// This module MUST NOT:
// - implement format-specific parsing logic
// - perform audio preprocessing
// - perform STT inference
// - represent live capture devices or recording sessions
//
// Design notes
// ------------
// - This interface should remain minimal and format-agnostic.
// - File loading should be modeled separately from live capture.
// - Implementations may reject unsupported formats explicitly.
// - Higher-level modules should depend on this abstraction, not on
//   concrete file readers.
//

#include <string>

#include "voice_engine/core/AudioBuffer.h"
#include "voice_engine/core/ErrorTypes.h"

namespace voice_engine::audio
{

class IAudioFileInput
{
public:
    virtual ~IAudioFileInput() = default;

    [[nodiscard]] virtual core::AudioBuffer loadFromFile(const std::string& filePath) = 0;

    [[nodiscard]] virtual core::Error lastError() const = 0;
};

} // namespace voice_engine::audio