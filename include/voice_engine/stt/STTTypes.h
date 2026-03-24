#pragma once

// include/voice_engine/stt/STTTypes.h
// ===================================
//
// STTTypes
// --------
//
// Shared speech-to-text domain types used across the STT pipeline.
//
// Architecture role
// -----------------
// STT layer.
//
// This module defines the minimal shared data structures used to represent
// speech-to-text results inside VoiceEngine.
//
// Current design scope
// --------------------
// At the current stage of the project, the STT flow is intentionally modeled as:
//
// - closed batch transcription
// - no partial/intermediate results
// - final text output only
//
// This means the types in this file should reflect the current contract,
// not hypothetical future capabilities.
//
// This module is responsible ONLY for:
// - defining shared STT-related result types
// - providing a stable representation of final transcription output
// - keeping STT result structures independent from concrete backend details
//
// Non-responsibilities
// --------------------
// This module MUST NOT:
// - implement transcription logic
// - capture audio
// - preprocess or convert audio
// - depend on a concrete STT provider
// - expose backend-specific payloads or internal inference data
//
// Design notes
// ------------
// - Keep these types provider-agnostic whenever possible.
// - Model the current system honestly: final batch transcription only.
// - Do not add speculative fields for features not implemented yet.
// - Keep this contract small so engines and orchestrators can depend on it safely.
//

#include <string>

namespace voice_engine::stt
{

// ======================================================
// Recognition status
// ======================================================

enum class RecognitionStatus
{
    Idle = 0,
    Processing,
    Completed,
    Failed
};

// ======================================================
// Final transcription result
// ======================================================

struct TranscriptionResult
{
    std::string fullText{};
    RecognitionStatus status = RecognitionStatus::Idle;
    std::string language{};
    std::string errorMessage{};
};

} // namespace voice_engine::stt