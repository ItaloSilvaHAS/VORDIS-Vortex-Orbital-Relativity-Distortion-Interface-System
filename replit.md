# Black Hole Simulation - 3D Spacetime Visualization

## Overview
Uma simulação visual 3D interativa de um buraco negro em C++ usando OpenGL. A simulação mostra:
- Buraco negro esférico com horizonte de eventos
- Grade do tecido espaço-tempo deformada pela gravidade
- Disco de acreção com partículas orbitando
- Efeito de lente gravitacional
- Estrelas de fundo

## Project Structure
```
├── src/
│   └── main.cpp          # Código principal da simulação
├── Makefile              # Script de compilação
├── README.md             # Instruções para rodar localmente
└── .gitignore            # Arquivos ignorados pelo git
```

## Technologies
- C++11
- OpenGL
- FreeGLUT (para janela e input)
- GLU (para funções de utilidade OpenGL)

## Controls
- **Mouse arrastar**: Rotacionar câmera
- **Scroll do mouse**: Zoom in/out
- **+/-**: Aumentar/diminuir massa do buraco negro
- **Espaço**: Pausar/continuar animação
- **R**: Resetar simulação
- **Q ou ESC**: Sair

## Building
```bash
make        # Compila o projeto
make run    # Compila e executa
make clean  # Remove arquivos compilados
```

## Recent Changes
- 2025-12-08: Projeto criado com simulação 3D completa

## User Preferences
- Linguagem: Português
- Ambiente alvo: VS Code no Windows/Linux
