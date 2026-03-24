#include <iostream>
#include <windows.h>

#include "voice_engine/audio/WavAudioInput.h"
#include "voice_engine/audio/AudioPreprocessor.h"
#include "voice_engine/core/AudioBuffer.h"
#include "voice_engine/stt/SpeechRecognizer.h"
#include "voice_engine/stt/STTConfig.h"
#include "voice_engine/stt/STTTypes.h"
#include "voice_engine/stt/WhisperEngine.h"

using namespace voice_engine;

int main()
{
    SetConsoleOutputCP(CP_UTF8);

    // ======================================================
    // 1. LOAD WAV
    // ======================================================

    audio::WavAudioInput wavInput;

    const std::string wavPath = "E:/Productos/VoiceEngine/test.wav";

    core::AudioBuffer buffer = wavInput.loadFromFile(wavPath);

    if (buffer.empty())
    {
        std::cout << "Failed to load WAV file." << std::endl;
        std::cout << "Press Enter to exit...";
        std::cin.get();
        return 1;
    }

    std::cout << "WAV loaded successfully" << std::endl;
    std::cout << "Sample rate: " << buffer.format().sampleRate << std::endl;
    std::cout << "Channels: " << buffer.format().channels << std::endl;
    std::cout << "Samples: " << buffer.sampleCount() << std::endl;
    std::cout << "Duration: " << buffer.durationSeconds() << " sec" << std::endl;

    // ======================================================
    // 2. INIT WHISPER
    // ======================================================

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
        std::cout << "Press Enter to exit...";
        std::cin.get();
        return 1;
    }

    audio::AudioPreprocessor preprocessor;
    stt::SpeechRecognizer recognizer(preprocessor, sttEngine);

    // ======================================================
    // 3. RUN STT
    // ======================================================

    const stt::TranscriptionResult result = recognizer.recognize(buffer);

    if (result.status != stt::RecognitionStatus::Completed)
    {
        std::cout << "STT failed." << std::endl;
        std::cout << "Reason: " << result.errorMessage << std::endl;
        std::cout << "Press Enter to exit...";
        std::cin.get();
        return 1;
    }

    std::cout << "\n========== TRANSCRIPTION ==========\n";
    std::cout << result.fullText << std::endl;
    std::cout << "==================================\n";

    std::cout << "Press Enter to exit...";
    std::cin.get();

    return 0;
}