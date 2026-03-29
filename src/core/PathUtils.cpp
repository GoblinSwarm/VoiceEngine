#include "voice_engine/core/PathUtils.h"

#define NOMINMAX
#include <windows.h>

namespace fs = std::filesystem;

namespace voice_engine::core
{

fs::path getExecutableDir()
{
    wchar_t buffer[MAX_PATH];
    GetModuleFileNameW(nullptr, buffer, MAX_PATH);
    return fs::path(buffer).parent_path();
}

fs::path getProjectRoot()
{
    // Estructura asumida:
    // build/Debug/VoiceEngine.exe
    // → subir 2 niveles = raíz del proyecto

    fs::path exeDir = getExecutableDir();
    return exeDir.parent_path().parent_path();
}

} // namespace voice_engine::core