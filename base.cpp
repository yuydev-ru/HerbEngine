#include <SFML/Graphics.hpp>

#include "base.h"
#include "interface.h"

#include <iostream>

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
    bool isPushed = false;

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

            if (event.type == sf::Event::Resized)
            {
                sf::Vector2f visibleArea = { (float) event.size.width
                                           , (float) event.size.height };
                window.setView(sf::View(visibleArea * 0.5f, visibleArea));
            }

            if (event.type == sf::Event::KeyPressed && state.axisData.count(event.key.code))
            {
                KeyData pressedKeyData = state.axisData[event.key.code];
                if (pressedKeyData.axisType == "push")
                {
                    if (isPushed)
                    {
                        pressedKeyData.value = 0;
                    }
                    else
                    {
                        isPushed = true;
                        pressedKeyData.value = 1;
                        std::cout << "E";
                    }
                }
                else
                {
                    state.axes[pressedKeyData.axis] = pressedKeyData.value;
                }
            }

            if (event.type == sf::Event::KeyReleased && state.axisData.count(event.key.code))
            {
                sf::Keyboard::Key oppositeKey;
                for (const auto& it : config.keys)
                {
                    if (it.second == event.key.code)
                    {
                        oppositeKey =
                                config.keys[config.oppositeKeys[it.first]];
                    }
                }

                KeyData pressedKeyData = state.axisData[event.key.code];
                KeyData oppositeKeyData = state.axisData[oppositeKey];

                if (pressedKeyData.axisType == "push")
                {
                    isPushed = false;
                }

                state.axes[pressedKeyData.axis] =
                    (sf::Keyboard::isKeyPressed(oppositeKey)) ? oppositeKeyData.value : 0;
            }
        }

        window.clear();
        updateState(&state, &storage);
        window.display();
    }

    return 0;
}