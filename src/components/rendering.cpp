#include <engine/components/rendering.h>
#include <engine/components/transform.h>

void
herb::render(herb::GameState *state, herb::Storage *storage, const herb::Entity id)
{
    auto camera = storage->getComponent<herb::Camera>(state->currentCamera);
    auto camTransform = storage->getComponent<herb::Transform>(state->currentCamera);
    auto camPos = camTransform->position;

    auto t = storage->getComponent<herb::Transform>(id);
    auto spr = storage->getComponent<herb::Sprite>(id);

    glm::vec2 screenPos = { (t->position.x - camPos.x) * camera->scale.x
            , (camPos.y - t->position.y) * camera->scale.y };

    screenPos += glm::vec2 { 
        state->config->windowWidth,
        state->config->windowHeight,
    } * 0.5f;

    auto spriteSize = glm::vec2 {
        spr->originalImage.width,
        spr->originalImage.height,
    };
    spriteSize.x *= t->scale.x * camera->scale.x;
    spriteSize.y *= t->scale.y * camera->scale.y;
    screenPos -= spriteSize * 0.5f;

    // NOTE(andrew): Осталось после SFML
    // auto screenScale = t->scale;
    // screenScale.x *= camera->scale.x;
    // screenScale.y *= camera->scale.y;

    if (spriteSize.x != spr->texture.width
        || spriteSize.y != spr->texture.height)
    {
        UnloadTexture(spr->texture);
        Image image = ImageCopy(spr->originalImage);
        ImageResize(&image, spriteSize.x, spriteSize.y);
        spr->texture = LoadTextureFromImage(image);
    }
    DrawTexture(spr->texture, screenPos.x, screenPos.y, WHITE);
}
