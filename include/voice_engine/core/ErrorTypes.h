#pragma once

// include/voice_engine/core/ErrorTypes.h
// ======================================
//
// ErrorTypes
// ----------
//
// Shared error domain types used across VoiceEngine.
//
// Architecture role
// -----------------
// Core layer.
//
// This header defines lightweight, reusable error types that can be
// shared across audio, STT, TTS, and orchestration modules.
//
// This module is responsible ONLY for:
// - defining generic error codes for the VoiceEngine domain
// - providing a simple error object for reporting failures
// - establishing a shared error vocabulary across the system
//
// Non-responsibilities
// --------------------
// This module MUST NOT:
// - implement logging
// - throw exceptions automatically
// - depend on provider-specific backends
// - contain recovery policies or retry logic
//
// Design notes
// ------------
// - Keep this file lightweight and dependency-minimal.
// - Prefer generic engine-level errors over backend-specific details.
// - This file should support both simple return-based flows and future
//   extension toward richer error handling if needed.
//

#include <string>

namespace voice_engine::core
{

enum class ErrorCode
{
    None = 0,

    Unknown,
    InvalidArgument,
    InvalidConfiguration,
    InvalidState,

    NotInitialized,
    AlreadyInitialized,
    NotSupported,

    AudioDeviceUnavailable,
    AudioCaptureFailed,
    AudioPlaybackFailed,
    AudioBufferEmpty,
    AudioFormatInvalid,

    PreprocessingFailed,

    STTInitializationFailed,
    STTInferenceFailed,

    TTSInitializationFailed,
    TTSSynthesisFailed,

    CommandRoutingFailed,

    IOError,
    Timeout
};

struct Error
{
    ErrorCode code = ErrorCode::None;
    std::string message{};

    [[nodiscard]] bool hasError() const noexcept
    {
        return code != ErrorCode::None;
    }

    [[nodiscard]] static Error ok()
    {
        return {};
    }
};

} // namespace voice_engine::core