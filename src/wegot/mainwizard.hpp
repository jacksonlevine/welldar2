
#include <SFML/Audio.hpp>
#include <map>
#include <vector>

class MainWizard {
public:
    MainWizard();
    void render();
private:
    std::map<int, std::vector<sf::Vector2f>> waveformDataMap;
    std::map<int, sf::SoundBuffer> audioDataMap;
    int lastEmptyTrack;
};


#ifdef MAINWIZARD_IMP

MainWizard::MainWizard()
: lastEmptyTrack(0)
{

}

MainWizard::render() {
    
}

#endif