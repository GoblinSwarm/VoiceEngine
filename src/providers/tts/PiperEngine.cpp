// src/providers/tts/PiperEngine.cpp
// =================================
//
// PiperEngine
// -----------
//
// Runtime implementation for the Piper-based TTS provider.
//
// Architecture role
// -----------------
// Provider layer (TTS).
//
// This module contains the concrete implementation of the ITTSEngine
// contract using Piper as an external CLI text-to-speech backend.
//
// It acts as the runtime integration boundary between VoiceEngine and
// `piper.exe`, adapting internal text-domain requests into Piper-compatible
// synthesis input and translating provider output back into VoiceEngine
// audio-domain types.
//
// Typical flow
// ------------
// text input
//        ↓
// PiperEngine
//        ↓
// piper.exe (external process)
//        ↓
// generated wav output
//        ↓
// AudioBuffer / SynthesisResult
//
// This module is responsible ONLY for:
// - implementing the ITTSEngine contract using Piper
// - adapting VoiceEngine synthesis requests to Piper-compatible CLI input
// - invoking the Piper CLI runtime and collecting synthesis output
// - mapping Piper-generated output into stable VoiceEngine TTS domain types
//
// Non-responsibilities
// --------------------
// This module MUST NOT:
// - decide what text should be spoken
// - coordinate the full TTS pipeline
// - play audio through output devices
// - leak Piper-specific CLI/process details outside the provider boundary
//
// Design notes
// ------------
// - Keep all Piper-specific logic isolated in this module.
// - This file behaves as an adapter, not as a high-level synthesizer.
// - Provider output is normalized into VoiceEngine domain structures.
// - This first implementation integrates Piper through its CLI runtime.
//

#include "voice_engine/providers/tts/PiperEngine.h"

#include <cctype>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <system_error>

#include "voice_engine/audio/WavAudioInput.h"

namespace fs = std::filesystem;

namespace voice_engine::providers::tts
{

namespace
{

core::Error makeError(core::ErrorCode code, const char* message)
{
    return core::Error{code, message};
}

bool isBlankText(const std::string& text)
{
    for (unsigned char ch : text)
    {
        if (!std::isspace(ch))
        {
            return false;
        }
    }

    return true;
}

std::string quotePath(const fs::path& path)
{
    return "\"" + path.string() + "\"";
}

fs::path buildUniqueTempPath(const std::string& filename)
{
    const auto now =
        std::chrono::high_resolution_clock::now().time_since_epoch().count();

    fs::path tempDir = fs::temp_directory_path() / "voiceengine_piper";
    std::error_code ec;
    fs::create_directories(tempDir, ec);

    return tempDir / (std::to_string(now) + "_" + filename);
}

bool writeTextFileUtf8(const fs::path& path, const std::string& text)
{
    std::error_code ec;
    const fs::path parent = path.parent_path();

    if (!parent.empty())
    {
        fs::create_directories(parent, ec);
        if (ec)
        {
            return false;
        }
    }

    std::ofstream file(path, std::ios::binary);
    if (!file.is_open())
    {
        return false;
    }

    file.write(text.data(), static_cast<std::streamsize>(text.size()));
    file.flush();

    return file.good();
}

void removeFileIfExists(const fs::path& path)
{
    std::error_code ec;
    fs::remove(path, ec);
}

fs::path resolvePiperExecutable()
{
    const fs::path cwd = fs::current_path();

    const fs::path candidates[] =
    {
        cwd / "piper.exe",
        cwd / "external" / "piper" / "piper.exe",
        cwd / "external" / "piper" / "build" / "piper.exe",
        cwd / "external" / "piper" / "build" / "src" / "cpp" / "piper.exe"
    };

    for (const auto& candidate : candidates)
    {
        if (fs::exists(candidate))
        {
            return candidate;
        }
    }

    // Fallback: rely on PATH
    return fs::path("piper.exe");
}

double clampSpeechRateToLengthScale(float speechRate)
{
    // Piper CLI uses length-scale rather than "speech rate".
    // Smaller values generally speak faster, larger values slower.
    // We keep the mapping intentionally conservative.
    if (speechRate <= 0.0f)
    {
        return 1.0;
    }

    const double mapped = 1.0 / static_cast<double>(speechRate);

    if (mapped < 0.5)
    {
        return 0.5;
    }

    if (mapped > 2.0)
    {
        return 2.0;
    }

    return mapped;
}

class ScopedCurrentPathGuard
{
public:
    ScopedCurrentPathGuard() = default;

    explicit ScopedCurrentPathGuard(const fs::path& newPath)
    {
        if (newPath.empty())
        {
            return;
        }

        std::error_code ec;
        m_originalPath = fs::current_path(ec);
        if (ec)
        {
            return;
        }

        fs::current_path(newPath, ec);
        if (!ec)
        {
            m_active = true;
        }
    }

    ~ScopedCurrentPathGuard()
    {
        if (!m_active)
        {
            return;
        }

        std::error_code ec;
        fs::current_path(m_originalPath, ec);
    }

    ScopedCurrentPathGuard(const ScopedCurrentPathGuard&) = delete;
    ScopedCurrentPathGuard& operator=(const ScopedCurrentPathGuard&) = delete;

private:
    fs::path m_originalPath{};
    bool m_active{false};
};

} // namespace

PiperEngine::PiperEngine() = default;

PiperEngine::~PiperEngine()
{
    shutdown();
}

bool PiperEngine::initialize(const core::VoiceConfig& config)
{
    if (m_initialized)
    {
        m_lastError = makeError(
            core::ErrorCode::AlreadyInitialized,
            "PiperEngine is already initialized."
        );
        return false;
    }

    if (!validateConfig(config))
    {
        m_lastError = makeError(
            core::ErrorCode::InvalidConfiguration,
            "Invalid Piper TTS configuration."
        );
        return false;
    }

    m_config = config;
    m_initialized = true;
    m_lastError = core::Error::ok();

    return true;
}

bool PiperEngine::isInitialized() const noexcept
{
    return m_initialized;
}

voice_engine::tts::SynthesisResult PiperEngine::synthesize(
    const voice_engine::tts::SynthesisRequest& request)
{
    using voice_engine::tts::SynthesisResult;
    using voice_engine::tts::SynthesisStatus;

    SynthesisResult result{};
    result.inputText = request.text;
    result.status = SynthesisStatus::Failed;
    result.errorMessage.clear();
    result.outputFilePath.clear();

    if (!m_initialized)
    {
        m_lastError = makeError(
            core::ErrorCode::NotInitialized,
            "PiperEngine must be initialized before synthesis."
        );
        result.errorMessage = m_lastError.message;
        return result;
    }

    if (!validateRequest(request))
    {
        m_lastError = makeError(
            core::ErrorCode::InvalidConfiguration,
            "Invalid synthesis request."
        );
        result.status = SynthesisStatus::InvalidInput;
        result.errorMessage = m_lastError.message;
        return result;
    }

    fs::path piperExe =
        !m_config.tts.executablePath.empty()
            ? fs::path(m_config.tts.executablePath)
            : resolvePiperExecutable();

    const fs::path modelPath = fs::path(m_config.tts.modelPath);
    const fs::path configPath = fs::path(m_config.tts.configPath);
    const fs::path workingDirectory = fs::path(m_config.tts.workingDirectory);

    if (!fs::exists(piperExe) && piperExe.filename() != "piper.exe")
    {
        m_lastError = makeError(
            core::ErrorCode::InvalidConfiguration,
            "Piper executable does not exist."
        );
        result.errorMessage = m_lastError.message;
        return result;
    }

    if (!fs::exists(modelPath))
    {
        m_lastError = makeError(
            core::ErrorCode::InvalidConfiguration,
            "Piper model file does not exist."
        );
        result.errorMessage = m_lastError.message;
        return result;
    }

    if (!configPath.empty() && !fs::exists(configPath))
    {
        m_lastError = makeError(
            core::ErrorCode::InvalidConfiguration,
            "Piper config file does not exist."
        );
        result.errorMessage = m_lastError.message;
        return result;
    }

    if (!workingDirectory.empty() && !fs::exists(workingDirectory))
    {
        m_lastError = makeError(
            core::ErrorCode::InvalidConfiguration,
            "Piper working directory does not exist."
        );
        result.errorMessage = m_lastError.message;
        return result;
    }

    const fs::path inputTextPath = buildUniqueTempPath("tts_input.txt");

    const bool keepOutputFile = !request.outputFilePath.empty();
    const fs::path outputWavPath =
        keepOutputFile
            ? fs::path(request.outputFilePath)
            : buildUniqueTempPath("tts_output.wav");

    std::error_code ec;
    if (!outputWavPath.parent_path().empty())
    {
        fs::create_directories(outputWavPath.parent_path(), ec);
        if (ec)
        {
            m_lastError = makeError(
                core::ErrorCode::InvalidState,
                "Failed to create Piper output directory."
            );
            result.errorMessage = m_lastError.message;
            return result;
        }
    }

    if (!writeTextFileUtf8(inputTextPath, request.text))
    {
        m_lastError = makeError(
            core::ErrorCode::InvalidState,
            "Failed to create Piper input text file."
        );
        result.errorMessage = m_lastError.message;
        return result;
    }

    // Build CLI command:
    //
    // cmd.exe /C type "input.txt" | "piper.exe" --model "voice.onnx"
    //     --config "voice.onnx.json" --length-scale 1.0
    //     --output_file "output.wav"
    //
    // Notes:
    // - text is fed through stdin using `type`
    // - modelPath and configPath come from VoiceConfig
    // - request.voiceName is not yet mapped in this first integration
    // - request.volume is not applied here because Piper CLI synthesizes audio;
    //   output gain can be handled later in playback or post-processing

    std::ostringstream command;
    command << "cmd.exe /C ";
    command << "type " << quotePath(inputTextPath) << " | ";
    command << quotePath(piperExe);
    command << " --model " << quotePath(modelPath);

    if (!configPath.empty())
    {
        command << " --config " << quotePath(configPath);
    }

    command << " --length-scale " << clampSpeechRateToLengthScale(request.speechRate);
    command << " --output_file " << quotePath(outputWavPath);

    {
        ScopedCurrentPathGuard cwdGuard(workingDirectory);
        const int exitCode = std::system(command.str().c_str());

        removeFileIfExists(inputTextPath);

        if (exitCode != 0)
        {
            if (!keepOutputFile)
            {
                removeFileIfExists(outputWavPath);
            }

            m_lastError = makeError(
                core::ErrorCode::InvalidState,
                "Piper CLI invocation failed."
            );
            result.errorMessage = m_lastError.message;
            return result;
        }
    }

    if (!fs::exists(outputWavPath))
    {
        m_lastError = makeError(
            core::ErrorCode::InvalidState,
            "Piper synthesis completed without producing an output WAV file."
        );
        result.errorMessage = m_lastError.message;
        return result;
    }

    audio::WavAudioInput wavLoader;
    core::AudioBuffer audioBuffer = wavLoader.loadFromFile(outputWavPath.string());

    if (audioBuffer.empty())
    {
        if (!keepOutputFile)
        {
            removeFileIfExists(outputWavPath);
        }

        m_lastError = makeError(
            core::ErrorCode::InvalidState,
            "Failed to load synthesized WAV output from Piper."
        );
        result.errorMessage = m_lastError.message;
        return result;
    }

    result.audio = std::move(audioBuffer);
    result.status = SynthesisStatus::Completed;
    result.errorMessage.clear();

    if (keepOutputFile)
    {
        result.outputFilePath = outputWavPath.string();
    }
    else
    {
        removeFileIfExists(outputWavPath);
        result.outputFilePath.clear();
    }

    m_lastError = core::Error::ok();
    return result;
}

void PiperEngine::shutdown()
{
    m_initialized = false;
    m_lastError = core::Error::ok();
}

core::Error PiperEngine::lastError() const
{
    return m_lastError;
}

bool PiperEngine::validateConfig(const core::VoiceConfig& config) const
{
    if (config.tts.modelPath.empty())
    {
        return false;
    }

    if (!config.tts.executablePath.empty())
    {
        const fs::path exePath(config.tts.executablePath);
        if (!fs::exists(exePath))
        {
            return false;
        }
    }

    if (!config.tts.configPath.empty())
    {
        const fs::path jsonPath(config.tts.configPath);
        if (!fs::exists(jsonPath))
        {
            return false;
        }
    }

    if (!config.tts.workingDirectory.empty())
    {
        const fs::path workingDir(config.tts.workingDirectory);
        if (!fs::exists(workingDir) || !fs::is_directory(workingDir))
        {
            return false;
        }
    }

    return true;
}

bool PiperEngine::validateRequest(
    const voice_engine::tts::SynthesisRequest& request) const
{
    if (request.text.empty())
    {
        return false;
    }

    if (isBlankText(request.text))
    {
        return false;
    }

    if (request.speechRate <= 0.0f)
    {
        return false;
    }

    if (request.volume <= 0.0f)
    {
        return false;
    }

    return true;
}

core::Error PiperEngine::makeError(
    core::ErrorCode code,
    const char* message) const
{
    return core::Error{code, message};
}

} // namespace voice_engine::providers::tts