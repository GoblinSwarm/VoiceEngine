// include/voice_engine/orchestration/InteractionLoop.h
// ====================================================
//
// InteractionLoop
// ---------------
//
// Runtime loop that drives continuous voice interaction cycles.
//
// Architecture role
// -----------------
// Orchestration layer.
//
// This module encapsulates the iterative interaction flow of the system.
// It coordinates repeated cycles of:
//
// - capturing input (via VoiceEngine)
// - obtaining transcription results
// - routing commands or generating responses
// - triggering speech synthesis and playback
//
// It is primarily intended for demo scenarios or simple runtime environments,
// and can later be replaced or integrated into more complex application loops.
//
// Typical flow
// ------------
// loop:
//   capture audio
//   → recognize speech
//   → interpret command
//   → synthesize response
//   → playback
//
// This module is responsible ONLY for:
// - driving the interaction cycle (listen → interpret → respond)
// - coordinating repeated use of VoiceEngine capabilities
// - providing a simple runtime loop for demos or basic applications
//
// Non-responsibilities
// --------------------
// This module MUST NOT:
// - implement STT or TTS logic
// - manage low-level audio devices
// - contain complex application or game logic
// - replace the role of VoiceEngine as system façade
//
// Design notes
// ------------
// - This module should remain simple and replaceable.
// - It is not part of the core engine logic, but a usage pattern.
// - In larger systems, this may be replaced by a game loop or event-driven system.
// - Keep control flow explicit and easy to reason about.