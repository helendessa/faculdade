#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "maquina-finita.h"

void menu() {
    printf("\nMenu:\n");
    printf("1 - Inserir novo numero\n");
    printf("2 - Redefinir valores de n, l e u\n");
    printf("3 - Sair\n");
    printf("Escolha uma opção: "); }

int main() {
    int n, l, u, expoente;
    long long int numInt;
    long double numDec, numFrac, decimal;
    bool negativo, overflow, underflow, truncamento, condicoes = false;
    char binInt[2048], binFrac[2048], numBin[4096], binNormal[4096], sinalMag[4096], comp1[4096], comp2[4096];
    int op;

    while (!condicoes) {
        printf("Precisao da mantissa: ");
        scanf("%d", &n); getchar();
        printf("Lower: ");
        scanf("%d", &l); getchar();
        printf("Upper: ");
        scanf("%d", &u); getchar();

        if (n < 0) {
            printf("Erro: A mantissa deve ser maior do que zero.\n");
            condicoes = false;
        } else
            condicoes = true;

        if (u < l) {
            printf("Erro: Upper deve ser maior ou igual a lower.\n");
            condicoes = false;
        } else
            condicoes = true;
    }

    do {
        menu();
        scanf("%d", &op);
        getchar();

        switch (op) {
    case 1:
        printf("Digite o numero: ");
        scanf("%Lf", &numDec);getchar();

        if (numDec < 0)
            negativo = true;
        else
            negativo = false;

        /* separacao partes inteira e decimal */
        numInt = (long long int) numDec;
        numFrac = numDec - numInt;

        /* conversao decimal para bin */
        int2Bin(numInt, binInt, negativo);
        frac2Bin(numFrac, binFrac, n, negativo);

        /* concatenacao int e frac bin */
        strcpy(numBin, binInt);
        if (strlen(binFrac) > 0) {
            strcat(numBin, ".");
            strcat(numBin, binFrac);
        }

        /* exibicao numero convertido sem normalizacao */
        printf("Conversao: %s\n", numBin);

        /* normalizacao */
        normalizaBin(binInt, binFrac, binNormal, n, l, u, &expoente, &underflow, &overflow, &truncamento);

        /* exibicao dos resultados */
        printf("\nNumero normalizado armazenado em binario: %d %s * 2^%d\n", negativo ? 1 : 0, binNormal + 2, expoente);

        if (overflow)
            printf("Erro: Overflow\n");
        else if (underflow)
            printf("Erro: Underflow\n");

        if (truncamento)
            printf("Aviso: Truncamento\n");

        /* representacoes negativas */
        if (negativo) {
            //printf("\nbinNormal = %s", binNormal);
            sinalMagnitude(binNormal, sinalMag, n+2);
            complemento1(binNormal, comp1, n);
            complemento2(binNormal, comp2, n);

            printf("Sinal-Magnitude: %s * 2^%d\n", sinalMag, expoente);
            printf("Complemento de 1: 1 %s * 2^%d\n", comp1, expoente);
            printf("Complemento de 2: 1 %s * 2^%d\n", comp2, expoente);
        }

        /* conversao de volta bin->dec */
        decimal = bin2Dec(binNormal, expoente, negativo);
        printf("Representacao decimal: %.50Lf\n", decimal);
        break;

    /* mudanca de parametros */
    case 2:
        condicoes = false;
        while (!condicoes) {
            printf("Precisao da mantissa: ");
            scanf("%d", &n);getchar();
            printf("Lower: ");
            scanf("%d", &l);getchar();
            printf("Upper: ");
            scanf("%d", &u);getchar();

            if (n < 0) {
                printf("Erro: A mantissa deve ser maior do que zero.\n");
                condicoes = false;
            } else {
                condicoes = true;
            }
            if (u < l) {
                printf("Erro: Upper deve ser maior ou igual a lower.\n");
                condicoes = false;
            } else {
                condicoes = true;
            }
        }
        break;

    case 3:
            printf("Saindo...\n");
            break;

    default:
            printf("Opcao invalida! Tente novamente.\n");
}

} while (op != 3);

return 0;

}
