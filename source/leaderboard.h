/*
 * ============================================================================
 * File        : leaderboard.h
 * Author      : 0rientd
 * Description : Cabeçalho do módulo de leaderboard (placar de recordes).
 *               
 *               Gerencia o sistema de pontuações máximas:
 *               - Armazena top 5 pontuações com iniciais do jogador
 *               - Persiste dados no SD card do 3DS
 *               - Verifica se uma pontuação entra no ranking
 *               
 *               Os dados são salvos em formato binário no arquivo:
 *               sdmc:/3ds/2048-port-3ds/leaderboard.sav
 * ============================================================================
 */

#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include "game.h"

/* ============================================================================
 * FUNÇÕES DE INICIALIZAÇÃO E PERSISTÊNCIA
 * ============================================================================ */

/**
 * Inicializa o leaderboard com entradas vazias.
 * Chamado quando não existe arquivo de save ou quando está corrompido.
 */
void initLeaderboard(void);

/**
 * Carrega o leaderboard do SD card.
 * Se o arquivo não existir ou estiver corrompido, inicializa com valores vazios.
 * 
 * @return true se carregou com sucesso, false se inicializou do zero
 */
bool loadLeaderboard(void);

/**
 * Salva o leaderboard no SD card.
 * Cria a estrutura de diretórios se não existir.
 * 
 * @return true se salvou com sucesso, false em caso de erro
 */
bool saveLeaderboard(void);

/* ============================================================================
 * FUNÇÕES DE CONSULTA
 * ============================================================================ */

/**
 * Verifica se uma pontuação entra no leaderboard.
 * Uma pontuação entra se for maior que a menor do top 5.
 * 
 * @param score Pontuação a verificar
 * @return true se é um high score, false caso contrário
 */
bool isHighScore(uint32_t score);

/**
 * Retorna a posição que uma pontuação ocuparia no ranking.
 * 
 * @param score Pontuação a verificar
 * @return Posição 0-4 se entra no ranking, -1 se não entra
 */
int getScoreRank(uint32_t score);

/* ============================================================================
 * FUNÇÕES DE MANIPULAÇÃO
 * ============================================================================ */

/**
 * Adiciona uma nova entrada ao leaderboard.
 * A entrada é inserida na posição correta e as pontuações menores
 * são deslocadas para baixo (a última é removida se necessário).
 * 
 * Nota: Esta função NÃO salva automaticamente. Chame saveLeaderboard()
 * após adicionar para persistir a mudança.
 * 
 * @param initials String de 3 caracteres com as iniciais do jogador
 * @param score Pontuação obtida
 */
void addToLeaderboard(const char *initials, uint32_t score);

#endif /* LEADERBOARD_H */
