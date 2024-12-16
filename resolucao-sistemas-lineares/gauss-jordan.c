#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define MAX 20 // Tamanho máximo da matriz
#define TOL 1e-10 // Tolerância para lidar com valores próximos de zero

bool spd = true; // Variável global para rastrear se o sistema é SPD

/* Leitura do arquivo */
void lerEntrada(const char *nomeArquivo, int *n, long double A[MAX][MAX+1]) {
    FILE *file = fopen(nomeArquivo, "r");
    if (!file) {
        printf("Erro ao abrir o arquivo");
        exit(EXIT_FAILURE);
    }

    fscanf(file, "%d", n);

    for (int i = 0; i < *n; i++) {
        for (int j = 0; j < *n; j++)
            fscanf(file, "%Lf", &A[i][j]);
    }

    for (int i = 0; i < *n; i++)
        fscanf(file, "%Lf", &A[i][*n]);

    fclose(file);
}

/* Impressão da matriz */
void imprimirMatriz(int n, long double A[MAX][MAX+1], int iteracao) {
    printf("Iteração %d:\n", iteracao);
    for (int i = 0; i < n; i++) {
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
    int temp = ordem[i];
    ordem[i] = ordem[j];
    ordem[j] = temp;
}

/* Verifica se a linha é toda zero exceto o termo independente */
bool linhaTodaZero(int n, long double A[MAX][MAX+1], int linha) {
    for (int j = 0; j < n; j++) {
        if (fabsl(A[linha][j]) > TOL) {
            return false;
        }
    }
    return true;
}

/* Pivotamento completo */
bool pivotamento(int n, long double A[MAX][MAX+1], int ordem[MAX], int j) {
    int maxLinha = j, maxColuna = j;
    for (int i = j; i < n; i++) {
        for (int k = j; k < n; k++) {
            if (fabsl(A[i][k]) > fabsl(A[maxLinha][maxColuna])) {
                maxLinha = i;
                maxColuna = k;
            }
        }
    }

    if (fabsl(A[maxLinha][maxColuna]) < TOL) {
        if (linhaTodaZero(n, A, maxLinha)) {
            if (fabsl(A[maxLinha][n]) > TOL)
                printf("Sistema Impossível (SI)\n");
            else
                printf("Sistema Possível Indeterminado (SPI)\n");
        }
        spd = false;
        return false;
    }

    if (maxLinha != j)
        trocarLinhas(A, n, j, maxLinha);

    if (maxColuna != j)
        trocarColunas(A, ordem, n, j, maxColuna);

    return true;
}

/* Gauss-Jordan */
void gaussJordan(int n, long double A[MAX][MAX+1]) {
    int ordem[MAX];
    for (int i = 0; i < n; i++)
        ordem[i] = i;

    imprimirMatriz(n, A, 0);

    for (int j = 0; j < n; j++) {
        if (!pivotamento(n, A, ordem, j)) {
            return;
        }

        long double pivo = A[j][j];
        for (int k = 0; k <= n; k++)
            A[j][k] /= pivo;

        for (int i = 0; i < n; i++) {
            if (i != j) {
                long double fator = A[i][j];
                for (int k = 0; k <= n; k++)
                    A[i][k] -= fator * A[j][k];
            }
        }

        imprimirMatriz(n, A, j + 1);
    }

    if (spd) {
        printf("Sistema Possível e Determinado (SPD)\n");
        printf("S = {(");
        for (int i = 0; i < n; i++) {
            if (i > 0)
                printf("; ");
            printf("%Lf", A[i][n]);
        }
        printf(")}\n");
    }
}

int main() {
    int n;
    long double A[MAX][MAX+1];

    lerEntrada("entrada01.txt", &n, A);

    gaussJordan(n, A);

    return 0;
}
