#pragma once

// include/voice_engine/providers/tts/PiperEngine.h
// =================================================
//
// PiperEngine
// -----------
//
// Concrete TTS provider implementation based on Piper CLI.
//
// Architecture role
// -----------------
// Provider layer (TTS).
//
// This module implements the ITTSEngine interface using Piper
// as an external command-line backend.
//
// It is responsible for translating VoiceEngine text input into
// Piper-compatible synthesis execution, invoking `piper.exe`,
// and mapping the generated output into VoiceEngine TTS-domain types.
//
// Typical flow
// ------------
// text input
//        ↓
// PiperEngine
//        ↓
// piper.exe (external process)
//        ↓
// generated wav output
//        ↓
// SynthesisResult / AudioBuffer
//
// Responsibilities
// ----------------
// This module is responsible ONLY for:
// - implementing the ITTSEngine contract using Piper
// - adapting VoiceEngine text requests to Piper-compatible CLI input
// - invoking `piper.exe` as an external synthesis process
// - validating Piper executable/model availability during initialization
// - mapping Piper-generated output into VoiceEngine TTS domain types
//
// Non-responsibilities
// --------------------
// This module MUST NOT:
// - decide what text should be spoken
// - play audio through output devices
// - perform high-level orchestration
// - depend on STT subsystems
// - leak Piper-specific CLI/process details outside this module
//
// Design notes
// ------------
// - This module acts as a boundary between VoiceEngine and an
//   external CLI TTS backend.
// - Keep Piper-specific paths, command construction, and process
//   execution logic isolated here.
// - Avoid leaking Piper-specific details outside this module.
// - All outputs should be mapped to VoiceEngine domain types
//   (e.g. AudioBuffer / SynthesisResult).
// - This first implementation is expected to be a synchronous,
//   local/offline TTS provider.
//

#include <string>

#include "voice_engine/core/ErrorTypes.h"
#include "voice_engine/core/VoiceConfig.h"
#include "voice_engine/tts/ITTSEngine.h"
#include "voice_engine/tts/TTSTypes.h"

namespace voice_engine::providers::tts
{

class PiperEngine : public voice_engine::tts::ITTSEngine
{
public:
    PiperEngine();
    ~PiperEngine() override;

    // ======================================================
    // ITTSEngine interface
    // ======================================================

    bool initialize(const core::VoiceConfig& config) override;

    [[nodiscard]] bool isInitialized() const noexcept override;

    [[nodiscard]] voice_engine::tts::SynthesisResult synthesize(
        const voice_engine::tts::SynthesisRequest& request) override;

    void shutdown() override;

    [[nodiscard]] core::Error lastError() const override;

private:
    // ======================================================
    // Internal helpers
    // ======================================================

    [[nodiscard]] bool validateConfig(const core::VoiceConfig& config) const;

    [[nodiscard]] bool validateRequest(
        const voice_engine::tts::SynthesisRequest& request) const;

    [[nodiscard]] core::Error makeError(
        core::ErrorCode code,
        const char* message) const;

private:
    // ======================================================
    // Configuration
    // ======================================================

    core::VoiceConfig m_config{};

    // ======================================================
    // State
    // ======================================================

    bool m_initialized{false};

    core::Error m_lastError{};
};

} // namespace voice_engine::providers::tts