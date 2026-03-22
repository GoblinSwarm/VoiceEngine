#pragma once

// include/voice_engine/audio/AudioInputDevice.h
// ============================================
//
// AudioInputDevice
// ----------------
//
// Concrete implementation of IAudioInput for capturing audio from a device.
//
// Architecture role
// -----------------
// Audio layer.
//
// This module provides a concrete implementation of audio input,
// typically backed by a real device (e.g., microphone).
//
// This module is responsible ONLY for:
// - managing audio input device lifecycle
// - capturing raw audio data into AudioBuffer
//
// Non-responsibilities
// --------------------
// This module MUST NOT:
// - perform audio preprocessing
// - perform STT
// - decide when or why to capture audio
//
// Design notes
// ------------
// - This class should wrap a backend (e.g., PortAudio).
// - For now, it can act as a stub/mock until backend is integrated.
// - Keep logic minimal and focused on device interaction.
//

#include "voice_engine/audio/IAudioInput.h"

namespace voice_engine::audio
{

class AudioInputDevice : public IAudioInput
{
public:
    AudioInputDevice();
    ~AudioInputDevice() override;

    bool initialize(const core::VoiceConfig& config) override;

    [[nodiscard]] bool isInitialized() const noexcept override;

    bool startCapture() override;

    void stopCapture() override;

    [[nodiscard]] bool isCapturing() const noexcept override;

    [[nodiscard]] core::AudioBuffer captureOnce() override;

    void shutdown() override;

    [[nodiscard]] core::Error lastError() const override;

private:
    bool m_initialized{false};
    bool m_capturing{false};

    core::VoiceConfig m_config{};
    core::Error m_lastError{};

    // Futuro:
    // - backend handle (e.g., PortAudio stream)
};

} // namespace voice_engine::audio