#include "rendering.h"
#include "transform.h"

void
AnimationController::setAnimation(const std::string &name)
{
    for (auto anim : this->animations)
    {
        if (anim->name == name)
        {
            this->currentAnimation = anim;
            this->currentFrame = 0;
            this->timeout = 0;
            break;
        }
    }
}

void
render(GameState *state, Storage *storage, const Entity id)
{
    auto camera = storage->getComponent<Camera>(state->currentCamera);
    auto camTransform = storage->getComponent<Transform>(state->currentCamera);
    auto camPos = camTransform->position;

    auto t = storage->getComponent<Transform>(id);
    auto spr = storage->getComponent<Sprite>(id);

    sf::Vector2f screenPos = { (t->position.x - camPos.x) * camera->scale.x
                             , (camPos.y - t->position.y) * camera->scale.y };

    screenPos += (sf::Vector2f) state->window->getSize() * 0.5f;

    auto sprRect = (sf::FloatRect) spr->sprite.getTextureRect();
    sf::Vector2f spriteSize = {sprRect.width, sprRect.height};
    spriteSize.x *= t->scale.x * camera->scale.x;
    spriteSize.y *= t->scale.y * camera->scale.y;
    screenPos -= spriteSize * 0.5f;

    auto screenScale = t->scale;
    screenScale.x *= camera->scale.x;
    screenScale.y *= camera->scale.y;

    spr->sprite.setPosition(screenPos);
    spr->sprite.setScale(screenScale);
    state->window->draw(spr->sprite);
}

void
updateAnimation(GameState *state, Storage *storage, const Entity id)
{
    auto spr = storage->getComponent<Sprite>(id);
    auto controller = storage->getComponent<AnimationController>(id);

    controller->timeout -= state->deltaTime;
    if (controller->timeout > 0) return;

    spr->sprite = controller->currentAnimation->sprite;
    spr->sprite.setTextureRect(controller->currentAnimation->frames.at(controller->currentFrame));

    controller->currentFrame = (controller->currentFrame + 1) % controller->currentAnimation->numberOfFrames;
    controller->timeout = controller->currentAnimation->timeout;
}