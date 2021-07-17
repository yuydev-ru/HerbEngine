#include <SFML/Graphics.hpp>

#include "base.h"
#include "interface.h"
#include "gui.h"

#include <iostream>

void
loadConfig(GameState *state, const std::string& configPath, Config *config)
{
    config->defaultScene = "MainMenu";
    config->oppositeKeys = { {"up", "down"}
                           , {"down", "up"}
                           , {"right", "left"}
                           , {"left", "right"} };
    config->keys = { {"up", sf::Keyboard::Key::W}
                   , {"down", sf::Keyboard::Key::S}
                   , {"left", sf::Keyboard::Key::A}
                   , {"right", sf::Keyboard::Key::D}
                   , {"interact", sf::Keyboard::Key::E}
                   , {"jump", sf::Keyboard::Key::Space} };
    config->axisData = { {config->keys["up"], KeyData("vertical", "hold", 1)}
                       , {config->keys["down"], KeyData("vertical", "hold", -1)}
                       , {config->keys["right"], KeyData("horizontal", "hold", 1)}
                       , {config->keys["left"], KeyData("horizontal", "hold", -1)}
                       , {config->keys["interact"], KeyData("", "push", 1)}
                       , {config->keys["jump"], KeyData("", "push", 0)} };
    state->axes = { {"vertical", 0}
                  , {"horizontal", 0} };
    config->logLevel = logger::INFO;
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
    std::unordered_map<std::string, std::string> imageStates = { {"normal", "assets/button/normal.png"}
                                                               , {"hovered", "assets/button/hovered.png"}
                                                               , {"clicked", "assets/button/clicked.png"} };

    sf::RenderWindow window(sf::VideoMode(480, 480), "Engine");
    Button button ( sf::Vector2f(100,100), imageStates, "PLAY", "assets/fonts/Neucha-Regular.ttf"
                  , sf::Color::White, 14, func );
    GameState state {};
    state.running = true;
    state.window = &window;

    Config config;
    loadConfig(&state, "data/config", &config);

    Storage storage;

    logger::Logger logger(config.logLevel);
    logger::ConsoleLogger consoleLogger;
    logger.addDestination(&consoleLogger);

    storage.logger = &logger;
    state.logger = &logger;

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

            if (event.type == sf::Event::KeyPressed && config.axisData.count(event.key.code))
            {
                KeyData pressedKeyData = config.axisData[event.key.code];
                if (pressedKeyData.axisType == "push")
                {
                    if (state.pushedKeys[event.key.code])
                    {
                        pressedKeyData.value = 0;
                    }
                    else
                    {
                        state.pushedKeys[event.key.code] = true;
                        pressedKeyData.value = 1;

                        for (const auto& it : config.keys)
                        {
                            if (it.second == event.key.code)
                            {
                                std::cout << it.first << '\n';
                            }
                        }
                    }
                }
                else if (pressedKeyData.axisType == "hold")
                {
                    state.axes[pressedKeyData.axis] = pressedKeyData.value;
                }
            }

            if (event.type == sf::Event::KeyReleased && config.axisData.count(event.key.code))
            {
                sf::Keyboard::Key oppositeKey;
                for (const auto& it : config.keys)
                {
                    if (it.second == event.key.code)
                    {
                        oppositeKey = config.keys[config.oppositeKeys[it.first]];
                    }
                }

                KeyData pressedKeyData = config.axisData[event.key.code];
                KeyData oppositeKeyData = config.axisData[oppositeKey];

                if (pressedKeyData.axisType == "push")
                {
                    state.pushedKeys[event.key.code] = false;
                }
                else if (pressedKeyData.axisType == "hold")
                {
                    state.axes[pressedKeyData.axis] =
                        (sf::Keyboard::isKeyPressed(oppositeKey)) ? oppositeKeyData.value : 0;
                }

            }
            button.updateButton(window, event);
        }

        window.clear();

        updateState(&state, &storage);
        button.drawButton(window);
        window.display();
    }

    return 0;
}