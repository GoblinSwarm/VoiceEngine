// src/audio/AudioInputDevice.cpp
// =================================
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
// audio backend (PortAudio).
//
// Typical flow
// ------------
// MicAudioInput
//        ↓
// AudioInputDevice (this module)
//        ↓
// PortAudio
//
// Responsibilities
// ----------------
// This module is responsible ONLY for:
// - initializing and shutting down the audio input device
// - starting and stopping low-level capture
// - providing raw audio buffers on demand
// - tracking device state (initialized, capturing)
//
// Non-responsibilities
// --------------------
// This module MUST NOT:
// - perform audio preprocessing (noise reduction, normalization, etc.)
// - perform speech-to-text (STT)
// - decide when or why audio should be captured
// - manage high-level capture sessions
// - accumulate long recording windows for STT
//
// Design notes
// ------------
// - This implementation uses PortAudio in blocking mode.
// - No callback model is used in this first version.
// - The device returns raw audio chunks as AudioBuffer values.
// - Higher-level modules should control capture orchestration.
//

#include "voice_engine/audio/AudioInputDevice.h"

#include <portaudio.h>
#include <vector>

namespace voice_engine::audio
{

namespace
{

core::Error makeError(core::ErrorCode code, const char* message)
{
    return core::Error{code, message};
}

core::Error makePaError(PaError errorCode, const char* context)
{
    const char* paMessage = Pa_GetErrorText(errorCode);
    std::string fullMessage = std::string(context) + ": " + paMessage;
    return core::Error{core::ErrorCode::AudioCaptureFailed, fullMessage};
}

PaStream* toPaStream(void* streamHandle)
{
    return static_cast<PaStream*>(streamHandle);
}

} // namespace

AudioInputDevice::AudioInputDevice() = default;

AudioInputDevice::~AudioInputDevice()
{
    shutdown();
}

bool AudioInputDevice::initialize(const core::VoiceConfig& config)
{
    if (m_initialized)
    {
        m_lastError = makeError(
            core::ErrorCode::AlreadyInitialized,
            "AudioInputDevice is already initialized."
        );
        return false;
    }

    const PaError initResult = Pa_Initialize();
    if (initResult != paNoError)
    {
        m_lastError = makePaError(initResult, "Failed to initialize PortAudio");
        return false;
    }

    m_config = config;
    m_initialized = true;
    m_capturing = false;
    m_stream = nullptr;
    m_lastError = core::Error::ok();

    return true;
}

bool AudioInputDevice::isInitialized() const noexcept
{
    return m_initialized;
}

bool AudioInputDevice::startCapture()
{
    if (!m_initialized)
    {
        m_lastError = makeError(
            core::ErrorCode::NotInitialized,
            "AudioInputDevice must be initialized before starting capture."
        );
        return false;
    }

    if (m_capturing)
    {
        m_lastError = makeError(
            core::ErrorCode::InvalidState,
            "AudioInputDevice is already capturing."
        );
        return false;
    }

    PaStream* stream = nullptr;

    const double sampleRate = static_cast<double>(m_config.audio.inputSampleRate);
    const unsigned long framesPerBuffer =
        static_cast<unsigned long>(m_config.audio.inputFramesPerBuffer);
    const int channelCount = static_cast<int>(m_config.audio.inputChannels);

    const PaError openResult = Pa_OpenDefaultStream(
        &stream,
        channelCount,   // num input channels
        0,              // num output channels
        paFloat32,      // sample format
        sampleRate,
        framesPerBuffer,
        nullptr,        // no callback (blocking mode)
        nullptr
    );

    if (openResult != paNoError)
    {
        m_lastError = makePaError(openResult, "Failed to open PortAudio input stream");
        return false;
    }

    const PaError startResult = Pa_StartStream(stream);
    if (startResult != paNoError)
    {
        Pa_CloseStream(stream);
        m_lastError = makePaError(startResult, "Failed to start PortAudio input stream");
        return false;
    }

    m_stream = stream;
    m_capturing = true;
    m_lastError = core::Error::ok();

    return true;
}

void AudioInputDevice::stopCapture()
{
    PaStream* stream = toPaStream(m_stream);

    if (stream != nullptr)
    {
        Pa_StopStream(stream);
        Pa_CloseStream(stream);
        m_stream = nullptr;
    }

    m_capturing = false;
}

bool AudioInputDevice::isCapturing() const noexcept
{
    return m_capturing;
}

core::AudioBuffer AudioInputDevice::captureOnce()
{
    core::AudioBuffer buffer{};

    if (!m_initialized)
    {
        m_lastError = makeError(
            core::ErrorCode::NotInitialized,
            "AudioInputDevice must be initialized before capture."
        );
        return buffer;
    }

    if (!m_capturing)
    {
        m_lastError = makeError(
            core::ErrorCode::InvalidState,
            "AudioInputDevice must be capturing before captureOnce()."
        );
        return buffer;
    }

    PaStream* stream = toPaStream(m_stream);
    if (stream == nullptr)
    {
        m_lastError = makeError(
            core::ErrorCode::InvalidState,
            "AudioInputDevice has no active PortAudio stream."
        );
        return buffer;
    }

    const core::FrameCount framesPerBuffer = m_config.audio.inputFramesPerBuffer;
    const core::ChannelCount channelCount = m_config.audio.inputChannels;

    std::vector<float> samples(
        static_cast<std::size_t>(framesPerBuffer * channelCount),
        0.0f
    );

    const PaError readResult = Pa_ReadStream(
        stream,
        samples.data(),
        static_cast<unsigned long>(framesPerBuffer)
    );

    if (readResult != paNoError)
    {
        m_lastError = makePaError(readResult, "Failed to read microphone audio");
        return core::AudioBuffer{};
    }

    core::AudioFormat format;
    format.sampleRate = m_config.audio.inputSampleRate;
    format.channels = m_config.audio.inputChannels;
    format.format = core::SampleFormat::Float32;

    m_lastError = core::Error::ok();
    return core::AudioBuffer(format, std::move(samples));
}

void AudioInputDevice::shutdown()
{
    stopCapture();

    if (m_initialized)
    {
        Pa_Terminate();
    }

    m_initialized = false;
    m_lastError = core::Error::ok();
}

core::Error AudioInputDevice::lastError() const
{
    return m_lastError;
}

} // namespace voice_engine::audio