// src/stt/WhisperEngine.cpp
// =========================
//
// WhisperEngine
// -------------
//
// Concrete whisper.cpp-based speech-to-text engine implementation.
//
// Architecture role
// -----------------
// STT layer (engine implementation).
//
// This module owns the concrete lifecycle and execution path for
// whisper.cpp-based transcription.
//
// This module is responsible ONLY for:
// - creating and destroying the Whisper context
// - loading the configured Whisper model
// - executing batch transcription on prepared audio buffers
// - translating backend execution results into TranscriptionResult
//
// Non-responsibilities
// --------------------
// This module MUST NOT:
// - capture audio
// - convert audio formats or resample input
// - manage streaming/incremental recognition
// - emit partial results
// - contain orchestration or UI logic
//
// Input contract
// --------------
// This engine expects:
//
// - mono PCM audio
// - normalized float samples in range [-1.0, 1.0]
// - sample rate compatible with Whisper (typically 16 kHz)
//
// Any required conversion must be completed before calling transcribe().
//

#include "voice_engine/stt/WhisperEngine.h"

#include <utility>

// whisper.cpp
#include "whisper.h"

namespace voice_engine::stt
{
namespace
{
    whisper_context* toWhisperContext(void* rawContext)
    {
        return static_cast<whisper_context*>(rawContext);
    }

    RecognitionStatus makeFailureStatus()
    {
        return RecognitionStatus::Failed;
    }

    TranscriptionResult makeErrorResult(const std::string& message)
    {
        TranscriptionResult result{};
        result.status = makeFailureStatus();
        result.errorMessage = message;
        return result;
    }
} // namespace

WhisperEngine::~WhisperEngine()
{
    shutdown();
}

bool WhisperEngine::initialize(const STTConfig& config)
{
    shutdown();

    if (config.modelPath.empty())
    {
        return false;
    }

    whisper_context* context = whisper_init_from_file(config.modelPath.c_str());
    if (context == nullptr)
    {
        return false;
    }

    m_context = context;
    m_config = config;
    m_initialized = true;

    return true;
}

void WhisperEngine::shutdown()
{
    whisper_context* context = toWhisperContext(m_context);
    if (context != nullptr)
    {
        whisper_free(context);
    }

    m_context = nullptr;
    m_initialized = false;
    m_config = STTConfig{};
}

bool WhisperEngine::isInitialized() const
{
    return m_initialized;
}

TranscriptionResult WhisperEngine::transcribe(const std::vector<float>& audioBuffer)
{
    if (!m_initialized)
    {
        return makeErrorResult("WhisperEngine is not initialized.");
    }

    if (audioBuffer.empty())
    {
        return makeErrorResult("Audio buffer is empty.");
    }

    whisper_context* context = toWhisperContext(m_context);
    if (context == nullptr)
    {
        return makeErrorResult("Whisper context is null.");
    }

    TranscriptionResult result{};
    result.language = m_config.language;

    whisper_full_params params = whisper_full_default_params(WHISPER_SAMPLING_BEAM_SEARCH);

    params.print_realtime = false;
    params.print_progress = false;
    params.print_timestamps = false;
    params.print_special = false;

    params.translate = m_config.translateToEnglish;
    params.n_threads = m_config.threads > 0 ? m_config.threads : 1;
    params.beam_search.beam_size = m_config.beamSize > 0 ? m_config.beamSize : 1;

    if (!m_config.language.empty() && m_config.language != "auto")
    {
        params.language = m_config.language.c_str();
    }

    const int runResult = whisper_full(
        context,
        params,
        audioBuffer.data(),
        static_cast<int>(audioBuffer.size())
    );

    if (runResult != 0)
    {
        result.status = RecognitionStatus::Failed;
        result.errorMessage = "whisper_full failed.";
        return result;
    }

    const int segmentCount = whisper_full_n_segments(context);
    std::string fullText{};

    for (int index = 0; index < segmentCount; ++index)
    {
        const char* segmentText = whisper_full_get_segment_text(context, index);
        if (segmentText != nullptr)
        {
            if (!fullText.empty())
            {
                fullText += " ";
            }

            fullText += segmentText;
        }
    }

    result.fullText = std::move(fullText);
    result.status = RecognitionStatus::Completed;
    result.errorMessage.clear();

    return result;
}

} // namespace voice_engine::stt