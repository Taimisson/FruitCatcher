/*
 * Fruit Catcher Game
 *
 * Trabalho Grau A - Processamento Gráfico: Fundamentos
 *
 * Jogo simples onde o jogador controla uma cesta que deve coletar frutas
 * que caem do topo da tela, evitando insetos.
 *
 * Controles:
 *   - Setas Esquerda/Direita ou A/D: Move a cesta
 *   - Mouse: Move a cesta seguindo o cursor
 *   - ESC: Sair do jogo
 */

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sstream>
#include <map>

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

// STB_IMAGE
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "GameObject.h"
#include "Background.h"
#include "AudioManager.h"

// Protótipos de funções
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
int setupShader();

// ========================================
// ESTRUTURA DE CONFIGURAÇÃO
// ========================================
struct GameConfig {
    // Janela
    int windowWidth = 800;
    int windowHeight = 800;
    string windowTitle = "Fruit Catcher - Trabalho Grau A";

    // Paths de assets
    map<string, string> texturePaths;
    map<string, string> audioPaths;
    map<string, string> spritesheetPaths;
    map<string, pair<int, int>> spritesheetDimensions; // nAnimations, nFrames

    // Gameplay
    int maxLives = 5;
    int initialLives = 3;
    float baseSpawnInterval = 1.5f;
    float minSpawnInterval = 0.3f;
    int fruitSpawnChance = 70;
    float baseFallSpeed = 100.0f;
    float maxFallSpeed = 500.0f;
    float basketSpeed = 300.0f;
    float difficultyIncreaseRate = 10.0f;

    // Parallax
    float parallaxSpeed = 20.0f;
    float layer1ScrollRate = 0.0f;
    float layer2ScrollRate = 0.5f;
    float layer3ScrollRate = 0.0f;

    // Animações
    float bushOscillationSpeed = 1.5f;
    float bushOscillationAmount = 15.0f;
    float bushMiddleShakeSpeed = 8.0f;
};

GameConfig config;

// Dimensões da janela - serão definidas a partir do config
// Estas constantes facilitam o uso no código
#define WIDTH config.windowWidth
#define HEIGHT config.windowHeight

// ========================================
// FUNÇÃO PARA CARREGAR CONFIG.TXT
// ========================================
bool loadConfig(const string& filename) {
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "AVISO: Não foi possível abrir " << filename << endl;
        cerr << "Usando configurações padrão." << endl;
        return false;
    }

    cout << "\n=== CARREGANDO CONFIGURAÇÕES ===" << endl;
    cout << "Arquivo: " << filename << endl;

    string line;
    int lineCount = 0;
    int configsLoaded = 0;

    while (getline(file, line)) {
        lineCount++;

        // Remover espaços em branco no início e fim
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);

        // Ignorar linhas vazias e comentários
        if (line.empty() || line[0] == '#') {
            continue;
        }

        // Procurar pelo '='
        size_t pos = line.find('=');
        if (pos == string::npos) {
            cerr << "Linha " << lineCount << " inválida (sem '='): " << line << endl;
            continue;
        }

        // Separar chave e valor
        string key = line.substr(0, pos);
        string value = line.substr(pos + 1);

        // Remover espaços
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);

        // PROCESSAR CONFIGURAÇÕES

        // === JANELA ===
        if (key == "WINDOW_WIDTH") {
            config.windowWidth = stoi(value);
            configsLoaded++;
        }
        else if (key == "WINDOW_HEIGHT") {
            config.windowHeight = stoi(value);
            configsLoaded++;
        }
        else if (key == "WINDOW_TITLE") {
            config.windowTitle = value;
            configsLoaded++;
        }

        // === TEXTURAS (BACKGROUNDS) ===
        else if (key == "BG_LAYER_1") {
            config.texturePaths["BG_LAYER_1"] = value;
            configsLoaded++;
        }
        else if (key == "CLOUD_1") {
            config.texturePaths["CLOUD_1"] = value;
            configsLoaded++;
        }
        else if (key == "CLOUD_2") {
            config.texturePaths["CLOUD_2"] = value;
            configsLoaded++;
        }
        else if (key == "CLOUD_3") {
            config.texturePaths["CLOUD_3"] = value;
            configsLoaded++;
        }
        else if (key == "CLOUD_4") {
            config.texturePaths["CLOUD_4"] = value;
            configsLoaded++;
        }
        else if (key == "BUSH_LEFT") {
            config.texturePaths["BUSH_LEFT"] = value;
            configsLoaded++;
        }
        else if (key == "BUSH_MIDDLE") {
            config.texturePaths["BUSH_MIDDLE"] = value;
            configsLoaded++;
        }
        else if (key == "BUSH_RIGHT") {
            config.texturePaths["BUSH_RIGHT"] = value;
            configsLoaded++;
        }

        // === SPRITES ===
        else if (key == "BASKET") {
            config.texturePaths["BASKET"] = value;
            configsLoaded++;
        }

        // === FRUTAS ===
        else if (key == "APPLE") {
            config.texturePaths["APPLE"] = value;
            configsLoaded++;
        }
        else if (key == "BANANA") {
            config.texturePaths["BANANA"] = value;
            configsLoaded++;
        }
        else if (key == "ORANGE") {
            config.texturePaths["ORANGE"] = value;
            configsLoaded++;
        }
        else if (key == "PINEAPPLE") {
            config.texturePaths["PINEAPPLE"] = value;
            configsLoaded++;
        }

        // === INSETOS ===
        else if (key == "BEE") {
            config.texturePaths["BEE"] = value;
            configsLoaded++;
        }
        else if (key == "FLY") {
            config.texturePaths["FLY"] = value;
            configsLoaded++;
        }
        else if (key == "LADYBUG") {
            config.texturePaths["LADYBUG"] = value;
            configsLoaded++;
        }

        // === SPRITESHEETS ANIMADOS (formato: path|nAnimations|nFrames) ===
        else if (key == "BEE_FLY_SPR" || key == "BIRD_FLY_SPR" || key == "BIRD2_FLY_SPR" ||
                 key == "RAT_WALK_SPR" || key == "RAT2_WALK_SPR") {
            // Dividir por '|'
            size_t pipe1 = value.find('|');
            size_t pipe2 = value.rfind('|');

            if (pipe1 != string::npos && pipe2 != string::npos && pipe1 != pipe2) {
                string path = value.substr(0, pipe1);
                int nAnimations = stoi(value.substr(pipe1 + 1, pipe2 - pipe1 - 1));
                int nFrames = stoi(value.substr(pipe2 + 1));

                config.spritesheetPaths[key] = path;
                config.spritesheetDimensions[key] = make_pair(nAnimations, nFrames);
                configsLoaded++;
            }
        }

        // === HUD ===
        else if (key == "HEART_FULL") {
            config.texturePaths["HEART_FULL"] = value;
            configsLoaded++;
        }
        else if (key == "HEART_EMPTY") {
            config.texturePaths["HEART_EMPTY"] = value;
            configsLoaded++;
        }

        // === TELAS ===
        else if (key == "MENU_TITLE") {
            config.texturePaths["MENU_TITLE"] = value;
            configsLoaded++;
        }
        else if (key == "MENU_START") {
            config.texturePaths["MENU_START"] = value;
            configsLoaded++;
        }
        else if (key == "GAME_OVER") {
            config.texturePaths["GAME_OVER"] = value;
            configsLoaded++;
        }

        // === NÚMEROS ===
        else if (key.find("NUMBER_") == 0) {
            config.texturePaths[key] = value;
            configsLoaded++;
        }

        // === ÁUDIO ===
        else if (key == "MUSIC_BACKGROUND") {
            config.audioPaths["MUSIC_BACKGROUND"] = value;
            configsLoaded++;
        }
        else if (key == "SOUND_FRUIT_COLLECT") {
            config.audioPaths["SOUND_FRUIT_COLLECT"] = value;
            configsLoaded++;
        }
        else if (key == "SOUND_BUG_COLLECT") {
            config.audioPaths["SOUND_BUG_COLLECT"] = value;
            configsLoaded++;
        }

        // === GAMEPLAY ===
        else if (key == "MAX_LIVES") {
            config.maxLives = stoi(value);
            configsLoaded++;
        }
        else if (key == "INITIAL_LIVES") {
            config.initialLives = stoi(value);
            configsLoaded++;
        }
        else if (key == "BASE_SPAWN_INTERVAL") {
            config.baseSpawnInterval = stof(value);
            configsLoaded++;
        }
        else if (key == "MIN_SPAWN_INTERVAL") {
            config.minSpawnInterval = stof(value);
            configsLoaded++;
        }
        else if (key == "FRUIT_SPAWN_CHANCE") {
            config.fruitSpawnChance = stoi(value);
            configsLoaded++;
        }
        else if (key == "BASE_FALL_SPEED") {
            config.baseFallSpeed = stof(value);
            configsLoaded++;
        }
        else if (key == "MAX_FALL_SPEED") {
            config.maxFallSpeed = stof(value);
            configsLoaded++;
        }
        else if (key == "BASKET_SPEED") {
            config.basketSpeed = stof(value);
            configsLoaded++;
        }
        else if (key == "DIFFICULTY_INCREASE_RATE") {
            config.difficultyIncreaseRate = stof(value);
            configsLoaded++;
        }

        // === PARALLAX ===
        else if (key == "PARALLAX_SPEED") {
            config.parallaxSpeed = stof(value);
            configsLoaded++;
        }
        else if (key == "LAYER1_SCROLL_RATE") {
            config.layer1ScrollRate = stof(value);
            configsLoaded++;
        }
        else if (key == "LAYER2_SCROLL_RATE") {
            config.layer2ScrollRate = stof(value);
            configsLoaded++;
        }
        else if (key == "LAYER3_SCROLL_RATE") {
            config.layer3ScrollRate = stof(value);
            configsLoaded++;
        }

        // === ANIMAÇÕES ===
        else if (key == "BUSH_OSCILLATION_SPEED") {
            config.bushOscillationSpeed = stof(value);
            configsLoaded++;
        }
        else if (key == "BUSH_OSCILLATION_AMOUNT") {
            config.bushOscillationAmount = stof(value);
            configsLoaded++;
        }
        else if (key == "BUSH_MIDDLE_SHAKE_SPEED") {
            config.bushMiddleShakeSpeed = stof(value);
            configsLoaded++;
        }
    }

    file.close();

    cout << "Configurações carregadas: " << configsLoaded << " itens" << endl;
    cout << "====================================\n" << endl;

    return true;
}

// Estrutura para armazenar textura com dimensões
struct TextureInfo {
    GLuint id;
    int width;
    int height;
    TextureInfo() : id(0), width(0), height(0) {}
    TextureInfo(GLuint texID, int w, int h) : id(texID), width(w), height(h) {}
};

TextureInfo loadTextureWithInfo(string filePath);
GLuint loadTexture(string filePath);
GLuint setupBackgroundGeometry();
GLuint setupCloudLayerGeometry();
GLuint setupBushLeftGeometry();
GLuint setupBushRightGeometry();
GLuint setupBushMiddleGeometry();
void drawBackgroundLayer(GLuint shaderID, GLuint texID, float xOffset, float zDepth);
void drawCloudLayer(GLuint shaderID, GLuint texID, float xPosition, float zDepth);
void drawBushLayer(GLuint shaderID, GLuint VAO, GLuint texID, float xOffset, float zDepth);
void spawnFallingObject();
void updateGame(float deltaTime);
void checkCollisions();
void resetGame();
GLuint setupHUDGeometry();
void drawHUD(GLuint shaderID);
void drawHUDElement(GLuint shaderID, GLuint texID, float x, float y, float width, float height);
void drawNumber(GLuint shaderID, int number, float x, float y, float digitSize, float spacing);
GLuint createColoredQuadTexture(float r, float g, float b, float a);


// Shaders
const GLchar *vertexShaderSource = R"glsl(
#version 400
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texc;
out vec2 tex_coord;
uniform mat4 projection;
uniform mat4 model;
void main()
{
    gl_Position = projection * model * vec4(position.x, position.y, position.z, 1.0);
    tex_coord = vec2(texc.s, 1.0 - texc.t);
}
)glsl";

const GLchar *fragmentShaderSource = R"glsl(
#version 400
in vec2 tex_coord;
out vec4 color;
uniform sampler2D tex_buffer;
uniform vec2 offsetTex;
void main()
{
    color = texture(tex_buffer, tex_coord + offsetTex);
}
)glsl";

// Variáveis globais do jogo
bool keys[1024];
GameObject basket;
vector<GameObject*> fallingObjects;
vector<Background*> backgroundLayers; // Camadas de fundo para Parallax

// Estados do jogo
enum GameState {
    MENU,      // Tela inicial
    PLAYING,   // Jogando
    GAME_OVER  // Tela de game over
};

GameState gameState = MENU; // Começar no menu

int score = 0;
int lives = 3;
int maxLives = 5; // Número máximo de vidas
float spawnTimer = 0.0f;
float spawnInterval = 1.5f; // Spawna objeto a cada 1.5 segundos
float objectFallSpeed = 100.0f; // pixels por segundo
double mouseX = 400.0; // Será inicializado com config.windowWidth / 2.0 na main
bool useMouseControl = false;

// Sistema de dificuldade progressiva
float baseSpawnInterval = 1.5f;    // Intervalo inicial de spawn
float minSpawnInterval = 0.3f;     // Intervalo mínimo (muito rápido!)
float baseFallSpeed = 100.0f;      // Velocidade inicial de queda
float maxFallSpeed = 500.0f;       // Velocidade máxima de queda
float difficultyTimer = 0.0f;      // Timer para aumentar dificuldade
float difficultyIncreaseRate = 10.0f; // Aumenta dificuldade a cada 10 segundos
int currentLevel = 1;              // Nível atual do jogador

// Sistema de Parallax (camadas de fundo com velocidades diferentes)
float parallaxOffset = 0.0f;       // Offset global de movimento do parallax
float parallaxSpeed = 20.0f;       // Velocidade base do parallax (pixels por segundo)
float layer1ScrollRate = 0.0f;     // Camada 1 (Sky) - estática (0%)
float layer2ScrollRate = 0.5f;     // Camada 2 (Clouds) - movimento contínuo (50%)
float layer3ScrollRate = 0.0f;     // Camada 3 (Bushes) - movimento oscilatório

// Movimento oscilatório das bushes
float bushOscillationTime = 0.0f;  // Timer para movimento oscilatório
float bushOscillationSpeed = 1.5f; // Velocidade da oscilação
float bushOscillationAmount = 15.0f; // Amplitude do movimento (pixels)

// Bush central (bushAlt1) com tremor
float bushMiddleShakeTime = 0.0f;
float bushMiddleShakeSpeed = 8.0f;  // Velocidade do tremor (mais rápido)
float bushMiddleShakeAmount = 3.0f; // Amplitude menor para tremor sutil

// Sistema de bounce para nuvens - Centralizadas no céu
float cloudPositions[4] = {200.0f, 600.0f, 100.0f, 500.0f}; // Posições X iniciais (espalhadas no céu)
float cloudSpeeds[4] = {40.0f, 45.0f, 35.0f, 50.0f};        // Velocidades diferentes
float cloudDirections[4] = {1.0f, -1.0f, 1.0f, -1.0f};      // 2 para direita, 2 para esquerda

// IDs de texturas - Arrays para múltiplas frutas e insetos
GLuint basketTexID = 0;
vector<GLuint> fruitTexIDs;  // Múltiplas frutas
vector<GLuint> insectTexIDs; // Múltiplos insetos
vector<GLuint> backgroundTexIDs; // Texturas das camadas de fundo
GLuint backgroundVAO = 0; // VAO para desenhar o background
GLuint cloudVAO = 0;      // VAO para desenhar as nuvens
GLuint bushLeftVAO = 0;   // VAO para bush esquerda
GLuint bushRightVAO = 0;  // VAO para bush direita
GLuint bushMiddleVAO = 0; // VAO para bush central (com tremor)

// HUD - Texturas para interface
GLuint heartFullTexID = 0;   // Coração cheio
GLuint heartEmptyTexID = 0;  // Coração vazio
GLuint hudVAO = 0;            // VAO para elementos do HUD
GLuint whitePixelTexID = 0;  // Textura de 1 pixel branco para desenhar formas coloridas

// Telas do jogo
TextureInfo menuTitleInfo;   // Título do menu
TextureInfo menuStartInfo;   // Botão "Iniciar Jogo"
TextureInfo gameOverInfo;    // Texto "Game Over"

// Números para pontuação (0-9)
vector<GLuint> numberTexIDs; // Texturas dos números 0 a 9

// Texturas auxiliares
GLuint blackTexID = 0;  // Textura preta para fundo dos números

// Estrutura para armazenar informações de spritesheet
struct SpritesheetInfo {
    GLuint texID;
    int nAnimations;  // número de linhas
    int nFrames;      // número de colunas
};

// Spritesheets de insetos animados (com fundo transparente)
vector<SpritesheetInfo> insectSpritesheets; // Lista de spritesheets com suas dimensões

// Sistema de áudio
AudioManager audioManager;

// Função MAIN
int main()
{
    srand(time(NULL));

    // ========================================
    // CARREGAR ARQUIVO DE CONFIGURAÇÃO
    // ========================================
    // Usar APENAS o config.txt da pasta src/FruitCatcher
    bool configLoaded = loadConfig("../src/FruitCatcher/config.txt");

    if (!configLoaded) {
        cout << "ERRO: Arquivo config.txt não encontrado em ../src/FruitCatcher/config.txt" << endl;
        cout << "Usando configurações padrão." << endl;
    }

    // Aplicar configurações carregadas
    lives = config.initialLives;
    maxLives = config.maxLives;
    baseSpawnInterval = config.baseSpawnInterval;
    spawnInterval = config.baseSpawnInterval;
    minSpawnInterval = config.minSpawnInterval;
    baseFallSpeed = config.baseFallSpeed;
    objectFallSpeed = config.baseFallSpeed;
    maxFallSpeed = config.maxFallSpeed;
    difficultyIncreaseRate = config.difficultyIncreaseRate;
    parallaxSpeed = config.parallaxSpeed;
    layer1ScrollRate = config.layer1ScrollRate;
    layer2ScrollRate = config.layer2ScrollRate;
    layer3ScrollRate = config.layer3ScrollRate;
    bushOscillationSpeed = config.bushOscillationSpeed;
    bushOscillationAmount = config.bushOscillationAmount;
    bushMiddleShakeSpeed = config.bushMiddleShakeSpeed;

    // Inicialização da GLFW
    glfwInit();

    // Criação da janela GLFW (usando configurações do arquivo)
    GLFWwindow *window = glfwCreateWindow(
        config.windowWidth,
        config.windowHeight,
        config.windowTitle.c_str(),
        nullptr, nullptr
    );
    if (!window)
    {
        cerr << "Falha ao criar a janela GLFW" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Callbacks
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // GLAD: carrega os ponteiros de funções da OpenGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cerr << "Falha ao inicializar GLAD" << endl;
        return -1;
    }

    // Informações de versão
    const GLubyte *renderer = glGetString(GL_RENDERER);
    const GLubyte *version = glGetString(GL_VERSION);
    cout << "Renderer: " << renderer << endl;
    cout << "OpenGL version: " << version << endl;

    // Inicializar array de teclas
    for (int i = 0; i < 1024; i++)
    {
        keys[i] = false;
    }

    // Compilar shaders
    GLuint shaderID = setupShader();

    // ========================================
    // INICIALIZAR SISTEMA DE ÁUDIO
    // ========================================
    cout << "\n=== INICIALIZANDO SISTEMA DE ÁUDIO ===" << endl;
    if (audioManager.initialize()) {
        // Carregar música de fundo
        if (audioManager.loadMusic("../assets/audio/musica_fundo.mp3")) {
            audioManager.setMusicVolume(0.5f); // Volume em 50%
            audioManager.playMusic(true); // Tocar em loop
            cout << "Música de fundo tocando!" << endl;
        } else {
            cout << "Aviso: Não foi possível carregar a música de fundo." << endl;
        }

        // Carregar efeitos sonoros
        cout << "\nCarregando efeitos sonoros:" << endl;
        audioManager.loadSound("fruit_collect", "../assets/audio/fruit_collect.mp3");
        audioManager.loadSound("bug_collect", "../assets/audio/bug_collect.mp3");
    } else {
        cout << "Aviso: Sistema de áudio não inicializado." << endl;
    }
    cout << "======================================\n" << endl;

    // Carregar texturas de background (Parallax) - NOVA CONFIGURAÇÃO
    // Layer 1: Fundo completo com céu (estático)
    GLuint bgTex1 = loadTexture("../kenney_background-elements-redux/Backgrounds/backgroundColorGrass.png");

    // Layer 2: Nuvens individuais (movimento com bounce)
    GLuint cloudTex1 = loadTexture("../kenney_background-elements-redux/PNG/Default/cloud1.png");
    GLuint cloudTex2 = loadTexture("../kenney_background-elements-redux/PNG/Default/cloud2.png");
    GLuint cloudTex3 = loadTexture("../kenney_background-elements-redux/PNG/Default/cloud3.png");
    GLuint cloudTex4 = loadTexture("../kenney_background-elements-redux/PNG/Default/cloud4.png");

    // Layer 3: Bushes com movimento oscilatório e tremor
    GLuint bushLeftTex = loadTexture("../kenney_background-elements-redux/PNG/Default/bushAlt2.png");
    GLuint bushMiddleTex = loadTexture("../kenney_background-elements-redux/PNG/Default/bushAlt1.png");
    GLuint bushRightTex = loadTexture("../kenney_background-elements-redux/PNG/Default/bushAlt3.png");

    if (bgTex1 > 0) backgroundTexIDs.push_back(bgTex1);
    if (cloudTex1 > 0) backgroundTexIDs.push_back(cloudTex1);
    if (cloudTex2 > 0) backgroundTexIDs.push_back(cloudTex2);
    if (cloudTex3 > 0) backgroundTexIDs.push_back(cloudTex3);
    if (cloudTex4 > 0) backgroundTexIDs.push_back(cloudTex4);
    if (bushLeftTex > 0) backgroundTexIDs.push_back(bushLeftTex);
    if (bushMiddleTex > 0) backgroundTexIDs.push_back(bushMiddleTex);
    if (bushRightTex > 0) backgroundTexIDs.push_back(bushRightTex);

    cout << "\n=== CAMADAS DE PARALLAX ===" << endl;
    cout << "Layer 1 (Background completo): " << (bgTex1 > 0 ? "OK" : "ERRO") << endl;
    cout << "Layer 2 (Nuvem 1): " << (cloudTex1 > 0 ? "OK" : "ERRO") << endl;
    cout << "Layer 2 (Nuvem 2): " << (cloudTex2 > 0 ? "OK" : "ERRO") << endl;
    cout << "Layer 2 (Nuvem 3): " << (cloudTex3 > 0 ? "OK" : "ERRO") << endl;
    cout << "Layer 2 (Nuvem 4): " << (cloudTex4 > 0 ? "OK" : "ERRO") << endl;
    cout << "Layer 3 (Bush Esquerda): " << (bushLeftTex > 0 ? "OK" : "ERRO") << endl;
    cout << "Layer 3 (Bush Central com tremor): " << (bushMiddleTex > 0 ? "OK" : "ERRO") << endl;
    cout << "Layer 3 (Bush Direita): " << (bushRightTex > 0 ? "OK" : "ERRO") << endl;

    // Carregar cesta
    basketTexID = loadTexture("../assets/sprites/fruitcatcher/ui/basket.png");

    // Carregar TODAS as frutas disponíveis
    GLuint appleTexID = loadTexture("../assets/sprites/fruitcatcher/fruits/apple.png");
    GLuint bananaTexID = loadTexture("../assets/sprites/fruitcatcher/fruits/banana.png");
    GLuint orangeTexID = loadTexture("../assets/sprites/fruitcatcher/fruits/orange.png");
    GLuint pineappleTexID = loadTexture("../assets/sprites/fruitcatcher/fruits/pineapple.png");

    if (appleTexID > 0) fruitTexIDs.push_back(appleTexID);
    if (bananaTexID > 0) fruitTexIDs.push_back(bananaTexID);
    if (orangeTexID > 0) fruitTexIDs.push_back(orangeTexID);
    if (pineappleTexID > 0) fruitTexIDs.push_back(pineappleTexID);

    // Carregar TODOS os insetos disponíveis
    GLuint beeTexID = loadTexture("../assets/sprites/fruitcatcher/insects/bee.png");
    GLuint flyTexID = loadTexture("../assets/sprites/fruitcatcher/insects/fly.png");
    GLuint ladybugTexID = loadTexture("../assets/sprites/fruitcatcher/insects/ladybug.png");

    if (beeTexID > 0) insectTexIDs.push_back(beeTexID);
    if (flyTexID > 0) insectTexIDs.push_back(flyTexID);
    if (ladybugTexID > 0) insectTexIDs.push_back(ladybugTexID);


    // ========================================
    // CARREGAR SPRITESHEETS ANIMADOS
    // ========================================
    cout << "\n=== CARREGANDO SPRITES ANIMADOS ===" << endl;

    // Abelhas voando (bee_fly_spr) - 1 linha, 6 frames
    GLuint beeFlyTexID = loadTexture("../assets/sprites/fruitcatcher/animated_sprites/bee_fly_spr.png");
    if (beeFlyTexID > 0) {
        insectSpritesheets.push_back({beeFlyTexID, 1, 6});
        cout << "  - Abelha voando: OK (1x6)" << endl;
    }

    // Abelhas 2 voando (bee2_fly_spr) - 1 linha, 6 frames
    GLuint bee2FlyTexID = loadTexture("../assets/sprites/fruitcatcher/animated_sprites/bee2_fly_spr.png");
    if (bee2FlyTexID > 0) {
        insectSpritesheets.push_back({bee2FlyTexID, 1, 6});
        cout << "  - Abelha 2 voando: OK (1x6)" << endl;
    }

    // Pássaro 1 voando (bird_fly_spr) - 1 linha, 6 frames
    GLuint birdFlyTexID = loadTexture("../assets/sprites/fruitcatcher/animated_sprites/bird_fly_spr.png");
    if (birdFlyTexID > 0) {
        insectSpritesheets.push_back({birdFlyTexID, 1, 6});
        cout << "  - Pássaro 1 voando: OK (1x6)" << endl;
    }

    // Pássaro 2 voando (bird2_fly_spr) - 1 linha, 6 frames
    GLuint bird2FlyTexID = loadTexture("../assets/sprites/fruitcatcher/animated_sprites/bird2_fly_spr.png");
    if (bird2FlyTexID > 0) {
        insectSpritesheets.push_back({bird2FlyTexID, 1, 6});
        cout << "  - Pássaro 2 voando: OK (1x6)" << endl;
    }

    // Rato 1 andando (rat_walk_spr) - 1 linha, 4 frames
    GLuint ratWalkTexID = loadTexture("../assets/sprites/fruitcatcher/animated_sprites/rat_walk_spr.png");
    if (ratWalkTexID > 0) {
        insectSpritesheets.push_back({ratWalkTexID, 1, 4});
        cout << "  - Rato 1 andando: OK (1x4)" << endl;
    }

    // Rato 2 andando (rat2_walk_spr) - 1 linha, 4 frames
    GLuint rat2WalkTexID = loadTexture("../assets/sprites/fruitcatcher/animated_sprites/rat2_walk_spr.png");
    if (rat2WalkTexID > 0) {
        insectSpritesheets.push_back({rat2WalkTexID, 1, 4});
        cout << "  - Rato 2 andando: OK (1x4)" << endl;
    }

    cout << "Total de sprites animados: " << insectSpritesheets.size() << endl;
    cout << "========================================\n" << endl;

    cout << "Frutas carregadas: " << fruitTexIDs.size() << endl;
    cout << "Insetos carregados: " << insectTexIDs.size() << endl;
    cout << "Sprites animados carregados: " << insectSpritesheets.size() << endl;
    cout << "Backgrounds carregados: " << backgroundTexIDs.size() << endl;

    // Carregar texturas do HUD (corações)
    heartFullTexID = loadTexture("../assets/sprites/fruitcatcher/ui/heart_full.png");
    heartEmptyTexID = loadTexture("../assets/sprites/fruitcatcher/ui/heart_empty.png");

    // Se não encontrar os corações, usar frutas como placeholder
    if (heartFullTexID == 0 && !fruitTexIDs.empty()) {
        heartFullTexID = fruitTexIDs[0];
        cout << "Aviso: Usando fruta como coração cheio (adicione heart_full.png)" << endl;
    }
    if (heartEmptyTexID == 0 && !insectTexIDs.empty()) {
        heartEmptyTexID = insectTexIDs[0];
        cout << "Aviso: Usando inseto como coração vazio (adicione heart_empty.png)" << endl;
    }

    // Criar textura branca para desenhar formas coloridas (números)
    whitePixelTexID = createColoredQuadTexture(1.0f, 1.0f, 1.0f, 1.0f);

    // Criar textura preta semi-transparente para fundo dos números
    blackTexID = createColoredQuadTexture(0.0f, 0.0f, 0.0f, 0.8f);

    // Carregar texturas das telas do menu e game over
    menuTitleInfo = loadTextureWithInfo("../assets/sprites/fruitcatcher/ui/menu_title.png");
    menuStartInfo = loadTextureWithInfo("../assets/sprites/fruitcatcher/ui/menu_start.png");
    gameOverInfo = loadTextureWithInfo("../assets/sprites/fruitcatcher/ui/game_over.png");

    cout << "\nTelas do jogo:" << endl;
    cout << "  - Menu Title: " << (menuTitleInfo.id > 0 ? "OK" : "FALTANDO") << " (" << menuTitleInfo.width << "x" << menuTitleInfo.height << ")" << endl;
    cout << "  - Menu Start: " << (menuStartInfo.id > 0 ? "OK" : "FALTANDO") << " (" << menuStartInfo.width << "x" << menuStartInfo.height << ")" << endl;
    cout << "  - Game Over: " << (gameOverInfo.id > 0 ? "OK" : "FALTANDO") << " (" << gameOverInfo.width << "x" << gameOverInfo.height << ")" << endl;

    // Carregar texturas dos números (0-9)
    string numberNames[] = {"zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine"};
    cout << "\nNúmeros para pontuação:" << endl;
    for (int i = 0; i < 10; i++)
    {
        string path = "../assets/sprites/fruitcatcher/numbers/" + numberNames[i] + ".png";
        GLuint numTex = loadTexture(path);
        numberTexIDs.push_back(numTex);
        cout << "  - Número " << i << ": " << (numTex > 0 ? "OK" : "FALTANDO") << endl;
    }

    // Inicializar a cesta
    basket.initialize(shaderID, basketTexID, vec3(WIDTH / 2.0f, 50.0f, 0.0f), vec3(80.0f, 60.0f, 1.0f));
    basket.setType(OBJ_BASKET);

    // Configurar o VAO para o fundo
    backgroundVAO = setupBackgroundGeometry();

    // Configurar VAOs para as camadas de parallax
    cloudVAO = setupCloudLayerGeometry();
    bushLeftVAO = setupBushLeftGeometry();
    bushRightVAO = setupBushRightGeometry();
    bushMiddleVAO = setupBushMiddleGeometry();

    // Configurar o VAO para o HUD
    hudVAO = setupHUDGeometry();

    // Configurações OpenGL
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(shaderID);

    // Matriz de projeção ortográfica
    mat4 projection = ortho(0.0f, (float)WIDTH, 0.0f, (float)HEIGHT, -1.0f, 1.0f);
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, value_ptr(projection));

    // Ativar textura
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(shaderID, "tex_buffer"), 0);

    // Variáveis de tempo
    float lastFrame = glfwGetTime();
    float deltaTime = 0.0f;

    cout << "\n=== FRUIT CATCHER ===" << endl;
    cout << "Controles:" << endl;
    cout << "  - Setas Esquerda/Direita ou A/D: Mover cesta" << endl;
    cout << "  - Mouse: Mover cesta seguindo cursor" << endl;
    cout << "  - ESC: Sair" << endl;
    cout << "Objetivo: Colete frutas e evite insetos!" << endl;
    cout << "=====================\n" << endl;

    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        // Calcular deltaTime
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Processar eventos
        glfwPollEvents();

        // Atualizar jogo
        updateGame(deltaTime);

        // Limpar tela
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Definir viewport
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        // Desenhar backgrounds com efeito PARALLAX
        glDisable(GL_DEPTH_TEST);
        glBindVertexArray(backgroundVAO);

        // Layer 1 (Sky/Background) - Estática (fundo mais distante)
        if (backgroundTexIDs.size() > 0) {
            drawBackgroundLayer(shaderID, backgroundTexIDs[0], 0.0f, -0.5f);
        }

        // Layer 2 (Clouds) - Múltiplas nuvens com bounce nos limites da tela
        if (backgroundTexIDs.size() >= 5) {
            // Desenhar 4 nuvens em posições diferentes com bounce
            drawCloudLayer(shaderID, backgroundTexIDs[1], cloudPositions[0], -0.4f);
            drawCloudLayer(shaderID, backgroundTexIDs[2], cloudPositions[1], -0.4f);
            drawCloudLayer(shaderID, backgroundTexIDs[3], cloudPositions[2], -0.4f);
            drawCloudLayer(shaderID, backgroundTexIDs[4], cloudPositions[3], -0.4f);
        }

        // Layer 3 (Bushes) - Movimento oscilatório e tremor (frente)
        if (backgroundTexIDs.size() >= 8) {
            // Calcular offset oscilatório para as bushes laterais
            float bushOffset = sin(bushOscillationTime) * bushOscillationAmount;

            // Bush esquerda - movimento para direita/esquerda
            drawBushLayer(shaderID, bushLeftVAO, backgroundTexIDs[5], bushOffset, -0.3f);

            // Bush central - tremor rápido e sutil
            float shakeX = sin(bushMiddleShakeTime) * bushMiddleShakeAmount;
            float shakeY = cos(bushMiddleShakeTime * 1.3f) * bushMiddleShakeAmount * 0.5f; // Tremor também em Y
            drawBushLayer(shaderID, bushMiddleVAO, backgroundTexIDs[6], shakeX, -0.3f);

            // Bush direita - movimento oposto (para criar efeito de balanço)
            drawBushLayer(shaderID, bushRightVAO, backgroundTexIDs[7], -bushOffset, -0.3f);
        }

        glEnable(GL_DEPTH_TEST);

        // Desenhar baseado no estado do jogo
        if (gameState == MENU)
        {
            // TELA DE MENU
            glDisable(GL_DEPTH_TEST);
            glBindVertexArray(hudVAO);

            // Desenhar título (centralizado, usando tamanho real)
            if (menuTitleInfo.id > 0 && menuTitleInfo.width > 0)
            {
                float w = menuTitleInfo.width;
                float h = menuTitleInfo.height;
                float x = (WIDTH - w) / 2.0f;
                float y = HEIGHT - h - 100.0f;
                drawHUDElement(shaderID, menuTitleInfo.id, x, y, w, h);
            }

            // Desenhar botão Start (centralizado)
            if (menuStartInfo.id > 0 && menuStartInfo.width > 0)
            {
                float w = menuStartInfo.width;
                float h = menuStartInfo.height;
                float x = (WIDTH - w) / 2.0f;
                float y = (HEIGHT - h) / 2.0f - 50.0f;
                drawHUDElement(shaderID, menuStartInfo.id, x, y, w, h);
            }

            glEnable(GL_DEPTH_TEST);
        }
        else if (gameState == PLAYING)
        {
            // JOGANDO - Desenhar objetos do jogo
            basket.update();
            basket.draw();

            for (auto obj : fallingObjects)
            {
                if (obj->isActive)
                {
                    obj->update();
                    obj->draw();
                }
            }

            // Desenhar HUD
            drawHUD(shaderID);
        }
        else if (gameState == GAME_OVER)
        {
            // TELA DE GAME OVER
            glDisable(GL_DEPTH_TEST);
            glBindVertexArray(hudVAO);

            // Desenhar texto Game Over (menor e no topo)
            if (gameOverInfo.id > 0 && gameOverInfo.width > 0)
            {
                // Reduzir o tamanho para 50% do tamanho original
                float scale = 0.5f;
                float w = gameOverInfo.width * scale;
                float h = gameOverInfo.height * scale;
                float x = (WIDTH - w) / 2.0f;
                float y = HEIGHT - h - 80.0f; // No topo
                drawHUDElement(shaderID, gameOverInfo.id, x, y, w, h);
            }

            // Desenhar pontuação final abaixo do Game Over
            if (!numberTexIDs.empty() && numberTexIDs[0] > 0)
            {
                // Calcular largura total do número para centralizar
                string scoreStr = to_string(score);
                float digitSize = 50.0f; // Números grandes
                float spacing = 10.0f;
                float iconSize = 50.0f; // Tamanho do ícone de frutas
                float iconSpacing = 15.0f; // Espaço entre ícone e números

                // Largura total = ícone + espaço + números
                float numbersWidth = scoreStr.length() * digitSize + (scoreStr.length() - 1) * spacing;
                float totalWidth = iconSize + iconSpacing + numbersWidth;

                // Posição Y na parte inferior da tela (deixando margem de 80px da borda)
                float scoreY = 80.0f;

                // Centralizar o conjunto (ícone + números)
                float startX = (WIDTH - totalWidth) / 2.0f;

                // Desenhar ícone de frutas à esquerda
                if (!fruitTexIDs.empty())
                {
                    // Tentar carregar fruits.png se ainda não carregamos
                    static GLuint fruitsIconTexID = 0;
                    if (fruitsIconTexID == 0)
                    {
                        fruitsIconTexID = loadTexture("../assets/sprites/fruitcatcher/ui/fruits.png");
                        if (fruitsIconTexID == 0 && !fruitTexIDs.empty())
                        {
                            fruitsIconTexID = fruitTexIDs[0]; // Fallback
                        }
                    }

                    if (fruitsIconTexID > 0)
                    {
                        drawHUDElement(shaderID, fruitsIconTexID, startX, scoreY, iconSize, iconSize);
                    }
                }

                // Desenhar números à direita do ícone
                float numbersX = startX + iconSize + iconSpacing;
                drawNumber(shaderID, score, numbersX, scoreY, digitSize, spacing);
            }

            // Desenhar botão Start novamente (centralizado, mais abaixo)
            if (menuStartInfo.id > 0 && menuStartInfo.width > 0)
            {
                float w = menuStartInfo.width;
                float h = menuStartInfo.height;
                float x = (WIDTH - w) / 2.0f;
                float y = HEIGHT / 2.0f - 150.0f; // Mais abaixo
                drawHUDElement(shaderID, menuStartInfo.id, x, y, w, h);
            }

            glEnable(GL_DEPTH_TEST);
        }

        // Swap buffers
        glfwSwapBuffers(window);
    }

    // Limpeza
    for (auto obj : fallingObjects)
    {
        delete obj;
    }
    fallingObjects.clear();

    for (auto bg : backgroundLayers)
    {
        delete bg;
    }
    backgroundLayers.clear();

    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (action == GLFW_PRESS)
    {
        keys[key] = true;
    }
    else if (action == GLFW_RELEASE)
    {
        keys[key] = false;
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    mouseX = xpos;
    useMouseControl = true;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    // Clicar para iniciar o jogo ou reiniciar após game over
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        if (gameState == MENU || gameState == GAME_OVER)
        {
            gameState = PLAYING;
            resetGame();
            cout << "Iniciando o jogo..." << endl;
        }
    }
}

int setupShader()
{
    // Vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
    }

    // Fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
    }

    // Linkar programa
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

TextureInfo loadTextureWithInfo(string filePath)
{
    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char *data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);

    if (data)
    {
        if (nrChannels == 3)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        else
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        glGenerateMipmap(GL_TEXTURE_2D);
        cout << "Textura carregada: " << filePath << endl;
    }
    else
    {
        cout << "Falha ao carregar textura: " << filePath << endl;
    }

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);

    return TextureInfo(texID, width, height);
}

GLuint loadTexture(string filePath)
{
    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char *data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);

    if (data)
    {
        if (nrChannels == 3)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        else
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        glGenerateMipmap(GL_TEXTURE_2D);
        cout << "Textura carregada: " << filePath << endl;
    }
    else
    {
        cout << "Falha ao carregar textura: " << filePath << endl;
    }

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);

    return texID;
}

GLuint setupBackgroundGeometry()
{
    // Quad que cobre a tela inteira, ajustando coordenadas de textura
    // para cortar a parte superior indesejada
    GLfloat vertices[] = {
        // Posições (x, y, z)     // Coordenadas de textura (s, t)
        0.0f,   0.0f,   0.0f,     0.0f, 0.30f,  // inferior esquerdo
        WIDTH,  0.0f,   0.0f,     1.0f, 0.30f,  // inferior direito
        WIDTH,  HEIGHT, 0.0f,     1.0f, 1.0f,  // superior direito

        0.0f,   0.0f,   0.0f,     0.0f, 0.30f,  // inferior esquerdo
        WIDTH,  HEIGHT, 0.0f,     1.0f, 1.0f,  // superior direito
        0.0f,   HEIGHT, 0.0f,     0.0f, 1.0f   // superior esquerdo
    };

    GLuint VBO, VAO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Atributo de posição (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    // Atributo de coordenadas de textura (location = 1)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;
}

GLuint setupCloudLayerGeometry()
{
    // Quad centralizado (origem no centro) para permitir posicionamento absoluto
    float cloudWidth = 150.0f;  // Largura da nuvem
    float cloudHeight = 80.0f;  // Altura da nuvem

    GLfloat vertices[] = {
        // Posições centradas (x, y, z)    // Coordenadas de textura (s, t)
        -cloudWidth/2, -cloudHeight/2, 0.0f,    0.0f, 0.0f,  // inferior esquerdo
         cloudWidth/2, -cloudHeight/2, 0.0f,    1.0f, 0.0f,  // inferior direito
         cloudWidth/2,  cloudHeight/2, 0.0f,    1.0f, 1.0f,  // superior direito

        -cloudWidth/2, -cloudHeight/2, 0.0f,    0.0f, 0.0f,  // inferior esquerdo
         cloudWidth/2,  cloudHeight/2, 0.0f,    1.0f, 1.0f,  // superior direito
        -cloudWidth/2,  cloudHeight/2, 0.0f,    0.0f, 1.0f   // superior esquerdo
    };

    GLuint VBO, VAO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Atributo de posição (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    // Atributo de coordenadas de textura (location = 1)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;
}

GLuint setupBushLeftGeometry()
{
    // Quad pequeno no canto inferior esquerdo para a árvore decorativa
    float treeWidth = 80.0f;   // Largura pequena da árvore
    float treeHeight = 120.0f;  // Altura da árvore
    float treeX = 20.0f;        // Canto esquerdo com margem
    float treeY = 0.0f;         // Base da tela

    GLfloat vertices[] = {
        // Posições (x, y, z)           // Coordenadas de textura (s, t)
        treeX, treeY, 0.0f,             0.0f, 0.0f,  // inferior esquerdo
        treeX + treeWidth, treeY, 0.0f,       1.0f, 0.0f,  // inferior direito
        treeX + treeWidth, treeY + treeHeight, 0.0f,  1.0f, 1.0f,  // superior direito

        treeX, treeY, 0.0f,             0.0f, 0.0f,  // inferior esquerdo
        treeX + treeWidth, treeY + treeHeight, 0.0f,  1.0f, 1.0f,  // superior direito
        treeX, treeY + treeHeight, 0.0f,       0.0f, 1.0f   // superior esquerdo
    };

    GLuint VBO, VAO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Atributo de posição (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    // Atributo de coordenadas de textura (location = 1)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;
}

GLuint setupBushRightGeometry()
{
    // Quad pequeno no canto inferior direito para a árvore decorativa
    float treeWidth = 80.0f;   // Largura pequena da árvore
    float treeHeight = 120.0f;  // Altura da árvore
    float treeX = WIDTH - treeWidth - 20.0f; // Canto direito com margem
    float treeY = 0.0f;         // Base da tela

    GLfloat vertices[] = {
        // Posições (x, y, z)           // Coordenadas de textura (s, t)
        treeX, treeY, 0.0f,             0.0f, 0.0f,  // inferior esquerdo
        treeX + treeWidth, treeY, 0.0f,       1.0f, 0.0f,  // inferior direito
        treeX + treeWidth, treeY + treeHeight, 0.0f,  1.0f, 1.0f,  // superior direito

        treeX, treeY, 0.0f,             0.0f, 0.0f,  // inferior esquerdo
        treeX + treeWidth, treeY + treeHeight, 0.0f,  1.0f, 1.0f,  // superior direito
        treeX, treeY + treeHeight, 0.0f,       0.0f, 1.0f   // superior esquerdo
    };

    GLuint VBO, VAO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Atributo de posição (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    // Atributo de coordenadas de textura (location = 1)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;
}

GLuint setupBushMiddleGeometry()
{
    // Quad pequeno no centro inferior para a bush com tremor
    float treeWidth = 80.0f;   // Largura pequena da árvore
    float treeHeight = 120.0f;  // Altura da árvore
    float treeX = (WIDTH - treeWidth) / 2.0f; // Centro da tela
    float treeY = 0.0f;         // Base da tela

    GLfloat vertices[] = {
        // Posições (x, y, z)           // Coordenadas de textura (s, t)
        treeX, treeY, 0.0f,             0.0f, 0.0f,  // inferior esquerdo
        treeX + treeWidth, treeY, 0.0f,       1.0f, 0.0f,  // inferior direito
        treeX + treeWidth, treeY + treeHeight, 0.0f,  1.0f, 1.0f,  // superior direito

        treeX, treeY, 0.0f,             0.0f, 0.0f,  // inferior esquerdo
        treeX + treeWidth, treeY + treeHeight, 0.0f,  1.0f, 1.0f,  // superior direito
        treeX, treeY + treeHeight, 0.0f,       0.0f, 1.0f   // superior esquerdo
    };

    GLuint VBO, VAO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Atributo de posição (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    // Atributo de coordenadas de textura (location = 1)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;
}

void drawBackgroundLayer(GLuint shaderID, GLuint texID, float xOffset, float zDepth)
{
    // Criar matriz de modelo com profundidade controlada
    mat4 model = mat4(1.0f);

    // Aplicar offset horizontal para parallax
    model = translate(model, vec3(xOffset, 0.0f, zDepth)); // Z negativo = atrás dos sprites

    // Enviar matriz para o shader
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, value_ptr(model));

    // Offset de textura zerado (sem animação de spritesheet)
    glUniform2f(glGetUniformLocation(shaderID, "offsetTex"), 0.0f, 0.0f);

    // Bind da textura e desenho
    glBindTexture(GL_TEXTURE_2D, texID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void drawCloudLayer(GLuint shaderID, GLuint texID, float xPosition, float zDepth)
{
    // Bind do VAO específico para nuvens
    glBindVertexArray(cloudVAO);

    // Posição Y centralizada no céu (parte superior da tela)
    float cloudY = HEIGHT - 120.0f; // 120px do topo da tela

    // Criar matriz de modelo com posição absoluta
    mat4 model = mat4(1.0f);

    // Posicionar a nuvem na posição X fornecida, centralizada verticalmente no céu
    model = translate(model, vec3(xPosition, cloudY, zDepth));

    // Enviar matriz para o shader
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, value_ptr(model));

    // Offset de textura zerado (sem animação de spritesheet)
    glUniform2f(glGetUniformLocation(shaderID, "offsetTex"), 0.0f, 0.0f);

    // Bind da textura e desenho
    glBindTexture(GL_TEXTURE_2D, texID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void drawBushLayer(GLuint shaderID, GLuint VAO, GLuint texID, float xOffset, float zDepth)
{
    // Bind do VAO específico para bushes
    glBindVertexArray(VAO);

    // Criar matriz de modelo com profundidade controlada
    mat4 model = mat4(1.0f);

    // Aplicar offset horizontal para parallax
    model = translate(model, vec3(xOffset, 0.0f, zDepth)); // Z negativo = atrás dos sprites

    // Enviar matriz para o shader
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, value_ptr(model));

    // Offset de textura zerado (sem animação de spritesheet)
    glUniform2f(glGetUniformLocation(shaderID, "offsetTex"), 0.0f, 0.0f);

    // Bind da textura e desenho
    glBindTexture(GL_TEXTURE_2D, texID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void spawnFallingObject()
{
    GameObject* obj = new GameObject();

    // Posição aleatória no topo da tela
    float x = rand() % (WIDTH - 100) + 50.0f;
    float y = HEIGHT + 50.0f;

    // 70% chance de ser fruta, 30% inseto
    bool isFruit = (rand() % 100) < 70;

    if (isFruit)
    {
        // Fruta normal (sprite único com rotação animada)
        GLuint texID = fruitTexIDs[rand() % fruitTexIDs.size()];
        obj->initialize(basket.getShaderID(), texID, vec3(x, y, 0.0f), vec3(50.0f, 50.0f, 1.0f));
        obj->setType(OBJ_FRUIT);

        float rotSpeed = 50.0f + (rand() % 100);
        if (rand() % 2 == 0) rotSpeed = -rotSpeed;
        obj->setRotationSpeed(rotSpeed);
        obj->setAnimated(false);
    }
    else
    {
        bool useAnimatedSprite = !insectSpritesheets.empty() && (rand() % 100) < 60;

        if (useAnimatedSprite)
        {
            SpritesheetInfo spriteInfo = insectSpritesheets[rand() % insectSpritesheets.size()];

            obj->initialize(basket.getShaderID(), spriteInfo.texID,
                          vec3(x, y, 0.0f), vec3(50.0f, 50.0f, 1.0f),
                          spriteInfo.nAnimations, spriteInfo.nFrames);

            obj->setType(OBJ_INSECT);
            obj->setAnimated(true);
            obj->setFPS(12.0f);

            cout << "Inseto animado spawnou! (" << spriteInfo.nAnimations << "x" << spriteInfo.nFrames << " frames)" << endl;
        }
        else
        {
            GLuint texID = insectTexIDs[rand() % insectTexIDs.size()];
            obj->initialize(basket.getShaderID(), texID, vec3(x, y, 0.0f), vec3(50.0f, 50.0f, 1.0f));
            obj->setType(OBJ_INSECT);

            float rotSpeed = 100.0f + (rand() % 150);
            if (rand() % 2 == 0) rotSpeed = -rotSpeed;
            obj->setRotationSpeed(rotSpeed);
            obj->setAnimated(false);
        }
    }

    obj->setVelocity(vec2(0.0f, -objectFallSpeed));
    fallingObjects.push_back(obj);
}

void updateGame(float deltaTime)
{
    // Só atualizar o jogo se estiver no estado PLAYING
    if (gameState != PLAYING)
        return;

    // Controle da cesta com teclado
    float basketSpeed = 300.0f * deltaTime;

    if (keys[GLFW_KEY_LEFT] || keys[GLFW_KEY_A])
    {
        basket.moveLeft(basketSpeed);
        useMouseControl = false;
    }
    if (keys[GLFW_KEY_RIGHT] || keys[GLFW_KEY_D])
    {
        basket.moveRight(basketSpeed);
        useMouseControl = false;
    }

    // Controle com mouse
    if (useMouseControl)
    {
        vec3 pos = basket.getPosition();
        pos.x = mouseX;
        basket.setPosition(pos);
    }

    // Limitar cesta dentro da tela
    vec3 basketPos = basket.getPosition();
    vec3 basketDim = basket.getDimensions();
    if (basketPos.x - basketDim.x / 2 < 0)
        basketPos.x = basketDim.x / 2;
    if (basketPos.x + basketDim.x / 2 > WIDTH)
        basketPos.x = WIDTH - basketDim.x / 2;
    basket.setPosition(basketPos);

    // Spawnar novos objetos
    spawnTimer += deltaTime;
    if (spawnTimer >= spawnInterval)
    {
        spawnFallingObject();
        spawnTimer = 0.0f;
    }

    // Atualizar objetos caindo
    for (int i = fallingObjects.size() - 1; i >= 0; i--)
    {
        GameObject* obj = fallingObjects[i];

        if (obj->isActive)
        {
            obj->updatePosition(deltaTime);

            // Verificar se saiu da tela
            if (obj->getPosition().y < -50.0f)
            {
                obj->isActive = false;
            }
        }

        // Remover objetos inativos
        if (!obj->isActive)
        {
            delete obj;
            fallingObjects.erase(fallingObjects.begin() + i);
        }
    }

    // Verificar colisões
    checkCollisions();

    // Aumentar dificuldade com o tempo
    difficultyTimer += deltaTime;
    if (difficultyTimer >= difficultyIncreaseRate)
    {
        currentLevel++;
        difficultyTimer = 0.0f;

        // Aumentar intervalo de spawn (até um mínimo)
        spawnInterval -= 0.1f;
        if (spawnInterval < minSpawnInterval) spawnInterval = minSpawnInterval;

        // Aumentar velocidade de queda (até um máximo)
        objectFallSpeed += 10.0f;
        if (objectFallSpeed > maxFallSpeed) objectFallSpeed = maxFallSpeed;

        cout << "Nível " << currentLevel << " - Dificuldade aumentada!" << endl;
        cout << "  Intervalo de spawn: " << spawnInterval << "s" << endl;
        cout << "  Velocidade de queda: " << objectFallSpeed << "px/s" << endl;
    }

    // Atualizar movimento do parallax
    parallaxOffset += parallaxSpeed * deltaTime;

    // Atualizar movimento oscilatório das bushes
    bushOscillationTime += deltaTime * bushOscillationSpeed;

    // Atualizar tremor da bush central (bushAlt1)
    bushMiddleShakeTime += deltaTime * bushMiddleShakeSpeed;

    // Atualizar posições das nuvens com efeito de bounce
    for (int i = 0; i < 4; i++)
    {
        cloudPositions[i] += cloudSpeeds[i] * cloudDirections[i] * deltaTime;

        // Inverter direção ao atingir as bordas da tela (bounce)
        if (cloudPositions[i] < 0.0f || cloudPositions[i] > WIDTH)
        {
            cloudDirections[i] *= -1.0f;
            // Corrigir posição para não sair da tela
            if (cloudPositions[i] < 0.0f) cloudPositions[i] = 0.0f;
            if (cloudPositions[i] > WIDTH) cloudPositions[i] = WIDTH;
        }
    }
}

void checkCollisions()
{
    for (auto obj : fallingObjects)
    {
        if (obj->isActive && basket.checkCollision(*obj))
        {
            if (obj->getType() == OBJ_FRUIT)
            {
                score++;
                cout << "Fruta coletada! Score: " << score << endl;
                audioManager.playSound("fruit_collect"); // Tocar som de coleta de fruta
            }
            else if (obj->getType() == OBJ_INSECT)
            {
                lives--;
                cout << "Inseto coletado! Vidas restantes: " << lives << endl;
                audioManager.playSound("bug_collect"); // Tocar som de coleta de inseto
                if (lives <= 0)
                {
                    cout << "Game Over! Score final: " << score << endl;
                    gameState = GAME_OVER;
                }
            }
            obj->isActive = false;
        }
    }
}

void resetGame()
{
    score = 0;
    lives = config.initialLives;  // Usar valor do config, não hardcoded

    for (auto obj : fallingObjects)
    {
        delete obj;
    }
    fallingObjects.clear();

    // Resetar dificuldade
    spawnInterval = baseSpawnInterval;
    objectFallSpeed = baseFallSpeed;
    difficultyTimer = 0.0f;
    currentLevel = 1;

    cout << "\n=== NOVO JOGO ===" << endl;
}

GLuint setupHUDGeometry()
{
    // Quad simples para desenhar elementos do HUD
    GLfloat vertices[] = {
        // Posições          // Coordenadas de textura
        -0.5f,  0.5f, 0.0f,  0.0f, 1.0f,  // topo esquerdo
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,  // base esquerdo
         0.5f,  0.5f, 0.0f,  1.0f, 1.0f,  // topo direito
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f   // base direito
    };

    GLuint VBO, VAO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;
}

void drawHUDElement(GLuint shaderID, GLuint texID, float x, float y, float width, float height)
{
    // Criar matriz de modelo para posicionar o elemento do HUD
    mat4 model = mat4(1.0f);
    model = translate(model, vec3(x + width/2.0f, y + height/2.0f, 0.9f)); // Z alto para ficar na frente
    model = scale(model, vec3(width, height, 1.0f));

    glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, value_ptr(model));
    glUniform2f(glGetUniformLocation(shaderID, "offsetTex"), 0.0f, 0.0f);

    glBindTexture(GL_TEXTURE_2D, texID);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

GLuint createColoredQuadTexture(float r, float g, float b, float a)
{
    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    // Criar um quadro colorido (1x1 pixel) com a cor desejada
    unsigned char pixel[4] = { (unsigned char)(r * 255), (unsigned char)(g * 255), (unsigned char)(b * 255), (unsigned char)(a * 255) };
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixel);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
    return texID;
}

void drawHUD(GLuint shaderID)
{
    // Desabilitar depth test para HUD ficar sempre na frente
    glDisable(GL_DEPTH_TEST);
    glBindVertexArray(hudVAO);

    // Desenhar corações de vida no canto superior esquerdo
    float heartSize = 30.0f;
    float heartSpacing = 35.0f;
    float startX = 10.0f;
    float startY = HEIGHT - 40.0f;

    // Se temos texturas de coração, desenhar as vidas
    if (heartFullTexID > 0 && heartEmptyTexID > 0)
    {
        for (int i = 0; i < maxLives; i++)
        {
            float x = startX + i * heartSpacing;
            GLuint texToUse = (i < lives) ? heartFullTexID : heartEmptyTexID;
            drawHUDElement(shaderID, texToUse, x, startY, heartSize, heartSize);
        }
    }

    // Desenhar score no canto superior direito
    if (!fruitTexIDs.empty())
    {
        float scoreIconSize = 30.0f;
        float scoreX = WIDTH - 150.0f;
        float scoreY = HEIGHT - 40.0f;

        // Desenhar ícone de fruta
        drawHUDElement(shaderID, fruitTexIDs[0], scoreX, scoreY, scoreIconSize, scoreIconSize);

        // Desenhar número do score
        if (!numberTexIDs.empty() && numberTexIDs[0] > 0)
        {
            drawNumber(shaderID, score, scoreX + 40.0f, scoreY, 25.0f, 5.0f);
        }
    }

    glEnable(GL_DEPTH_TEST);
}

// Função para desenhar um número usando as texturas dos dígitos
void drawNumber(GLuint shaderID, int number, float x, float y, float digitSize, float spacing)
{
    // Converter número para string para pegar cada dígito
    string numStr = to_string(number);

    // Desenhar cada dígito da esquerda para a direita
    float currentX = x;
    for (char c : numStr)
    {
        int digit = c - '0'; // Converter char para int

        // Verificar se temos a textura desse dígito
        if (digit >= 0 && digit <= 9 && numberTexIDs[digit] > 0)
        {
            drawHUDElement(shaderID, numberTexIDs[digit], currentX, y, digitSize, digitSize);
            currentX += digitSize + spacing; // Avançar para o próximo dígito
        }
    }
}
