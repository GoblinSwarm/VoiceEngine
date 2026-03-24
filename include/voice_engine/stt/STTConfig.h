#pragma once

// include/voice_engine/stt/STTConfig.h
// ====================================
//
// STTConfig
// ---------
//
// Configuration for speech-to-text engines.
//
// Architecture role
// -----------------
// STT layer.
//
// This module defines the configuration required to initialize
// and run a speech-to-text engine.
//
// Current design scope
// --------------------
// The system is currently designed around:
//
// - batch transcription
// - Whisper-based backend (whisper.cpp)
// - local/offline execution
//
// This configuration reflects only what is required for that setup.
//
// This module is responsible ONLY for:
// - defining runtime configuration for STT engines
// - grouping parameters required for initialization and inference
//
// Non-responsibilities
// --------------------
// This module MUST NOT:
// - implement transcription logic
// - validate file paths or perform I/O
// - depend on a concrete backend implementation
//
// Design notes
// ------------
// - Keep configuration minimal and aligned with real usage.
// - Avoid speculative or future-only parameters.
// - Expand only when new requirements are introduced.
//

#include <string>

namespace voice_engine::stt
{

struct STTConfig
{
    // Path to Whisper model file (e.g. ggml-medium.bin)
    std::string modelPath{};

    // Language hint (e.g. "es", "en")
    // "auto" can be used if detection is enabled
    std::string language = "es";

    // Number of CPU threads to use
    int threads = 4;

    // Beam search size (affects accuracy vs speed)
    int beamSize = 5;

    // Whether to translate output to English
    bool translateToEnglish = false;
};

} // namespace voice_engine::stt