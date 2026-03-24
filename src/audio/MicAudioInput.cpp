// src/audio/MicAudioInput.cpp
// ======================================================
//
// MicAudioInput
// -------------
//
// Microphone-based audio input implementation for VoiceEngine.
//

#include "voice_engine/audio/MicAudioInput.h"

#include <algorithm>

namespace voice_engine::audio
{

MicAudioInput::MicAudioInput() = default;

MicAudioInput::~MicAudioInput()
{
    shutdown();
}

core::Error MicAudioInput::makeError(
    core::ErrorCode code,
    const char* message
)
{
    return core::Error{code, message};
}

bool MicAudioInput::initialize(const core::VoiceConfig& voiceConfig)
{
    if (initialized)
    {
        lastErrorCode = makeError(
            core::ErrorCode::AlreadyInitialized,
            "MicAudioInput is already initialized."
        );
        return false;
    }

    if (!validateConfig(voiceConfig))
    {
        lastErrorCode = makeError(
            core::ErrorCode::InvalidConfiguration,
            "Invalid microphone capture configuration."
        );
        return false;
    }

    config = voiceConfig;

    if (!device.initialize(config))
    {
        lastErrorCode = device.lastError();
        return false;
    }

    tempBuffer.clear();
    stopRequested = false;
    capturing = false;
    initialized = true;
    lastErrorCode = core::Error::ok();

    return true;
}

bool MicAudioInput::isInitialized() const noexcept
{
    return initialized;
}

bool MicAudioInput::startCapture()
{
    if (!initialized)
    {
        lastErrorCode = makeError(
            core::ErrorCode::NotInitialized,
            "MicAudioInput must be initialized before capture."
        );
        return false;
    }

    if (capturing)
    {
        lastErrorCode = makeError(
            core::ErrorCode::InvalidState,
            "Capture is already in progress."
        );
        return false;
    }

    if (!device.startCapture())
    {
        lastErrorCode = device.lastError();
        return false;
    }

    tempBuffer.clear();
    stopRequested = false;
    capturing = true;
    lastErrorCode = core::Error::ok();

    return true;
}

bool MicAudioInput::isCapturing() const noexcept
{
    return capturing;
}

core::AudioBuffer MicAudioInput::captureOnce()
{
    core::AudioBuffer emptyBuffer;

    if (!initialized)
    {
        lastErrorCode = makeError(
            core::ErrorCode::NotInitialized,
            "MicAudioInput must be initialized before capture."
        );
        return emptyBuffer;
    }

    if (!capturing)
    {
        lastErrorCode = makeError(
            core::ErrorCode::InvalidState,
            "Capture must be started before calling captureOnce()."
        );
        return emptyBuffer;
    }

    const core::FrameCount maxFrames = calculateMaxFrames();
    if (maxFrames == 0)
    {
        lastErrorCode = makeError(
            core::ErrorCode::InvalidConfiguration,
            "Maximum capture duration produced zero frames."
        );
        return emptyBuffer;
    }

    tempBuffer.clear();
    tempBuffer.reserve(
        static_cast<std::size_t>(maxFrames * config.audio.inputChannels)
    );

    core::FrameCount accumulatedFrames = 0;

    while (!stopRequested && accumulatedFrames < maxFrames)
    {
        core::AudioBuffer chunk = device.captureOnce();

        if (chunk.empty())
        {
            lastErrorCode = device.lastError();
            capturing = false;
            device.stopCapture();
            tempBuffer.clear();
            return emptyBuffer;
        }

        const core::FrameCount remainingFrames = maxFrames - accumulatedFrames;
        const core::FrameCount chunkFrames = chunk.frameCount();
        const core::FrameCount framesToCopy = std::min(chunkFrames, remainingFrames);

        if (framesToCopy == 0)
        {
            break;
        }

        const std::size_t samplesToCopy =
            static_cast<std::size_t>(framesToCopy * chunk.format().channels);

        const std::vector<float>& chunkSamples = chunk.samples();

        tempBuffer.insert(
            tempBuffer.end(),
            chunkSamples.begin(),
            chunkSamples.begin() + static_cast<std::ptrdiff_t>(samplesToCopy)
        );

        accumulatedFrames += framesToCopy;
    }

    device.stopCapture();
    capturing = false;

    if (tempBuffer.empty())
    {
        lastErrorCode = makeError(
            core::ErrorCode::AudioCaptureFailed,
            "Microphone capture produced no audio samples."
        );
        return emptyBuffer;
    }

    lastErrorCode = core::Error::ok();
    return buildAudioBuffer();
}

void MicAudioInput::stopCapture()
{
    if (!capturing)
    {
        return;
    }

    stopRequested = true;
}

void MicAudioInput::shutdown()
{
    if (capturing)
    {
        stopRequested = true;
        device.stopCapture();
        capturing = false;
    }

    device.shutdown();

    tempBuffer.clear();
    stopRequested = false;
    initialized = false;
    lastErrorCode = core::Error::ok();
}

core::Error MicAudioInput::lastError() const
{
    return lastErrorCode;
}

bool MicAudioInput::validateConfig(const core::VoiceConfig& voiceConfig) const
{
    const auto& audio = voiceConfig.audio;

    if (audio.inputSampleRate == 0)
        return false;

    if (audio.inputChannels == 0)
        return false;

    if (audio.inputFramesPerBuffer == 0)
        return false;

    if (audio.maxInputCaptureDurationMs == 0)
        return false;

    return true;
}

core::AudioBuffer MicAudioInput::buildAudioBuffer() const
{
    core::AudioFormat format;
    format.sampleRate = config.audio.inputSampleRate;
    format.channels = config.audio.inputChannels;
    format.format = core::SampleFormat::Float32;

    return core::AudioBuffer(format, tempBuffer);
}

core::FrameCount MicAudioInput::calculateMaxFrames() const
{
    const double durationSeconds =
        static_cast<double>(config.audio.maxInputCaptureDurationMs) / 1000.0;

    return static_cast<core::FrameCount>(
        durationSeconds * static_cast<double>(config.audio.inputSampleRate)
    );
}

} // namespace voice_engine::audio