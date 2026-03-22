// src/orchestration/CommandRouter.cpp
// ===================================
//
// CommandRouter
// -------------
//
// Runtime implementation for semantic command interpretation and routing.
//
// Architecture role
// -----------------
// Orchestration layer.
//
// This module contains the concrete implementation of the semantic routing
// behavior that maps recognized text into actions, intents, or response text.
//
// It acts as the interpretation boundary between low-level recognition output
// and the higher-level behavior expected by the VoiceEngine system.
//
// Typical flow
// ------------
// recognized text / TranscriptionResult
//        ↓
// CommandRouter
//        ↓
// action / intent / response selection
//
// This module is responsible ONLY for:
// - implementing semantic interpretation of recognized text
// - mapping input text to commands, intents, or responses
// - isolating command-handling logic from STT and TTS mechanics
//
// Non-responsibilities
// --------------------
// This module MUST NOT:
// - transcribe audio
// - synthesize speech
// - manage audio devices
// - implement provider-specific logic
//
// Design notes
// ------------
// - Keep this module focused on semantic routing, not technical processing.
// - It should consume stable VoiceEngine domain types rather than raw provider output.
// - Command interpretation should remain replaceable and easy to evolve.
// - Avoid leaking orchestration concerns into lower layers.

#include "voice_engine/orchestration/CommandRouter.h"

#include <algorithm>
#include <cctype>
#include <string>

namespace voice_engine::orchestration
{
namespace
{

std::string toLowerCopy(const std::string& text)
{
    std::string normalized = text;
    std::transform(
        normalized.begin(),
        normalized.end(),
        normalized.begin(),
        [](unsigned char c)
        {
            return static_cast<char>(std::tolower(c));
        });

    return normalized;
}

bool isBlank(const std::string& text)
{
    return std::all_of(
        text.begin(),
        text.end(),
        [](unsigned char c)
        {
            return std::isspace(c) != 0;
        });
}

} // namespace

CommandRouteResult CommandRouter::route(const stt::TranscriptionResult& transcription)
{
    m_lastError = {};

    CommandRouteResult result{};

    const std::string recognizedText = transcription.fullText;

    if (recognizedText.empty() || isBlank(recognizedText))
    {
        result.action = CommandAction::None;
        result.success = true;
        return result;
    }

    const std::string normalizedText = toLowerCopy(recognizedText);

    if (normalizedText == "hello" || normalizedText == "hi")
    {
        result.action = CommandAction::SpeakResponse;
        result.responseText = "Hello. How can I help you?";
        return result;
    }

    if (normalizedText == "status")
    {
        result.action = CommandAction::SpeakResponse;
        result.responseText = "Voice engine is running.";
        return result;
    }

    if (normalizedText == "exit" || normalizedText == "quit")
    {
        result.action = CommandAction::ExecuteCommand;
        result.commandText = "exit";
        return result;
    }

    result.action = CommandAction::SpeakResponse;
    result.responseText = "I heard: " + recognizedText;
    return result;
}

core::Error CommandRouter::lastError() const
{
    return m_lastError;
}

} // namespace voice_engine::orchestration