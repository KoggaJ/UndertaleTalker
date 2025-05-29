#include <iostream>
#include <conio.h>
#include <vector>
#include <string>
#include <random>
#include <cmath>
#include <filesystem>
#include <regex>

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

namespace fs = std::filesystem;

#define SAMPLE_RATE 44100
#define BITS_PER_SAMPLE 16
#define CHANNELS 1

using Sample = int16_t;

struct VoiceSample {
    std::vector<Sample> data;
};

std::vector<Sample> pitch_shift(const std::vector<Sample>& input, float pitchFactor) {
    std::vector<Sample> output;
    size_t newLength = static_cast<size_t>(input.size() / pitchFactor);
    output.reserve(newLength);

    for (size_t i = 0; i < newLength; ++i) {
        float index = i * pitchFactor;
        size_t i0 = static_cast<size_t>(index);
        size_t i1 = std::min(i0 + 1, input.size() - 1);
        float frac = index - i0;
        float sample = input[i0] * (1.0f - frac) + input[i1] * frac;
        output.push_back(static_cast<Sample>(sample));
    }
    return output;
}

std::string sanitize_filename(const std::string& input) {
    std::string name = input.substr(0, 30);
    for (char& c : name) {
        if (!std::isalnum(c) && c != '-' && c != '_') {
            c = '_';
        }
    }
    return name + ".wav";
}

int main() {
    std::string userInput;
    std::cout << "Enter your text: ";
    std::getline(std::cin, userInput);

    if (userInput.empty()) {
        std::cerr << "Input is empty.\n";
        getch();
        return 1;
    }

    std::string voiceFolder;
    std::cout << "Enter voice folder name: ";
    std::getline(std::cin, voiceFolder);

    if (!fs::exists(voiceFolder) || !fs::is_directory(voiceFolder)) {
        std::cerr << "Folder not found: " << voiceFolder << "\n";
        getch();
        return 1;
    }

    std::string filename = sanitize_filename(userInput);

    std::vector<VoiceSample> voiceVariants;

    for (const auto& entry : fs::directory_iterator(voiceFolder)) {
        if (!entry.is_regular_file() || entry.path().extension() != ".wav")
            continue;

        drwav wav;
        if (!drwav_init_file(&wav, entry.path().string().c_str(), nullptr)) {
            std::cerr << "Failed to open " << entry.path() << "\n";
            continue;
        }

        if (wav.channels != 1 || wav.sampleRate != SAMPLE_RATE || wav.bitsPerSample != BITS_PER_SAMPLE) {
            std::cerr << "Skipping " << entry.path().filename() << " (must be 16-bit mono 44100 Hz)\n";
            drwav_uninit(&wav);
            continue;
        }

        std::vector<Sample> data(wav.totalPCMFrameCount);
        drwav_read_pcm_frames_s16(&wav, wav.totalPCMFrameCount, data.data());
        drwav_uninit(&wav);

        voiceVariants.push_back({ data });
    }

    if (voiceVariants.empty()) {
        std::cerr << "No usable WAV files found in " << voiceFolder << "\n";
        getch();
        return 1;
    }

    const float delayBetweenChars = 0.05f;
    const float delayForSilence = 0.3f;
    const float minPitch = 1.0f;
    const float maxPitch = 1.0f;

    std::default_random_engine rng(std::random_device{}());
    std::uniform_real_distribution<float> pitchDist(minPitch, maxPitch);

    std::vector<Sample> outputBuffer;
    outputBuffer.resize(static_cast<size_t>(userInput.length() * 2.0f * SAMPLE_RATE), 0);

    float currentTime = 0.0f;
    size_t maxWritePos = 0;

    for (char c : userInput) {
        if (c == ' ' || c == ',' || c == '.') {
            currentTime += delayForSilence;
            continue;
        }

        float pitch = pitchDist(rng);
        const auto& selectedVoice = voiceVariants[rng() % voiceVariants.size()];
        auto shifted = pitch_shift(selectedVoice.data, pitch);

        size_t startSample = static_cast<size_t>(currentTime * SAMPLE_RATE);
        size_t endSample = startSample + shifted.size();

        if (endSample > outputBuffer.size()) {
            outputBuffer.resize(endSample + SAMPLE_RATE, 0);
        }

        for (size_t i = 0; i < shifted.size(); ++i) {
            size_t pos = startSample + i;
            int mixed = outputBuffer[pos] + shifted[i];
            if (mixed > 32767) mixed = 32767;
            if (mixed < -32768) mixed = -32768;
            outputBuffer[pos] = static_cast<Sample>(mixed);
        }

        if (endSample > maxWritePos) maxWritePos = endSample;
        currentTime += delayBetweenChars;
    }

    outputBuffer.resize(maxWritePos);

    drwav_data_format format;
    format.container = drwav_container_riff;
    format.format = DR_WAVE_FORMAT_PCM;
    format.channels = CHANNELS;
    format.sampleRate = SAMPLE_RATE;
    format.bitsPerSample = BITS_PER_SAMPLE;

    drwav outWav;
    if (!drwav_init_file_write(&outWav, filename.c_str(), &format, nullptr)) {
        std::cerr << "Failed to create " << filename << "\n";
        getch();
        return 1;
    }

    drwav_write_pcm_frames(&outWav, outputBuffer.size(), outputBuffer.data());
    drwav_uninit(&outWav);

    std::cout << "Generated: " << filename << " (" << maxWritePos << " samples)\n";
    return 0;
}
