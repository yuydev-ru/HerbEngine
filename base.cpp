#include <SFML/Graphics.hpp>

#include "base.h"
#include "interface.h"

void
loadConfig(const std::string& configPath, Config *config)
{
    config->defaultScene = "MainMenu";
}

void
call(GameState *state, Storage *storage, const System f, const Entity id)
{
    Signature entSig = storage->entitySignatures[id];
    Signature sysSig = storage->systemSignature[f];
    if ((entSig & sysSig) == sysSig)
    {
        f(state, storage, id);
    }
}

void
updateState(GameState *state, Storage *storage)
{
    for (Entity id : storage->usedIds)
    {
        for (System system : storage->systemsArray)
        {
            call(state, storage, system, id);
        }
    }
}

int
main()
{
    Config config;
    loadConfig("data/config", &config);

    sf::RenderWindow window(sf::VideoMode(480, 480), "Engine");

    GameState state {};
    state.running = true;
    state.window = &window;

    Storage storage;
    initializeEngine(&state, &storage);
    loadScene(&config, config.defaultScene, &state, &storage);

    while (state.running)
    {
        sf::Event event {};

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
                state.running = false;
            }
        }

        window.clear();
        updateState(&state, &storage);
        window.display();
    }

    return 0;
}