#pragma once

// include/voice_engine/audio/IAudioInput.h
// ========================================
//
// IAudioInput
// -----------
//
// Contract for audio input sources used by VoiceEngine.
//
// Architecture role
// -----------------
// Audio layer.
//
// This module defines the abstract contract that any audio input source
// must satisfy in order to provide incoming audio data to the system.
//
// Typical implementations may include:
// - microphone capture devices
// - file-based audio readers
// - mock or test audio sources
//
// This module is responsible ONLY for:
// - defining the input-side audio capture contract
// - abstracting how audio enters the system
// - allowing higher-level modules to depend on capture capability instead of concrete backends
//
// Non-responsibilities
// --------------------
// This module MUST NOT:
// - implement backend-specific capture logic
// - perform audio preprocessing
// - perform STT inference
// - decide orchestration or interaction flow
//
// Design notes
// ------------
// - This interface should remain minimal and stable.
// - Prefer explicit lifecycle methods for device/resource ownership.
// - Keep this contract backend-agnostic so different input implementations can be swapped freely.
// - Higher-level modules should depend on this abstraction, not concrete capture devices.
//

#include "voice_engine/core/AudioBuffer.h"
#include "voice_engine/core/ErrorTypes.h"
#include "voice_engine/core/VoiceConfig.h"

namespace voice_engine::audio
{

class IAudioInput
{
public:
    virtual ~IAudioInput() = default;

    virtual bool initialize(const core::VoiceConfig& config) = 0;

    [[nodiscard]] virtual bool isInitialized() const noexcept = 0;

    virtual bool startCapture() = 0;

    virtual void stopCapture() = 0;

    [[nodiscard]] virtual bool isCapturing() const noexcept = 0;

    [[nodiscard]] virtual core::AudioBuffer captureOnce() = 0;

    virtual void shutdown() = 0;

    [[nodiscard]] virtual core::Error lastError() const = 0;
};

} // namespace voice_engine::audio