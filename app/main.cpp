
#include <iostream>
#include <thread>
#include <atomic>
#include <limits>

#define NOMINMAX
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

// ======================================================
// runSttDemo
// ======================================================
//
// Speech-to-Text isolated demo entry point.
//
// Architecture role
// -----------------
// Demo / validation entry point.
//
// This function executes the current microphone-based
// speech-to-text validation path for VoiceEngine.
//
// Responsibilities
// ----------------
// This function is responsible for:
// - configuring microphone-based audio capture
// - initializing MicAudioInput
// - starting microphone capture
// - allowing the user to stop the recording early
// - collecting the final AudioBuffer
// - initializing WhisperEngine
// - invoking SpeechRecognizer
// - printing the resulting transcription
//
// Non-responsibilities
// --------------------
// This function MUST NOT:
// - contain reusable production orchestration logic
// - integrate Text-to-Speech
// - integrate command routing or response logic
// - become the permanent application flow
//
// Design notes
// ------------
// - This function intentionally isolates the STT path.
// - The purpose is to validate:
//   mic -> buffer -> STT -> text
// - The early-stop thread exists only for demo control.
// - If capture ends naturally before Enter is pressed,
//   the stop thread must not block the rest of the flow.
//

void runSttDemo()
{
    // ======================================================
    // 1. CONFIGURE MICROPHONE INPUT
    // ======================================================
    //
    // This block defines the audio capture configuration
    // used by MicAudioInput during this demo run.
    //
    // Current choices:
    // - 16000 Hz
    // - mono
    // - 2048 frames per buffer
    // - max capture duration = 8 seconds
    //
    // These values are chosen to remain aligned with
    // Whisper expectations while keeping the demo responsive.
    //

    core::VoiceConfig voiceConfig{};
    voiceConfig.audio.inputSampleRate = 16000;
    voiceConfig.audio.inputChannels = 1;
    voiceConfig.audio.inputFramesPerBuffer = 2048;
    voiceConfig.audio.maxInputCaptureDurationMs = 8000;
    voiceConfig.audio.enablePreprocessing = true;

    // ======================================================
    // 2. CREATE MIC INPUT OBJECT
    // ======================================================
    //
    // MicAudioInput is the high-level audio input module
    // responsible for:
    // - managing a batch capture session
    // - accumulating chunks from AudioInputDevice
    // - returning a final AudioBuffer
    //

    audio::MicAudioInput micInput;

    // ======================================================
    // 3. INITIALIZE MIC INPUT
    // ======================================================
    //
    // At this point, the microphone capture path is prepared.
    // If initialization fails, we print the explicit error
    // message exposed by MicAudioInput and abort the demo run.
    //

    if (!micInput.initialize(voiceConfig))
    {
        std::cout << "Failed to initialize microphone input." << std::endl;
        std::cout << "Reason: " << micInput.lastError().message << std::endl;
        return;
    }

    // ======================================================
    // 4. START CAPTURE SESSION
    // ======================================================
    //
    // This transitions MicAudioInput into capturing state.
    // Internally this should open and start the underlying
    // PortAudio input stream through AudioInputDevice.
    //

    if (!micInput.startCapture())
    {
        std::cout << "Failed to start microphone capture." << std::endl;
        std::cout << "Reason: " << micInput.lastError().message << std::endl;
        return;
    }

    // ======================================================
    // 5. USER FEEDBACK
    // ======================================================
    //
    // This block prints explicit status information so the user
    // understands:
    // - that microphone capture has started
    // - that the max duration is bounded
    // - that pressing Enter may stop the capture early
    //

    std::cout << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << " STT DEMO - MICROPHONE CAPTURE" << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << "Recording from microphone..." << std::endl;
    std::cout << "Current max duration: 8 seconds" << std::endl;
    std::cout << "Press Enter to stop recording early." << std::endl;
    std::cout << "Waiting for captured audio..." << std::endl;
    std::cout << std::endl;

    // ======================================================
    // 6. EARLY STOP CONTROL THREAD
    // ======================================================
    //
    // Why this exists
    // ---------------
    // captureOnce() is synchronous and blocking.
    //
    // That means if we want the user to be able to stop
    // recording early, we need another execution path that can
    // listen for Enter while captureOnce() is running.
    //
    // Important behavior
    // ------------------
    // If the user presses Enter before capture ends naturally:
    // - micInput.stopCapture() is called
    //
    // If capture ends naturally before Enter is pressed:
    // - the thread must NOT block the flow
    // - the thread must NOT call stopCapture() after capture
    //   already finished
    //
    // To support that behavior:
    // - we use an atomic flag
    // - we mark completion after captureOnce() returns
    // - we detach the thread instead of joining it
    //
    // This keeps the demo from hanging on std::cin.get()
    // when capture finishes by timeout.
    //

    std::atomic<bool> stopThreadFinished{false};

    std::thread stopThread([&micInput, &stopThreadFinished]()
    {
        std::cin.get();

        if (!stopThreadFinished)
        {
            micInput.stopCapture();
        }

        stopThreadFinished = true;
    });

    // ======================================================
    // 7. CAPTURE AUDIO BUFFER
    // ======================================================
    //
    // This is the main blocking STT input step.
    //
    // MicAudioInput::captureOnce():
    // - reads chunks from AudioInputDevice
    // - accumulates them
    // - stops when:
    //   - max duration is reached
    //   - or stopCapture() is requested
    // - returns the final AudioBuffer
    //

    core::AudioBuffer buffer = micInput.captureOnce();

    // ======================================================
    // 8. SIGNAL CAPTURE COMPLETION TO STOP THREAD
    // ======================================================
    //
    // Once captureOnce() returns, we know the capture session is
    // finished, whether:
    // - naturally by timeout
    // - or early by user stop
    //
    // We set the atomic flag so the stop thread knows that
    // capture is already over.
    //

    stopThreadFinished = true;

    // ======================================================
    // 9. HANDLE STOP THREAD WITHOUT BLOCKING
    // ======================================================
    //
    // Why NOT join()
    // --------------
    // If the user never pressed Enter, the stop thread may still
    // be blocked on std::cin.get().
    //
    // Joining such a thread would block the demo flow and create
    // an unnecessary hang.
    //
    // Why detach()
    // ------------
    // This is a console demo. The thread is short-lived and tied
    // to process lifetime. Detaching is acceptable here because:
    // - it avoids blocking
    // - the thread only waits for input
    // - it no longer mutates capture state after the atomic flag
    //   marks completion
    //
    // In a production UI flow, this control path would likely be
    // redesigned with non-blocking input or event-driven input.
    //

    if (stopThread.joinable())
    {
        stopThread.detach();
    }

    // ======================================================
    // 10. VALIDATE CAPTURE RESULT
    // ======================================================
    //
    // If capture failed or returned no samples, abort the STT
    // demo before touching Whisper.
    //

    if (buffer.empty())
    {
        std::cout << "Microphone capture failed." << std::endl;
        std::cout << "Reason: " << micInput.lastError().message << std::endl;
        return;
    }

    // ======================================================
    // 11. PRINT CAPTURE METADATA
    // ======================================================
    //
    // This is useful for validation:
    // - sample rate
    // - channel count
    // - number of samples
    // - resulting duration
    //

    std::cout << "Microphone capture completed successfully." << std::endl;
    std::cout << "Sample rate: " << buffer.format().sampleRate << std::endl;
    std::cout << "Channels: " << buffer.format().channels << std::endl;
    std::cout << "Samples: " << buffer.sampleCount() << std::endl;
    std::cout << "Duration: " << buffer.durationSeconds() << " sec" << std::endl;

    // ======================================================
    // 12. CREATE AND CONFIGURE WHISPER ENGINE
    // ======================================================
    //
    // This block prepares the local Whisper backend.
    //
    // Current configuration:
    // - model: ggml-base
    // - language: es
    // - threads: 4
    // - beamSize: 5
    // - no translation to English
    //

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
        return;
    }

    // ======================================================
    // 13. CREATE SPEECH RECOGNIZER
    // ======================================================
    //
    // SpeechRecognizer is responsible for:
    // - applying the current audio preprocessing path
    // - validating the prepared buffer
    // - delegating actual transcription to WhisperEngine
    //

    audio::AudioPreprocessor preprocessor;
    stt::SpeechRecognizer recognizer(preprocessor, sttEngine);

    // ======================================================
    // 14. RUN STT
    // ======================================================
    //
    // This is the actual transcription step:
    // AudioBuffer -> SpeechRecognizer -> Whisper -> Text
    //

    const stt::TranscriptionResult result = recognizer.recognize(buffer);

    // ======================================================
    // 15. VALIDATE STT RESULT
    // ======================================================
    //
    // If Whisper failed or recognition did not complete,
    // print the returned error message and abort the demo.
    //

    if (result.status != stt::RecognitionStatus::Completed)
    {
        std::cout << "STT failed." << std::endl;
        std::cout << "Reason: " << result.errorMessage << std::endl;
        return;
    }

    // ======================================================
    // 16. PRINT TRANSCRIPTION
    // ======================================================
    //
    // Final result of the isolated STT demo.
    //

    std::cout << std::endl;
    std::cout << "========== TRANSCRIPTION ==========" << std::endl;
    std::cout << result.fullText << std::endl;
    std::cout << "===================================" << std::endl;
}

// ======================================================
// runTtsDemo
// ======================================================
//
// Text-to-Speech isolated demo entry point.
//
// Architecture role
// -----------------
// Demo / validation entry point.
//
// Responsibilities
// ----------------
// This function is responsible for:
// - acting as the dedicated TTS test entry
// - keeping the future TTS path isolated from STT
//
// Non-responsibilities
// --------------------
// This function MUST NOT:
// - synthesize audio before the backend exists
// - integrate STT
// - integrate logic / command routing
//
// Design notes
// ------------
// - This path is intentionally separate.
// - It will later be replaced by a real TTS test flow.
//

void runTtsDemo()
{
    std::cout << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << " TTS DEMO" << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << "TTS demo is not implemented yet." << std::endl;
    std::cout << "Next step: integrate local TTS backend." << std::endl;
}

// ======================================================
// showMenu
// ======================================================
//
// Prints the current VoiceEngine demo menu.
//
// Responsibilities
// ----------------
// This function is responsible for:
// - displaying the available demo modes
// - making the current separation between STT and TTS explicit
//
// Design notes
// ------------
// - The menu is intentionally minimal.
// - It exists to isolate testing paths while the engine grows.
//

void showMenu()
{
    std::cout << "==================================" << std::endl;
    std::cout << " VoiceEngine Test Menu" << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << "1 - Speech to Text (Microphone)" << std::endl;
    std::cout << "2 - Text to Speech" << std::endl;
    std::cout << "0 - Exit" << std::endl;
    std::cout << "Select option: ";
}

// ======================================================
// readMenuOption
// ======================================================
//
// Reads and sanitizes the selected menu option.
//
// Responsibilities
// ----------------
// This function is responsible for:
// - reading the integer option from stdin
// - cleaning invalid input state
// - consuming the pending newline after numeric input
//
// Non-responsibilities
// --------------------
// This function MUST NOT:
// - execute any demo path
// - contain business logic
//

int readMenuOption()
{
    int option = -1;

    if (!(std::cin >> option))
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return -1;
    }

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return option;
}

// ======================================================
// main
// ======================================================
//
// Current temporary demo launcher for VoiceEngine.
//
// Architecture role
// -----------------
// Demo launcher / test selector.
//
// This function is responsible for:
// - initializing UTF-8 console output
// - displaying the current test menu
// - dispatching the user-selected demo path
//
// Non-responsibilities
// --------------------
// This function MUST NOT:
// - act as the final integrated pipeline
// - contain the permanent interaction loop
// - mix STT and TTS test flows prematurely
//
// Design notes
// ------------
// - The current separation is intentional.
// - STT and TTS are being validated independently first.
// - Later, once both halves are stable, a higher-level
//   orchestration flow can replace this menu.
//

int main()
{
    SetConsoleOutputCP(CP_UTF8);

    // ======================================================
    // 1. SHOW MENU
    // ======================================================

    showMenu();

    // ======================================================
    // 2. READ OPTION
    // ======================================================

    const int option = readMenuOption();

    // ======================================================
    // 3. DISPATCH SELECTED DEMO PATH
    // ======================================================

    switch (option)
    {
        case 1:
            runSttDemo();
            break;

        case 2:
            runTtsDemo();
            break;

        case 0:
            std::cout << "Exiting..." << std::endl;
            break;

        default:
            std::cout << "Invalid option." << std::endl;
            break;
    }

    // ======================================================
    // 4. HOLD CONSOLE OPEN
    // ======================================================

    std::cout << std::endl;
    std::cout << "Press Enter to exit...";
    std::cin.get();

    return 0;
}