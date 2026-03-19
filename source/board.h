/*
 * ============================================================================
 * File        : board.h
 * Author      : 0rientd
 * Description : Cabeçalho do módulo de tabuleiro do jogo 2048.
 *               
 *               Define as funções para manipulação do tabuleiro 4x4:
 *               - Inicialização e reset do tabuleiro
 *               - Movimentos (cima, baixo, esquerda, direita)
 *               - Rotação do tabuleiro (usado internamente pelos movimentos)
 *               - Verificação de fim de jogo
 *               - Adição de peças aleatórias
 * ============================================================================
 */

#ifndef BOARD_H
#define BOARD_H

#include "game.h"

/* ============================================================================
 * FUNÇÕES DE INICIALIZAÇÃO
 * ============================================================================ */

/**
 * Inicializa o tabuleiro com todas as células zeradas
 * e adiciona 2 peças aleatórias para começar o jogo.
 * 
 * @param board Matriz SIZE x SIZE representando o tabuleiro
 */
void initBoard(uint8_t board[SIZE][SIZE]);

/* ============================================================================
 * FUNÇÕES DE MOVIMENTO
 * ============================================================================ */

/**
 * Move todas as peças para cima, combinando peças iguais.
 * 
 * @param board Matriz do tabuleiro
 * @param score Ponteiro para a pontuação (atualizado se houver merge)
 * @return true se alguma peça se moveu, false caso contrário
 */
bool moveUp(uint8_t board[SIZE][SIZE], uint32_t *score);

/**
 * Move todas as peças para baixo, combinando peças iguais.
 */
bool moveDown(uint8_t board[SIZE][SIZE], uint32_t *score);

/**
 * Move todas as peças para a esquerda, combinando peças iguais.
 */
bool moveLeft(uint8_t board[SIZE][SIZE], uint32_t *score);

/**
 * Move todas as peças para a direita, combinando peças iguais.
 */
bool moveRight(uint8_t board[SIZE][SIZE], uint32_t *score);

/* ============================================================================
 * FUNÇÕES DE ESTADO DO JOGO
 * ============================================================================ */

/**
 * Verifica se o jogo terminou (sem movimentos possíveis).
 * O jogo termina quando não há células vazias E não há
 * peças adjacentes iguais que possam ser combinadas.
 * 
 * @param board Matriz do tabuleiro
 * @return true se o jogo acabou, false se ainda há movimentos
 */
bool gameEnded(uint8_t board[SIZE][SIZE]);

/**
 * Adiciona uma peça aleatória (2 ou 4) em uma célula vazia.
 * Probabilidade: 90% de ser 2, 10% de ser 4.
 * 
 * @param board Matriz do tabuleiro
 */
void addRandom(uint8_t board[SIZE][SIZE]);

/**
 * Conta quantas células estão vazias no tabuleiro.
 * 
 * @param board Matriz do tabuleiro
 * @return Número de células vazias (0 a SIZE*SIZE)
 */
uint8_t countEmpty(uint8_t board[SIZE][SIZE]);

#endif /* BOARD_H */
