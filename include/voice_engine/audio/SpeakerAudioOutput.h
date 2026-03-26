#pragma once

// include/voice_engine/audio/SpeakerAudioOutput.h
// ===============================================
//
// SpeakerAudioOutput
// ------------------
//
// Concrete audio output module for playing AudioBuffer data
// through the system's audio device.
//
// Architecture role
// -----------------
// Audio output layer.
//
// This module is responsible for sending audio data from
// VoiceEngine (AudioBuffer) to the system's audio output device
// (e.g. speakers or headphones).
//
// It acts as the symmetric counterpart of MicAudioInput,
// but for playback instead of capture.
//
// Typical flow
// ------------
// AudioBuffer (float samples)
//        ↓
// SpeakerAudioOutput
//        ↓
// audio device (PortAudio)
//        ↓
// speakers
//
// Responsibilities
// ----------------
// This module is responsible ONLY for:
// - initializing the audio output device
// - validating AudioBuffer input
// - playing audio buffers through the system output
// - handling playback lifecycle (open, play, close)
// - reporting playback errors
//
// Non-responsibilities
// --------------------
// This module MUST NOT:
// - load audio from files (WAV, etc.)
// - perform audio preprocessing or resampling
// - synthesize audio (TTS)
// - capture audio from input devices
// - orchestrate application flow
//
// Design notes
// ------------
// - This module assumes AudioBuffer uses float32 samples.
// - Playback is currently blocking (synchronous).
// - Future extensions may include:
//   - non-blocking playback
//   - streaming playback
//   - device selection
// - Keep this module backend-focused and isolated.
//

#include "voice_engine/core/AudioBuffer.h"
#include "voice_engine/core/ErrorTypes.h"
#include "voice_engine/core/VoiceConfig.h"

namespace voice_engine::audio
{

class SpeakerAudioOutput
{
public:
    SpeakerAudioOutput();
    ~SpeakerAudioOutput();

    // ======================================================
    // Lifecycle
    // ======================================================

    bool initialize(const core::VoiceConfig& config);

    [[nodiscard]] bool isInitialized() const noexcept;

    void shutdown();

    // ======================================================
    // Playback
    // ======================================================

    // Plays the given AudioBuffer through the output device.
    // This call is blocking until playback completes.
    bool play(const core::AudioBuffer& buffer);

    // ======================================================
    // Error handling
    // ======================================================

    [[nodiscard]] core::Error lastError() const;

private:
    // ======================================================
    // Internal helpers
    // ======================================================

    [[nodiscard]] bool validateBuffer(const core::AudioBuffer& buffer) const;

private:
    // ======================================================
    // State
    // ======================================================

    bool m_initialized{false};
    core::Error m_lastError{};
};

} // namespace voice_engine::audio