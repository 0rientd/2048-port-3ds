/*
 * ============================================================================
 * File        : board.c
 * Author      : 0rientd
 * Description : Implementação da lógica do tabuleiro do jogo 2048.
 *               
 *               Este módulo contém toda a mecânica central do jogo:
 *               
 *               COMO FUNCIONA O MOVIMENTO:
 *               1. O movimento "para cima" é o movimento base
 *               2. Outros movimentos (baixo, esquerda, direita) são implementados
 *                  rotacionando o tabuleiro, aplicando moveUp, e rotacionando de volta
 *               3. Isso simplifica o código - só precisamos implementar um algoritmo
 *               
 *               COMO FUNCIONA O MERGE:
 *               1. slideArray() desliza uma linha/coluna na direção do movimento
 *               2. findTarget() encontra a posição alvo para cada peça
 *               3. Se duas peças iguais colidem, elas se fundem (valor dobra)
 *               4. stop evita merge duplo (ex: [2,2,2,2] vira [4,4], não [8])
 *               
 *               VALORES DAS PEÇAS:
 *               - Internamente, usamos potências de 2 (1=2, 2=4, 3=8, etc.)
 *               - O valor exibido é 1 << board[x][y] (2^valor)
 * ============================================================================
 */

#include "board.h"
#include <stdlib.h>
#include <time.h>

/* ============================================================================
 * FUNÇÕES AUXILIARES (INTERNAS)
 * ============================================================================ */

/**
 * Encontra a posição alvo para uma peça durante o slide.
 * Procura a primeira posição disponível ou uma peça igual para merge.
 * 
 * @param array Linha/coluna sendo processada
 * @param x Posição atual da peça
 * @param stop Posição limite (evita merge duplo)
 * @return Posição alvo para a peça
 */
static uint8_t findTarget(uint8_t array[SIZE], uint8_t x, uint8_t stop) {
    uint8_t t;
    
    // Se já está na primeira posição, não precisa mover
    if (x == 0) {
        return x;
    }

    // Procura da posição atual até o início
    for (t = x - 1;; t--) {
        if (array[t] != 0) {
            // Encontrou uma peça
            if (array[t] != array[x]) {
                // Peças diferentes - não pode fazer merge, para na próxima posição
                return t + 1;
            }
            // Peças iguais - pode fazer merge nesta posição
            return t;
        } else {
            // Célula vazia - continua procurando, mas respeita o limite
            if (t == stop) {
                return t;
            }
        }
    }

    // Não encontrou alvo - mantém posição original
    return x;
}

/**
 * Desliza uma linha/coluna, fazendo merges quando possível.
 * 
 * @param array Linha/coluna a ser processada
 * @param score Ponteiro para pontuação (atualizado nos merges)
 * @return true se houve movimento, false caso contrário
 */
static bool slideArray(uint8_t array[SIZE], uint32_t *score) {
    bool success = false;
    uint8_t x, t, stop = 0;

    for (x = 0; x < SIZE; x++) {
        if (array[x] != 0) {
            t = findTarget(array, x, stop);

            // Se o alvo é diferente da posição atual, move ou faz merge
            if (t != x) {
                if (array[t] == 0) {
                    // Alvo vazio - apenas move
                    array[t] = array[x];
                } else if (array[t] == array[x]) {
                    // Merge! Aumenta a potência de 2
                    array[t]++;
                    // Adiciona pontos (valor da nova peça)
                    *score += 1 << array[t];
                    // Define stop para evitar merge duplo nesta iteração
                    stop = t + 1;
                }

                array[x] = 0;
                success = true;
            }
        }
    }

    return success;
}

/**
 * Rotaciona o tabuleiro 90 graus no sentido horário.
 * Usado para implementar movimentos em todas as direções
 * usando apenas a lógica de moveUp.
 */
static void rotateBoard(uint8_t board[SIZE][SIZE]) {
    uint8_t i, j, n = SIZE;
    uint8_t tmp;

    for (i = 0; i < n / 2; i++) {
        for (j = i; j < n - i - 1; j++) {
            tmp = board[i][j];
            board[i][j] = board[j][n - i - 1];
            board[j][n - i - 1] = board[n - i - 1][n - j - 1];
            board[n - i - 1][n - j - 1] = board[n - j - 1][i];
            board[n - j - 1][i] = tmp;
        }
    }
}

/**
 * Verifica se existe um par de peças iguais adjacentes verticalmente.
 * Usado para detectar se ainda há movimentos possíveis.
 */
static bool findPairDown(uint8_t board[SIZE][SIZE]) {
    uint8_t x, y;
    
    for (x = 0; x < SIZE; x++) {
        for (y = 0; y < SIZE - 1; y++) {
            if (board[x][y] == board[x][y + 1]) {
                return true;
            }
        }
    }

    return false;
}

/* ============================================================================
 * FUNÇÕES PÚBLICAS
 * ============================================================================ */

void initBoard(uint8_t board[SIZE][SIZE]) {
    uint8_t x, y;

    // Zera todas as células
    for (x = 0; x < SIZE; x++) {
        for (y = 0; y < SIZE; y++) {
            board[x][y] = 0;
        }
    }

    // Adiciona duas peças aleatórias para começar
    addRandom(board);
    addRandom(board);
}

bool moveUp(uint8_t board[SIZE][SIZE], uint32_t *score) {
    bool success = false;
    uint8_t x;

    // Aplica o slide em cada coluna
    for (x = 0; x < SIZE; x++) {
        success |= slideArray(board[x], score);
    }
    
    return success;
}

bool moveLeft(uint8_t board[SIZE][SIZE], uint32_t *score) {
    bool success;

    // Rotaciona 90° horário, move para cima, rotaciona 270° (3x90°)
    rotateBoard(board);
    success = moveUp(board, score);
    rotateBoard(board);
    rotateBoard(board);
    rotateBoard(board);
    
    return success;
}

bool moveDown(uint8_t board[SIZE][SIZE], uint32_t *score) {
    bool success;
    
    // Rotaciona 180° (2x90°), move para cima, rotaciona 180°
    rotateBoard(board);
    rotateBoard(board);
    success = moveUp(board, score);
    rotateBoard(board);
    rotateBoard(board);
    
    return success;
}

bool moveRight(uint8_t board[SIZE][SIZE], uint32_t *score) {
    bool success;
    
    // Rotaciona 270° (3x90°), move para cima, rotaciona 90°
    rotateBoard(board);
    rotateBoard(board);
    rotateBoard(board);
    success = moveUp(board, score);
    rotateBoard(board);
    
    return success;
}

uint8_t countEmpty(uint8_t board[SIZE][SIZE]) {
    uint8_t x, y;
    uint8_t count = 0;
    
    for (x = 0; x < SIZE; x++) {
        for (y = 0; y < SIZE; y++) {
            if (board[x][y] == 0) {
                count++;
            }
        }
    }

    return count;
}

bool gameEnded(uint8_t board[SIZE][SIZE]) {
    // Se há células vazias, o jogo continua
    if (countEmpty(board) > 0) {
        return false;
    }
    
    // Verifica pares verticais
    if (findPairDown(board)) {
        return false;
    }
    
    // Verifica pares horizontais (rotaciona e verifica verticais)
    rotateBoard(board);
    bool hasPairs = findPairDown(board);
    // Restaura a orientação original
    rotateBoard(board);
    rotateBoard(board);
    rotateBoard(board);
    
    return !hasPairs;
}

void addRandom(uint8_t board[SIZE][SIZE]) {
    static bool initialized = false;
    uint8_t x, y;
    uint8_t r, len = 0;
    uint8_t n, list[SIZE * SIZE][2];

    // Inicializa o gerador de números aleatórios uma vez
    if (!initialized) {
        srand(time(NULL));
        initialized = true;
    }

    // Encontra todas as células vazias
    for (x = 0; x < SIZE; x++) {
        for (y = 0; y < SIZE; y++) {
            if (board[x][y] == 0) {
                list[len][0] = x;
                list[len][1] = y;
                len++;
            }
        }
    }

    // Escolhe uma célula vazia aleatória e adiciona uma peça
    if (len > 0) {
        r = rand() % len;
        x = list[r][0];
        y = list[r][1];
        // 90% chance de 2 (valor 1), 10% chance de 4 (valor 2)
        n = (rand() % 10) / 9 + 1;
        
        board[x][y] = n;
    }
}
