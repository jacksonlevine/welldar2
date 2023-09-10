#pragma once

#include <SFML/Audio.hpp>

sf::SoundBuffer mixBuffers(const std::vector<sf::SoundBuffer>& soundBuffers);


#ifdef MIXBUFFERS_IMP
#include <SFML/Audio.hpp>

sf::SoundBuffer mixBuffers(const std::vector<sf::SoundBuffer>& soundBuffers) {
    // Ensure at least one sound buffer is provided
    if (soundBuffers.empty()) {
        throw std::runtime_error("No sound buffers provided for mixing.");
    }

    // Determine the maximum sample count and channel count
    sf::Uint64 maxSampleCount = 0;
    unsigned int maxChannelCount = 0;
    unsigned int sampleRate = soundBuffers[0].getSampleRate();

    for (const sf::SoundBuffer& buffer : soundBuffers) {
        maxSampleCount = std::max(maxSampleCount, static_cast<sf::Uint64>(buffer.getSampleCount()));
        maxChannelCount = std::max(maxChannelCount, buffer.getChannelCount());

        // Ensure all sound buffers have the same sample rate
        if (buffer.getSampleRate() != sampleRate) {
            throw std::runtime_error("Input sound buffers have different sample rates.");
        }
    }

    // Create a vector to hold the mixed audio samples
    std::vector<sf::Int16> mixedSamples(maxSampleCount * maxChannelCount, 0);

    // Mix the audio samples from all sound buffers
    for (const sf::SoundBuffer& buffer : soundBuffers) {
        const sf::Int16* samples = buffer.getSamples();
        sf::Uint64 sampleCount = buffer.getSampleCount();
        unsigned int channelCount = buffer.getChannelCount();

        for (sf::Uint64 i = 0; i < sampleCount; ++i) {
            for (unsigned int channel = 0; channel < channelCount; ++channel) {
                if(i * maxChannelCount + channel < mixedSamples.size() && i * channelCount + channel < sampleCount)
                    mixedSamples[i * maxChannelCount + channel] += samples[i * channelCount + channel];
            }
        }
    }

    // Create a new sound buffer from the mixed samples
    sf::SoundBuffer mixedBuffer;
    mixedBuffer.loadFromSamples(mixedSamples.data(), maxSampleCount, maxChannelCount, sampleRate);

    return mixedBuffer;
}






#endif