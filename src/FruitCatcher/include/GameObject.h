#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Sprite.h"

// Tipos de objetos do jogo
enum GameObjectType
{
    OBJ_BASKET,
    OBJ_FRUIT,
    OBJ_INSECT
};

class GameObject : public Sprite
{
public:
    GameObject();
    ~GameObject();

    void setType(GameObjectType type);
    GameObjectType getType();
    void setVelocity(vec2 vel);
    vec2 getVelocity();
    void updatePosition(float deltaTime);

    // Animação de rotação
    void setRotationSpeed(float speed);
    float getRotationSpeed();

    // Controle de animação
    void setAnimated(bool animated);
    bool isAnimated();

protected:
    GameObjectType type;
    vec2 velocity; // velocidade em x e y
    float rotationSpeed; // velocidade de rotação (graus por segundo)
    bool animated; // se é um sprite animado (não gira)
};

#endif
