#include <iostream>
#include <windows.h>

#include "voice_engine/audio/MicAudioInput.h"
#include "voice_engine/audio/AudioPreprocessor.h"
#include "voice_engine/core/AudioBuffer.h"
#include "voice_engine/core/VoiceConfig.h"
#include "voice_engine/stt/SpeechRecognizer.h"
#include "voice_engine/stt/STTConfig.h"
#include "voice_engine/stt/STTTypes.h"
#include "voice_engine/stt/WhisperEngine.h"

using namespace voice_engine;

int main()
{
    SetConsoleOutputCP(CP_UTF8);

    // ======================================================
    // 1. CONFIGURE MIC INPUT
    // ======================================================

    core::VoiceConfig voiceConfig{};
    voiceConfig.audio.inputSampleRate = 16000;
    voiceConfig.audio.inputChannels = 1;
    voiceConfig.audio.inputFramesPerBuffer = 1024;
    voiceConfig.audio.maxInputCaptureDurationMs = 30000;
    voiceConfig.audio.enablePreprocessing = true;

    audio::MicAudioInput micInput;

    if (!micInput.initialize(voiceConfig))
    {
        std::cout << "Failed to initialize microphone input." << std::endl;
        std::cout << "Reason: " << micInput.lastError().message << std::endl;
        std::cout << "Press Enter to exit...";
        std::cin.get();
        return 1;
    }

    if (!micInput.startCapture())
    {
        std::cout << "Failed to start microphone capture." << std::endl;
        std::cout << "Reason: " << micInput.lastError().message << std::endl;
        std::cout << "Press Enter to exit...";
        std::cin.get();
        return 1;
    }

    std::cout << "Recording from microphone..." << std::endl;
    std::cout << "Current max duration: 30 seconds" << std::endl;
    std::cout << "Waiting for captured audio..." << std::endl;

    core::AudioBuffer buffer = micInput.captureOnce();

    if (buffer.empty())
    {
        std::cout << "Microphone capture failed." << std::endl;
        std::cout << "Reason: " << micInput.lastError().message << std::endl;
        std::cout << "Press Enter to exit...";
        std::cin.get();
        return 1;
    }

    std::cout << "Microphone capture completed successfully." << std::endl;
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