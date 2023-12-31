#pragma once
#include <SFML/Audio.hpp>
#include <map>
#include <vector>
#include <string>

#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>

#define MIXBUFFERS_IMP
#include "mixbuffers.hpp"

class MainWizard {
public:
    MainWizard();
    void render();
    void addFile(std::string f);
    void addFromBuffer(sf::SoundBuffer& buffer);
    void play();
    void stop();
    void pause();
    void record();
    void exp();

    std::map<int, sf::SoundBuffer> audioDataMap;
private:
    std::map<int, std::vector<sf::Vector2f>> waveformDataMap;
    std::map<int, float> volumes;

    std::vector<sf::Sound> currentSounds;
    std::vector<std::shared_ptr<sf::SoundBufferRecorder>> recorders;

    sf::Clock clock;
    sf::Time elapsed;

    std::string recordingDevice;
    bool playing;
    bool recording;

    int lastEmptyTrack;
};


#ifdef MAINWIZARD_IMP

MainWizard::MainWizard()
    : lastEmptyTrack(0), playing(false), recording(false), recordingDevice(sf::SoundBufferRecorder::getDefaultDevice())
{

}

void MainWizard::exp() {
    std::vector<sf::SoundBuffer> bufferPile;
    for(auto kv : this->audioDataMap) {
        if(kv.second.getSampleCount() != 0)
            bufferPile.push_back(kv.second);
    }
    sf::SoundBuffer mixedBuffer = mixBuffers(bufferPile);
    if(mixedBuffer.saveToFile("export.wav")) {
        std::cout << "Exported to export.wav" << std::endl;
    } else {
        std::cerr << "Failed to save to export.wav" << std::endl;
    }
}

void MainWizard::play() {
    this->playing = true;
    if(this->currentSounds.size() < 1) {
        for( auto & audioBuffer : audioDataMap) {
            sf::Sound sound;
            sound.setBuffer(audioBuffer.second);
            this->currentSounds.push_back(sound);
        }
    }

    for( sf::Sound & sound : this->currentSounds) {
        sound.play();
    }

    this->clock.restart();
}

void MainWizard::stop() {

    if(this->recording) {

        auto recorder = recorders.back();
        recorder->stop();
        sf::SoundBuffer buf = recorder->getBuffer();
        this->addFromBuffer(buf);
        this->recording = false;
        this->recorders.clear();

    }

    this->elapsed = sf::Time::Zero;
    for( sf::Sound & sound : this->currentSounds) {
        sound.stop();
    }
    this->currentSounds.clear();
    this->playing = false;

}

void MainWizard::pause() {
    if(this->playing)
    {
        this->elapsed += this->clock.getElapsedTime();
    }
    for( sf::Sound & sound : this->currentSounds) {
        sound.pause();
    }
    this->playing = false;
}

void MainWizard::record() {

    if(!this->recording) {
        this->recorders.clear();

        auto recorder = std::make_shared<sf::SoundBufferRecorder>();
        recorder->start();
        recorders.push_back(recorder);

        this->recording = true;
    } else {
        this->stop();
    }

}

float SCALE = 16.0f;


void MainWizard::render() {

    static int setWindowSize = 0;
    if(!setWindowSize) {
        ImGui::SetNextWindowSize(ImVec2(820, 600));
    }

    if (ImGui::Begin("Welldar2")) {

        // Get the width of the inner window
        float innerWindowWidth = ImGui::GetContentRegionAvail().x;
        float controlsWidth = 100.0f;
        float laneHeight = 80.0f;

        ImVec2 controlsSize(controlsWidth, laneHeight);
        ImVec2 laneSize(innerWindowWidth*10.0f, laneHeight); // Size of each rectangle (width, height)

        ImGui::BeginGroup();

        int elementId = 0;

        for (int i = 0; i < 10; ++i) {
            // You can set different colors, positions, or other properties for each rectangles

            ImVec4 color = ImVec4(0.2f + i * 0.1f, 0.4f + i * 0.1f, 0.6f + i * 0.1f, 1.0f);
            ImVec4 controlsColor = ImVec4(0.6f + i * 0.1f, 0.2f + i * 0.1f, 0.2f + i * 0.1f, 1.0f);



            if (waveformDataMap.find(i) != waveformDataMap.end()) {
                ImGui::PushID(i + 26);
                if (ImGui::VSliderFloat((std::string("Vol") + std::to_string(i)).c_str(), ImVec2(40, 80), &this->volumes[i], 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp)) {
                    // Volume slider value changed, update the volume
                    //if (currentSounds.size() > i) {
                    //    currentSounds[i].setVolume(this->volumes[i]);
                    // }
                }
                ImGui::PopID();
                ImGui::SameLine();
                ImGui::PushID(i + 27);
                if(ImGui::Button("Remove")) {
                    this->waveformDataMap.erase(i);
                    this->audioDataMap.erase(i);
                    this->volumes.erase(i);
                }
                ImGui::PopID();
            }
        }


        ImGui::EndGroup();

        ImGui::SameLine();

        // Render waveform as a child window
        ImGui::BeginChild("WaveformsChild", ImVec2(700, 600), true, ImGuiWindowFlags_HorizontalScrollbar);
        for (int i = 0; i < 10; ++i) {

            if (waveformDataMap.find(i) != waveformDataMap.end()) {
                ImGui::BeginChild((std::string("Waveform") + std::to_string(i)).c_str(), ImVec2(10'000, 80), true, ImGuiWindowFlags_None);
                // Render the waveform here using ImGui drawing commands
                ImDrawList* drawList = ImGui::GetWindowDrawList();
                std::vector<sf::Vector2f>& waveform = waveformDataMap[i];
                ImVec2 startPos = ImGui::GetCursorScreenPos();

                float playerHeadX =
                    this->playing ?
                    (this->elapsed.asSeconds() + this->clock.getElapsedTime().asSeconds()) * SCALE
                    : this->elapsed.asSeconds() * SCALE;/* Calculate the X position for the player head */
                ImVec2 playerHeadStart(startPos.x + playerHeadX, startPos.y);
                ImVec2 playerHeadEnd(startPos.x + playerHeadX, startPos.y + 80); // Adjust the height as needed

                // Draw the vertical line
                drawList->AddLine(playerHeadStart, playerHeadEnd, IM_COL32(255, 0, 0, 255), 1.0f);


                // Iterate through the waveform data and draw lines connecting the points
                for (size_t j = 1; j < waveform.size(); ++j) {
                    ImVec2 point1(startPos.x+6 + waveform[j - 1].x, startPos.y + waveform[j - 1].y);
                    ImVec2 point2(startPos.x+6 + waveform[j].x, startPos.y + waveform[j].y);
                    drawList->AddLine(point1, point2, IM_COL32(255, 255, 255, 255), 1.0f);
                }

                ImGui::EndChild();
                ImGui::Separator();
            }
        }

        ImGui::EndChild();


    }


    // Rest of your ImGui content
    ImGui::End();

    static int setControlsBeginSpot = 0;

    if(!setControlsBeginSpot) {
        setControlsBeginSpot = 1;
        ImGui::SetNextWindowSize(ImVec2(350, 100));
        ImGui::SetNextWindowPos(ImVec2(890, 600));
    }


    if(ImGui::Begin("Controls")) {
        if(ImGui::Button("Play"))
        {
            this->play();
        }
        ImGui::SameLine();
        if(ImGui::Button("Pause"))
        {
            this->pause();
        }
        ImGui::SameLine();
        if(ImGui::Button("Stop"))
        {
            this->stop();
        }
        ImGui::SameLine();
        if(ImGui::Button("Record"))
        {
            this->record();
        }
        if(ImGui::Button("Export"))
        {
            this->exp();
        }
        if(ImGui::Button("Print volumes")) {
            std::cout << "Volumes: " << std::endl;
            for(auto s : this->volumes) {
                std::cout << std::to_string(s.second) << std::endl;

            }
        }

    }
    ImGui::End();



    static int setRecordBeginSpot = 0;

    if(!setRecordBeginSpot) {
        setRecordBeginSpot = 1;
        ImGui::SetNextWindowSize(ImVec2(350, 300));
        ImGui::SetNextWindowPos(ImVec2(890, 25));
    }

    if(ImGui::Begin("Recording Device")) {

        static std::vector<std::string> devices = sf::SoundBufferRecorder::getAvailableDevices();
        const char** items = new const char*[devices.size()];
        for (size_t i = 0; i < devices.size(); ++i) {
            items[i] = devices[i].c_str();
        }


        static int item_current_idx = 0; // Here we store our selection data as an index.
        if (ImGui::BeginListBox("Devices"))
        {
            for (int n = 0; n < IM_ARRAYSIZE(items); n++)
            {
                const bool is_selected = (item_current_idx == n);
                if (ImGui::Selectable(items[n], is_selected))
                {
                    item_current_idx = n;
                    this->recordingDevice = devices[n];
                    std::cout << this->recordingDevice << std::endl;
                }
                    
                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndListBox();
        }
        delete[] items;
    }
    ImGui::End();

}


void MainWizard::addFile(std::string f) {

    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile(f)) {
        std::cerr << "Failed to load .wav file: " << f << std::endl;
        return;
    }
    const sf::Int16* audioData = buffer.getSamples();
    std::size_t sampleCount = buffer.getSampleCount();
    
    float duration = static_cast<float>(sampleCount) / buffer.getSampleRate();
    std::cout << "Sample rate: " << std::to_string(buffer.getSampleRate()) << std::endl;
    float waveformWidth = duration;
    std::vector<sf::Vector2f> waveform;
    for (std::size_t i = 0; i < sampleCount; i += 700) {
        float x = (static_cast<float>(i) / sampleCount) * waveformWidth;
        float y = static_cast<float>(audioData[i]) / 32767.0f * 80.0f / 2.0f;
        waveform.push_back(sf::Vector2f(x * SCALE, 80.0f / 2.0f - y));
    }
    this->waveformDataMap[this->lastEmptyTrack] = waveform;
    this->audioDataMap[this->lastEmptyTrack] = buffer;
    this->volumes[this->lastEmptyTrack] = 1.0f;
    this->lastEmptyTrack++;
}


void MainWizard::addFromBuffer(sf::SoundBuffer& buffer) {

    
    const sf::Int16* audioData = buffer.getSamples();
    std::size_t sampleCount = buffer.getSampleCount();
    float duration = static_cast<float>(sampleCount) / buffer.getSampleRate();
    
    std::cout << "Sample rate: " << std::to_string(buffer.getSampleRate()) << std::endl;
    float waveformWidth = duration;
    std::vector<sf::Vector2f> waveform;
    for (std::size_t i = 0; i < sampleCount; i += 700) {
        float x = (static_cast<float>(i) / sampleCount) * waveformWidth;
        float y = static_cast<float>(audioData[i]) / 32767.0f * 80.0f / 2.0f;
        waveform.push_back(sf::Vector2f(x * SCALE, 80.0f / 2.0f - y));
    }
    this->waveformDataMap[this->lastEmptyTrack] = waveform;
    this->audioDataMap[this->lastEmptyTrack] = buffer;
    this->volumes[this->lastEmptyTrack] = 1.0f;
    this->lastEmptyTrack++;
}


#endif