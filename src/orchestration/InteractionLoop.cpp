// src/orchestration/InteractionLoop.cpp
// =====================================
//
// InteractionLoop
// ---------------
//
// Runtime implementation for the continuous voice interaction loop.
//
// Architecture role
// -----------------
// Orchestration layer.
//
// This module contains the concrete implementation of the repeated
// interaction cycle used to drive VoiceEngine in demo or simple runtime
// scenarios.
//
// It coordinates the recurring sequence of:
// - listening for input
// - recognizing speech
// - routing commands or generating responses
// - synthesizing output
// - triggering playback
//
// Typical flow
// ------------
// loop:
//   capture input
//   → recognize speech
//   → interpret text
//   → synthesize response
//   → playback
//
// This module is responsible ONLY for:
// - implementing the repeated interaction cycle of the system
// - coordinating iterative use of VoiceEngine capabilities
// - providing a simple runtime loop for demos or lightweight applications
//
// Non-responsibilities
// --------------------
// This module MUST NOT:
// - implement STT or TTS provider logic
// - manage low-level audio backends directly
// - contain complex game or application-specific behavior
// - replace VoiceEngine as the primary façade of the system
//
// Design notes
// ------------
// - Keep this module simple, explicit, and replaceable.
// - This is a usage pattern module, not a core engine primitive.
// - In larger systems, this may be replaced by an event loop, game loop,
//   or other higher-level runtime controller.
// - Avoid turning this file into a second orchestration hub.