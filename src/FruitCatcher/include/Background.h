#ifndef BACKGROUND_H
#define BACKGROUND_H

#include "Sprite.h"

class Background : public Sprite
{
public:
    Background();
    ~Background();

    void setScrollSpeed(float speed);
    float getScrollSpeed();
    void updateScroll(float deltaTime);
    void resetPosition();

    // Métodos herdados/adicionais necessários
    void setVelocity(vec2 vel);
    vec2 getVelocity();
    void updatePosition(float deltaTime);

private:
    float scrollSpeed;
    float scrollOffset;
    vec2 velocity;
};

#endif

