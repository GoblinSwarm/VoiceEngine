// src/audio/AudioInputDevice.cpp
// =================================
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
// This module implements a concrete audio input device, typically backed by
// a system audio backend (e.g., microphone via PortAudio or similar).
//
// It is responsible ONLY for:
// - managing the lifecycle of an audio input device
// - capturing raw audio data into AudioBuffer objects
//
// Typical flow
// ------------
// VoiceEngine / InteractionLoop
//        ↓
//   IAudioInput (interface)
//        ↓
//   AudioInputDevice (this implementation)
//        ↓
//   backend (future: PortAudio / OS API)
//
// Responsibilities
// ----------------
// This module is responsible ONLY for:
// - initializing and shutting down the audio input device
// - starting and stopping capture sessions
// - providing captured audio buffers on demand
// - tracking device state (initialized, capturing)
//
// Non-responsibilities
// --------------------
// This module MUST NOT:
// - perform audio preprocessing (noise reduction, normalization, etc.)
// - perform speech-to-text (STT)
// - decide when or why audio should be captured
// - manage high-level orchestration or interaction logic
//
// Design notes
// ------------
// - This implementation currently acts as a stub (no real audio backend).
// - Future versions should integrate a backend such as PortAudio.
// - Keep this module focused on device interaction only.
// - Avoid introducing threading or async behavior at this stage.
// - Higher-level modules should control when capture is triggered.
//

#include "voice_engine/audio/AudioInputDevice.h"

namespace voice_engine::audio
{

AudioInputDevice::AudioInputDevice() = default;

AudioInputDevice::~AudioInputDevice()
{
    shutdown();
}

bool AudioInputDevice::initialize(const core::VoiceConfig& config)
{
    m_config = config;
    m_initialized = true;
    m_capturing = false;
    m_lastError = {};
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
        m_lastError = core::Error{};
        return false;
    }

    m_capturing = true;
    return true;
}

void AudioInputDevice::stopCapture()
{
    m_capturing = false;
}

bool AudioInputDevice::isCapturing() const noexcept
{
    return m_capturing;
}

core::AudioBuffer AudioInputDevice::captureOnce()
{
    core::AudioBuffer buffer{};

    if (!m_initialized || !m_capturing)
    {
        m_lastError = core::Error{};
        return buffer;
    }

    // Stub temporal:
    // por ahora devolvemos un buffer vacío/silencioso.
    // Más adelante acá irá la lectura real desde el backend de audio.
    return buffer;
}

void AudioInputDevice::shutdown()
{
    m_capturing = false;
    m_initialized = false;
}

core::Error AudioInputDevice::lastError() const
{
    return m_lastError;
}

} // namespace voice_engine::audio