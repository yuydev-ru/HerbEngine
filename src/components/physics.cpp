#include <engine/components/physics.h>

void
Physics::evalResForce()
{
    resForce = {0, 0};
    for (const auto& force : forces)
    {
        resForce += force.second;
    }
}


void
physics(GameState *state, Storage *storage, const Entity id)
{
    auto p = storage->getComponent<Physics>(id);
    auto coll = storage->getComponent<Collider>(id);

    if (p->allowGravity && coll->normal.y != 1)
    {
        p->forces["gravity"] = {0, -1 * p->gravityAcceleration * p->mass};
        p->forces["normal"] = {0, 0};
    }

    if (p->allowGravity && coll->normal.y == 1)
    {
        p->speed.y = 0;
        p->forces["normal"] = {0, -1 * p->forces["gravity"].y};
    }
    if (coll->normal.y == -1)
    {
        p->speed.y = 0;
    }
    p->evalResForce();

    auto resForce = p->resForce / p->mass;

    p->speed += resForce * state->deltaTime;
    auto t = storage->getComponent<Transform>(id);

    if (p->activeAxes.x == 1)
    {
        t->position.x += p->speed.x * state->deltaTime;
    }

    if (p->activeAxes.y == 1)
    {
        t->position.y += p->speed.y * state->deltaTime;
    }

}

void
pushOut(GameState *state, Storage *storage, const Entity id)
{
    auto coll = storage->getComponent<Collider>(id);
    auto p = storage->getComponent<Physics>(id);
    if (coll != nullptr && !coll->collisionList.empty() && !coll->allowCollision)
    {

        auto t = storage->getComponent<Transform>(id);
        sf::Vector2f move = coll->normal * coll->penetration;
        move.x *= p->activeAxes.x;
        move.y *= p->activeAxes.y;
        t->position += move;
    }
}

void
collision (GameState *state, Storage *storage, const Entity id)
{
    auto c = storage->getComponent<Collider>(id);
    auto t = storage->getComponent<Transform>(id);
    auto tPos = t->position;
    auto dC = c->deltaCenter;
    float w = c->width * 0.5f;
    float h = c->height * 0.5f;

    for (Entity id2 : storage->usedIds)
    {
        // NOTE(Roma): Берётся коллайдер другого объекта, и если он есть и этот объект не совпадает с текущим.
        auto c2 = storage->getComponent<Collider>(id2);
        if (c2 != nullptr && id2 != id)
        {
            auto t2 = storage->getComponent<Transform>(id2);
            auto tPos2 = t2->position;
            auto dC2 = c2->deltaCenter;
            float w2 = c2->width * 0.5f;
            float h2 = c2->height * 0.5f;

            sf::Vector2f betweenCenters = {tPos.x + dC.x - tPos2.x - dC2.x, tPos.y + dC.y - tPos2.y - dC2.y};
            // Степень наложения одного коллайдера на другой по оси Х.
            float overlapX = w + w2 - (float) fabs(betweenCenters.x);

            if (overlapX > 0)
            {
                // Степень наложения по оси Y.
                float overlapY = h + h2 - (float) fabs(betweenCenters.y);

                if (overlapY > 0)
                {
                    // Выталкивать нужно в ту сторону, где степень наложения меньше.
                    if (overlapX < overlapY)
                    {
                        if (betweenCenters.x > 0)
                        {
                            c->normal = {1, 0 };
                            c2->normal = {-1, 0 };
                        }
                        else
                        {
                            c->normal = {-1, 0};
                            c2->normal = {1, 0};
                        }
                        c->penetration = overlapX;
                        c2->penetration = overlapX;
                        c->collisionList.insert(id2);
                        c2->collisionList.insert(id);
                        break;
                    }
                    else
                    {
                        if (betweenCenters.y > 0)
                        {
                            c->normal = {0, 1 };
                            c2->normal = {0, -1 };
                        }
                        else
                        {
                            c->normal = {0, -1};
                            c2->normal = {0, 1 };
                        }
                        c->penetration = overlapY;
                        c2->penetration = overlapY;
                        c->collisionList.insert(id2);
                        c2->collisionList.insert(id);
                        break;
                    }
                }
            }
            c->collisionList.erase(id2);
            c2->collisionList.erase(id);
            c->normal = {0, 0};
            c->penetration = 0;
            c2->normal = {0, 0};
            c2->penetration = 0;
        }
    }
}
