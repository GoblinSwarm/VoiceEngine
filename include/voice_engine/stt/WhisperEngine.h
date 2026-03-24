#pragma once

// include/voice_engine/stt/WhisperEngine.h
// ========================================
//
// WhisperEngine
// -------------
//
// Whisper-based speech-to-text engine implementation.
//
// Architecture role
// -----------------
// STT layer (engine implementation).
//
// This module provides a concrete speech-to-text engine backed by
// whisper.cpp. It is responsible for running transcription on already
// prepared audio data.
//
// Current design scope
// --------------------
// The engine is currently designed for:
//
// - batch transcription only
// - no partial/intermediate results
// - offline/local inference using whisper.cpp
//
// This module is responsible ONLY for:
// - loading and owning the Whisper model
// - executing transcription on input audio buffers
// - returning final transcription results
//
// Non-responsibilities
// --------------------
// This module MUST NOT:
// - capture audio from microphone or devices
// - perform audio format conversion (resampling, decoding, etc.)
// - handle streaming or incremental transcription
// - emit partial results
// - manage UI or orchestration logic
//
// Input contract
// --------------
// The engine expects audio data as:
//
// - mono PCM
// - normalized float samples in range [-1.0, 1.0]
// - sample rate compatible with Whisper (typically 16 kHz)
//
// Any required audio conversion must be performed BEFORE calling this engine.
//
// Design notes
// ------------
// - Keep this class focused and minimal.
// - Avoid leaking whisper.cpp internals into the interface.
// - Treat this as a thin execution layer over the model.
// - Errors should be reflected in TranscriptionResult.
//

#include <vector>

#include "voice_engine/stt/ISTTEngine.h"
#include "voice_engine/stt/STTConfig.h"
#include "voice_engine/stt/STTTypes.h"

namespace voice_engine::stt
{

class WhisperEngine : public ISTTEngine
{
public:
    WhisperEngine() = default;
    ~WhisperEngine() override;

    // Non-copyable
    WhisperEngine(const WhisperEngine&) = delete;
    WhisperEngine& operator=(const WhisperEngine&) = delete;

    // Movable (optional but useful)
    WhisperEngine(WhisperEngine&&) noexcept = default;
    WhisperEngine& operator=(WhisperEngine&&) noexcept = default;

    // --------------------------------------------------
    // Lifecycle
    // --------------------------------------------------

    // Initializes the engine and loads the Whisper model
    bool initialize(const STTConfig& config) override;

    // Releases all resources (model, context, etc.)
    void shutdown() override;

    // Returns whether the engine has been successfully initialized
    bool isInitialized() const override;

    // --------------------------------------------------
    // Transcription
    // --------------------------------------------------

    // Transcribes a full audio buffer (batch mode)
    TranscriptionResult transcribe(const std::vector<float>& audioBuffer) override;

private:
    // Opaque pointer to whisper.cpp context
    void* m_context = nullptr;

    STTConfig m_config{};
    bool m_initialized = false;
};

} // namespace voice_engine::stt