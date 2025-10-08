#include "GameObject.h"

GameObject::GameObject() : Sprite()
{
    velocity = vec2(0.0, 0.0);
    type = OBJ_FRUIT;
    rotationSpeed = 0.0f; // sem rotação por padrão
    animated = false; // não é animado por padrão
}

GameObject::~GameObject()
{
}

void GameObject::setType(GameObjectType type)
{
    this->type = type;
}

GameObjectType GameObject::getType()
{
    return type;
}

void GameObject::setVelocity(vec2 vel)
{
    this->velocity = vel;
}

vec2 GameObject::getVelocity()
{
    return velocity;
}

void GameObject::setRotationSpeed(float speed)
{
    this->rotationSpeed = speed;
}

float GameObject::getRotationSpeed()
{
    return rotationSpeed;
}

void GameObject::setAnimated(bool anim)
{
    this->animated = anim;
}

bool GameObject::isAnimated()
{
    return animated;
}

void GameObject::updatePosition(float deltaTime)
{
    pos.x += velocity.x * deltaTime;
    pos.y += velocity.y * deltaTime;

    // Aplicar rotação apenas se NÃO for animado
    if (!animated) {
        angle += rotationSpeed * deltaTime;
    }

    // Manter ângulo entre 0 e 360 graus
    if (angle >= 360.0f) angle -= 360.0f;
    if (angle < 0.0f) angle += 360.0f;
}
