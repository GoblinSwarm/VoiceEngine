#pragma once

// include/voice_engine/audio/WavAudioInput.h
// =========================================
//
// WavAudioInput
// -------------
//
// WAV file loader for VoiceEngine.
//
// Architecture role
// -----------------
// Audio layer.
//
// This module provides a concrete file-based audio input implementation
// for loading uncompressed WAV audio files into the system.
//
// It is intended to be the first real STT input path, allowing the system
// to load speech audio from disk, convert it into a normalized AudioBuffer,
// and pass it to the STT layer without involving live capture.
//
// This module is responsible ONLY for:
// - opening .wav files from disk
// - validating basic WAV / RIFF structure
// - extracting supported PCM metadata
// - loading raw sample data
// - converting PCM samples into normalized float samples
// - returning a ready-to-use AudioBuffer
//
// Non-responsibilities
// --------------------
// This module MUST NOT:
// - perform live microphone capture
// - perform audio preprocessing beyond basic sample conversion
// - perform STT inference
// - perform playback
// - decide transcription orchestration
//
// Design notes
// ------------
// - This first implementation should stay intentionally narrow.
// - Support should initially be limited to uncompressed PCM WAV.
// - Prefer mono, 16-bit PCM, 16 kHz input for the first real STT path.
// - Unsupported formats should be rejected explicitly.
// - Keep parsing details encapsulated inside this implementation.
//

#include <cstdint>
#include <string>

#include "voice_engine/audio/IAudioFileInput.h"

namespace voice_engine::audio
{

class WavAudioInput : public IAudioFileInput
{
public:
    WavAudioInput() = default;
    ~WavAudioInput() override = default;

    [[nodiscard]] core::AudioBuffer loadFromFile(const std::string& filePath) override;
    [[nodiscard]] core::Error lastError() const override;

private:
    struct WavMetadata
    {
        std::uint16_t audioFormat = 0;
        std::uint16_t channelCount = 0;
        std::uint32_t sampleRate = 0;
        std::uint16_t bitsPerSample = 0;
        std::uint32_t dataSize = 0;
    };

private:
    core::Error m_lastError {};

private:
    void setError(const core::Error& error);

    static float pcm16ToFloat(std::int16_t sample);
};

} // namespace voice_engine::audio