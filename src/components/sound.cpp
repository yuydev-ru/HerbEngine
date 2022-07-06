#include <engine/components/sound.h>

// TODO(andrew): Заменить на другой бэкенд
void
herb::Sound::play()
{
    // if (this->isLoaded)
    // {
    //     this->sound.play();
    // }
}

void
herb::Sound::pause()
{
    // if (this->isLoaded)
    // {
    //     this->sound.pause();
    // }
}

void
herb::Sound::stop()
{
    // if (this->isLoaded)
    // {
    //     this->sound.stop();
    // }
}

void
herb::setupSound(GameState *state, Storage *storage, const Entity id)
{
    //TODO: Эта функция должна вызываться 1 раз вместо постоянного вызова в game loop.
    // auto snd = storage->getComponent<Sound>(id);
    // if (snd->playOnStart == true)
    // {
    //     snd->play();
    //     snd->playOnStart = false;
    // }
}
