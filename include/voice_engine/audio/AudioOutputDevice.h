#pragma once

// include/voice_engine/audio/AudioOutputDevice.h
// ==============================================
//
// AudioOutputDevice
// -----------------
//
// Concrete implementation of IAudioOutput for audio playback.
//
// Architecture role
// -----------------
// Audio layer.
//
// This module provides a concrete implementation of audio output,
// typically backed by a system audio device (e.g., speakers).
//
// This module is responsible ONLY for:
// - managing audio output device lifecycle
// - playing AudioBuffer data through an output backend
//
// Non-responsibilities
// --------------------
// This module MUST NOT:
// - perform audio preprocessing
// - perform TTS synthesis
// - decide what audio should be played
//
// Design notes
// ------------
// - This class should wrap a backend (e.g., PortAudio).
// - For now, it acts as a stub implementation.
// - Keep logic minimal and focused on playback.
//

#include "voice_engine/audio/IAudioOutput.h"

namespace voice_engine::audio
{

class AudioOutputDevice : public IAudioOutput
{
public:
    AudioOutputDevice();
    ~AudioOutputDevice() override;

    bool initialize(const core::VoiceConfig& config) override;

    [[nodiscard]] bool isInitialized() const noexcept override;

    bool startPlayback() override;

    void stopPlayback() override;

    [[nodiscard]] bool isPlaying() const noexcept override;

    bool play(const core::AudioBuffer& buffer) override;

    void shutdown() override;

    [[nodiscard]] core::Error lastError() const override;

private:
    bool m_initialized{false};
    bool m_playing{false};

    core::VoiceConfig m_config{};
    core::Error m_lastError{};

    // Futuro:
    // - backend handle (PortAudio stream, etc.)
};

} // namespace voice_engine::audio