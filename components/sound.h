#ifndef ENGINE_SOUND_H
#define ENGINE_SOUND_H

#include "../base.h"
#include "../parser.h"
#include <SFML/Audio.hpp>

struct Sound : Component
{
    // TODO: Добавить вектор звуков (для их удаления после воспроизв-я). Лимит в SFML - 256.

    std::string name;
    std::string assetPath;
    float volume;
    bool isLooped;
    bool playOnStart = false;

    sf::SoundBuffer buffer;
    sf::Sound sound;
    bool isLoaded = false;

    void play();

    static Component *
    deserialize(Parser &parser)
    {
        auto s = new Sound;
        s->name = parser.parseElement<std::string>("name");
        s->assetPath = parser.parseElement<std::string>("assetPath");
        s->volume =  parser.parseElement<float>("volume");
        s->isLooped = parser.parseElement<bool>("isLooped");
        s->playOnStart = parser.parseElement<bool>("playOnStart");

        if (s->buffer.loadFromFile(s->assetPath))
        {
            s->isLoaded = true;
            s->sound.setBuffer(s->buffer);
            s->sound.setLoop(s->isLooped);
            s->sound.setVolume(s->volume);
            std::cout <<"Sound registered, assetPath: " << s->assetPath << "\n";
        }
        else
        {
            std::cout << "ERROR: Sound doesn't load!\n";
        }
        return s;
    }
};

void
setupSound(GameState *, Storage *, Entity);

#endif
