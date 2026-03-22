#pragma once

// include/voice_engine/stt/ISTTEngine.h
// =====================================
//
// ISTTEngine
// ----------
//
// Contract for speech-to-text engines used by VoiceEngine.
//
// Architecture role
// -----------------
// STT layer.
//
// This module defines the abstract contract that any speech-to-text engine
// must satisfy in order to transcribe audio into text within the VoiceEngine system.
//
// Typical implementations may include:
// - Whisper-based engines
// - offline/local STT backends
// - mock or test transcription engines
//
// This module is responsible ONLY for:
// - defining the transcription contract for STT providers
// - abstracting speech recognition capabilities behind a stable interface
// - allowing higher-level modules to depend on transcription behavior instead of concrete engines
//
// Non-responsibilities
// --------------------
// This module MUST NOT:
// - implement transcription logic directly
// - capture audio from devices
// - preprocess audio
// - decide how transcribed text should be interpreted or used
//
// Design notes
// ------------
// - This interface should remain minimal and provider-agnostic.
// - Prefer a contract centered on clear audio-to-text transformation.
// - Keep backend-specific details out of this interface.
// - Higher-level modules should depend on this abstraction, not on Whisper or any specific STT backend.
//

#include "voice_engine/core/AudioBuffer.h"
#include "voice_engine/core/ErrorTypes.h"
#include "voice_engine/core/VoiceConfig.h"
#include "voice_engine/stt/STTTypes.h"

namespace voice_engine::stt
{

class ISTTEngine
{
public:
    virtual ~ISTTEngine() = default;

    virtual bool initialize(const core::VoiceConfig& config) = 0;

    [[nodiscard]] virtual bool isInitialized() const noexcept = 0;

    [[nodiscard]] virtual TranscriptionResult transcribe(
        const core::AudioBuffer& audioBuffer,
        const TranscriptionOptions& options = {}) = 0;

    virtual void shutdown() = 0;

    [[nodiscard]] virtual core::Error lastError() const = 0;
};

} // namespace voice_engine::stt