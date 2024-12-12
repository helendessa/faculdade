#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

/* 1) CONVERSAO */

/* funçao para inverter o binario posteriormente, pq ele é gerado invertido */
void inverteBin(char bin[]) {
    int n = strlen(bin);
    char aux;
    for (int i = 0; i < n / 2; i++) { // vai ate a metade, pq se trocou a primeira metade pela metade final, ja esta certo
        aux = bin[i];
        bin[i] = bin[n - i - 1]; // vai trocando os iniciais pelos finais
        bin[n - i - 1] = aux;
    }
    //printf("\nbinário invertido: %s", bin);
}

/* funçao para converter para binario a parte inteira do decimal */
void int2Bin(long long int decInt, char binInt[], bool negativo) {
    int i = 0;

    if (negativo) { // pega o modulo do negativo
        decInt = -decInt;
    }

    if (decInt == 0) // se parte inteira decimal for 0, apenas coloca o zero
        binInt[i] = '0';
    else {
        while (decInt > 0) { //  se parte inteira do decimal for maior que zero, divisoes sucessivas por 2
            int resto = decInt % 2;
            binInt[i++] = (resto) + '0';
            decInt /= 2;
        }
    }

    binInt[i] = '\0'; // trabalhando com string, entao por fim se coloca \0
    //printf("\nINT: %s\n", binInt);

    inverteBin(binInt); // inverte o binario, pq ele é gerado invertido
}

/* funçao para converter ppara binario a parte fracionaria do decimal */
void frac2Bin(long double decFrac, char binFrac[], int precisao, bool negativo) {
    int i = 0;

    if (negativo) { // se negativo, pega o modulo
        decFrac = -decFrac;
    }

    while (decFrac > 0) { // enquanto for maior do que zero
        decFrac *= 2; // divisoes sucessivas por 2
        if (decFrac >= 1) { // se maior ou igual a 1, coloca 1
            binFrac[i++] = '1';
            decFrac -= 1; // diminui 1 para continuar o calculo
        } else // se 0, coloca 0
            binFrac[i++] = '0';
    }

    binFrac[i] = '\0';
    //printf("\nFRAC: %s\n", binFrac);
}

/* 2) NORMALIZACAO */
void normalizaBin(char binInt[], char binFrac[], char binNormal[], int n, int l, int u, int *expoente, bool *underflow, bool *overflow, bool *truncamento) {
    *underflow = false; // booleanos para verificar existencia de erros
    *overflow = false;
    *truncamento = false;
    *expoente = 0;

    /* concatena partes inteira e fracionaria */
    char numBin[4096];
    strcpy(numBin, binInt);
    if (strlen(binFrac) > 0)
        strcat(numBin, binFrac); // a partir daqui intBin e fracBin concatenados em numBin

    //printf("\nNumBin: %s", numBin);

    /* encontra a posicao do primeiro 1 do numero binario convertido */
    int pos1 = -1;
    for (int i = 0; numBin[i] != '\0'; i++) {
        if (numBin[i] == '1') {
            pos1 = i;
            break;
        }
    }

    //printf("\nPos1: %d", pos1);
    /* se nao tiver 1, o numero é zero */
    if (pos1 == -1) {
        strcpy(binNormal, "0.0");
        *expoente = 0;
        return;
    }

    /* calculo do expoente a partir da posicao do primeiro 1 encontrado */
    if (pos1 < strlen(binInt))
        *expoente = strlen(binInt) - pos1;
    else
        *expoente = -(pos1 - strlen(binInt));

    //printf("\nExpoente: %d\n", *expoente);

    /* mantissa normalizada */
    int j = 0;
    binNormal[j++] = '0';
    binNormal[j++] = '.';
    //binNormal[j++] = '1'; // sempre 0.1

    //printf("\n n: %d", n);

    for (int i = pos1; numBin[i] != '\0'; i++){ // depois do primeiro 1, ja inserido, coloca-se o restante
        binNormal[j++] = numBin[i];
        //printf("\nj: %d i: %d\n", j, i);
    }

    binNormal[n+2] = '\0';

    /* preenche com zeros se necessário, ate atingir a precisao da mantissa */
    while (j < n + 2)
        binNormal[j++] = '0';

    binNormal[j] = '\0'; // finaliza com \0 pq string

    /* verificar se houve truncamento */

    if (strlen(numBin) > n)
        *truncamento = true;

    /* for (int k = pos1 + n + 1; numBin[k] != '\0'; k++) {
        if (numBin[k] == '1') {
            *truncamento = true;
            break;
        }
    } */

    /* verificacao overflow e underflow */
    if (*expoente > u) {
        *overflow = true;
    } else if (*expoente < l) {
        *underflow = true;
    }
}

/* 3) TRANSFORMACAO NEGATIVA */

/* sinal-magnitude */
void sinalMagnitude(char bin[], char sinalMag[], int n) {
    sinalMag[0] = '1'; // coloca o bit de sinal negativo
    for (int i = 1; i < n; i++) {
        if (bin[i] == '.') {
            sinalMag[i] = ' '; // substitui o ponto por um espaço
        } else {
            sinalMag[i] = bin[i]; // copia o restante
        }
    }
    sinalMag[n] = '\0';
}

/* complemento de 1 */
void complemento1(char bin[], char comp1[], int n) {
    //printf("\nbin[2] = %c",bin[2]);
    int k = 0;
    for (int i = 2; i < n+2; i++){
        comp1[k++] = (bin[i] == '0') ? '1' : '0'; // aplica negacao
        //printf("i: %d  comp[i] = %c\n", i, comp1[i]);
    }
    //printf("n: %d", n);
    comp1[k] = '\0';
    //printf("\ncomp1: %s", comp1);
}

/* complemento de 2 */
void complemento2(char bin[], char comp2[], int n) {
    /* adiciona 1 ao complemento de 1 */
    complemento1(bin, comp2, n);
    int carry = 1; // para carregar a unidade em excesso
    for (int i = n - 1; i >= 0; i--) {
        if (comp2[i] == '1' && carry == 1) // se 1 e carrega 1, 0
            comp2[i] = '0';
        else if (comp2[i] == '0' && carry == 1) { // se 0 e carrega 1, 1
            comp2[i] = '1';
            carry = 0;
        }
    }
}

/* 4) CONVERSAO BINARIO PARA DECIMAL */
long double bin2Dec(char binNormal[], int expoente, bool negativo) {
    char intBin[2048], fracBin[2048];
    int i = 0, j = 0;

    /* separa as partes inteira e fracionaria -- caminho inverso */
    while (binNormal[i] != '.' && binNormal[i] != '\0') { // ate chegar ao ponto e nao chegar ao final
        intBin[i] = binNormal[i]; // copia o binario normalizado para o inteiro
        i++;
    }
    intBin[i] = '\0';

    if (binNormal[i] == '.') {
        i++; // pula o ponto
        while (binNormal[i] != '\0') // enquanto nao chegar ao fim do normalizado
            fracBin[j++] = binNormal[i++]; // copia o binario normalizado para o fracionario
    }
    fracBin[j] = '\0';

    long long int intDec = 0;
    for (int k = 0; k < strlen(intBin); k++) {
        if (intBin[k] == '1') // se encontra 1
            intDec += (1LL << (strlen(intBin) - k - 1)); // calcula a potencia de 2 correspondente a posicao de bit
    }

    long double fracDec = 0;
    for (int k = 0; k < strlen(fracBin); k++) {
        if (fracBin[k] == '1') // se encontra 1
            fracDec += (1.0 / (1LL << (k + 1))); // calcula a fracao correspondente a posicao de bit
    }

    /* junta as duas partes e transforma para decimal */
    long double decimal = intDec + fracDec;

    if (expoente >= 0) {
        decimal *= (1LL << expoente); // calcula expoente na base 2 com base no deslocamento de bits
    } else {
        for (int k = 0; k < -expoente; k++) { // se expoente menor do que 0, divide por 2, expoente negativo
            decimal /= 2.0;
        }
    }

    /* como ate entao se lidou com modulo, aplica a magnitude */
    if (negativo)
        decimal = -decimal;

    return decimal;
}
