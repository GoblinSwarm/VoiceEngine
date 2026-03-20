// include/voice_engine/orchestration/CommandRouter.h
// ===================================================
//
// CommandRouter
// -------------
//
// Semantic interpretation component that maps recognized text into actions or responses.
//
// Architecture role
// -----------------
// Orchestration layer.
//
// This module is responsible for interpreting recognized user text and deciding
// what the system should do next. It acts as the semantic bridge between
// speech recognition output and higher-level behavior.
//
// Typical responsibilities may include:
// - matching known commands
// - selecting an engine response
// - routing text toward gameplay, demo logic, or system actions
//
// Typical flow
// ------------
// TranscriptionResult / recognized text
//        ↓
// CommandRouter
//        ↓
// command / response / action decision
//
// This module is responsible ONLY for:
// - interpreting recognized text at the semantic level
// - mapping input text to actions, intents, or responses
// - isolating command-handling logic from STT and TTS pipelines
//
// Non-responsibilities
// --------------------
// This module MUST NOT:
// - transcribe audio
// - synthesize speech
// - capture or play audio
// - depend on provider-specific STT or TTS details
//
// Design notes
// ------------
// - Keep this module focused on interpretation and routing.
// - This module should operate on stable VoiceEngine domain types, not raw provider outputs.
// - It should be easy to replace or expand this logic as the system evolves.
// - Command semantics should remain separate from recognition and synthesis mechanics.