/*
 * ============================================================================
 * Name        : 2048 3DS Port
 * File        : main.c
 * Author      : 0rientd
 * Version     : 1.1.1
 * Description : Ponto de entrada e loop principal do jogo 2048 para Nintendo 3DS.
 *               
 *               Este arquivo contém:
 *               - Inicialização do hardware (gráficos, consoles)
 *               - Loop principal do jogo
 *               - Tratamento de entrada (D-Pad, START, SELECT)
 *               - Lógica de game over e reinício
 *               
 *               ESTRUTURA DO PROJETO:
 *               ├── main.c         - Este arquivo (loop principal)
 *               ├── game.h         - Tipos e constantes compartilhadas
 *               ├── board.c/h      - Lógica do tabuleiro e movimentos
 *               ├── graphics.c/h   - Renderização visual
 *               ├── leaderboard.c/h- Sistema de pontuação e salvamento
 *               └── ui.c/h         - Interface do usuário (telas, diálogos)
 *               
 *               FLUXO DO JOGO:
 *               1. Inicializa hardware e carrega leaderboard
 *               2. Loop principal:
 *                  a. Lê entrada do D-Pad
 *                  b. Executa movimento se válido
 *                  c. Adiciona peça aleatória
 *                  d. Verifica game over
 *               3. No game over:
 *                  a. Se high score → entrada de iniciais → salva
 *                  b. Pergunta se quer reiniciar
 *               4. START sai, SELECT reinicia
 * ============================================================================
 */

#include <stdio.h>
#include <unistd.h>

#include "game.h"
#include "board.h"
#include "graphics.h"
#include "leaderboard.h"
#include "ui.h"

/* ============================================================================
 * VARIÁVEIS GLOBAIS
 * ============================================================================ */

// Consoles para as telas (declarados em game.h, definidos aqui)
PrintConsole topScreen;
PrintConsole bottomScreen;

/* ============================================================================
 * FUNÇÃO PRINCIPAL
 * ============================================================================ */

int main(int argc, char **argv) {
    // Estado do jogo
    uint8_t board[SIZE][SIZE];   // Tabuleiro 4x4
    uint8_t scheme = 0;          // Esquema de cores (0 = padrão)
    uint32_t score = 0;          // Pontuação atual
    bool success;                // Resultado do último movimento
    bool gameOver = false;       // Flag de fim de jogo

    // ========================================
    // INICIALIZAÇÃO DO HARDWARE
    // ========================================
    
    gfxInitDefault();

    // Configura a tela inferior (leaderboard e controles)
    consoleInit(GFX_BOTTOM, &bottomScreen);
    consoleSelect(&bottomScreen);
    printf("\x1b[2;14H2048 for 3DS\n");

    // Carrega pontuações salvas e exibe o leaderboard
    loadLeaderboard();
    drawLeaderboard();

    // Configura a tela superior (tabuleiro do jogo)
    consoleInit(GFX_TOP, &topScreen);
    consoleSelect(&topScreen);
    
    printf("\x1b[2;2HPorted by \x1b[35;40m0rientd\x1b[0m");
    
    // Inicializa o tabuleiro e desenha
    initBoard(board);
    drawBoard(board, scheme, score);

    // ========================================
    // LOOP PRINCIPAL DO JOGO
    // ========================================
    
    while (aptMainLoop()) {
        // Lê entrada dos botões
        hidScanInput();
        u32 kDown = hidKeysDown();

        // Processa entrada apenas se o jogo não acabou
        if (!gameOver) {
            // Determina o movimento baseado no D-Pad
            if (kDown & KEY_LEFT) {
                success = moveLeft(board, &score);
            } else if (kDown & KEY_RIGHT) {
                success = moveRight(board, &score);
            } else if (kDown & KEY_UP) {
                success = moveUp(board, &score);
            } else if (kDown & KEY_DOWN) {
                success = moveDown(board, &score);
            } else {
                success = false;
            }

            // Se houve movimento válido
            if (success) {
                // Redesenha o tabuleiro
                drawBoard(board, scheme, score);
                
                // Pequena pausa para feedback visual
                usleep(150 * 1000); // 150 ms
                
                // Adiciona nova peça e redesenha
                addRandom(board);
                drawBoard(board, scheme, score);
                
                // Verifica se o jogo acabou
                if (gameEnded(board)) {
                    gameOver = true;
                    
                    // Exibe mensagem de game over na tela superior
                    consoleSelect(&topScreen);
                    printf("\n\x1b[22;18H\x1b[31;40m=== GAME OVER ===\x1b[0m\n");
                    
                    // Verifica se é um high score
                    if (isHighScore(score)) {
                        char initials[INITIALS_LENGTH + 1];
                        
                        // Obtém as iniciais do jogador
                        getPlayerInitials(initials, score);
                        
                        // Adiciona ao leaderboard e salva no SD
                        addToLeaderboard(initials, score);
                        saveLeaderboard();
                        
                        // Pergunta se quer reiniciar
                        if (askRestart()) {
                            // Reinicia o jogo
                            gameOver = false;
                            score = 0;
                            initBoard(board);
                            
                            // Limpa e redesenha a tela superior
                            consoleSelect(&topScreen);
                            printf("\x1b[2J\x1b[0;0H");
                            printf("\x1b[2;2HPorted by \x1b[35;40m0rientd\x1b[0m");
                            drawBoard(board, scheme, score);
                            
                            // Atualiza o leaderboard na tela inferior
                            drawLeaderboard();
                        } else {
                            // Mostra o leaderboard
                            drawLeaderboard();
                        }
                    } else {
                        // Não é high score - exibe mensagem na tela inferior
                        consoleSelect(&bottomScreen);
                        printf("\x1b[2J");
                        printf("\x1b[2;16H\x1b[31;40mGAME OVER\x1b[0m\n");
                        printf("\x1b[8;15H\x1b[32;40m%lu points\x1b[0m", score);
                        printf("\x1b[15;8HNot a high score this time");

                        printf("\x1b[29;2H\x1b[32;40mSELECT\x1b[0m to restart");
                        printf("\x1b[29;27H\x1b[31;40mSTART\x1b[0m to exit");
                    }
                }
            }
        }

        // START - Sai do jogo
        if (kDown & KEY_START) {
            break;
        }

        // SELECT - Reinicia o jogo
        if (kDown & KEY_SELECT) {
            gameOver = false;
            initBoard(board);
            score = 0;

            // Limpa e redesenha a tela superior
            consoleSelect(&topScreen);
            printf("\x1b[2J\x1b[0;0H");
            printf("\x1b[2;2HPorted by \x1b[35;40m0rientd\x1b[0m");
            drawBoard(board, scheme, score);
            
            // Atualiza o leaderboard
            drawLeaderboard();
        }

        // Atualiza o relógio
        updateTimeOnScreen();

        // Atualiza os buffers de vídeo
        gfxFlushBuffers();
        gfxSwapBuffers();

        // Sincroniza com o VBlank (60 FPS)
        gspWaitForVBlank();
    }

    // ========================================
    // FINALIZAÇÃO
    // ========================================
    
    gfxExit();
    return 0;
}
