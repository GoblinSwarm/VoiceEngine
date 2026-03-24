// include/voice_engine/audio/MicAudioInput.h
// ======================================================
//
// MicAudioInput
// -------------
//
// Microphone-based audio input implementation for VoiceEngine.
//
// Architecture role
// -----------------
// Audio Input Layer (concrete implementation of IAudioInput).
//
// This module provides a high-level microphone input abstraction
// that captures audio in batch mode and produces an AudioBuffer
// ready for speech-to-text processing.
//
// It sits above the low-level AudioInputDevice and is responsible
// for orchestrating a complete capture session.
//

#pragma once

#include <atomic>
#include <vector>

#include "voice_engine/audio/IAudioInput.h"
#include "voice_engine/audio/AudioInputDevice.h"
#include "voice_engine/core/AudioBuffer.h"
#include "voice_engine/core/ErrorTypes.h"
#include "voice_engine/core/VoiceConfig.h"

namespace voice_engine::audio
{

class MicAudioInput : public IAudioInput
{
public:
    MicAudioInput();
    ~MicAudioInput() override;

    bool initialize(const core::VoiceConfig& config) override;
    [[nodiscard]] bool isInitialized() const noexcept override;

    bool startCapture() override;
    void stopCapture() override;
    [[nodiscard]] bool isCapturing() const noexcept override;

    [[nodiscard]] core::AudioBuffer captureOnce() override;

    void shutdown() override;

    [[nodiscard]] core::Error lastError() const override;

private:
    bool validateConfig(const core::VoiceConfig& config) const;
    core::AudioBuffer buildAudioBuffer() const;
    core::FrameCount calculateMaxFrames() const;

    static core::Error makeError(
        core::ErrorCode code,
        const char* message
    );

private:
    AudioInputDevice device;
    core::VoiceConfig config{};

    bool initialized = false;
    bool capturing = false;
    std::atomic<bool> stopRequested{false};

    core::Error lastErrorCode = {};
    std::vector<float> tempBuffer;
};

} // namespace voice_engine::audio