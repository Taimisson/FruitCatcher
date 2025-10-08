#include "Sprite.h"

Sprite::Sprite()
{
    isActive = true;
    FPS = 12.0;
    lastTime = 0.0;
    iAnimation = 0;
    iFrame = 0;
}

Sprite::~Sprite()
{
}

void Sprite::initialize(GLuint shaderID, GLuint texID, vec3 pos, vec3 dimensions, int nAnimations, int nFrames, float angle)
{
    this->shaderID = shaderID;
    this->texID = texID;
    this->pos = pos;
    this->dimensions = dimensions;
    this->angle = angle;
    this->nAnimations = nAnimations;
    this->nFrames = nFrames;
    this->iAnimation = 0;
    this->iFrame = 0;
    this->d.s = 1.0 / (float)nFrames;
    this->d.t = 1.0 / (float)nAnimations;
    this->VAO = setupGeometry();
}

void Sprite::update()
{
    mat4 model = mat4(1);
    model = translate(model, pos);
    model = rotate(model, radians(angle), vec3(0.0, 0.0, 1.0));
    model = scale(model, dimensions);

    glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, value_ptr(model));

    vec2 offsetTex = vec2(iFrame * d.s, iAnimation * d.t);
    glUniform2f(glGetUniformLocation(shaderID, "offsetTex"), offsetTex.s, offsetTex.t);

    // Atualização da animação
    float now = glfwGetTime();
    float dt = now - lastTime;

    if (dt >= 1.0 / FPS && nFrames > 1)
    {
        iFrame = (iFrame + 1) % nFrames;
        lastTime = now;
    }
}

void Sprite::draw()
{
    if (!isActive) return;

    glBindVertexArray(VAO);
    glBindTexture(GL_TEXTURE_2D, texID);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void Sprite::setPosition(vec3 pos)
{
    this->pos = pos;
}

vec3 Sprite::getPosition()
{
    return pos;
}

vec3 Sprite::getDimensions()
{
    return dimensions;
}

void Sprite::moveRight(float speed)
{
    pos.x += speed;
}

void Sprite::moveLeft(float speed)
{
    pos.x -= speed;
}

void Sprite::moveUp(float speed)
{
    pos.y += speed;
}

void Sprite::moveDown(float speed)
{
    pos.y -= speed;
}

bool Sprite::checkCollision(Sprite& other)
{
    if (!isActive || !other.isActive) return false;

    // AABB collision detection
    vec3 pos1 = this->pos;
    vec3 dim1 = this->dimensions;
    vec3 pos2 = other.getPosition();
    vec3 dim2 = other.getDimensions();

    // Calcula as bordas dos sprites
    float left1 = pos1.x - dim1.x / 2.0f;
    float right1 = pos1.x + dim1.x / 2.0f;
    float top1 = pos1.y + dim1.y / 2.0f;
    float bottom1 = pos1.y - dim1.y / 2.0f;

    float left2 = pos2.x - dim2.x / 2.0f;
    float right2 = pos2.x + dim2.x / 2.0f;
    float top2 = pos2.y + dim2.y / 2.0f;
    float bottom2 = pos2.y - dim2.y / 2.0f;

    // Verifica sobreposição
    bool collisionX = right1 >= left2 && right2 >= left1;
    bool collisionY = top1 >= bottom2 && top2 >= bottom1;

    return collisionX && collisionY;
}

void Sprite::setAnimation(int animIndex)
{
    if (animIndex >= 0 && animIndex < nAnimations)
    {
        iAnimation = animIndex;
    }
}

void Sprite::setFPS(float fps)
{
    this->FPS = fps;
}

GLuint Sprite::setupGeometry()
{
    // Cada frame deve usar apenas uma porção da textura (d.s x d.t)
    // O offsetTex no shader irá deslocar para a linha/coluna correta
    GLfloat vertices[] = {
        // x     y     z     s    t
        -0.5,  0.5,  0.0,  0.0, d.t,  // v0 - topo esquerdo
        -0.5, -0.5,  0.0,  0.0, 0.0,  // v1 - base esquerdo
         0.5,  0.5,  0.0,  d.s, d.t,  // v2 - topo direito
         0.5, -0.5,  0.0,  d.s, 0.0   // v3 - base direito
    };

    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Atributo posição (x, y, z)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // Atributo coordenada de textura (s, t)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;
}
