# 🎮 Fruit Catcher Game

## Equipe
- **Taimisson de Carvalho Schardosim**
- **Guilherme Lenzi de Oliveira**

---

## Descrição do Projeto

> Este programa foi desenvolvido como **Trabalho do Grau A** da disciplina *Processamento Gráfico: Fundamentos* com o objetivo de aplicar conceitos de OpenGL moderna e C++ na criação de um jogo interativo 2D. O projeto explora a utilização de shaders programáveis, transformações geométricas, projeções ortográficas, animação por spritesheets, detecção de colisões AABB e parallax scrolling em múltiplas camadas.

**Fruit Catcher** é um jogo estilo "catch" onde o jogador controla uma cesta para coletar frutas que caem do topo da tela, evitando insetos. O jogo implementa sistema de pontuação, vidas, HUD, áudio, telas de menu/game over e arquivo de configuração externo.

---

## Estrutura do Projeto

### Arquivos Principais de Código

| Arquivo                     | Descrição                                                                 |
|----------------------------|---------------------------------------------------------------------------|
| `FruitCatcher.cpp`         | Ponto de entrada, game loop, lógica principal e gerenciamento de estados |
| `Sprite.cpp / .h`          | Classe base para objetos renderizáveis (VAO, VBO, texturas, transformações) |
| `GameObject.cpp / .h`      | Classe derivada de Sprite com física, tipos (fruta/inseto) e colisão |
| `Background.cpp / .h`      | Classe para camadas de parallax scrolling com loop infinito |
| `AudioManager.cpp / .h`    | Gerenciamento de música de fundo e efeitos sonoros (miniaudio) |
| `config.txt`               | Arquivo de configuração externo (dimensões, velocidades, caminhos de assets) |

### Shaders

| Shader                 | Descrição                                                                 |
|------------------------|---------------------------------------------------------------------------|
| `vertex_shader.glsl`   | Transformações (projection × model × position), passa coordenadas de textura |
| `fragment_shader.glsl` | Sampling de textura com offset para animação de spritesheets |

### Estrutura de Pastas

```
FruitCatcher/
├── src/FruitCatcher/
│   ├── FruitCatcher.cpp          # Main e game loop
│   ├── include/                  # Headers das classes
│   │   ├── Sprite.h
│   │   ├── GameObject.h
│   │   ├── Background.h
│   │   └── AudioManager.h
│   ├── Sprite.cpp                # Implementação da classe base
│   ├── GameObject.cpp            # Lógica de objetos do jogo
│   ├── Background.cpp            # Sistema de parallax
│   └── AudioManager.cpp          # Sistema de áudio
├── assets/
│   ├── sprites/fruitcatcher/
│   │   ├── fruits/               # 4 tipos de frutas
│   │   ├── insects/              # 3 tipos de insetos (estáticos)
│   │   ├── animated_sprites/     # 6 spritesheets animados
│   │   ├── numbers/              # Dígitos 0-9 para HUD
│   │   └── ui/                   # Cesta, corações, telas
│   ├── backgrounds/              # 3 camadas de parallax
│   └── audio/                    # Música e efeitos sonoros
├── Common/
│   └── glad.c                    # Loader do OpenGL
├── include/
│   ├── glad/                     # Headers do GLAD
│   └── miniaudio.h               # Biblioteca de áudio
├── CMakeLists.txt                # Build system
└── README.md                     # Este arquivo
```

---

## Informações Técnicas

- **Linguagem:** C++17
- **API Gráfica:** OpenGL 3.3+ (core profile, GLSL 4.0)
- **Bibliotecas:**
  - **GLFW 3.3+** - Criação de janela e contexto OpenGL
  - **GLAD** - Loader de funções OpenGL
  - **GLM** - Matemática 3D (matrizes, vetores)
  - **stb_image** - Carregamento de texturas PNG
  - **miniaudio** - Sistema de áudio (música + efeitos)
- **Build System:** CMake 3.10+
- **IDE:** CLion / Visual Studio Code
- **Compilador:** MinGW-w64 (GCC 11+) / MSVC / GCC / Clang
- **Plataforma-alvo:** Windows, Linux, macOS

---

## Checklist de Requisitos

### ✅ Requisitos Obrigatórios (100% Completos)

- [x] **OpenGL 3.3+** com pipeline programável (shaders)
- [x] **Vertex Shader** e **Fragment Shader** customizados
- [x] **VAO/VBO** configurados corretamente
  - [x] Sprites (quad com 4 vértices, TRIANGLE_STRIP)
  - [x] Background (quad fullscreen)
  - [x] HUD (elementos de interface)
- [x] **Transformações geométricas**
  - [x] Model Matrix (translate, rotate, scale)
  - [x] Projection Matrix (ortho 2D)
  - [x] Atualização via uniforms a cada frame
- [x] **Sprites texturizados**
  - [x] 4 tipos de frutas
  - [x] 9 tipos de insetos (3 estáticos + 6 animados)
  - [x] Background em 3 camadas
  - [x] Cesta do jogador
  - [x] UI (corações, números, telas)
- [x] **Animação de sprites**
  - [x] Spritesheets com múltiplos frames (1x4, 1x6, 1x9)
  - [x] Controle de FPS independente
  - [x] Offset de textura no shader
  - [x] Rotação animada para objetos estáticos
- [x] **Controle do personagem**
  - [x] Teclado (setas, WASD)
  - [x] Mouse (seguir cursor)
  - [x] Limitação de movimento na tela
- [x] **Detecção de colisão AABB**
  - [x] Cesta vs frutas (+pontuação)
  - [x] Cesta vs insetos (-vida)
  - [x] Desativação de objetos colididos
- [x] **Arquivo de configuração externo** (`config.txt`)
  - [x] Dimensões da janela
  - [x] Velocidades (queda, parallax, animação)
  - [x] Caminhos de assets
  - [x] Parâmetros de gameplay

### ✅ Recursos Extras Implementados

- [x] **Sistema de HUD completo**
  - [x] Pontuação com sprites numéricos
  - [x] Vidas com corações (cheio/vazio)
  - [x] Ícones personalizados
- [x] **Sistema de áudio (miniaudio)**
  - [x] Música de fundo em loop
  - [x] Efeitos sonoros (coleta de fruta, colisão com inseto)
  - [x] Controle de volume
- [x] **Parallax scrolling avançado**
  - [x] 3 camadas com velocidades diferentes
  - [x] 4 nuvens com movimento bounce
  - [x] 3 bushes com animações (oscilação, tremor)
  - [x] Loop infinito contínuo
- [x] **Sistema de estados**
  - [x] Tela de menu inicial
  - [x] Gameplay
  - [x] Tela de game over
  - [x] Transições suaves
- [x] **Sistema de física**
  - [x] Velocidade e gravidade
  - [x] Rotação contínua
  - [x] Spawn aleatório
- [x] **Sistema de dificuldade progressiva**
  - [x] Velocidade de queda aumenta com o tempo
  - [x] Intervalo de spawn diminui
  - [x] Sistema de níveis
- [x] **Arquitetura orientada a objetos**
  - [x] Herança (Sprite → GameObject, Background)
  - [x] Encapsulamento
  - [x] Separação de responsabilidades

---

## Como Compilar e Executar

### Pré-requisitos
- CMake 3.10+
- Compilador C++17 (GCC 11+, MSVC 2019+, Clang 10+)
- Git (para clonar dependências)

### Windows (MinGW)

```bash
# Clone o repositório
git clone <url-do-repositorio>
cd FruitCatcher

# Criar pasta de build
mkdir cmake-build-debug
cd cmake-build-debug

# Configurar CMake
cmake ..

# Compilar
cmake --build . --target FruitCatcher

# Executar
./FruitCatcher.exe
```

### Linux / macOS

```bash
# Clone o repositório
git clone <url-do-repositorio>
cd FruitCatcher

# Instalar dependências (Ubuntu/Debian)
sudo apt-get install build-essential cmake libglfw3-dev

# Criar pasta de build
mkdir build && cd build

# Configurar e compilar
cmake ..
make

# Executar
./FruitCatcher
```

---

## Controles do Jogo

| Tecla/Mouse          | Ação                           |
|---------------------|--------------------------------|
| **Seta Esquerda** / **A** | Mover cesta para esquerda |
| **Seta Direita** / **D**  | Mover cesta para direita  |
| **Mouse**           | Seguir posição do cursor       |
| **Clique Esquerdo** | Iniciar jogo / Reiniciar       |
| **ESC**             | Sair do jogo                   |

---

## Referências e Créditos

### Bibliotecas Utilizadas
- **GLFW** - [https://www.glfw.org/](https://www.glfw.org/)
- **GLAD** - [https://glad.dav1d.de/](https://glad.dav1d.de/)
- **GLM** - [https://github.com/g-truc/glm](https://github.com/g-truc/glm)
- **stb_image** - [https://github.com/nothings/stb](https://github.com/nothings/stb)
- **miniaudio** - [https://miniaud.io/](https://miniaud.io/)

### Assets Gráficos
- **Kenney Background Elements Redux** - [https://kenney.nl/assets/background-elements-redux](https://kenney.nl/assets/background-elements-redux)
- Sprites de frutas e insetos: criados/adaptados para o projeto

### Documentação e Tutoriais
- **LearnOpenGL** - [https://learnopengl.com/](https://learnopengl.com/)
- **OpenGL Documentation** - [https://www.khronos.org/opengl/](https://www.khronos.org/opengl/)
- **GLM Documentation** - [https://glm.g-truc.net/](https://glm.g-truc.net/)

---

## Licença

Este projeto foi desenvolvido para fins educacionais como parte da disciplina de Processamento Gráfico da UNISINOS.


