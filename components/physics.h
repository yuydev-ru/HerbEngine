#ifndef ENGINE_PHYSICS_H
#define ENGINE_PHYSICS_H

#include "transform.h"
#include "../parser.h"
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
    deserialize(Parser &parser)
    {
        auto c = new Collider;

        c->deltaCenter = parser.parseVector2<float>("deltaCenter");
        c->allowCollision = parser.parseElement<bool>("allowCollision");
        c->width = parser.parseElement<float>("width");
        c->height = parser.parseElement<float>("height");

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
    deserialize(Parser &parser)
    {
        auto p = new Physics;
        p->mass = parser.parseElement<float>("mass");
        p->allowGravity = parser.parseElement<bool>("allowGravity");
        p->activeAxes = parser.parseVector2<float>("activeAxes");

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
