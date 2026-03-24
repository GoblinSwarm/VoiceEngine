#pragma once

// include/voice_engine/core/AudioBuffer.h
// =======================================
//
// AudioBuffer
// -----------
//
// Lightweight container for audio sample data and metadata.
//
// Architecture role
// -----------------
// Core layer.
//
// This module provides a reusable abstraction for storing and passing
// audio data across the VoiceEngine system. It is used as the primary
// data carrier between audio input, preprocessing, STT, and TTS modules.
//
// This module is responsible ONLY for:
// - encapsulating raw audio sample data (e.g. float buffers)
// - storing basic audio metadata (sample rate, channels, length)
// - providing safe and simple access to audio contents
//
// Non-responsibilities
// --------------------
// This module MUST NOT:
// - capture audio from devices
// - perform preprocessing (normalize, trim, resample)
// - perform STT or TTS operations
// - depend on any external audio backend or provider
//
// Design notes
// ------------
// - Keep this class simple and efficient (no heavy logic).
// - Prefer std::vector<float> (or equivalent) for sample storage.
// - Avoid embedding processing logic; this is a data container, not a processor.
// - This type is expected to flow through multiple layers, so stability is critical.
//

#include <cstddef>
#include <utility>
#include <vector>

#include "voice_engine/core/AudioTypes.h"

namespace voice_engine::core
{

class AudioBuffer
{
public:
    AudioBuffer() = default;

    AudioBuffer(AudioFormat format, std::vector<Sample> samples)
        : format_(std::move(format)), samples_(std::move(samples))
    {
    }

    [[nodiscard]] const AudioFormat& format() const noexcept
    {
        return format_;
    }

    [[nodiscard]] AudioFormat& format() noexcept
    {
        return format_;
    }

    [[nodiscard]] const std::vector<Sample>& samples() const noexcept
    {
        return samples_;
    }

    [[nodiscard]] std::vector<Sample>& samples() noexcept
    {
        return samples_;
    }

    [[nodiscard]] const Sample* data() const noexcept
    {
        return samples_.data();
    }

    [[nodiscard]] Sample* data() noexcept
    {
        return samples_.data();
    }

    [[nodiscard]] std::size_t sampleCount() const noexcept
    {
        return samples_.size();
    }

    [[nodiscard]] FrameCount frameCount() const noexcept
    {
        if (format_.channels == 0)
        {
            return 0;
        }

        return static_cast<FrameCount>(samples_.size() / format_.channels);
    }

    [[nodiscard]] bool empty() const noexcept
    {
        return samples_.empty();
    }

    [[nodiscard]] double durationSeconds() const noexcept
    {
        if (format_.sampleRate == 0)
        {
            return 0.0;
        }

        return static_cast<double>(frameCount()) /
               static_cast<double>(format_.sampleRate);
    }

    void clear() noexcept
    {
        samples_.clear();
    }

private:
    AudioFormat format_{};
    std::vector<Sample> samples_{};
};

} // namespace voice_engine::core