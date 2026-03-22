@echo off
echo =====================================
echo   VoiceEngine Build & Run Script
echo =====================================

cd /d E:\Productos\VoiceEngine

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
    echo ❌ CMake generation failed
    pause
    exit /b %errorlevel%
)

echo.
echo [4/5] Building project...
cmake --build . --config Debug
if %errorlevel% neq 0 (
    echo.
    echo ❌ Build failed
    pause
    exit /b %errorlevel%
)

echo.
echo [5/5] Running executable...
if exist Debug\VoiceEngine.exe (
    echo.
    echo -------------------------------------
    echo Running VoiceEngine...
    echo -------------------------------------
    Debug\VoiceEngine.exe
) else (
    echo.
    echo ❌ Executable not found
)

echo.
echo =====================================
echo   Done
echo =====================================
pause