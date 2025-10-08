#ifndef SPRITE_H
#define SPRITE_H

#include <iostream>
#include <string>
#include <assert.h>

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

class Sprite
{
public:
    Sprite();
    ~Sprite();
    void initialize(GLuint shaderID, GLuint texID, vec3 pos, vec3 dimensions, int nAnimations = 1, int nFrames = 1, float angle = 0.0);
    void update();
    void draw();
    void setPosition(vec3 pos);
    vec3 getPosition();
    vec3 getDimensions();
    GLuint getShaderID() { return shaderID; }
    void moveRight(float speed = 1.0);
    void moveLeft(float speed = 1.0);
    void moveUp(float speed = 1.0);
    void moveDown(float speed = 1.0);

    // Para colisão AABB (Axis-Aligned Bounding Box)
    bool checkCollision(Sprite& other);

    // Controle de animação
    void setAnimation(int animIndex);
    void setFPS(float fps);

    // Estado do sprite
    bool isActive;

protected:
    GLuint VAO;
    GLuint texID;
    vec3 pos;
    vec3 dimensions;
    float angle;
    GLuint shaderID;

    // Animação
    int nAnimations, nFrames;
    int iAnimation, iFrame;
    vec2 d; // dimensões do frame na textura
    float lastTime, FPS;

    GLuint setupGeometry();
};

#endif
