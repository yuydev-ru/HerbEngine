#include <SFML/Graphics.hpp>

#include "base.h"
#include "interface.h"
#include "components.h"
#include <unordered_map>

#include "parser.h"
#include "inputdata.h"
#include <iostream>

#include "components/rendering.h"

void
loadConfig(GameState *state, const std::string& configPath, Config *config)
{
    Parser file(configPath);
    inputData inputArray;

    config->windowWidth = file.parseElement<int>("window", "width");
    config->windowHeight = file.parseElement<int>("window", "height");
    config->windowTitle = file.parseElement<std::string>("window", "title");

    config->defaultScene = file.parseElement<std::string>("scene", "default");

    Parser opKeys(configPath,{"input", "oppositeKeys"});
    for (int i = 0; i < (int) opKeys.size(); ++i)
    {
        std::string key1 = opKeys.parseObjectElement<std::string>(i, 0);
        std::string key2 = opKeys.parseObjectElement<std::string>(i,  1);
        config->oppositeKeys.insert(make_pair(key1,key2));
    }

    Parser mainKeys(configPath,{"input", "keys"});
    for (int i = 0; i < (int) mainKeys.size(); ++i)
    {
        std::string key = mainKeys.parseObjectElement<std::string>(i,0);
        std::string value = mainKeys.parseObjectElement<std::string>(i, 1);
        config->keys.insert(make_pair(key,inputArray.stringToKeyboardKey(value)));
    }

    Parser axisKeys(configPath,{"input", "axisData"});
    for (int i = 0; i < (int) axisKeys.size(); ++i)
    {
       KeyData data;

       std::string key_tmp = axisKeys.parseObjectElement<std::string>(i, "key");

       data.axis = axisKeys.parseObjectElement<std::string>(i, "axis");
       data.axisType = axisKeys.parseObjectElement<std::string>(i, "axisType");
       data.value = axisKeys.parseObjectElement<float>(i, "value");

       config->axisData.insert(std::make_pair(config->keys[key_tmp],data));
    }

    Parser axes(configPath,{"input","axes"});
    for (int i = 0; i < (int) axes.size(); ++i)
    {
        std::string axis = axes.parseObjectElement<std::string>(i, 0);
        auto value = axes.parseObjectElement<float>(i, 1);
        state->axes.insert(make_pair(axis,value));
    }

    config->logLevel = static_cast<logger::LogLevel>(file.parseElement<int>("debug", "logLevel"));
}

Entity
loadEntity(Parser &components, GameState *state, Storage *storage)
{
    Entity entity = storage->createEntity();
    for (components.iterator = components.data.begin(); components.iterator  != components.data.end(); ++components.iterator)
    {
        Parser component(*components.iterator);
        std::string name = component.parseElement<std::string>("type");
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
    Parser file(sceneName,{"entities"});

    for (file.iterator = file.data.begin(); file.iterator  != file.data.end(); ++file.iterator)
    {
        Parser parser(*file.iterator);
        loadEntity(parser, state, storage);
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

void
internalRegisterComponents(GameState *state, Storage *storage)
{
    storage->registerComponent<Transform>("Transform");
    storage->registerComponent<Sprite>("Sprite");
    storage->registerComponent<Camera>("Camera");
    storage->registerComponent<Gui>("Gui");
    storage->registerComponent<Collider>("Collider");
    storage->registerComponent<Physics>("Physics");
    storage->registerComponent<Sound>("Sound");

    storage->registerSystem(setupSound,{TYPE(Sound)});
    storage->registerSystem(render, {TYPE(Transform), TYPE(Sprite)});
    storage->registerSystem(renderGui, {TYPE(Gui)});
    storage->registerSystem(collision, {TYPE(Collider), TYPE(Transform)});
    storage->registerSystem(pushOut, {TYPE(Collider), TYPE(Physics), TYPE(Transform)});
    storage->registerSystem(physics, {TYPE(Collider), TYPE(Physics), TYPE(Transform)});
}

void
button_action()
{
    puts("Click!");
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

    internalRegisterComponents(&state, &storage);
    initializeEngine(&state, &storage);
    loadScene(&config, config.defaultScene, &state, &storage);

    sf::Clock clock;
    state.deltaTime = 0;

//    Text title(sf::Vector2f(float(window.getSize().x) / 2.f, 150.f), "Titled game", 48);

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
            updateGui(&state, &storage, event);
        }

        window.clear();
        updateState(&state, &storage);
        window.display();

        state.deltaTime = clock.restart().asSeconds();
    }

    return 0;
}