#pragma once

// include/voice_engine/core/VoiceConfig.h
// =======================================
//
// VoiceConfig
// -----------
//
// Central configuration model for VoiceEngine runtime settings.
//
// Architecture role
// -----------------
// Core layer.
//
// This module defines the shared runtime configuration used across the
// VoiceEngine system. It centralizes values that affect audio behavior,
// model loading, language selection, and engine-level feature toggles.
//
// This module is responsible ONLY for:
// - defining the configuration structure for the engine
// - centralizing shared runtime parameters
// - exposing stable configuration values to multiple layers
//
// Non-responsibilities
// --------------------
// This module MUST NOT:
// - open devices or initialize backends
// - load models directly
// - perform audio processing
// - orchestrate interaction flow
//
// Design notes
// ------------
// - Keep this file focused on configuration data only.
// - Prefer explicit, strongly typed fields over ambiguous flags.
// - This module should remain provider-agnostic whenever possible.
// - Configuration should describe runtime behavior, not implement it.
//

#include <string>

#include "voice_engine/core/AudioTypes.h"

namespace voice_engine::core
{

struct AudioConfig
{
    SampleRate inputSampleRate = 16000;
    SampleRate outputSampleRate = 22050;
    ChannelCount inputChannels = 1;
    ChannelCount outputChannels = 1;
    bool enablePreprocessing = true;
};

struct STTConfig
{
    std::string modelPath{};
    std::string language = "auto";
    bool translateToEnglish = false;
    bool enablePartialResults = false;
};

struct TTSConfig
{
    std::string modelPath{};
    std::string configPath{};
    std::string voiceName{};
    float speechRate = 1.0f;
    float volume = 1.0f;
};

struct EngineConfig
{
    bool enableSTT = true;
    bool enableTTS = true;
    bool enableCommandRouting = true;
    bool verboseLogging = false;
};

struct VoiceConfig
{
    AudioConfig audio{};
    STTConfig stt{};
    TTSConfig tts{};
    EngineConfig engine{};
};

} // namespace voice_engine::core