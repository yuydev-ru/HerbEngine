#ifndef ENGINE_PHYSICS_H
#define ENGINE_PHYSICS_H

#include "transform.h"
#include <SFML/Graphics.hpp>

struct Collider : Component
{
    float width = 0;
    float height = 0;
    // NOTE(Roma): Разница между центром объекта и центром его коллайдера.
    sf::Vector2f deltaCenter = {0, 0};
    // NOTE(Roma): Список всех объектов, с которыми пересекается данный.
    std::set<Entity> collisionList;
    bool allowCollision = false;
    // NOTE(Roma): Глубина проникновения одного коллайдера в другой.
    float penetration = 0;
    // NOTE(Roma): Направление коллизии.
    sf::Vector2f normal = {0, 0};

    static Component *
    deserialize(Parsing::configFile &dict)
    {
        auto c = new Collider;

        c->deltaCenter = Parsing::parseVector2<float>(dict, "deltaCenter");
        c->allowCollision = Parsing::parseElement<bool>(dict, "allowCollision");
        c->width = Parsing::parseElement<float>(dict, "width");
        c->height = Parsing::parseElement<float>(dict, "height");

        return c;
    }
};

struct Physics : Component
{
    sf::Vector2f speed = {0, 0};
    sf::Vector2f position = {0, 0};
    sf::Vector2f activeAxes = {1, 1};
    const float gravityAcceleration = 500;
    float mass = 1;

    bool allowGravity = true;

    std::map<std::string, sf::Vector2f> forces = {{"gravity", {0, 0}}
            , {"normal", {0, 0}}
            , {"friction", {0, 0}}};
    sf::Vector2f resForce = {0, 0};

    void evalResForce();

    static Component *
    deserialize(Parsing::configFile &dict)
    {
        auto p = new Physics;

        p->mass = Parsing::parseElement<float>(dict, "mass");
        p->allowGravity = Parsing::parseElement<bool>(dict, "allowGravity");
        p->activeAxes = Parsing::parseVector2<float>(dict, "activeAxes");

        return p;
    }
};

void
physics(GameState *, Storage *, Entity);

void
pushOut(GameState *, Storage *, Entity);

void
collision (GameState *, Storage *, Entity);

#endif
