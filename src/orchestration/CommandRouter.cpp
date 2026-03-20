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