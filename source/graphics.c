/*
 * ============================================================================
 * File        : graphics.c
 * Author      : 0rientd
 * Description : Implementação da renderização gráfica do jogo 2048.
 *               
 *               COMO FUNCIONA A RENDERIZAÇÃO NO 3DS:
 *               - O 3DS usa um console de texto (40x30 caracteres na tela superior)
 *               - Códigos de escape ANSI controlam posição e cores
 *               - \x1b[linha;colunaH move o cursor para posição específica
 *               - \x1b[38;5;Nm define cor do texto (256 cores)
 *               - \x1b[48;5;Nm define cor de fundo (256 cores)
 *               - \x1b[m reseta todas as formatações
 *               
 *               LAYOUT DO TABULEIRO:
 *               - Cada célula ocupa 7 caracteres de largura x 3 linhas de altura
 *               - 11 espaços de padding à esquerda para centralizar
 *               - Total: 11 + (7 * 4) = 39 caracteres (cabe nos 40 da tela)
 *               
 *               ESQUEMA DE CORES:
 *               - Valores baixos (2-128): fundo claro, texto escuro
 *               - Valores altos (256+): fundo escuro, texto claro
 *               - Cada potência de 2 tem sua cor única
 * ============================================================================
 */

#include "graphics.h"
#include <stdio.h>
#include <time.h>

/* ============================================================================
 * ESQUEMAS DE CORES
 * ============================================================================ */

// Array de cores: [bg1, fg1, bg2, fg2, bg3, fg3, ...]
// Índice par = background, índice ímpar = foreground
// Cores usam o padrão ANSI de 256 cores
static uint8_t colorSchemeOriginal[] = {
    8, 255,    // 0: vazio (cinza escuro, branco)
    1, 255,    // 1: 2 (vermelho escuro, branco)
    2, 255,    // 2: 4 (verde escuro, branco)
    3, 255,    // 3: 8 (amarelo escuro, branco)
    4, 255,    // 4: 16 (azul escuro, branco)
    5, 255,    // 5: 32 (magenta escuro, branco)
    6, 255,    // 6: 64 (ciano escuro, branco)
    7, 255,    // 7: 128 (branco escuro, branco)
    9, 0,      // 8: 256 (vermelho claro, preto)
    10, 0,     // 9: 512 (verde claro, preto)
    11, 0,     // 10: 1024 (amarelo claro, preto)
    12, 0,     // 11: 2048 (azul claro, preto)
    13, 0,     // 12: 4096 (magenta claro, preto)
    14, 0,     // 13: 8192 (ciano claro, preto)
    255, 0,    // 14: 16384 (branco, preto)
    255, 0     // 15: 32768+ (branco, preto)
};

/* ============================================================================
 * FUNÇÕES PÚBLICAS
 * ============================================================================ */

void getColors(uint8_t value, uint8_t scheme, uint8_t *foreground, uint8_t *background) {
    uint8_t *schemes[] = { colorSchemeOriginal };
    
    // Calcula índice no array de cores (value * 2 para bg, value * 2 + 1 para fg)
    // Usa módulo para evitar overflow
    *foreground = *(schemes[scheme] + (1 + value * 2) % sizeof(colorSchemeOriginal));
    *background = *(schemes[scheme] + (0 + value * 2) % sizeof(colorSchemeOriginal));
}

uint8_t getDigitCount(uint32_t number) {
    uint8_t count = 0;
    
    do {
        number /= 10;
        count += 1;
    } while (number);

    return count;
}

void drawBoard(uint8_t board[SIZE][SIZE], uint8_t scheme, uint32_t score) {
    uint8_t x, y, fg, bg;

    // Seleciona a tela superior para desenhar
    consoleSelect(&topScreen);
    
    // Exibe a pontuação na linha 7, alinhada à direita
    printf("\x1b[7;15H%17lu points\n\n", score);
    
    // Desenha cada linha do tabuleiro
    for (y = 0; y < SIZE; y++) {
        // === LINHA SUPERIOR DA CÉLULA ===
        printf("           "); // padding esquerdo para centralizar
        for (x = 0; x < SIZE; x++) {
            getColors(board[x][y], scheme, &fg, &bg);
            
            printf("\x1b[38;5;%u;48;5;%um", fg, bg); // define cores
            printf("       ");  // 7 espaços de largura
            printf("\x1b[m");   // reset cores
        }
        printf("\n");

        // === LINHA DO MEIO (COM O NÚMERO) ===
        printf("           "); // padding esquerdo
        for (x = 0; x < SIZE; x++) {
            getColors(board[x][y], scheme, &fg, &bg);
            printf("\x1b[38;5;%u;48;5;%um", fg, bg);
            
            if (board[x][y] != 0) {
                // Calcula o valor real (2^n) e centraliza
                uint32_t number = 1 << board[x][y];
                uint8_t t = 7 - getDigitCount(number);
                
                // Padding para centralizar: espaços à esquerda e à direita
                printf("%*s%lu%*s", t - t / 2, "", number, t / 2, "");
            } else {
                // Célula vazia - exibe ponto centralizado
                printf("   .   ");
            }

            printf("\x1b[m");
        }
        printf("\n");
        
        // === LINHA INFERIOR DA CÉLULA ===
        printf("           ");
        for (x = 0; x < SIZE; x++) {
            getColors(board[x][y], scheme, &fg, &bg);
        
            printf("\x1b[38;5;%u;48;5;%um", fg, bg);
            printf("       ");
            printf("\x1b[m");
        }
        printf("\n");
    }

    printf("\n");
    printf("\x1b[22;18H\x1b[32;40mD-Pad\x1b[0m Move tiles");
    printf("\x1b[A"); // move cursor uma linha para cima
}

void updateTimeOnScreen(void) {
    u32 hours, minutes, seconds;

    time_t unixTime = time(NULL);
    struct tm* timeStruct = gmtime((const time_t *)&unixTime);

    hours = timeStruct->tm_hour;
    minutes = timeStruct->tm_min;
    seconds = timeStruct->tm_sec;

    // Seleciona tela superior e desenha o relógio no canto
    consoleSelect(&topScreen);
    printf("\x1b[2;40H%02lu:%02lu:%02lu", hours, minutes, seconds);
}
