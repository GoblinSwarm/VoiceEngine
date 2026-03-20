// src/orchestration/VoiceEngine.cpp
// =================================
//
// VoiceEngine
// -----------
//
// Runtime implementation for the high-level VoiceEngine façade.
//
// Architecture role
// -----------------
// Orchestration layer.
//
// This module contains the concrete implementation of the main VoiceEngine
// system boundary. It coordinates the interaction between the major internal
// subsystems, including:
//
// - audio input/output
// - speech recognition
// - command interpretation
// - speech synthesis
//
// It provides the runtime behavior behind the top-level API exposed by
// VoiceEngine.h and serves as the primary internal composition point for
// the system's voice interaction flow.
//
// Typical flow
// ------------
// external caller / application
//        ↓
// VoiceEngine
//        ↓
// SpeechRecognizer
//        ↓
// CommandRouter
//        ↓
// SpeechSynthesizer
//        ↓
// Audio output
//
// This module is responsible ONLY for:
// - implementing high-level orchestration behavior across subsystems
// - coordinating recognition, interpretation, and synthesis flow
// - exposing stable runtime behavior through the VoiceEngine façade
//
// Non-responsibilities
// --------------------
// This module MUST NOT:
// - implement STT or TTS provider-specific logic
// - implement low-level audio backend behavior
// - absorb all system logic into a monolithic god object
// - replace specialized modules that already own narrower responsibilities
//
// Design notes
// ------------
// - Favor delegation over direct implementation.
// - Keep orchestration readable and explicit.
// - This file should compose subsystems, not replace them.
// - Maintain clear boundaries so VoiceEngine remains a façade, not a dumping ground.