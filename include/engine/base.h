#ifndef ENGINE_BASE_H
#define ENGINE_BASE_H

#include <raylib.h>

#include <logger/logger.hpp>

#include "parser.h"
#include "inputdata.h"

#include <utility>
#include <vector>
#include <queue>
#include <set>
#include <unordered_map>
#include <bitset>
#include <iterator>
#include <typeindex>
#include <string>
#include <iostream>

#define TYPE(x) std::type_index(typeid(x))
#define MAX_COMPONENTS 32

namespace herb {

struct Storage;
struct GameState;
struct Component {};

typedef unsigned int Entity;

typedef void (*System)(herb::GameState *, herb::Storage *, herb::Entity);
typedef herb::Component *(*Deserializer)(herb::Parser&);
typedef std::bitset<MAX_COMPONENTS> Signature;

struct KeyData
{
    std::string axis;
    std::string axisType;
    float value = 0;

    KeyData(std::string axis, std::string axisType, float value)
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
    std::unordered_map<std::string, KeyboardKey> keys;
    std::map<KeyboardKey, KeyData> axisData;
    logger::LogLevel logLevel;
};

struct GameState
{
    bool running = true;
    // NOTE(andrew): Осталось после SFML
    // sf::RenderWindow *window;
    herb::Entity currentCamera;
    std::unordered_map<std::string, float> axes;
    std::map<KeyboardKey, bool> pushedKeys;
    float deltaTime;
    logger::Logger *logger;
    herb::Config *config;
};

struct Storage
{

    std::unordered_map<std::type_index, int> componentTypes;
    std::unordered_map<std::string, std::type_index> typeNames;
    std::unordered_map<std::string, herb::Deserializer> deserializers;

    std::unordered_map<std::type_index, std::vector<herb::Component *>> entities;
    std::unordered_map<herb::System, herb::Signature> systemSignature;
    std::vector<herb::System> systemsArray;
    std::vector<herb::Signature> entitySignatures;

    herb::Entity lastEntityId = 0;
    std::queue<herb::Entity> freeIds;
    std::set<herb::Entity> usedIds;
    logger::Logger* logger;
    int componentsCount = 0;

    void
    registerSystem(const herb::System& system, const std::vector<std::type_index>& types)
    {
        this->systemSignature[system] = this->createSignature(types);
        this->systemsArray.push_back(system);
    }

    herb::Entity
    createEntity()
    {
        herb::Entity idx;
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
    destroyEntity(herb::Entity eid)
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

    herb::Signature
    createSignature(const std::vector<std::type_index>& components)
    {
        herb::Signature signature;
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

        this->typeNames.insert(std::make_pair(name, TYPE(T)));
        this->deserializers.insert(std::make_pair(name, T::deserialize));

        this->componentTypes[TYPE(T)] = this->componentsCount++;
        std::vector<herb::Component *> tmp;
        this->entities.insert(std::make_pair(TYPE(T), tmp));
    }

    template <class T> T*
    addComponent(herb::Entity eid)
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
    getComponent(herb::Entity eid)
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

} // namespace herb

#endif
