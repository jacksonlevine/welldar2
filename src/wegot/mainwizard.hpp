#pragma once
#include <SFML/Audio.hpp>
#include <map>
#include <vector>
#include <string>

class MainWizard {
public:
    MainWizard();
    void render();
    void addFile(std::string f);
    void play();
    void stop();
    void pause();
    
    std::map<int, sf::SoundBuffer> audioDataMap;
private:
    std::map<int, std::vector<sf::Vector2f>> waveformDataMap;

    std::vector<sf::Sound> currentSounds;
    int lastEmptyTrack;
};


//#ifdef MAINWIZARD_IMP

MainWizard::MainWizard()
: lastEmptyTrack(0)
{

}

void MainWizard::play() {
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
    


}

void MainWizard::stop() {
    for( sf::Sound & sound : this->currentSounds) {
        sound.stop();
    }
    this->currentSounds.clear();


}

void MainWizard::pause() {
    for( sf::Sound & sound : this->currentSounds) {
        sound.pause();
    }
}


void MainWizard::render() {

    static int setWindowSize = 0;
    if(!setWindowSize) {
        ImGui::SetNextWindowSize(ImVec2(800, 600)); 
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

        ImGui::BeginGroup();

        
      if (waveformDataMap.find(i) != waveformDataMap.end()) {
    // Render waveform as a child window
      ImGui::BeginChild(("WaveformChild" + std::to_string(i)).c_str(), ImVec2(laneSize.x, laneSize.y), true);
    
    // Render the waveform here using ImGui drawing commands
       ImDrawList* drawList = ImGui::GetWindowDrawList();
      std::vector<sf::Vector2f>& waveform = waveformDataMap[i];
      ImVec2 startPos = ImGui::GetCursorScreenPos();
    
    // Iterate through the waveform data and draw lines connecting the points
      for (size_t j = 1; j < waveform.size(); ++j) {
          ImVec2 point1(startPos.x + waveform[j - 1].x*0.3f, startPos.y + waveform[j - 1].y);
          ImVec2 point2(startPos.x + waveform[j].x*0.3f, startPos.y + waveform[j].y);
          drawList->AddLine(point1, point2, IM_COL32(255, 255, 255, 255), 2.0f);
      }
    
    ImGui::EndChild();
        
    ImGui::EndGroup();

   } else {
        ImGui::PushID(elementId); // Ensure each rectangle has a unique ID
        ImGui::PushStyleColor(ImGuiCol_Button, color);
        ImGui::Button("##rect", laneSize); // Draw a colored rectangle
        ImGui::PopStyleColor();
        ImGui::PopID();

      elementId++;

      }

       

        // Add spacing between rectangles
        ImGui::Separator();
    }
 ImGui::EndGroup();

               // Rest of your ImGui content
          ImGui::End();

    static int setControlsBeginSpot = 0;

    if(!setControlsBeginSpot) {
      setControlsBeginSpot = 1;
      ImGui::SetNextWindowSize(ImVec2(500, 80));
      ImGui::SetNextWindowPos(ImVec2(90, 650));
    }


     if(ImGui::Begin("Controls")) {
       if(ImGui::Button("Play"))
       {
         this->play();
       }
       if(ImGui::Button("Pause"))
       {
         this->pause();
       }
        if(ImGui::Button("Stop"))
       {
         this->stop();
       }
    }
    ImGui::End();
     
    }

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
    float waveformWidth = duration * 50.0f;
    std::vector<sf::Vector2f> waveform;
    for (std::size_t i = 0; i < sampleCount; i += 1000) {
        float x = static_cast<float>(i) / sampleCount * waveformWidth;
        float y = static_cast<float>(audioData[i]) / 32767.0f * 100 / 2.0f;
        waveform.push_back(sf::Vector2f(x, 100 / 3.0f - y));
    }
    this->waveformDataMap[this->lastEmptyTrack] = waveform;
    this->audioDataMap[this->lastEmptyTrack] = buffer;
    this->lastEmptyTrack++;
}


//#endif