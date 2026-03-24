#pragma once

// include/voice_engine/stt/SpeechRecognizer.h
// ============================================
//
// SpeechRecognizer
// ----------------
//
// High-level STT pipeline component that coordinates audio-to-text recognition.
//
// Architecture role
// -----------------
// STT layer.
//
// This module owns the VoiceEngine-side speech recognition flow.
// It receives audio prepared by upstream components, applies any required
// preprocessing coordination, and delegates transcription to an ISTTEngine.
//
// Typical flow
// ------------
// AudioInput / upstream audio source
//        ↓
// AudioPreprocessor
//        ↓
// SpeechRecognizer
//        ↓
// ISTTEngine
//        ↓
// TranscriptionResult
//
// Current design scope
// --------------------
// At the current stage of the project, the STT pipeline is intentionally
// modeled around:
//
// - batch transcription
// - final results only
// - no partial/intermediate recognition output
//
// This recognizer should reflect that scope and remain aligned with the
// simplified STT contract.
//
// This module is responsible ONLY for:
// - coordinating the speech recognition pipeline at the system level
// - receiving audio data intended for transcription
// - delegating actual transcription work to an ISTTEngine
// - returning stable STT domain results to higher-level modules
//
// Non-responsibilities
// --------------------
// This module MUST NOT:
// - implement provider-specific STT logic
// - capture audio directly from hardware devices
// - decide how recognized text should be interpreted
// - own high-level interaction or application flow
//
// Design notes
// ------------
// - This module should depend on abstractions, not concrete STT providers.
// - Keep the recognizer focused on pipeline coordination, not engine internals.
// - Audio preparation responsibilities should remain explicit and not hidden.
// - The recognizer should return VoiceEngine domain types, not raw provider payloads.
//

#include "voice_engine/audio/AudioPreprocessor.h"
#include "voice_engine/core/AudioBuffer.h"
#include "voice_engine/stt/ISTTEngine.h"
#include "voice_engine/stt/STTTypes.h"

namespace voice_engine::stt
{

class SpeechRecognizer
{
public:
    SpeechRecognizer(audio::AudioPreprocessor& preprocessor, ISTTEngine& engine);
    ~SpeechRecognizer() = default;

    [[nodiscard]] TranscriptionResult recognize(const core::AudioBuffer& input);

private:
    audio::AudioPreprocessor& m_preprocessor;
    ISTTEngine& m_engine;
};

} // namespace voice_engine::stt