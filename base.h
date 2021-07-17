#pragma once

#include <SFML/Graphics.hpp>
#include <logger/logger.h>

#include <nlohmann/json.hpp>

#include <utility>
#include <vector>
#include <queue>
#include <set>
#include <unordered_map>
#include <bitset>
#include <typeindex>
#include <string>
#include <iostream>

#define TYPE(x) std::type_index(typeid(x))
#define MAX_COMPONENTS 32

using json = nlohmann::json;

struct Storage;
struct GameState;
struct Component {};

typedef unsigned int Entity;
typedef void (*System)(GameState *, Storage *, Entity);
typedef Component *(*Deserializer)(json&);
typedef std::bitset<MAX_COMPONENTS> Signature;

struct KeyData
{
    std::string axis;
    std::string axisType;
    float value = 0;

    KeyData (std::string axis, std::string axisType, float value)
    {
        this->axis = std::move(axis);
        this->axisType = std::move(axisType);
        this->value = value;
    }

    KeyData () = default;
};

struct Config
{
    int windowWidth;
    int windowHeight;
    std::string windowTitle;
    std::string defaultScene;
    std::unordered_map<std::string, std::string> oppositeKeys;
    std::unordered_map<std::string, sf::Keyboard::Key> keys;
    std::map<sf::Keyboard::Key, KeyData> axisData;
    logger::LogLevel logLevel;
};
struct GameState
{
    bool running = true;
    sf::RenderWindow *window;
    Entity currentCamera;
    std::unordered_map<std::string, float> axes;
    std::map<sf::Keyboard::Key, bool> pushedKeys;
    logger::Logger* logger;
};

struct Storage
{

    std::unordered_map<std::type_index, int> componentTypes;
    std::unordered_map<std::string, std::type_index> typeNames;
    std::unordered_map<std::string, Deserializer> deserializers;

    std::unordered_map<std::type_index, std::vector<Component *>> entities;
    std::unordered_map<System, Signature> systemSignature;
    std::vector<System> systemsArray;
    std::vector<Signature> entitySignatures;

    Entity lastEntityId = 0;
    std::queue<Entity> freeIds;
    std::set<Entity> usedIds;
    logger::Logger* logger;
    int componentsCount = 0;

    void
    registerSystem(const System& system, const std::vector<std::type_index>& types)
    {
        this->systemSignature[system] = this->createSignature(types);
        this->systemsArray.push_back(system);
    }

    Entity
    createEntity()
    {
        Entity idx;
        if (this->freeIds.empty())
        {
            idx = this->lastEntityId++;
            this->entitySignatures.emplace_back(0);

            for (auto &componentPair : this->entities)
            {
                componentPair.second.push_back(nullptr);
            }
        }
        else
        {
            idx = this->freeIds.front();
            this->freeIds.pop();
            this->entitySignatures[idx] = 0;
        }
        this->usedIds.insert(idx);
        logger->info("Entity " + std::to_string(idx) + " successfully created.");
        return idx;
    }

    void
    destroyEntity(Entity eid)
    {
        for (auto &componentPair : this->entities)
        {
            if (componentPair.second[eid] != nullptr)
            {
                delete componentPair.second[eid];
                componentPair.second[eid] = nullptr;
            }
        }
        logger->info("Entity " + std::to_string(eid) + " successfully destroyed.");
        this->freeIds.push(eid);
    }

    Signature
    createSignature(const std::vector<std::type_index>& components)
    {
        Signature signature;
        for (auto component : components)
        {
            signature.set(this->componentTypes[component]);
        }
        return signature;
    }

    template <class T> void
    registerComponent(const std::string &name)
    {
        std::string componentName = TYPE(T).name();
        if (this->componentTypes.find(TYPE(T)) != this->componentTypes.end())
        {
            logger->warning("Component " + componentName + " is already registered.");
            return;
        }
        logger->info("Component " + componentName + " is registered with Component ID " +
            std::to_string(this->componentsCount) + ".");
        if (this->componentTypes.find(TYPE(T)) != this->componentTypes.end()) return;

//        this->typeNames.insert(std::make_pair(name, TYPE(T)));
        this->typeNames.insert(std::make_pair(name, TYPE(T)));
        this->deserializers.insert(std::make_pair(name, T::deserialize));

        this->componentTypes[TYPE(T)] = this->componentsCount++;
        std::vector<Component *> tmp;
        this->entities.insert(std::make_pair(TYPE(T), tmp));
    }

    template <class T> T*
    addComponent(Entity eid)
    {
        std::string componentName = TYPE(T).name();
        std::string entityID = std::to_string(eid);
        if (this->componentTypes.find(TYPE(T)) == this->componentTypes.end())
        {
            logger->warning("Unknown component " + componentName
                + " found while adding a new component.");
            return nullptr;
        }
        if (this->usedIds.find(eid) == this->usedIds.end())
        {
            logger->warning("Unknown Entity " + entityID
                + " found while adding a new component.");
            return nullptr;
        }

        T *obj = new T;
        this->entities[TYPE(T)][eid] = obj;
        this->entitySignatures[eid].set(this->componentTypes[TYPE(T)]);


        logger->info("Component " + componentName
            + " added to Entity " + entityID + ".");
        return obj;
    }

    template <class T> T*
    getComponent(Entity eid)
    {
        std::string componentName = TYPE(T).name();
        std::string entityID = std::to_string(eid);
        if (this->componentTypes.find(TYPE(T)) == this->componentTypes.end())
        {
            logger->warning("No component " + componentName
                + " found for Entity " + entityID + ".");
            return nullptr;
        }
        if (this->usedIds.find(eid) == this->usedIds.end())
        {
            logger->warning("Unknown Entity " + entityID
                + " found while adding a new component.");
            return nullptr;
        }

        return (T*) this->entities[TYPE(T)][eid];
    }
};