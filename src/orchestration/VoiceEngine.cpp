// src/orchestration/VoiceEngine.cpp
// =================================

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

bool VoiceEngine::processOnce()
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
            return false;
        }
    }

    if (!m_audioOutput.isPlaying())
    {
        if (!m_audioOutput.startPlayback())
        {
            m_lastError = m_audioOutput.lastError();
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
        m_speechRecognizer.recognize(capturedAudio);

    if (transcription.status != stt::RecognitionStatus::Completed)
    {
        m_lastError = {
            core::ErrorCode::STTInferenceFailed,
            transcription.errorMessage.empty()
                ? "Speech recognition failed."
                : transcription.errorMessage
        };
        return false;
    }

    const CommandRouteResult routeResult = m_commandRouter.route(transcription);

    if (!routeResult.success)
    {
        m_lastError = m_commandRouter.lastError();
        return false;
    }

    if (routeResult.action == CommandAction::None)
    {
        return true;
    }

    if (routeResult.action == CommandAction::ExecuteCommand)
    {
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