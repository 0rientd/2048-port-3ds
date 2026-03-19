/*
 * ============================================================================
 * File        : leaderboard.c
 * Author      : 0rientd
 * Description : Implementação do sistema de leaderboard.
 *               
 *               COMO FUNCIONA O SALVAMENTO:
 *               - Os dados são salvos em formato binário (struct direta)
 *               - Arquivo: sdmc:/3ds/2048-port-3ds/leaderboard.sav
 *               - Tamanho fixo: LEADERBOARD_SIZE * sizeof(LeaderboardEntry)
 *               - A estrutura de diretórios é criada automaticamente
 *               
 *               ESTRUTURA DO ARQUIVO:
 *               | Offset | Tamanho | Conteúdo                    |
 *               |--------|---------|------------------------------|
 *               | 0      | 4 bytes | Iniciais entrada 1 + null    |
 *               | 4      | 4 bytes | Score entrada 1 (uint32)     |
 *               | 8      | 4 bytes | Iniciais entrada 2 + null    |
 *               | ...    | ...     | ...                          |
 *               
 *               ORDENAÇÃO:
 *               - O leaderboard é sempre mantido ordenado (maior para menor)
 *               - Ao adicionar, entradas menores são deslocadas para baixo
 *               - A 5ª menor pontuação é descartada quando uma nova entra
 * ============================================================================
 */

#include "leaderboard.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

/* ============================================================================
 * VARIÁVEL GLOBAL
 * ============================================================================ */

// Array com as entradas do leaderboard (exportado para outros módulos)
LeaderboardEntry leaderboard[LEADERBOARD_SIZE];

/* ============================================================================
 * FUNÇÕES PÚBLICAS
 * ============================================================================ */

void initLeaderboard(void) {
    for (int i = 0; i < LEADERBOARD_SIZE; i++) {
        // Iniciais "---" indicam entrada vazia
        strcpy(leaderboard[i].initials, "---");
        leaderboard[i].score = 0;
    }
}

bool loadLeaderboard(void) {
    FILE *file = fopen(SAVE_FILE, "rb");
    
    if (file == NULL) {
        // Arquivo não existe - inicializa do zero
        initLeaderboard();
        return false;
    }
    
    // Lê todas as entradas de uma vez
    size_t entriesRead = fread(leaderboard, sizeof(LeaderboardEntry), LEADERBOARD_SIZE, file);
    fclose(file);
    
    // Verifica se o arquivo tem o tamanho correto
    if (entriesRead != LEADERBOARD_SIZE) {
        // Arquivo corrompido ou incompleto - inicializa do zero
        initLeaderboard();
        return false;
    }
    
    return true;
}

bool saveLeaderboard(void) {
    // Cria a estrutura de diretórios (ignora erro se já existir)
    mkdir("sdmc:/3ds", 0777);
    mkdir(SAVE_PATH, 0777);
    
    FILE *file = fopen(SAVE_FILE, "wb");
    
    if (file == NULL) {
        // Não conseguiu criar o arquivo
        return false;
    }
    
    // Escreve todas as entradas de uma vez
    size_t entriesWritten = fwrite(leaderboard, sizeof(LeaderboardEntry), LEADERBOARD_SIZE, file);
    fclose(file);
    
    return (entriesWritten == LEADERBOARD_SIZE);
}

bool isHighScore(uint32_t score) {
    // Zero pontos nunca é high score
    if (score == 0) {
        return false;
    }
    
    // É high score se for maior que a menor pontuação do ranking
    return score > leaderboard[LEADERBOARD_SIZE - 1].score;
}

int getScoreRank(uint32_t score) {
    // Encontra a posição onde esta pontuação ficaria
    for (int i = 0; i < LEADERBOARD_SIZE; i++) {
        if (score > leaderboard[i].score) {
            return i;
        }
    }
    
    // Não entra no ranking
    return -1;
}

void addToLeaderboard(const char *initials, uint32_t score) {
    int rank = getScoreRank(score);
    
    // Se não entra no ranking, não faz nada
    if (rank < 0) {
        return;
    }
    
    // Desloca entradas para baixo para abrir espaço
    // Começa do final e vai até a posição do novo rank
    for (int i = LEADERBOARD_SIZE - 1; i > rank; i--) {
        leaderboard[i] = leaderboard[i - 1];
    }
    
    // Insere a nova entrada na posição correta
    strncpy(leaderboard[rank].initials, initials, INITIALS_LENGTH);
    leaderboard[rank].initials[INITIALS_LENGTH] = '\0';  // Garante terminação
    leaderboard[rank].score = score;
}
