// src/audio/SpeakerAudioOutput.cpp
// =================================
//
// SpeakerAudioOutput
// ------------------
//
// Runtime implementation for playing AudioBuffer data
// through the system audio output device.
//
// Architecture role
// -----------------
// Audio output layer.
//
// This module contains the concrete implementation of
// SpeakerAudioOutput using PortAudio as the underlying
// playback backend.
//
// It acts as the runtime boundary between VoiceEngine's
// internal AudioBuffer representation and the system's
// output audio device.
//
// Typical flow
// ------------
// AudioBuffer (float samples)
//        ↓
// SpeakerAudioOutput
//        ↓
// PortAudio output stream
//        ↓
// speakers
//
// Responsibilities
// ----------------
// This module is responsible ONLY for:
// - initializing PortAudio for playback
// - validating AudioBuffer input before playback
// - opening and closing the output stream
// - writing float32 sample data to the device
// - reporting playback errors
//
// Non-responsibilities
// --------------------
// This module MUST NOT:
// - load WAV files
// - perform audio preprocessing or resampling
// - synthesize audio
// - capture microphone input
// - orchestrate application flow
//
// Design notes
// ------------
// - Playback is currently synchronous/blocking.
// - This module expects AudioBuffer to carry float32 samples.
// - The current implementation uses the default output device.
// - Future versions may support non-blocking playback,
//   streaming, and explicit device selection.
//

#include "voice_engine/audio/SpeakerAudioOutput.h"

#include <portaudio.h>

namespace voice_engine::audio
{

namespace
{

core::Error makeError(core::ErrorCode code, const char* message)
{
    return core::Error{code, message};
}

core::Error makePortAudioError(core::ErrorCode code, const char* prefix, PaError paError)
{
    std::string message = prefix;
    message += " ";
    message += Pa_GetErrorText(paError);
    return core::Error{code, message};
}

} // namespace

SpeakerAudioOutput::SpeakerAudioOutput() = default;

SpeakerAudioOutput::~SpeakerAudioOutput()
{
    shutdown();
}

bool SpeakerAudioOutput::initialize(const core::VoiceConfig& /*config*/)
{
    if (m_initialized)
    {
        m_lastError = makeError(
            core::ErrorCode::AlreadyInitialized,
            "SpeakerAudioOutput is already initialized."
        );
        return false;
    }

    const PaError paError = Pa_Initialize();
    if (paError != paNoError)
    {
        m_lastError = makePortAudioError(
            core::ErrorCode::AudioDeviceUnavailable,
            "Failed to initialize PortAudio for playback.",
            paError
        );
        return false;
    }

    m_initialized = true;
    m_lastError = core::Error::ok();
    return true;
}

bool SpeakerAudioOutput::isInitialized() const noexcept
{
    return m_initialized;
}

void SpeakerAudioOutput::shutdown()
{
    if (!m_initialized)
    {
        m_lastError = core::Error::ok();
        return;
    }

    const PaError paError = Pa_Terminate();
    if (paError != paNoError)
    {
        m_lastError = makePortAudioError(
            core::ErrorCode::AudioPlaybackFailed,
            "Failed to terminate PortAudio playback runtime.",
            paError
        );
        m_initialized = false;
        return;
    }

    m_initialized = false;
    m_lastError = core::Error::ok();
}

bool SpeakerAudioOutput::play(const core::AudioBuffer& buffer)
{
    if (!m_initialized)
    {
        m_lastError = makeError(
            core::ErrorCode::NotInitialized,
            "SpeakerAudioOutput must be initialized before playback."
        );
        return false;
    }

    if (!validateBuffer(buffer))
    {
        return false;
    }

    PaStream* stream = nullptr;

    const int channelCount = static_cast<int>(buffer.format().channels);
    const double sampleRate = static_cast<double>(buffer.format().sampleRate);
    const unsigned long framesPerBuffer = paFramesPerBufferUnspecified;

    const PaError openError = Pa_OpenDefaultStream(
        &stream,
        0, // no input channels
        channelCount,
        paFloat32,
        sampleRate,
        framesPerBuffer,
        nullptr,
        nullptr
    );

    if (openError != paNoError)
    {
        m_lastError = makePortAudioError(
            core::ErrorCode::AudioDeviceUnavailable,
            "Failed to open default output stream.",
            openError
        );
        return false;
    }

    const PaError startError = Pa_StartStream(stream);
    if (startError != paNoError)
    {
        Pa_CloseStream(stream);
        m_lastError = makePortAudioError(
            core::ErrorCode::AudioPlaybackFailed,
            "Failed to start audio playback stream.",
            startError
        );
        return false;
    }

    const PaError writeError = Pa_WriteStream(
        stream,
        buffer.data(),
        static_cast<unsigned long>(buffer.frameCount())
    );

    if (writeError != paNoError)
    {
        Pa_AbortStream(stream);
        Pa_CloseStream(stream);

        m_lastError = makePortAudioError(
            core::ErrorCode::AudioPlaybackFailed,
            "Failed while writing audio data to output stream.",
            writeError
        );
        return false;
    }

    const PaError stopError = Pa_StopStream(stream);
    if (stopError != paNoError)
    {
        Pa_CloseStream(stream);

        m_lastError = makePortAudioError(
            core::ErrorCode::AudioPlaybackFailed,
            "Failed to stop audio playback stream.",
            stopError
        );
        return false;
    }

    const PaError closeError = Pa_CloseStream(stream);
    if (closeError != paNoError)
    {
        m_lastError = makePortAudioError(
            core::ErrorCode::AudioPlaybackFailed,
            "Failed to close audio playback stream.",
            closeError
        );
        return false;
    }

    m_lastError = core::Error::ok();
    return true;
}

core::Error SpeakerAudioOutput::lastError() const
{
    return m_lastError;
}

bool SpeakerAudioOutput::validateBuffer(const core::AudioBuffer& buffer) const
{
    if (buffer.empty())
    {
        const_cast<SpeakerAudioOutput*>(this)->m_lastError = makeError(
            core::ErrorCode::AudioBufferEmpty,
            "Cannot play an empty AudioBuffer."
        );
        return false;
    }

    if (!core::isValidFormat(buffer.format()))
    {
        const_cast<SpeakerAudioOutput*>(this)->m_lastError = makeError(
            core::ErrorCode::AudioFormatInvalid,
            "AudioBuffer format is invalid."
        );
        return false;
    }

    if (buffer.format().format != core::SampleFormat::Float32)
    {
        const_cast<SpeakerAudioOutput*>(this)->m_lastError = makeError(
            core::ErrorCode::AudioFormatInvalid,
            "SpeakerAudioOutput only supports Float32 AudioBuffer playback."
        );
        return false;
    }

    if (buffer.frameCount() == 0)
    {
        const_cast<SpeakerAudioOutput*>(this)->m_lastError = makeError(
            core::ErrorCode::AudioBufferEmpty,
            "AudioBuffer has no playable frames."
        );
        return false;
    }

    return true;
}

} // namespace voice_engine::audio