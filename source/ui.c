/*
 * ============================================================================
 * File        : ui.c
 * Author      : 0rientd
 * Description : Implementação da interface do usuário na tela inferior.
 *               
 *               LAYOUT DA TELA INFERIOR (40 colunas x 30 linhas):
 *               
 *               Leaderboard:
 *               ┌────────────────────────────────────────┐
 *               │       === LEADERBOARD ===              │  Linha 1
 *               │                                        │
 *               │       1. ABC    12345 pts              │  Linha 4
 *               │       2. XYZ     9876 pts              │  Linha 6
 *               │       ...                              │
 *               │────────────────────────────────────────│  Linha 20
 *               │       D-Pad  Move tiles                │  Linha 22
 *               │       SELECT Restart game              │  Linha 24
 *               │       START  Exit                      │  Linha 26
 *               └────────────────────────────────────────┘
 *               
 *               ENTRADA DE INICIAIS:
 *               - 3 letras selecionáveis independentemente
 *               - Setas visuais indicam qual letra está selecionada
 *               - Letra selecionada aparece em vídeo reverso
 *               
 *               DEBOUNCE:
 *               - Implementado comparando kDown atual com o anterior
 *               - Evita que um toque seja registrado múltiplas vezes
 * ============================================================================
 */

#include "ui.h"
#include <stdio.h>
#include <stdlib.h>

/* ============================================================================
 * FUNÇÕES PÚBLICAS
 * ============================================================================ */

void drawLeaderboard(void) {
    consoleSelect(&bottomScreen);
    
    // Limpa a tela
    printf("\x1b[2J");
    
    // Título
    printf("\x1b[2;12H\x1b[33;40m=== LEADERBOARD ===\x1b[0m\n\n");
    
    // Exibe cada entrada do ranking
    for (int i = 0; i < LEADERBOARD_SIZE; i++) {
        if (leaderboard[i].score > 0) {
            // Entrada válida - exibe com cores
            printf("\x1b[%d;9H%d. \x1b[36;40m%s\x1b[0m  %10lu pts\n", 
                6 + i * 2,                    // Linha (6, 8, 10, 12, 14)
                i + 1,                        // Posição (1-5)
                leaderboard[i].initials,      // Iniciais
                leaderboard[i].score);        // Pontuação
        } else {
            // Entrada vazia - exibe em cinza
            printf("\x1b[%d;9H%d. \x1b[90;40m---\x1b[0m  %10s\n", 
                6 + i * 2, 
                i + 1, 
                "---");
        }
    }
    
    // Controles do jogo
    printf("\x1b[29;2H\x1b[32;40mSELECT\x1b[0m to restart");
	printf("\x1b[29;27H\x1b[31;40mSTART\x1b[0m to exit");
}

void getPlayerInitials(char *initials, uint32_t score) {
    // Inicializa com "AAA"
    char letters[INITIALS_LENGTH] = {'A', 'A', 'A'};
    int currentPos = 0;      // Posição atual (0, 1 ou 2)
    u32 kDownOld = 0;        // Para debounce
    
    consoleSelect(&bottomScreen);
    
    while (aptMainLoop()) {
        // === RENDERIZAÇÃO ===
        printf("\x1b[2J");  // Limpa tela
        
        // Título
        printf("\x1b[2;10H\x1b[33;40m=== NEW HIGH SCORE! ===\x1b[0m");
        
        // Mostra a pontuação obtida
        printf("\x1b[6;16H\x1b[32;40m%lu points\x1b[0m", score);
        
        // Instrução
        printf("\x1b[10;12HEnter your initials");
        
        // === DESENHA AS LETRAS ===
        printf("\x1b[13;17H");
        for (int i = 0; i < INITIALS_LENGTH; i++) {
            if (i == currentPos) {
                // Letra selecionada - vídeo reverso
                printf("\x1b[7m %c \x1b[0m", letters[i]);
            } else {
                // Letra não selecionada - normal
                printf(" %c ", letters[i]);
            }
        }
        
        // === SETAS INDICADORAS (CIMA) ===
        printf("\x1b[12;17H");
        for (int i = 0; i < INITIALS_LENGTH; i++) {
            if (i == currentPos) {
                printf(" ^ ");
            } else {
                printf("   ");
            }
        }
        
        // === SETAS INDICADORAS (BAIXO) ===
        printf("\x1b[14;17H");
        for (int i = 0; i < INITIALS_LENGTH; i++) {
            if (i == currentPos) {
                printf(" v ");
            } else {
                printf("   ");
            }
        }
        
        // Instruções de controle
        printf("\x1b[17;12H\x1b[32;40mA: Confirm initials\x1b[0m");
        printf("\x1b[28;2H\x1b[90;40mUP/DOWN: Change letter\x1b[0m");
        printf("\x1b[30;2HLEFT/RIGHT: Move position");
        
        // Atualiza a tela
        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
        
        // === ENTRADA ===
        hidScanInput();
        u32 kDown = hidKeysDown();
        
        // Debounce - ignora se é o mesmo botão do frame anterior
        if (kDown == kDownOld) {
            kDown = 0;
        }
        kDownOld = hidKeysDown();
        
        // Processa entrada
        if (kDown & KEY_UP) {
            // Próxima letra (com wrap A→Z→A)
            letters[currentPos]++;
            if (letters[currentPos] > 'Z') {
                letters[currentPos] = 'A';
            }
        }
        
        if (kDown & KEY_DOWN) {
            // Letra anterior (com wrap Z→A→Z)
            letters[currentPos]--;
            if (letters[currentPos] < 'A') {
                letters[currentPos] = 'Z';
            }
        }
        
        if (kDown & KEY_LEFT) {
            // Move para a esquerda
            if (currentPos > 0) {
                currentPos--;
            }
        }
        
        if (kDown & KEY_RIGHT) {
            // Move para a direita
            if (currentPos < INITIALS_LENGTH - 1) {
                currentPos++;
            }
        }
        
        if (kDown & KEY_A) {
            // Confirma
            break;
        }
    }
    
    // Copia resultado para o buffer de saída
    for (int i = 0; i < INITIALS_LENGTH; i++) {
        initials[i] = letters[i];
    }
    initials[INITIALS_LENGTH] = '\0';
}

bool askRestart(void) {
    consoleSelect(&bottomScreen);
    
    // Exibe diálogo
    printf("\x1b[2J");
    printf("\x1b[2;15H\x1b[33;40mScore saved!\x1b[0m");
    printf("\x1b[15;15HPlay again?");
    printf("\x1b[29;25H\x1b[32;40mA\x1b[0m: Yes, restart");
    printf("\x1b[29;2H\x1b[31;40mB\x1b[0m: No, view scores");
    
    while (aptMainLoop()) {
        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
        
        hidScanInput();
        u32 kDown = hidKeysDown();
        
        if (kDown & KEY_A) {
            return true;   // Reiniciar
        }
        
        if (kDown & KEY_B) {
            return false;  // Ver leaderboard
        }
        
        if (kDown & KEY_START) {
            // Sair do jogo completamente
            gfxExit();
            exit(0);
        }
    }
    
    return false;
}
