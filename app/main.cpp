#include <iostream>
#include <vector>
#include <cmath>

#include "voice_engine/audio/AudioPreprocessor.h"
#include "voice_engine/audio/IAudioInput.h"
#include "voice_engine/audio/IAudioOutput.h"
#include "voice_engine/core/AudioBuffer.h"
#include "voice_engine/core/VoiceConfig.h"
#include "voice_engine/orchestration/CommandRouter.h"
#include "voice_engine/orchestration/VoiceEngine.h"
#include "voice_engine/stt/SpeechRecognizer.h"
#include "voice_engine/stt/STTConfig.h"
#include "voice_engine/stt/WhisperEngine.h"
#include "voice_engine/tts/ITTSEngine.h"
#include "voice_engine/tts/SpeechSynthesizer.h"

using namespace voice_engine;

// ======================================================
// MOCK AUDIO INPUT (UPDATED)
// ======================================================

class MockAudioInput : public audio::IAudioInput
{
public:
    bool initialize(const core::VoiceConfig&) override
    {
        m_initialized = true;
        m_lastError = {};
        return true;
    }

    [[nodiscard]] bool isInitialized() const noexcept override
    {
        return m_initialized;
    }

    bool startCapture() override
    {
        if (!m_initialized)
        {
            m_lastError = {};
            return false;
        }

        m_capturing = true;
        return true;
    }

    void stopCapture() override
    {
        m_capturing = false;
    }

    [[nodiscard]] bool isCapturing() const noexcept override
    {
        return m_capturing;
    }

    [[nodiscard]] core::AudioBuffer captureOnce() override
    {
        if (!m_initialized || !m_capturing)
        {
            m_lastError = {};
            return {};
        }

        core::AudioFormat format{};
        format.sampleRate = 16000;
        format.channels = 1;
        format.format = core::SampleFormat::Float32;

        constexpr int sampleRate = 16000;
        constexpr float durationSeconds = 1.0f;
        const std::size_t totalSamples =
            static_cast<std::size_t>(sampleRate * durationSeconds);

        std::vector<core::Sample> samples;
        samples.reserve(totalSamples);

        constexpr float amplitude = 0.2f;
        constexpr float frequencyHz = 220.0f;
        constexpr float twoPi = 6.28318530718f;

        for (std::size_t i = 0; i < totalSamples; ++i)
        {
            const float t = static_cast<float>(i) / static_cast<float>(sampleRate);
            const float value = amplitude * std::sin(twoPi * frequencyHz * t);
            samples.push_back(value);
        }

        return core::AudioBuffer(format, std::move(samples));
    }

    void shutdown() override
    {
        m_capturing = false;
        m_initialized = false;
    }

    [[nodiscard]] core::Error lastError() const override
    {
        return m_lastError;
    }

private:
    bool m_initialized{false};
    bool m_capturing{false};
    core::Error m_lastError{};
};

// ======================================================
// MOCK AUDIO OUTPUT
// ======================================================

class MockAudioOutput : public audio::IAudioOutput
{
public:
    bool initialize(const core::VoiceConfig&) override
    {
        m_initialized = true;
        m_lastError = {};
        return true;
    }

    [[nodiscard]] bool isInitialized() const noexcept override
    {
        return m_initialized;
    }

    bool startPlayback() override
    {
        if (!m_initialized)
        {
            m_lastError = {};
            return false;
        }

        m_playing = true;
        return true;
    }

    void stopPlayback() override
    {
        m_playing = false;
    }

    [[nodiscard]] bool isPlaying() const noexcept override
    {
        return m_playing;
    }

    bool play(const core::AudioBuffer& buffer) override
    {
        std::cout
            << "[AUDIO OUTPUT] Playing "
            << buffer.sampleCount()
            << " samples at "
            << buffer.format().sampleRate
            << " Hz, channels="
            << buffer.format().channels
            << std::endl;

        return true;
    }

    void shutdown() override
    {
        m_playing = false;
        m_initialized = false;
    }

    [[nodiscard]] core::Error lastError() const override
    {
        return m_lastError;
    }

private:
    bool m_initialized{false};
    bool m_playing{false};
    core::Error m_lastError{};
};

// ======================================================
// MOCK TTS ENGINE
// ======================================================

class MockTTSEngine : public tts::ITTSEngine
{
public:
    bool initialize(const core::VoiceConfig&) override
    {
        m_initialized = true;
        m_lastError = {};
        return true;
    }

    [[nodiscard]] bool isInitialized() const noexcept override
    {
        return m_initialized;
    }

    [[nodiscard]] tts::SynthesisResult synthesize(
        const tts::SynthesisRequest& request) override
    {
        std::cout << "[TTS OUTPUT] " << request.text << std::endl;

        core::AudioFormat format{};
        format.sampleRate = 22050;
        format.channels = 1;

        std::vector<core::Sample> samples{
            0.2f, 0.1f, -0.1f, 0.0f
        };

        tts::SynthesisResult result;
        result.status = tts::SynthesisStatus::Completed;
        result.audio = core::AudioBuffer(format, std::move(samples));
        return result;
    }

    void shutdown() override
    {
        m_initialized = false;
    }

    [[nodiscard]] core::Error lastError() const override
    {
        return m_lastError;
    }

private:
    bool m_initialized{false};
    core::Error m_lastError{};
};

// ======================================================
// MAIN
// ======================================================

int main()
{
    core::VoiceConfig config{};

    MockAudioInput input;
    MockAudioOutput output;

    input.initialize(config);
    output.initialize(config);

    audio::AudioPreprocessor preprocessor;

    stt::WhisperEngine sttEngine;
    stt::STTConfig sttConfig{};
    sttConfig.modelPath = "E:/Productos/VoiceEngine/models/stt/ggml-base.bin";
    sttConfig.language = "es";
    sttConfig.threads = 4;
    sttConfig.beamSize = 5;
    sttConfig.translateToEnglish = false;

    if (!sttEngine.initialize(sttConfig))
    {
        std::cout << "Failed to initialize WhisperEngine." << std::endl;
        std::cout << "Check model path and model file availability." << std::endl;
        std::cout << "Press Enter to exit...";
        std::cin.get();
        return 1;
    }

    stt::SpeechRecognizer recognizer(preprocessor, sttEngine);

    MockTTSEngine ttsEngine;
    ttsEngine.initialize(config);
    tts::SpeechSynthesizer synthesizer(ttsEngine);

    orchestration::CommandRouter router;

    orchestration::VoiceEngine engine(
        input,
        output,
        recognizer,
        synthesizer,
        router);

    std::cout << "Running VoiceEngine (Whisper mode)..." << std::endl;

    if (!engine.processOnce())
    {
        std::cout << "VoiceEngine failed." << std::endl;
        std::cout << "Error: " << engine.lastError().message << std::endl;
        std::cout << "Press Enter to exit...";
        std::cin.get();
        return 1;
    }

    std::cout << "Done." << std::endl;
    std::cout << "Press Enter to exit...";
    std::cin.get();

    return 0;
}