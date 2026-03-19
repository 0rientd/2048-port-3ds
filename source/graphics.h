/*
 * ============================================================================
 * File        : graphics.h
 * Author      : 0rientd
 * Description : Cabeçalho do módulo de renderização gráfica.
 *               
 *               Responsável por toda a parte visual do jogo:
 *               - Desenho do tabuleiro na tela superior
 *               - Esquema de cores para cada valor de peça
 *               - Exibição do relógio e pontuação
 *               
 *               O 3DS usa um console de texto com códigos ANSI para cores.
 *               Este módulo abstrai essa complexidade em funções simples.
 * ============================================================================
 */

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "game.h"

/* ============================================================================
 * FUNÇÕES DE RENDERIZAÇÃO
 * ============================================================================ */

/**
 * Desenha o tabuleiro completo na tela superior.
 * Cada célula é exibida como um bloco 7x3 caracteres com:
 * - Cor de fundo baseada no valor da peça
 * - Número centralizado (ou "." para células vazias)
 * 
 * @param board Matriz SIZE x SIZE do tabuleiro
 * @param scheme Índice do esquema de cores (0 = padrão)
 * @param score Pontuação atual para exibir
 */
void drawBoard(uint8_t board[SIZE][SIZE], uint8_t scheme, uint32_t score);

/**
 * Atualiza o relógio exibido no canto superior esquerdo.
 * Mostra a hora atual no formato HH:MM:SS.
 */
void updateTimeOnScreen(void);

/**
 * Obtém as cores de foreground e background para um valor de peça.
 * Cores são retornadas através de ponteiros (padrão C para múltiplos retornos).
 * 
 * @param value Valor da peça (potência de 2: 1=2, 2=4, 3=8, etc.)
 * @param scheme Índice do esquema de cores
 * @param foreground Ponteiro para receber a cor do texto
 * @param background Ponteiro para receber a cor de fundo
 */
void getColors(uint8_t value, uint8_t scheme, uint8_t *foreground, uint8_t *background);

/**
 * Conta quantos dígitos um número tem.
 * Usado para centralizar números nas células do tabuleiro.
 * 
 * @param number Número a ser contado
 * @return Quantidade de dígitos (mínimo 1)
 */
uint8_t getDigitCount(uint32_t number);

#endif /* GRAPHICS_H */
