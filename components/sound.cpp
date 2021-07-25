#include "sound.h"

void
Sound::play()
{
    if (this->isLoaded)
    {
        this->sound.play();
    }
}
void
setupSound(GameState *state, Storage *storage, const Entity id)
{
    //TODO: Эта функция должна вызываться 1 раз вместо постоянного вызова в game loop.
    auto snd = storage->getComponent<Sound>(id);
    if (snd->playOnStart == true)
    {
        snd->play();
        snd->playOnStart = false;
    }
}