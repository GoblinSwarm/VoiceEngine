#pragma once

// include/voice_engine/tts/SpeechSynthesizer.h
// =============================================
//
// SpeechSynthesizer
// -----------------
//
// High-level TTS pipeline component that coordinates text-to-audio synthesis.
//
// Architecture role
// -----------------
// TTS layer.
//
// This module owns the VoiceEngine-side speech synthesis flow.
// It receives text prepared by upstream components, delegates synthesis
// to an ITTSEngine, and returns stable audio-domain results to the rest
// of the system.
//
// Typical flow
// ------------
// text input / upstream response source
//        ↓
// SpeechSynthesizer
//        ↓
// ITTSEngine
//        ↓
// SynthesisResult / AudioBuffer
//
// This module is responsible ONLY for:
// - coordinating the speech synthesis pipeline at the system level
// - receiving text intended for voice generation
// - delegating actual synthesis work to an ITTSEngine
// - returning stable TTS domain results to higher-level modules
//
// Non-responsibilities
// --------------------
// This module MUST NOT:
// - implement provider-specific TTS logic
// - play audio directly through hardware devices
// - decide what text should be spoken
// - own high-level interaction or application flow
//
// Design notes
// ------------
// - This module should depend on abstractions, not concrete TTS providers.
// - Keep the synthesizer focused on pipeline coordination, not engine internals.
// - The synthesizer should return VoiceEngine domain types, not raw provider payloads.
// - Playback concerns should remain outside this module unless explicitly modeled elsewhere.
//

#include "voice_engine/core/ErrorTypes.h"
#include "voice_engine/tts/ITTSEngine.h"
#include "voice_engine/tts/TTSTypes.h"

namespace voice_engine::tts
{

class SpeechSynthesizer
{
public:
    explicit SpeechSynthesizer(ITTSEngine& engine);
    ~SpeechSynthesizer() = default;

    [[nodiscard]] SynthesisResult synthesize(const SynthesisRequest& request);

    [[nodiscard]] core::Error lastError() const;

private:
    ITTSEngine& m_engine;
    core::Error m_lastError{};
};

} // namespace voice_engine::tts