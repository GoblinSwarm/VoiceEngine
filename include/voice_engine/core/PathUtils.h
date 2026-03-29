#pragma once

#include <filesystem>

namespace voice_engine::core
{

std::filesystem::path getExecutableDir();
std::filesystem::path getProjectRoot();

} // namespace voice_engine::core