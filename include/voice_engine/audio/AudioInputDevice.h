#pragma once

// include/voice_engine/audio/AudioInputDevice.h
// ============================================
//
// AudioInputDevice
// ----------------
//
// Low-level audio input device abstraction.
//
// Architecture role
// -----------------
// Audio infrastructure layer.
//
// This module provides a thin wrapper over the underlying
// audio backend (future: PortAudio / OS API).
//
// This module is responsible ONLY for:
// - managing audio input device lifecycle
// - opening and closing input streams
// - capturing raw audio chunks into AudioBuffer
//
// Non-responsibilities
// --------------------
// This module MUST NOT:
// - perform audio preprocessing
// - perform STT
// - decide when or why to capture audio
// - manage high-level capture sessions
//
// Design notes
// ------------
// - This class should wrap a backend (e.g., PortAudio).
// - For now, it can act as a stub/mock until backend is integrated.
// - Keep logic minimal and focused on device interaction.
//

#include "voice_engine/core/AudioBuffer.h"
#include "voice_engine/core/ErrorTypes.h"
#include "voice_engine/core/VoiceConfig.h"

namespace voice_engine::audio
{

class AudioInputDevice
{
public:
    AudioInputDevice();
    ~AudioInputDevice();

    bool initialize(const core::VoiceConfig& config);

    [[nodiscard]] bool isInitialized() const noexcept;

    bool startCapture();

    void stopCapture();

    [[nodiscard]] bool isCapturing() const noexcept;

    [[nodiscard]] core::AudioBuffer captureOnce();

    void shutdown();

    [[nodiscard]] core::Error lastError() const;

private:
    bool m_initialized{false};
    bool m_capturing{false};

    core::VoiceConfig m_config{};
    core::Error m_lastError{};

    // Futuro:
    // - backend handle (e.g., PortAudio stream)
    void* m_stream{nullptr};
};

} // namespace voice_engine::audio