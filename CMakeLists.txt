cmake_minimum_required(VERSION 3.16)

project(VoiceEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ----------------------------------------
# Whisper
# ----------------------------------------

add_subdirectory(external/whisper.cpp)

# ----------------------------------------
# VoiceEngine sources
# ----------------------------------------

file(GLOB_RECURSE SOURCES
    src/*.cpp
    app/*.cpp
)

add_executable(VoiceEngine ${SOURCES})

target_include_directories(VoiceEngine PRIVATE
    include
)

target_link_libraries(VoiceEngine PRIVATE whisper)

# ----------------------------------------
# Copy runtime DLLs next to VoiceEngine.exe
# ----------------------------------------

add_custom_command(TARGET VoiceEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:whisper>
        $<TARGET_FILE_DIR:VoiceEngine>
)

add_custom_command(TARGET VoiceEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:ggml>
        $<TARGET_FILE_DIR:VoiceEngine>
)

add_custom_command(TARGET VoiceEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:ggml-base>
        $<TARGET_FILE_DIR:VoiceEngine>
)

add_custom_command(TARGET VoiceEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:ggml-cpu>
        $<TARGET_FILE_DIR:VoiceEngine>
)