/*
 * ============================================================================
 * File        : ui.h
 * Author      : 0rientd
 * Description : Cabeçalho do módulo de interface do usuário.
 *               
 *               Gerencia todas as telas e interações na tela inferior:
 *               - Exibição do leaderboard
 *               - Tela de entrada de iniciais (estilo arcade)
 *               - Diálogos de confirmação
 *               
 *               A tela inferior do 3DS tem 40x30 caracteres e é usada
 *               para informações secundárias e interação com o jogador.
 * ============================================================================
 */

#ifndef UI_H
#define UI_H

#include "game.h"

/* ============================================================================
 * FUNÇÕES DE EXIBIÇÃO
 * ============================================================================ */

/**
 * Desenha o leaderboard na tela inferior.
 * Mostra o top 5 de pontuações com iniciais e também os controles do jogo.
 * 
 * Layout:
 * - Título "=== LEADERBOARD ==="
 * - 5 entradas: "1. ABC  12345 pts"
 * - Controles: D-Pad, SELECT, START
 */
void drawLeaderboard(void);

/* ============================================================================
 * FUNÇÕES DE ENTRADA
 * ============================================================================ */

/**
 * Exibe a tela de entrada de iniciais estilo arcade.
 * O jogador usa o D-Pad para selecionar 3 letras (A-Z).
 * 
 * Controles:
 * - UP/DOWN: Muda a letra atual (A→B→...→Z→A)
 * - LEFT/RIGHT: Move entre as 3 posições
 * - A: Confirma as iniciais
 * 
 * @param initials Buffer de 4 chars para receber as iniciais (3 + null)
 * @param score Pontuação obtida (exibida na tela)
 */
void getPlayerInitials(char *initials, uint32_t score);

/**
 * Exibe um diálogo perguntando se o jogador quer reiniciar.
 * Mostrado após salvar uma pontuação no leaderboard.
 * 
 * Controles:
 * - A: Sim, reiniciar
 * - B: Não, ver leaderboard
 * - START: Sair do jogo
 * 
 * @return true se o jogador quer reiniciar, false caso contrário
 */
bool askRestart(void);

#endif /* UI_H */
