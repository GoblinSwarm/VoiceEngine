// src/audio/AudioOutputDevice.cpp
// ===============================
//
// AudioOutputDevice
// -----------------
//
// Runtime implementation for a concrete audio output device.
//
// Architecture role
// -----------------
// Audio layer.
//
// This module provides the concrete implementation of the audio output side
// of VoiceEngine. It is responsible for interacting with a real audio playback
// backend and exposing outgoing audio through the IAudioOutput contract.
//
// It acts as the system-facing adapter between VoiceEngine audio-domain data
// and low-level playback mechanisms such as speakers or other output targets.
//
// Typical flow
// ------------
// AudioBuffer / synthesized audio
//        ↓
// AudioOutputDevice
//        ↓
// backend playback API / physical output device
//
// This module is responsible ONLY for:
// - implementing real audio playback behavior
// - adapting VoiceEngine audio buffers to backend/device output requirements
// - managing the lifecycle of the concrete output device at runtime
//
// Non-responsibilities
// --------------------
// This module MUST NOT:
// - perform TTS synthesis
// - decide what audio should be played
// - perform high-level orchestration
// - contain reusable audio preprocessing logic
//
// Design notes
// ------------
// - Keep backend-specific playback logic isolated in this module.
// - Avoid leaking external audio backend details outside the audio layer.
// - Device lifecycle should remain explicit and predictable.
// - This module should focus on playback, not on synthesis or orchestration.