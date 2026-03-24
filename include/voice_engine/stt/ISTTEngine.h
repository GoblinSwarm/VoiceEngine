#pragma once

// include/voice_engine/stt/ISTTEngine.h
// =====================================
//
// ISTTEngine
// ----------
//
// Abstract contract for speech-to-text engine implementations.
//
// Architecture role
// -----------------
// STT layer.
//
// This module defines the backend-agnostic contract that any concrete
// speech-to-text engine must satisfy inside VoiceEngine.
//
// Current design scope
// --------------------
// At the current stage of the project, the STT system is intentionally
// modeled around:
//
// - batch transcription
// - final results only
// - local/offline execution
//
// This interface reflects that scope and must remain minimal.
//
// This module is responsible ONLY for:
// - defining the lifecycle contract for STT engines
// - defining the batch transcription contract for prepared audio input
// - allowing higher-level modules to depend on abstraction instead of
//   concrete backend implementations
//
// Non-responsibilities
// --------------------
// This module MUST NOT:
// - implement transcription logic
// - expose backend-specific types or details
// - capture audio
// - perform audio conversion or resampling
// - define orchestration policy
//
// Input contract
// --------------
// Implementations are expected to receive:
//
// - mono PCM audio
// - normalized float samples in range [-1.0, 1.0]
// - sample rate already prepared for the target engine
//
// Any required audio conversion must happen before calling transcribe().
//
// Design notes
// ------------
// - Keep this interface backend-agnostic.
// - Keep it aligned with the current batch-only architecture.
// - Do not add speculative methods for streaming or partial results.
// - Prefer a small and stable contract.
//

#include <vector>

#include "voice_engine/stt/STTConfig.h"
#include "voice_engine/stt/STTTypes.h"

namespace voice_engine::stt
{

class ISTTEngine
{
public:
    virtual ~ISTTEngine() = default;

    // --------------------------------------------------
    // Lifecycle
    // --------------------------------------------------

    virtual bool initialize(const STTConfig& config) = 0;
    virtual void shutdown() = 0;
    virtual bool isInitialized() const = 0;

    // --------------------------------------------------
    // Transcription
    // --------------------------------------------------

    virtual TranscriptionResult transcribe(const std::vector<float>& audioBuffer) = 0;
};

} // namespace voice_engine::stt