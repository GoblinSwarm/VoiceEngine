// src/audio/AudioOutputDevice.cpp
// ===============================
//
// AudioOutputDevice
// -----------------
//
// Runtime implementation for a concrete audio output device.
//
// Architecture role
// -----------------
// Audio layer.
//
// This module provides the concrete implementation of the audio output side
// of VoiceEngine. It is responsible for interacting with a real audio playback
// backend and exposing outgoing audio through the IAudioOutput contract.
//
// It acts as the system-facing adapter between VoiceEngine audio-domain data
// and low-level playback mechanisms such as speakers or other output targets.
//
// Typical flow
// ------------
// AudioBuffer / synthesized audio
//        ↓
// AudioOutputDevice
//        ↓
// backend playback API / physical output device
//
// This module is responsible ONLY for:
// - implementing real audio playback behavior
// - adapting VoiceEngine audio buffers to backend/device output requirements
// - managing the lifecycle of the concrete output device at runtime
//
// Non-responsibilities
// --------------------
// This module MUST NOT:
// - perform TTS synthesis
// - decide what audio should be played
// - perform high-level orchestration
// - contain reusable audio preprocessing logic
//
// Design notes
// ------------
// - Keep backend-specific playback logic isolated in this module.
// - Avoid leaking external audio backend details outside the audio layer.
// - Device lifecycle should remain explicit and predictable.
// - This module should focus on playback, not on synthesis or orchestration.

#include "voice_engine/audio/AudioOutputDevice.h"

namespace voice_engine::audio
{

AudioOutputDevice::AudioOutputDevice() = default;

AudioOutputDevice::~AudioOutputDevice()
{
    shutdown();
}

bool AudioOutputDevice::initialize(const core::VoiceConfig& config)
{
    m_config = config;
    m_initialized = true;
    m_playing = false;
    m_lastError = {};
    return true;
}

bool AudioOutputDevice::isInitialized() const noexcept
{
    return m_initialized;
}

bool AudioOutputDevice::startPlayback()
{
    if (!m_initialized)
    {
        m_lastError = core::Error{};
        return false;
    }

    m_playing = true;
    return true;
}

void AudioOutputDevice::stopPlayback()
{
    m_playing = false;
}

bool AudioOutputDevice::isPlaying() const noexcept
{
    return m_playing;
}

bool AudioOutputDevice::play(const core::AudioBuffer& buffer)
{
    if (!m_initialized || !m_playing)
    {
        m_lastError = core::Error{};
        return false;
    }

    // Stub temporal:
    // por ahora no enviamos audio a ningún backend real.
    // Más adelante acá irá la reproducción concreta del contenido de `buffer`.
    (void)buffer;
    return true;
}

void AudioOutputDevice::shutdown()
{
    m_playing = false;
    m_initialized = false;
}

core::Error AudioOutputDevice::lastError() const
{
    return m_lastError;
}

} // namespace voice_engine::audio