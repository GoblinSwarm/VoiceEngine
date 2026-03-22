#pragma once

// include/voice_engine/audio/IAudioOutput.h
// =========================================
//
// IAudioOutput
// ------------
//
// Contract for audio output targets used by VoiceEngine.
//
// Architecture role
// -----------------
// Audio layer.
//
// This module defines the abstract contract that any audio output target
// must satisfy in order to receive and play audio generated or handled by the system.
//
// Typical implementations may include:
// - speaker output devices
// - file-based audio writers
// - test or mock playback sinks
//
// This module is responsible ONLY for:
// - defining the output-side audio playback contract
// - abstracting the destination of outgoing audio data
// - allowing higher-level modules to depend on playback capability instead of concrete backends
//
// Non-responsibilities
// --------------------
// This module MUST NOT:
// - implement backend-specific playback logic
// - perform audio preprocessing
// - perform TTS synthesis
// - decide what audio should be played
//
// Design notes
// ------------
// - This interface should remain minimal and stable.
// - Prefer methods that clearly express playback lifecycle and audio delivery.
// - Keep this contract backend-agnostic so different output implementations can be swapped freely.
// - Higher-level modules should depend on this interface, not concrete playback devices.
//

#include "voice_engine/core/AudioBuffer.h"
#include "voice_engine/core/ErrorTypes.h"
#include "voice_engine/core/VoiceConfig.h"

namespace voice_engine::audio
{

class IAudioOutput
{
public:
    virtual ~IAudioOutput() = default;

    virtual bool initialize(const core::VoiceConfig& config) = 0;

    [[nodiscard]] virtual bool isInitialized() const noexcept = 0;

    virtual bool startPlayback() = 0;

    virtual void stopPlayback() = 0;

    [[nodiscard]] virtual bool isPlaying() const noexcept = 0;

    virtual bool play(const core::AudioBuffer& buffer) = 0;

    virtual void shutdown() = 0;

    [[nodiscard]] virtual core::Error lastError() const = 0;
};

} // namespace voice_engine::audio