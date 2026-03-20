// src/core/VoiceConfig.cpp
// ========================
//
// VoiceConfig
// -----------
//
// Runtime implementation for the VoiceEngine configuration model.
//
// Architecture role
// -----------------
// Core layer.
//
// This module provides the concrete implementation of the VoiceConfig
// structure, which centralizes runtime configuration values used across
// the VoiceEngine system.
//
// It is responsible for managing configuration data at runtime, including
// initialization, validation (if applicable), and access patterns for
// system-wide parameters such as audio settings, model paths, and feature flags.
//
// Typical usage
// -------------
// Application bootstrap
//        ↓
// VoiceConfig initialization
//        ↓
// distributed across subsystems (audio, STT, TTS, orchestration)
//
// This module is responsible ONLY for:
// - implementing the runtime behavior of configuration storage
// - providing access to centralized configuration values
// - maintaining a consistent configuration source for the system
//
// Non-responsibilities
// --------------------
// This module MUST NOT:
// - initialize audio devices
// - load STT or TTS models directly
// - perform audio processing
// - orchestrate system behavior
//
// Design notes
// ------------
// - Keep this module focused on configuration data handling only.
// - Avoid embedding operational logic inside configuration.
// - Configuration should describe behavior, not implement it.
// - Ensure values remain consistent and easy to trace across the system.