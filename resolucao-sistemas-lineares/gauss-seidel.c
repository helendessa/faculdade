#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define MAX 20
#define TOL 1e-10

/* Códigos de escape ANSI para cores */
#define ANSI_COLOR_BLUE "\x1b[1;34m"
#define ANSI_COLOR_CYAN "\x1b[0;36m"
#define ANSI_COLOR_PURPLE "\x1b[1;35m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_RED "\x1b[1;31m"
#define ANSI_COLOR_RESET "\x1b[0m"
#define ANSI_COLOR_ORANGE "\x1b[1;33m"
#define ANSI_COLOR_HIGHINTENSITY_RED "\x1b[41m"

/* Leitura do arquivo */
void lerEntrada(const char *nomeArquivo, int *n, long double A[MAX][MAX+1], int *maxIteracoes, long double *epsilon) {
    FILE *file = fopen(nomeArquivo, "r");
    if (!file) {
        printf("Erro ao abrir o arquivo\n");
        exit(EXIT_FAILURE);
    }

    fscanf(file, "%d", n);

    for (int i = 0; i < *n; i++) {
        for (int j = 0; j < *n; j++)
            fscanf(file, "%Lf", &A[i][j]);
    }

    for (int i = 0; i < *n; i++)
        fscanf(file, "%Lf", &A[i][*n]);

    fscanf(file, "%d", maxIteracoes);
    fscanf(file, "%Lf", epsilon);

    fclose(file);
}

/* Impressão da matriz */
void imprimirMatriz(int n, long double A[MAX][MAX+1], int iteracao, bool escalonada) {
    if (iteracao == 0 && !escalonada) {
        printf(ANSI_COLOR_ORANGE "Matriz original:\n" ANSI_COLOR_RESET);
    } else if (escalonada) {
        printf(ANSI_COLOR_ORANGE "Matriz escalonada:\n" ANSI_COLOR_RESET);
    } else {
        printf("Iteração %d:\n", iteracao);
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j <= n; j++) {
            printf("%Lf\t ", A[i][j]);
        }
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

/* Ajusta valores muito pequenos para zero -- correção dos zeros negativos */
void ajustarValoresPequenos(int n, long double A[MAX][MAX+1]) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j <= n; j++) {
            if (fabsl(A[i][j]) < TOL) {
                A[i][j] = 0.0;
            }
        }
    }
}

/* Pivotamento para evitar zero na diagonal */
bool pivotamento(long double A[MAX][MAX+1], int n, int coluna) {
    int maxLinha = coluna;
    long double maxValor = fabsl(A[coluna][coluna]);

    for (int i = coluna + 1; i < n; i++) {
        if (fabsl(A[i][coluna]) > maxValor) {
            maxValor = fabsl(A[i][coluna]);
            maxLinha = i;
        }
    }

    if (maxLinha != coluna)
        trocarLinhas(A, n, coluna, maxLinha);

    if (fabsl(A[coluna][coluna]) < TOL || isnan(A[coluna][coluna])) {
        printf("Elemento pivô muito pequeno ou zero na diagonal. Indicativo de problema ao encontrar solução.\n");
        return false;
    }
    return true;
}

/* Escalonamento abaixo da diagonal principal */
bool escalonamento(int n, long double A[MAX][MAX+1]) {
    for (int i = 0; i < n; i++) {
        if (!pivotamento(A, n, i))
            return false;

        for (int j = i + 1; j < n; j++) {
            long double fator = A[j][i] / A[i][i];
            if (isnan(fator)) {
                printf("Divisão por zero\n");
                return false;
            }
            for (int k = i; k <= n; k++)
                A[j][k] -= fator * A[i][k];
        }

        ajustarValoresPequenos(n, A);

        /* Verifica se há uma linha com todos os elementos iguais a zero */
        bool linhaNula = true;
        for (int j = 0; j <= n; j++) {
            if (fabsl(A[i][j]) > TOL) {
                linhaNula = false;
                break;
            }
        }
        if (linhaNula) {
            printf("Linha %d é nula. Parando o escalonamento.\n", i + 1);
            return false;
        }
    }
    return true;
}

/* Verifica convergência usando o critério de Sassenfeld */
void verificaSassenfeld(int n, long double A[MAX][MAX+1]) {
    long double beta[MAX] = {0};
    long double maxBeta = 0;

    for (int i = 0; i < n; i++) {
        printf(ANSI_COLOR_ORANGE "Linha %d:\n" ANSI_COLOR_RESET, i + 1);

        long double soma = 0;
        for (int j = 0; j < n; j++) {
            if (i != j)
                soma += fabsl(A[i][j]);
        }
        beta[i] = soma / fabsl(A[i][i]);

        printf("Soma = %Lf, A[%d][%d] = %Lf\n", soma, i, fabsl(A[i][i]));
        printf(ANSI_COLOR_GREEN "beta[%d] = " ANSI_COLOR_CYAN "%Lf\n" ANSI_COLOR_RESET, i, beta[i]);

        if (beta[i] > maxBeta) {
            maxBeta = beta[i];
        }
    }

    printf(ANSI_COLOR_ORANGE "Maior beta" ANSI_COLOR_RESET " = " ANSI_COLOR_PURPLE "%Lf\n" ANSI_COLOR_RESET, maxBeta);
    if (maxBeta < 1)
        printf("Já que o maior beta < 1, " ANSI_COLOR_RED "converge" ANSI_COLOR_RESET ".\n\n");
    else
        printf("Já que o maior beta > 1, " ANSI_COLOR_RED "não há certeza de convergencia" ANSI_COLOR_RESET ".\n\n");
}

/*
    Classifica o sistema -- de acordo com o escalonamento, independe do Seidel em si:
    - Se a linha inteira for 0 e b também, SPI
    - Se a linha inteira for 0 e b != 0, SI
    - Diferente disso, SPD
 */
void classificaSistema(int n, long double A[MAX][MAX+1]) {
    int rankA = 0;
    int rankAum = 0;

    for (int i = 0; i < n; i++) {
        bool linhaNulaA = true;
        bool linhaNulaAum = true;

        /* Verifica se a linha da matriz dos coeficientes é nula */
        for (int j = 0; j < n; j++) {
            if (fabsl(A[i][j]) > TOL) {
                linhaNulaA = false;
                break;
            }
        }

        /* Verifica se a linha da matriz aumentada é nula */
        for (int j = 0; j <= n; j++) {
            if (fabsl(A[i][j]) > TOL) {
                linhaNulaAum = false;
                break;
            }
        }

        /* Se a linha dos coeficientes não for nula, aumenta o ranking dessa matriz */
        if (!linhaNulaA)
            rankA++;

        /* Se a linha da matriz aumentada não for nula, aumenta o ranking dessa matriz */
        if (!linhaNulaAum)
            rankAum++;
    }

    if (rankA < rankAum) { // Se tem linha com todos coeficientes 0 e b != 0
        printf(ANSI_COLOR_RED "Sistema Impossível (SI)\n" ANSI_COLOR_RESET);
    } else if (rankA < n) { // Se tem menos linhas não-zeradas do que número de variáveis
        printf(ANSI_COLOR_RED "Sistema Possível e Indeterminado (SPI)\n"ANSI_COLOR_RESET);
    } else { // Mesmo número de variáveis e linhas válidas
        printf(ANSI_COLOR_RED "Sistema Possível e Determinado (SPD)\n" ANSI_COLOR_RESET);
    }
}

/* Calcula o erro entre iterações */
long double calculaErro(int n, long double x[MAX], long double xo[MAX]) {
    long double soma = 0;
    for (int i = 0; i < n; i++)
        soma += (x[i] - xo[i]) * (x[i] - xo[i]);
    return sqrtl(soma);
}

/* Gauss-Seidel */
void gaussSeidel(int n, long double A[MAX][MAX+1], int maxIteracoes, long double epsilon) {
    long double x[MAX] = {0};
    long double xo[MAX];
    int k = 1;

    while (k <= maxIteracoes) {
        for (int i = 0; i < n; i++)
            xo[i] = x[i];

        for (int i = 0; i < n; i++) {
            long double soma = 0;

            for (int j = 0; j < i; j++)
                soma += A[i][j] * x[j];

            for (int j = i + 1; j < n; j++)
                soma += A[i][j] * xo[j];

            x[i] = (A[i][n] - soma) / A[i][i];
        }

        printf(ANSI_COLOR_BLUE "Iteração %d:\n" ANSI_COLOR_RESET, k);
        for (int i = 0; i < n; i++)
            printf(ANSI_COLOR_GREEN"x%d" ANSI_COLOR_RESET" = %.10Lf\n", i + 1, x[i]);

        long double erro = calculaErro(n, x, xo);
        printf(ANSI_COLOR_GREEN "Epsilon%d" ANSI_COLOR_RESET " = %.20Lf\n\n" ANSI_COLOR_RESET, k, erro);

        if (erro <= epsilon) {
            printf("\n" ANSI_COLOR_HIGHINTENSITY_RED "S = {");
            for (int i = 0; i < n; i++) {
                printf("%.10Lf", x[i]);
                if (i < n - 1)
                    printf("; ");
            }
            printf("}" ANSI_COLOR_RESET "\n");
            return;
        }

        k++;
    }

    printf("Parada: numero de iteracoes excedido.\n");
}

/* Sistema equivalente na forma x = Fx + d */
void xfxd(int n, long double A[MAX][MAX+1], long double x[MAX]) {
        printf(ANSI_COLOR_ORANGE "Sistema na forma x = Fx + d:\n" ANSI_COLOR_RESET);
        for (int i = 0; i < n; i++) {
            long double soma = 0;
            printf(ANSI_COLOR_BLUE "x%d"ANSI_COLOR_RESET, i + 1);
            printf(" = (%.10Lg - (", A[i][n]);
            for (int j = 0; j < n; j++) {
                if (i != j) {
                    soma += A[i][j] * x[j];
                    printf("%.10Lg * %.10Lg", A[i][j], x[j]);
                    if (j < n - 1) {
                        printf(" + ");
                    }
                }
            }
            x[i] = (A[i][n] - soma) / A[i][i];
            printf(")) / %.10Lg = ", A[i][i]);
            printf(ANSI_COLOR_CYAN "%.10Lg\n" ANSI_COLOR_RESET, x[i]);
        }
        printf("\n");
}

int main() {
    int n, maxIteracoes;
    long double epsilon;
    long double A[MAX][MAX+1];
    long double x[MAX];

    /* Vai de entrada01.txt à entrada13.txt */
    lerEntrada("entrada11.txt", &n, A, &maxIteracoes, &epsilon);

    /* Imprime a matriz original */
    imprimirMatriz(n, A, 0, false);

    if (!escalonamento(n, A)) {
        imprimirMatriz(n, A, 0, true);
        classificaSistema(n, A);
        return 1;
    }

    /* Imprime a matriz pivotada e escalonada */
    imprimirMatriz(n, A, 0, true);

    /* Calculo do sistema equivalente */
    xfxd(n, A, x);

    verificaSassenfeld(n, A);

    gaussSeidel(n, A, maxIteracoes, epsilon);

    /* Classifica o sistema em SI, SPI ou SPD de acordo com o escalonamento */
    classificaSistema(n, A);

    return 0;
}