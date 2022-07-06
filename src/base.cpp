#include <raylib.h>

#include <unordered_map>
#include <iostream>

#include <engine/parser.h>
#include <engine/inputdata.h>
#include <engine/base.h>
#include <engine/interface.h>
#include <engine/components.h>

void initializeEngine(herb::GameState*, herb::Storage*);

void
loadConfig(herb::GameState *state, const std::string& configPath, herb::Config *config)
{
    herb::Parser file(configPath);
    inputData inputArray;

    config->windowWidth = file.parseElement<int>("window", "width");
    config->windowHeight = file.parseElement<int>("window", "height");
    config->windowTitle = file.parseElement<std::string>("window", "title");

    config->defaultScene = file.parseElement<std::string>("scene", "default");

    herb::Parser opKeys(configPath,{"input", "oppositeKeys"});
    for (int i = 0; i < (int) opKeys.size(); ++i)
    {
        std::string key1 = opKeys.parseObjectElement<std::string>(i, 0);
        std::string key2 = opKeys.parseObjectElement<std::string>(i,  1);
        config->oppositeKeys.insert(make_pair(key1,key2));
    }

    herb::Parser mainKeys(configPath,{"input", "keys"});
    for (int i = 0; i < (int) mainKeys.size(); ++i)
    {
        std::string key = mainKeys.parseObjectElement<std::string>(i,0);
        std::string value = mainKeys.parseObjectElement<std::string>(i, 1);
        config->keys.insert(make_pair(key,inputArray.stringToKeyboardKey(value)));
    }

    herb::Parser axisKeys(configPath,{"input", "axisData"});
    for (int i = 0; i < (int) axisKeys.size(); ++i)
    {
        herb::KeyData data;

        std::string key_tmp = axisKeys.parseObjectElement<std::string>(i, "key");

        data.axis = axisKeys.parseObjectElement<std::string>(i, "axis");
        data.axisType = axisKeys.parseObjectElement<std::string>(i, "axisType");
        data.value = axisKeys.parseObjectElement<float>(i, "value");

        config->axisData.insert(std::make_pair(config->keys[key_tmp],data));
    }

    herb::Parser axes(configPath,{"input","axes"});
    for (int i = 0; i < (int) axes.size(); ++i)
    {
        std::string axis = axes.parseObjectElement<std::string>(i, 0);
        auto value = axes.parseObjectElement<float>(i, 1);
        state->axes.insert(make_pair(axis,value));
    }

    config->logLevel = static_cast<logger::LogLevel>(file.parseElement<int>("debug", "logLevel"));
}

herb::Entity
loadEntity(herb::Parser &components, herb::GameState *state, herb::Storage *storage)
{
    herb::Entity entity = storage->createEntity();
    for (components.iterator = components.data.begin(); components.iterator  != components.data.end(); ++components.iterator)
    {
        herb::Parser component(*components.iterator);
        std::string name = component.parseElement<std::string>("type");
        if (storage->deserializers.find(name) != storage->deserializers.end())
        {
            herb::Component *comp = storage->deserializers[name](component);
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
loadScene(const herb::Config *config, const std::string& sceneName, herb::GameState *state, herb::Storage *storage)
{
    herb::Parser file(sceneName,{"entities"});

    for (file.iterator = file.data.begin(); file.iterator  != file.data.end(); ++file.iterator)
    {
        herb::Parser parser(*file.iterator);
        loadEntity(parser, state, storage);
    }
}

void
call(herb::GameState *state, herb::Storage *storage, const herb::System f, const herb::Entity id)
{
    herb::Signature entSig = storage->entitySignatures[id];
    herb::Signature sysSig = storage->systemSignature[f];
    if ((entSig & sysSig) == sysSig)
    {
        f(state, storage, id);
    }
}

void
updateState(herb::GameState *state, herb::Storage *storage)
{
    for (herb::Entity id : storage->usedIds)
    {
        for (herb::System system : storage->systemsArray)
        {
            call(state, storage, system, id);
        }
    }
}

void
internalRegisterComponents(herb::GameState *state, herb::Storage *storage)
{
    storage->registerComponent<herb::Transform>("Transform");
    storage->registerComponent<herb::Sprite>("Sprite");
    storage->registerComponent<herb::Camera>("Camera");
    storage->registerComponent<herb::Collider>("Collider");
    storage->registerComponent<herb::Physics>("Physics");
    storage->registerComponent<herb::Sound>("Sound");

    storage->registerSystem(herb::setupSound, {TYPE(herb::Sound)});
    storage->registerSystem(herb::render, {TYPE(herb::Transform), TYPE(herb::Sprite)});
    storage->registerSystem(herb::collision, {TYPE(herb::Collider), TYPE(herb::Transform)});
    storage->registerSystem(herb::pushOut, {TYPE(herb::Collider), TYPE(herb::Physics), TYPE(herb::Transform)});
    storage->registerSystem(herb::physics, {TYPE(herb::Collider), TYPE(herb::Physics), TYPE(herb::Transform)});
}

int
main(int argc, char *argv[])
{
    herb::GameState state {};
    herb::Config config;
    loadConfig(&state, "assets/config.json", &config);

    InitWindow(
        config.windowWidth,
        config.windowHeight,
        config.windowTitle.c_str()
    );

    state.running = true;
    // NOTE(andrew): Осталось после SFML
    // state.window = &window;

    herb::Storage storage;

    logger::Logger logger(config.logLevel);
    logger::ConsoleLogger consoleLogger;
    logger.addDestination(&consoleLogger);

    storage.logger = &logger;
    state.logger = &logger;
    state.config = &config;

    internalRegisterComponents(&state, &storage);
    initializeEngine(&state, &storage);
    loadScene(&config, config.defaultScene, &state, &storage);

    state.deltaTime = 0;

    while (state.running)
    {
#if 0
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
                herb::KeyData pressedKeyData = config.axisData[event.key.code];
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

                herb::KeyData pressedKeyData = config.axisData[event.key.code];
                herb::KeyData oppositeKeyData = config.axisData[oppositeKey];

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
#endif

        BeginDrawing();
        ClearBackground(BLACK);
        updateState(&state, &storage);
        EndDrawing();
        state.deltaTime = GetFrameTime();
    }

    return 0;
}
