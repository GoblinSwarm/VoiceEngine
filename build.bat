@echo off
setlocal

echo =====================================
echo   VoiceEngine Build Script
echo =====================================

cd /d E:\Productos\VoiceEngine

echo.
echo [0/5] Verifying dependencies...
if not exist external\whisper.cpp\CMakeLists.txt (
    echo.
    echo Missing dependency: external\whisper.cpp
    echo Please clone whisper.cpp into:
    echo E:\Productos\VoiceEngine\external\whisper.cpp
    echo.
    pause
    exit /b 1
)

echo.
echo [1/5] Cleaning build folder...
if exist build (
    rmdir /s /q build
)

echo.
echo [2/5] Creating build folder...
mkdir build
cd build

echo.
echo [3/5] Generating project with CMake...
cmake .. -G "Visual Studio 17 2022" -A x64
if %errorlevel% neq 0 (
    echo.
    echo CMake generation failed
    pause
    exit /b %errorlevel%
)

echo.
echo [4/5] Building project...
cmake --build . --config Debug
if %errorlevel% neq 0 (
    echo.
    echo Build failed
    pause
    exit /b %errorlevel%
)

echo.
echo [5/5] Build completed successfully.

if exist Debug\VoiceEngine.exe (
    echo.
    echo Executable found:
    echo %cd%\Debug\VoiceEngine.exe
    echo.
    echo Run it manually when the app entrypoint is ready.
) else (
    echo.
    echo Build completed, but VoiceEngine.exe was not found.
    echo Check target name and output configuration.
)

echo.
echo =====================================
echo   Done
echo =====================================
pause
endlocal