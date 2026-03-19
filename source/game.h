/*
 * ============================================================================
 * File        : game.h
 * Author      : 0rientd
 * Description : Arquivo de cabeçalho principal do jogo 2048 para 3DS.
 *               
 *               Este arquivo contém todas as definições compartilhadas entre
 *               os módulos do jogo:
 *               - Constantes globais (tamanho do tabuleiro, versão, paths)
 *               - Tipos de dados (LeaderboardEntry)
 *               - Variáveis globais externas (consoles de tela)
 *               
 *               Deve ser incluído por todos os outros módulos que precisam
 *               acessar essas definições compartilhadas.
 * ============================================================================
 */

#ifndef GAME_H
#define GAME_H

#include <3ds.h>
#include <stdbool.h>
#include <stdint.h>

/* ============================================================================
 * CONSTANTES DO JOGO
 * ============================================================================ */

#define VERSION "1.0.0"

// Tamanho do tabuleiro (4x4)
#define SIZE 4

// Configurações do leaderboard
#define LEADERBOARD_SIZE 5      // Top 5 pontuações
#define INITIALS_LENGTH 3       // 3 letras para iniciais (estilo arcade)

// Paths para salvamento no SD card do 3DS
#define SAVE_PATH "sdmc:/3ds/2048-port-3ds/"
#define SAVE_FILE "sdmc:/3ds/2048-port-3ds/leaderboard.sav"

/* ============================================================================
 * TIPOS DE DADOS
 * ============================================================================ */

// Estrutura para uma entrada no leaderboard
// Armazena as iniciais do jogador (3 letras) e a pontuação obtida
typedef struct {
    char initials[INITIALS_LENGTH + 1];  // 3 letras + terminador nulo
    uint32_t score;                       // Pontuação do jogador
} LeaderboardEntry;

/* ============================================================================
 * VARIÁVEIS GLOBAIS EXTERNAS
 * ============================================================================ */

// Consoles para as telas do 3DS (definidos em main.c)
// topScreen: tela superior (onde o tabuleiro é desenhado)
// bottomScreen: tela inferior (leaderboard e controles)
extern PrintConsole topScreen;
extern PrintConsole bottomScreen;

// Array global do leaderboard (definido em leaderboard.c)
extern LeaderboardEntry leaderboard[LEADERBOARD_SIZE];

#endif /* GAME_H */
