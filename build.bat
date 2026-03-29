@echo off
setlocal

echo =====================================
echo   VoiceEngine Build Script
echo =====================================

set "ROOT_DIR=%~dp0"
cd /d "%ROOT_DIR%"

echo.
echo [0/6] Verifying dependencies...

REM -------------------------------------
REM Git
REM -------------------------------------
where git >nul 2>nul
if %errorlevel% neq 0 (
    echo.
    echo [ERROR] Git is not installed or not available in PATH.
    echo Please install Git and try again.
    echo.
    pause
    exit /b 1
)

REM -------------------------------------
REM Whisper
REM -------------------------------------
if not exist external\whisper.cpp\CMakeLists.txt (
    echo.
    echo [INFO] whisper.cpp not found. Cloning repository...
    git clone https://github.com/ggml-org/whisper.cpp external\whisper.cpp
    if %errorlevel% neq 0 (
        echo.
        echo [ERROR] Failed to clone whisper.cpp
        pause
        exit /b %errorlevel%
    )
) else (
    echo [OK] whisper.cpp found.
)

REM -------------------------------------
REM PortAudio (auto-clone)
REM -------------------------------------
if not exist external\portaudio\CMakeLists.txt (
    echo.
    echo [INFO] PortAudio not found. Cloning repository...
    git clone https://github.com/PortAudio/portaudio external\portaudio
    if %errorlevel% neq 0 (
        echo.
        echo [ERROR] Failed to clone PortAudio
        pause
        exit /b %errorlevel%
    )
) else (
    echo [OK] PortAudio found.
)

REM -------------------------------------
REM Piper (auto-download)
REM -------------------------------------
if not exist external\piper\piper.exe (
    echo.
    echo [INFO] piper.exe not found. Downloading...

    mkdir external\piper 2>nul

    set "PIPER_URL=https://github.com/rhasspy/piper/releases/latest/download/piper_windows_amd64.zip"
    set "PIPER_ZIP=external\piper\piper.zip"

    powershell -Command "Invoke-WebRequest -Uri '%PIPER_URL%' -OutFile '%PIPER_ZIP%'"
    if %errorlevel% neq 0 (
        echo.
        echo [ERROR] Failed to download Piper
        pause
        exit /b 1
    )

    echo [INFO] Extracting Piper...
    powershell -Command "Expand-Archive -Path '%PIPER_ZIP%' -DestinationPath 'external\piper' -Force"
    if %errorlevel% neq 0 (
        echo.
        echo [ERROR] Failed to extract Piper
        pause
        exit /b 1
    )

    del "%PIPER_ZIP%"

    if not exist external\piper\piper.exe (
        echo.
        echo [ERROR] Piper setup failed (piper.exe not found after extraction)
        pause
        exit /b 1
    )

    echo [OK] Piper installed successfully.
) else (
    echo [OK] piper.exe found.
)

REM -------------------------------------
REM Piper voice (auto-download - sharvard)
REM -------------------------------------
if not exist external\piper\voices\es\es_ES\sharvard\medium\es_ES-sharvard-medium.onnx (
    echo.
    echo [INFO] Piper voice sharvard not found. Downloading...

    mkdir external\piper\voices\es\es_ES\sharvard\medium 2>nul

    set "VOICE_ONNX_URL=https://huggingface.co/rhasspy/piper-voices/resolve/main/es/es_ES/sharvard/medium/es_ES-sharvard-medium.onnx"
    set "VOICE_JSON_URL=https://huggingface.co/rhasspy/piper-voices/resolve/main/es/es_ES/sharvard/medium/es_ES-sharvard-medium.onnx.json"

    powershell -Command "Invoke-WebRequest -Uri '%VOICE_ONNX_URL%' -OutFile 'external\piper\voices\es\es_ES\sharvard\medium\es_ES-sharvard-medium.onnx'"
    if %errorlevel% neq 0 (
        echo.
        echo [ERROR] Failed to download Piper voice .onnx
        pause
        exit /b 1
    )

    powershell -Command "Invoke-WebRequest -Uri '%VOICE_JSON_URL%' -OutFile 'external\piper\voices\es\es_ES\sharvard\medium\es_ES-sharvard-medium.onnx.json'"
    if %errorlevel% neq 0 (
        echo.
        echo [ERROR] Failed to download Piper voice config .json
        pause
        exit /b 1
    )

    echo [OK] Piper voice sharvard installed.
) else (
    echo [OK] Piper voice sharvard found.
)

REM -------------------------------------
REM Whisper model (auto-download)
REM -------------------------------------
if not exist models\stt\ggml-base.bin (
    echo.
    echo [INFO] Whisper model not found. Downloading ggml-base.bin...

    mkdir models\stt 2>nul

    set "WHISPER_MODEL_URL=https://huggingface.co/ggerganov/whisper.cpp/resolve/main/ggml-base.bin"
    set "WHISPER_MODEL_PATH=models\stt\ggml-base.bin"

    powershell -Command "Invoke-WebRequest -Uri '%WHISPER_MODEL_URL%' -OutFile '%WHISPER_MODEL_PATH%'"
    if %errorlevel% neq 0 (
        echo.
        echo [ERROR] Failed to download Whisper model
        pause
        exit /b 1
    )

    echo [OK] Whisper model downloaded successfully.
) else (
    echo [OK] Whisper model found.
)

echo.
echo [1/6] Cleaning build folder...
if exist build (
    rmdir /s /q build
)

echo.
echo [2/6] Creating build folder...
mkdir build
cd build

echo.
echo [3/6] Generating project with CMake...
cmake .. -G "Visual Studio 17 2022" -A x64
if %errorlevel% neq 0 (
    echo.
    echo CMake generation failed
    pause
    exit /b %errorlevel%
)

echo.
echo [4/6] Building project...
cmake --build . --config Debug
if %errorlevel% neq 0 (
    echo.
    echo Build failed
    pause
    exit /b %errorlevel%
)

echo.
echo [5/6] Build completed successfully.

if exist Debug\VoiceEngine.exe (
    echo.
    echo Executable found:
    echo %cd%\Debug\VoiceEngine.exe
    echo.

    choice /c YN /m "Do you want to run VoiceEngine now?"
    if errorlevel 2 goto :end
    if errorlevel 1 (
        echo.
        echo [INFO] Launching VoiceEngine...
        start "" "%cd%\Debug\VoiceEngine.exe"
    )
) else (
    echo.
    echo Build completed, but VoiceEngine.exe was not found.
)

:end
echo.
echo =====================================
echo   Done
echo =====================================
pause
endlocal