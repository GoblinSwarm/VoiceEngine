// include/voice_engine/orchestration/VoiceEngine.h
// =================================================
//
// VoiceEngine
// -----------
//
// High-level façade and orchestration entry point for the VoiceEngine system.
//
// Architecture role
// -----------------
// Orchestration layer.
//
// This module represents the main entry point of the VoiceEngine system.
// It coordinates the interaction between core subsystems such as:
//
// - audio input/output
// - speech recognition (STT)
// - speech synthesis (TTS)
// - command routing and interpretation
//
// It exposes a simplified API for external consumers (apps, demos, games)
// to interact with the voice system without needing to understand internal details.
//
// Typical flow
// ------------
// Application / caller
//        ↓
// VoiceEngine
//        ↓
// (SpeechRecognizer → ISTTEngine)
// (CommandRouter)
// (SpeechSynthesizer → ITTSEngine)
//        ↓
// Audio output
//
// This module is responsible ONLY for:
// - orchestrating high-level interactions between subsystems
// - exposing a clean and stable interface to the outside world
// - coordinating recognition, interpretation, and synthesis flows
//
// Non-responsibilities
// --------------------
// This module MUST NOT:
// - implement STT or TTS provider logic
// - perform low-level audio capture or playback directly
// - contain heavy preprocessing or transformation logic
// - become a monolithic "god object"
//
// Design notes
// ------------
// - This module should depend on abstractions and composed components.
// - Keep orchestration logic clear and readable.
// - Favor delegation over direct implementation.
// - This class acts as the system boundary: external code should talk to this,
//   not to internal modules directly.