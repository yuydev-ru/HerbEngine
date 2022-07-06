#ifndef ENGINE_PHYSICS_H
#define ENGINE_PHYSICS_H

#include <glm/vec2.hpp>

#include "transform.h"
#include "../parser.h"

namespace herb {

struct Collider : Component
{
    float width = 0;
    float height = 0;
    // NOTE(Roma): Разница между центром объекта и центром его коллайдера.
    glm::vec2 deltaCenter = {0, 0};
    // NOTE(Roma): Список всех объектов, с которыми пересекается данный.
    std::set<herb::Entity> collisionList;
    bool allowCollision = false;
    // NOTE(Roma): Глубина проникновения одного коллайдера в другой.
    float penetration = 0;
    // NOTE(Roma): Направление коллизии.
    glm::vec2 normal = {0, 0};

    static herb::Component *
    deserialize(herb::Parser &parser)
    {
        auto c = new herb::Collider;

        c->deltaCenter = parser.parseVector2("deltaCenter");
        c->allowCollision = parser.parseElement<bool>("allowCollision");
        c->width = parser.parseElement<float>("width");
        c->height = parser.parseElement<float>("height");

        return c;
    }
};

struct Physics : Component
{
    glm::vec2 speed = {0, 0};
    glm::vec2 position = {0, 0};
    glm::vec2 activeAxes = {1, 1};
    const float gravityAcceleration = 500;
    float mass = 1;

    bool allowGravity = true;

    std::map<std::string, glm::vec2> forces = {{"gravity", {0, 0}}
            , {"normal", {0, 0}}
            , {"friction", {0, 0}}};
    glm::vec2 resForce = {0, 0};

    void evalResForce();

    static Component *
    deserialize(herb::Parser &parser)
    {
        auto p = new Physics;
        p->mass = parser.parseElement<float>("mass");
        p->allowGravity = parser.parseElement<bool>("allowGravity");
        p->activeAxes = parser.parseVector2("activeAxes");

        return p;
    }
};

void
physics(herb::GameState *, herb::Storage *, herb::Entity);

void
pushOut(herb::GameState *, herb::Storage *, herb::Entity);

void
collision (herb::GameState *, herb::Storage *, herb::Entity);

} // namespace herb

#endif
