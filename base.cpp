#include <SFML/Graphics.hpp>

#include "base.h"
#include "interface.h"

#include "parsing.h"
#include <iostream>

void
loadConfig(GameState *state, const std::string& configPath, Config *config)
{
    Parsing::configFile file = Parsing::parseConfigFile(configPath);

    auto windowArray = file.find("window");
    for (auto & windowElement: *windowArray)
    {
        config->windowWidth = Parsing::parseElement<int>(windowElement,"width");
        config->windowHeight = Parsing::parseElement<int>(windowElement,"height");
        config->windowTitle = Parsing::parseElement<std::string>(windowElement,"title");
    }

    auto sceneArray =  file.find("scene");
    for (auto & sceneElement: *sceneArray)
    {
        config->defaultScene = Parsing::parseElement<std::string>(sceneElement,"default");
    }
    auto inputArray =  file.find("input");
    for (auto & inputElement: *inputArray)
    {
       //config->oppositeKeys = Parsing::parseKeyData<std::string>(file,"oppositeKeys","key","opposite");
    }
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
                       , {config->keys["jump"], KeyData("jump", "push", 1)} };
    state->axes = { {"vertical", 0}
                  , {"horizontal", 0}
                  , {"jump", 0}
                  };
    config->logLevel = logger::INFO;
}

Entity
loadEntity(Parsing::configFile &components, GameState *state, Storage *storage)
{
    Entity entity = storage->createEntity();

    for (auto & component : components)
    {
        std::string name = Parsing::parseElement<std::string>(component, "type");
        if (storage->deserializers.find(name) != storage->deserializers.end())
        {
            Component *comp = storage->deserializers[name](component);
            auto type = storage->typeNames.at(name);
            storage->entities[type][entity] = comp;
            storage->entitySignatures[entity].set(storage->componentTypes[type]);

            if (name == "Camera")
            {
                state->currentCamera = entity;
            }
        }
    }

    return entity;
}

void
loadScene(const Config *config, const std::string& sceneName, GameState *state, Storage *storage)
{
    Parsing::configFile f = Parsing::parseConfigFile(sceneName);
    auto entities = f.find("entities");

    for (auto components : *entities)
    {
        loadEntity(components, state, storage);
    }
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
    GameState state {};
    Config config;
    loadConfig(&state, "assets/config.json", &config);

    sf::RenderWindow window(sf::VideoMode(config.windowWidth, config.windowHeight), config.windowTitle);


    state.running = true;
    state.window = &window;

    Storage storage;

    logger::Logger logger(config.logLevel);
    logger::ConsoleLogger consoleLogger;
    logger.addDestination(&consoleLogger);

    storage.logger = &logger;
    state.logger = &logger;

    initializeEngine(&state, &storage);
    loadScene(&config, config.defaultScene, &state, &storage);

    sf::Clock clock;
    state.deltaTime = 0;

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
                        state.axes[pressedKeyData.axis] = 0;
                    }
                    else
                    {
                        state.pushedKeys[event.key.code] = true;
                        state.axes[pressedKeyData.axis] = pressedKeyData.value;
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
                    state.axes[pressedKeyData.axis] = 0;
                }
                else if (pressedKeyData.axisType == "hold")
                {
                    state.axes[pressedKeyData.axis] =
                        (sf::Keyboard::isKeyPressed(oppositeKey)) ? oppositeKeyData.value : 0;
                }
            }
        }

        window.clear();
        updateState(&state, &storage);
        window.display();
        state.deltaTime = clock.restart().asSeconds();
    }

    return 0;
}