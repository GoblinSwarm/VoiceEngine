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

#include "voice_engine/orchestration/VoiceEngine.h"

namespace voice_engine
{
namespace orchestration
{

VoiceEngine::VoiceEngine(
    audio::IAudioInput& audioInput,
    audio::IAudioOutput& audioOutput,
    stt::SpeechRecognizer& speechRecognizer,
    tts::SpeechSynthesizer& speechSynthesizer,
    voice_engine::orchestration::CommandRouter& commandRouter)
    : m_audioInput(audioInput),
      m_audioOutput(audioOutput),
      m_speechRecognizer(speechRecognizer),
      m_speechSynthesizer(speechSynthesizer),
      m_commandRouter(commandRouter)
{
}

bool VoiceEngine::processOnce(const stt::TranscriptionOptions& transcriptionOptions)
{
    m_lastError = core::Error::ok();

    if (!m_audioInput.isInitialized())
    {
        m_lastError = {
            core::ErrorCode::NotInitialized,
            "Audio input is not initialized."
        };
        return false;
    }

    if (!m_audioOutput.isInitialized())
    {
        m_lastError = {
            core::ErrorCode::NotInitialized,
            "Audio output is not initialized."
        };
        return false;
    }

    if (!m_audioInput.isCapturing())
    {
        if (!m_audioInput.startCapture())
        {
            m_lastError = m_audioInput.lastError();
            if (!m_lastError.hasError())
            {
                m_lastError = {
                    core::ErrorCode::AudioCaptureFailed,
                    "Failed to start audio capture."
                };
            }
            return false;
        }
    }

    if (!m_audioOutput.isPlaying())
    {
        if (!m_audioOutput.startPlayback())
        {
            m_lastError = m_audioOutput.lastError();
            if (!m_lastError.hasError())
            {
                m_lastError = {
                    core::ErrorCode::AudioPlaybackFailed,
                    "Failed to start audio playback."
                };
            }
            return false;
        }
    }

    const core::AudioBuffer capturedAudio = m_audioInput.captureOnce();

    if (capturedAudio.empty())
    {
        m_lastError = m_audioInput.lastError();
        if (!m_lastError.hasError())
        {
            m_lastError = {
                core::ErrorCode::AudioBufferEmpty,
                "Captured audio buffer is empty."
            };
        }
        return false;
    }

    const stt::TranscriptionResult transcription =
        m_speechRecognizer.recognize(capturedAudio, transcriptionOptions);

    if (transcription.status != stt::RecognitionStatus::Completed)
    {
        m_lastError = m_speechRecognizer.lastError();
        if (!m_lastError.hasError())
        {
            m_lastError = {
                core::ErrorCode::STTInferenceFailed,
                "Speech recognition did not complete successfully."
            };
        }
        return false;
    }

    const CommandRouteResult routeResult = m_commandRouter.route(transcription);

    if (!routeResult.success)
    {
        m_lastError = m_commandRouter.lastError();
        if (!m_lastError.hasError())
        {
            m_lastError = {
                core::ErrorCode::CommandRoutingFailed,
                "Command routing failed."
            };
        }
        return false;
    }

    if (routeResult.action == CommandAction::None)
    {
        return true;
    }

    if (routeResult.action == CommandAction::ExecuteCommand)
    {
        // Por ahora, VoiceEngine no ejecuta comandos de aplicación directamente.
        // Esta acción se reconoce y se deja lista para una futura capa superior.
        return true;
    }

    if (routeResult.action == CommandAction::SpeakResponse)
    {
        tts::SynthesisRequest request{};
        request.text = routeResult.responseText;

        const tts::SynthesisResult synthesisResult =
            m_speechSynthesizer.synthesize(request);

        if (synthesisResult.status != tts::SynthesisStatus::Completed)
        {
            m_lastError = m_speechSynthesizer.lastError();
            if (!m_lastError.hasError())
            {
                m_lastError = {
                    core::ErrorCode::TTSSynthesisFailed,
                    "Speech synthesis did not complete successfully."
                };
            }
            return false;
        }

        if (synthesisResult.audio.empty())
        {
            m_lastError = {
                core::ErrorCode::AudioBufferEmpty,
                "Synthesized audio buffer is empty."
            };
            return false;
        }

        if (!m_audioOutput.play(synthesisResult.audio))
        {
            m_lastError = m_audioOutput.lastError();
            if (!m_lastError.hasError())
            {
                m_lastError = {
                    core::ErrorCode::AudioPlaybackFailed,
                    "Failed to play synthesized audio."
                };
            }
            return false;
        }

        return true;
    }

    m_lastError = {
        core::ErrorCode::Unknown,
        "Unhandled command action."
    };
    return false;
}

core::Error VoiceEngine::lastError() const
{
    return m_lastError;
}

} // namespace orchestration
} // namespace voice_engine