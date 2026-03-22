#pragma once

// include/voice_engine/tts/ITTSEngine.h
// =====================================
//
// ITTSEngine
// ----------
//
// Contract for text-to-speech engines used by VoiceEngine.
//
// Architecture role
// -----------------
// TTS layer.
//
// This module defines the abstract contract that any text-to-speech engine
// must satisfy in order to synthesize text into audio within the VoiceEngine system.
//
// Typical implementations may include:
// - Piper-based engines
// - offline/local TTS backends
// - mock or test synthesis engines
//
// This module is responsible ONLY for:
// - defining the synthesis contract for TTS providers
// - abstracting speech synthesis capabilities behind a stable interface
// - allowing higher-level modules to depend on synthesis behavior instead of concrete engines
//
// Non-responsibilities
// --------------------
// This module MUST NOT:
// - implement synthesis logic directly
// - play audio to output devices
// - preprocess synthesized audio
// - decide what text should be synthesized or why
//
// Design notes
// ------------
// - This interface should remain minimal and provider-agnostic.
// - Prefer a contract centered on clear text-to-audio transformation.
// - Keep backend-specific details out of this interface.
// - Higher-level modules should depend on this abstraction, not on Piper or any specific TTS backend.
//

#include "voice_engine/core/ErrorTypes.h"
#include "voice_engine/core/VoiceConfig.h"
#include "voice_engine/tts/TTSTypes.h"

namespace voice_engine::tts
{

class ITTSEngine
{
public:
    virtual ~ITTSEngine() = default;

    virtual bool initialize(const core::VoiceConfig& config) = 0;

    [[nodiscard]] virtual bool isInitialized() const noexcept = 0;

    [[nodiscard]] virtual SynthesisResult synthesize(
        const SynthesisRequest& request) = 0;

    virtual void shutdown() = 0;

    [[nodiscard]] virtual core::Error lastError() const = 0;
};

} // namespace voice_engine::tts