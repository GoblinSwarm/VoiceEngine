// src/audio/WavAudioInput.cpp
// ===========================
//
// WavAudioInput
// -------------
//
// Concrete WAV file loader implementation for VoiceEngine.
//

#include "voice_engine/audio/WavAudioInput.h"

#include <cstring>
#include <fstream>
#include <stdexcept>
#include <vector>

namespace voice_engine::audio
{
namespace
{
    constexpr char kRiffId[] = "RIFF";
    constexpr char kWaveId[] = "WAVE";
    constexpr char kFmtId[]  = "fmt ";
    constexpr char kDataId[] = "data";

    constexpr std::uint16_t kPcmFormat = 1;
    constexpr std::uint16_t kRequiredChannels = 1;
    constexpr std::uint16_t kRequiredBitsPerSample = 16;
    constexpr std::uint32_t kRequiredSampleRate = 16000;

    struct ChunkHeader
    {
        char id[4];
        std::uint32_t size = 0;
    };

    [[nodiscard]] bool chunkIdEquals(const char* lhs, const char* rhs)
    {
        return std::memcmp(lhs, rhs, 4) == 0;
    }

    template <typename T>
    void readOrThrow(std::ifstream& inputFile, T& value, const char* contextMessage)
    {
        inputFile.read(reinterpret_cast<char*>(&value), sizeof(T));

        if (!inputFile)
        {
            throw std::runtime_error(contextMessage);
        }
    }

    void readBytesOrThrow(
        std::ifstream& inputFile,
        char* buffer,
        std::streamsize size,
        const char* contextMessage)
    {
        inputFile.read(buffer, size);

        if (!inputFile)
        {
            throw std::runtime_error(contextMessage);
        }
    }

    [[nodiscard]] core::Error makeAudioError(const char* /*message*/)
    {
        return {};
    }
} // namespace

core::AudioBuffer WavAudioInput::loadFromFile(const std::string& filePath)
{
    m_lastError = {};

    core::AudioBuffer audioBuffer {};

    try
    {
        std::ifstream inputFile(filePath, std::ios::binary);

        if (!inputFile.is_open())
        {
            setError(makeAudioError("Failed to open WAV file."));
            return audioBuffer;
        }

        char riffId[4] {};
        std::uint32_t riffChunkSize = 0;
        char waveId[4] {};

        readBytesOrThrow(inputFile, riffId, 4, "Failed to read RIFF id.");
        readOrThrow(inputFile, riffChunkSize, "Failed to read RIFF chunk size.");
        readBytesOrThrow(inputFile, waveId, 4, "Failed to read WAVE id.");

        if (!chunkIdEquals(riffId, kRiffId))
        {
            setError(makeAudioError("Invalid WAV file: missing RIFF header."));
            return audioBuffer;
        }

        if (!chunkIdEquals(waveId, kWaveId))
        {
            setError(makeAudioError("Invalid WAV file: missing WAVE signature."));
            return audioBuffer;
        }

        WavMetadata metadata {};
        bool foundFmtChunk = false;
        bool foundDataChunk = false;
        std::vector<std::int16_t> pcmSamples;

        while (inputFile && (!foundFmtChunk || !foundDataChunk))
        {
            ChunkHeader chunkHeader {};
            readOrThrow(inputFile, chunkHeader, "Failed to read WAV chunk header.");

            if (chunkIdEquals(chunkHeader.id, kFmtId))
            {
                if (chunkHeader.size < 16)
                {
                    setError(makeAudioError("Invalid WAV file: fmt chunk too small."));
                    return core::AudioBuffer {};
                }

                readOrThrow(inputFile, metadata.audioFormat, "Failed to read audio format.");
                readOrThrow(inputFile, metadata.channelCount, "Failed to read channel count.");
                readOrThrow(inputFile, metadata.sampleRate, "Failed to read sample rate.");

                std::uint32_t byteRate = 0;
                std::uint16_t blockAlign = 0;

                readOrThrow(inputFile, byteRate, "Failed to read byte rate.");
                readOrThrow(inputFile, blockAlign, "Failed to read block align.");
                readOrThrow(inputFile, metadata.bitsPerSample, "Failed to read bits per sample.");

                if (chunkHeader.size > 16)
                {
                    const std::streamoff remainingFmtBytes =
                        static_cast<std::streamoff>(chunkHeader.size - 16);

                    inputFile.seekg(remainingFmtBytes, std::ios::cur);

                    if (!inputFile)
                    {
                        setError(makeAudioError("Failed to skip extended fmt chunk bytes."));
                        return core::AudioBuffer {};
                    }
                }

                foundFmtChunk = true;
            }
            else if (chunkIdEquals(chunkHeader.id, kDataId))
            {
                if (!foundFmtChunk)
                {
                    setError(makeAudioError("Invalid WAV file: data chunk found before fmt chunk."));
                    return core::AudioBuffer {};
                }

                metadata.dataSize = chunkHeader.size;

                if (metadata.audioFormat != kPcmFormat)
                {
                    setError(makeAudioError("Unsupported WAV format: only PCM is supported."));
                    return core::AudioBuffer {};
                }

                if (metadata.channelCount != kRequiredChannels)
                {
                    setError(makeAudioError("Unsupported WAV format: only mono files are supported."));
                    return core::AudioBuffer {};
                }

                if (metadata.bitsPerSample != kRequiredBitsPerSample)
                {
                    setError(makeAudioError("Unsupported WAV format: only 16-bit PCM is supported."));
                    return core::AudioBuffer {};
                }

                if (metadata.sampleRate != kRequiredSampleRate)
                {
                    setError(makeAudioError("Unsupported WAV format: sample rate must be 16000 Hz."));
                    return core::AudioBuffer {};
                }

                if ((metadata.dataSize % sizeof(std::int16_t)) != 0)
                {
                    setError(makeAudioError("Invalid WAV file: PCM16 data chunk size is misaligned."));
                    return core::AudioBuffer {};
                }

                const std::size_t sampleCount =
                    static_cast<std::size_t>(metadata.dataSize / sizeof(std::int16_t));

                pcmSamples.resize(sampleCount);

                inputFile.read(
                    reinterpret_cast<char*>(pcmSamples.data()),
                    static_cast<std::streamsize>(metadata.dataSize));

                if (!inputFile)
                {
                    setError(makeAudioError("Failed to read WAV sample data."));
                    return core::AudioBuffer {};
                }

                foundDataChunk = true;
            }
            else
            {
                inputFile.seekg(static_cast<std::streamoff>(chunkHeader.size), std::ios::cur);

                if (!inputFile)
                {
                    setError(makeAudioError("Failed to skip unknown WAV chunk."));
                    return core::AudioBuffer {};
                }
            }

            if ((chunkHeader.size % 2) != 0)
            {
                inputFile.seekg(1, std::ios::cur);

                if (!inputFile)
                {
                    setError(makeAudioError("Failed to skip padded WAV chunk byte."));
                    return core::AudioBuffer {};
                }
            }
        }

        if (!foundFmtChunk)
        {
            setError(makeAudioError("Invalid WAV file: missing fmt chunk."));
            return core::AudioBuffer {};
        }

        if (!foundDataChunk)
        {
            setError(makeAudioError("Invalid WAV file: missing data chunk."));
            return core::AudioBuffer {};
        }

        audioBuffer.samples().reserve(pcmSamples.size());

        for (const std::int16_t sample : pcmSamples)
        {
            audioBuffer.samples().push_back(pcm16ToFloat(sample));
        }

        audioBuffer.format().sampleRate = metadata.sampleRate;
        audioBuffer.format().channels = metadata.channelCount;
        audioBuffer.format().format = core::SampleFormat::Float32;

        return audioBuffer;
    }
    catch (const std::exception&)
    {
        setError(makeAudioError("Unexpected error while loading WAV file."));
        return core::AudioBuffer {};
    }
}

core::Error WavAudioInput::lastError() const
{
    return m_lastError;
}

void WavAudioInput::setError(const core::Error& error)
{
    m_lastError = error;
}

float WavAudioInput::pcm16ToFloat(std::int16_t sample)
{
    return static_cast<float>(sample) / 32768.0f;
}

} // namespace voice_engine::audio