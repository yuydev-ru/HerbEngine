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

            if (event.type == sf::Event::KeyPressed)
            {
                switch (event.key.code)
                {
                    case sf::Keyboard::Key::W:
                    {
                        state.axes["vertical"] = 1.f;
                    } break;
                    case sf::Keyboard::Key::S:
                    {
                        state.axes["vertical"] = -1.f;
                    } break;
                    case sf::Keyboard::Key::D:
                    {
                        state.axes["horizontal"] = 1.f;
                    } break;
                    case sf::Keyboard::Key::A:
                    {
                        state.axes["horizontal"] = -1.f;
                    } break;
                    
                    default:
                    {

                    } break;
                }
            }
            if (event.type == sf::Event::KeyReleased)
            {
                switch (event.key.code)
                {
                    case sf::Keyboard::Key::W:
                    {
                        state.axes["vertical"] = 0;
                    } break;
                    case sf::Keyboard::Key::S:
                    {
                        state.axes["vertical"] = 0;
                    } break;
                    case sf::Keyboard::Key::D:
                    {
                        state.axes["horizontal"] = 0;
                    } break;
                    case sf::Keyboard::Key::A:
                    {
                        state.axes["horizontal"] = 0;
                    } break;
                    
                    default:
                    {

                    } break;
                }
            }
        }

        window.clear();
        updateState(&state, &storage);
        window.display();
    }

    return 0;
}