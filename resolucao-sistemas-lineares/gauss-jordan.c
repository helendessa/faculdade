#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX 20 // Tamanho maximo da matriz
#define TOL 1e-10 // Tolerancia para lidar com valores proximos de zero

/* Leitura do arquivo */
void lerEntrada(const char *nomeArquivo, int *n, long double A[MAX][MAX+1]) {
    /* Verifica erro na leitura */
    FILE *file = fopen(nomeArquivo, "r");
    if (!file) {
        printf("Erro ao abrir o arquivo");
        exit(EXIT_FAILURE); // Para nao entrar na proxima funcao se a leitura da entrada der erro
    }

    /* Primeiro valor eh n, entao faz leitura dele */
    fscanf(file, "%d", n);

    /* Leitura dos coeficientes */
    for (int i = 0; i < *n; i++) {
        for (int j = 0; j < *n; j++)
            fscanf(file, "%Lf", &A[i][j]);
    }

    /* Leitura dos termos independentes */
    for (int i = 0; i < *n; i++)
        fscanf(file, "%Lf", &A[i][*n]);

    fclose(file);
}

/* Impressao da matriz */
void imprimirMatriz(int n, long double A[MAX][MAX+1], int ordem[MAX], int iteracao) {
    printf("Iteracao %d:\n", iteracao);
    for (int i = 0; i < n; i++) {
        printf("Linha para x%d: ", ordem[i] + 1);
        for (int j = 0; j <= n; j++)
            printf("%15.10Lf ", A[i][j]);

        printf("\n");
    }
    printf("\n");
}

/* Troca duas linhas */
void trocarLinhas(long double A[MAX][MAX+1], int n, int i, int j) {
    for (int k = 0; k <= n; k++) {
        long double temp = A[i][k];
        A[i][k] = A[j][k];
        A[j][k] = temp;
    }
}

/* Troca duas colunas */
void trocarColunas(long double A[MAX][MAX+1], int ordem[MAX], int n, int i, int j) {
    for (int k = 0; k < n; k++) {
        long double temp = A[k][i];
        A[k][i] = A[k][j];
        A[k][j] = temp;
    }
    /* Atualiza o vetor de ordem para identificar as trocas */
    int temp = ordem[i];
    ordem[i] = ordem[j];
    ordem[j] = temp;
}

/* Gauss-Jordan */
void gaussJordan(int n, long double A[MAX][MAX+1]) {
    int ordem[MAX];
    for (int i = 0; i < n; i++)
        ordem[i] = i;

    imprimirMatriz(n, A, ordem, 0);

    /* Pivotamento */
    for (int j = 0; j < n; j++) { // Le cada coluna para identificar o valor maximo de cada uma
        int maxLinha = j, maxColuna = j;
        for (int i = j; i < n; i++) {
            for (int k = j; k < n; k++) {
                if (fabsl(A[i][k]) > fabsl(A[maxLinha][maxColuna])) {
                    maxLinha = i; // Atualiza a linha com o valor maximo
                    maxColuna = k; // Atualiza a coluna com o valor maximo
                }
            }
        }

        /* Verifica se o maior elemento encontrado eh nulo ou proximo de zero */
        if (fabsl(A[maxLinha][maxColuna]) < TOL) {
            /* Verifica se eh SI ou SPI */
            int todosZeros = 1;
            for (int k = 0; k < n; k++) {
                if (fabsl(A[maxLinha][k]) > TOL) {
                    todosZeros = 0;
                    break;
                }
            }

            /* Se todos os coeficientes forem 0 */
            if (todosZeros && fabsl(A[maxLinha][n]) > TOL) { // Se b != 0, entao SI
                printf("Sistema Impossivel (SI)\n");
            } else if (todosZeros && fabsl(A[maxLinha][n]) < TOL) { // Se b = 0, entao SPI
                printf("Sistema Possivel Indeterminado (SPI)\n");
            } else {
                printf("Sistema Impossivel (SI)\n");
            }
            return;
        }

        /* Pivoamento de novo */
        if (maxLinha != j)
            trocarLinhas(A, n, j, maxLinha);

        if (maxColuna != j)
            trocarColunas(A, ordem, n, j, maxColuna);

        /* Divide a linha pivo pelo elemento pivo */
        long double pivo = A[j][j];
        for (int k = 0; k <= n; k++)
            A[j][k] /= pivo;

        /* Armazena a linha pivo temporariamente para fazer as manipulacoes */
        long double L_j[MAX + 1];
        for (int k = 0; k <= n; k++)
            L_j[k] = A[j][k];

        /* Elimina os elementos acima e abaixo do pivo */
        for (int i = 0; i < n; i++) {
            if (i != j) {
                long double fator = A[i][j]; // Calcula o fator de eliminacao
                for (int k = 0; k <= n; k++)
                    A[i][k] -= fator * L_j[k]; // Subtrai a linha do pivo escalonada
            }
        }

        /* Imprime a matriz apos a eliminacao na coluna atual */
        imprimirMatriz(n, A, ordem, j + 1);
    }

    /* Verifica se eh SPI novamente */
    for (int i = 0; i < n; i++) {
        int todosZeros = 1;
        for (int j = 0; j < n; j++) {
            if (fabsl(A[i][j]) > TOL) {
                todosZeros = 0;
                break;
            }
        }
        if (todosZeros && fabsl(A[i][n]) < TOL) {
            printf("Sistema Possivel Indeterminado (SPI)\n");
            return;
        }
    }

    /* Calcula a solucao final */
    printf("Solucao do sistema:\n");
    for (int i = 0; i < n; i++)
        printf("x%d = %15.10Lf\n", ordem[i] + 1, A[i][n]);
}

int main() {
    int n;
    long double A[MAX][MAX+1];

    lerEntrada("entrada05.txt", &n, A);

    printf("Matriz aumentada original:\n");
    imprimirMatriz(n, A, (int[MAX]){0}, 0);

    gaussJordan(n, A);

    return 0;
}
