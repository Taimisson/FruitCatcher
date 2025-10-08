#include "Background.h"

Background::Background()
{
    scrollSpeed = 0.0f;
    scrollOffset = 0.0f;
    velocity = vec2(0.0f, 0.0f);
}

Background::~Background()
{
}

void Background::setScrollSpeed(float speed)
{
    this->scrollSpeed = speed;
}

float Background::getScrollSpeed()
{
    return scrollSpeed;
}

void Background::updateScroll(float deltaTime)
{
    // Move o background horizontalmente
    scrollOffset += scrollSpeed * deltaTime;

    // Reset quando completar um ciclo (para loop infinito)
    if (scrollOffset >= dimensions.x)
    {
        scrollOffset = 0.0f;
    }
    else if (scrollOffset <= -dimensions.x)
    {
        scrollOffset = 0.0f;
    }
}

void Background::resetPosition()
{
    scrollOffset = 0.0f;
}

void Background::setVelocity(vec2 vel)
{
    this->velocity = vel;
}

vec2 Background::getVelocity()
{
    return velocity;
}

void Background::updatePosition(float deltaTime)
{
    pos.x += velocity.x * deltaTime;
    pos.y += velocity.y * deltaTime;
}
