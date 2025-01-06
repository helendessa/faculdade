char identificaClasse(char ipDecimal[]) {
    int tresPrimeirosDigitos = 0;

    for (int i = 0; i < 3; i++) {
        if (ipDecimal[i] == '.') break;
        tresPrimeirosDigitos = tresPrimeirosDigitos * 10 + (ipDecimal[i] - '0');
    }
    //printf("Primeiros três dígitos: %d\n", tresPrimeirosDigitos);

    if (tresPrimeirosDigitos >= 1 && tresPrimeirosDigitos <= 126)
        return 'A';

    if (tresPrimeirosDigitos >= 128 && tresPrimeirosDigitos <= 191)
        return 'B';

    if (tresPrimeirosDigitos >= 192 && tresPrimeirosDigitos <= 223)
        return 'C';

    if (tresPrimeirosDigitos >= 224 && tresPrimeirosDigitos <= 239)
        return 'D';

    if (tresPrimeirosDigitos >= 240 && tresPrimeirosDigitos <= 255)
        return 'E';

    return 'F';
}

int numeroSubRedes(char classe, int mascara) {
    if (classe == 'A')
        return (int)pow(2, mascara - 8);

    if (classe == 'B')
        return (int)pow(2, mascara - 16);

    if (classe == 'C')
        return (int)pow(2, mascara - 24);

    return -1;
}

int hostsPorSubrede(int mascara) {
    int h = 32 - mascara;

    //int num = pow(2, h) - 2;
    //printf("%d", num);

    return (int)pow(2, h) - 2;
}

void decimalParaBinario(int numeroDecimal, char binario[]) {
    for (int i = 7; i >= 0; i--) {
        binario[i] = (numeroDecimal % 2) + '0';
        numeroDecimal /= 2;
    }
    binario[8] = '\0';
}

void ipDecimalParaBinario(char ipDecimal[], char ipBinario[]) {
    int trio01, trio02, trio03, trio04;
    char binario[9];

    trio01 = (ipDecimal[0] - '0') * 100 + (ipDecimal[1] - '0') * 10 + (ipDecimal[2] - '0');
    trio02 = (ipDecimal[4] - '0') * 100 + (ipDecimal[5] - '0') * 10 + (ipDecimal[6] - '0');
    trio03 = (ipDecimal[8] - '0') * 100 + (ipDecimal[9] - '0') * 10 + (ipDecimal[10] - '0');
    trio04 = (ipDecimal[12] - '0') * 100 + (ipDecimal[13] - '0') * 10 + (ipDecimal[14] - '0');

    ipBinario[0] = '\0';

    decimalParaBinario(trio01, binario);
    strcat(ipBinario, binario);
    strcat(ipBinario, ".");

    decimalParaBinario(trio02, binario);
    strcat(ipBinario, binario);
    strcat(ipBinario, ".");

    decimalParaBinario(trio03, binario);
    strcat(ipBinario, binario);
    strcat(ipBinario, ".");

    decimalParaBinario(trio04, binario);
    strcat(ipBinario, binario);
}

void mascaraBooleana(int mascara, char mascaraBool[]) {
    int i;
    int j = 0;

    for (i = 0; i < 34; i++) {
        if (i < mascara)
            mascaraBool[j++] = '1';
        else
            mascaraBool[j++] = '0';

        if (i % 8 == 7 && i != 31)
            mascaraBool[j++] = '.';
    }

    mascaraBool[j] = '\0';
}

void geraEnderecoRede(char ipBinario[], char mascaraBool[], char enderecoRede[]) {
    for (int i = 0; ipBinario[i] != '\0'; i++) {
        if (ipBinario[i] == '1' && mascaraBool[i] == '1')
            enderecoRede[i] = '1';
        else if (ipBinario[i] == '.' || mascaraBool[i] == '.')
            enderecoRede[i] = '.';
        else
            enderecoRede[i] = '0';
    }
    enderecoRede[strlen(ipBinario)] = '\0';
}

void geraEnderecoBroadcast(char ipBinario[], char mascaraBool[], char enderecoBroadcast[]) {
    for (int i = 0; ipBinario[i] != '\0'; i++) {
        if (ipBinario[i] == '1' || mascaraBool[i] == '0')
            enderecoBroadcast[i] = '1';
        else if (ipBinario[i] == '.' || mascaraBool[i] == '.')
            enderecoBroadcast[i] = '.';
        else
            enderecoBroadcast[i] = '0';
    }
    enderecoBroadcast[strlen(ipBinario)] = '\0';
}

int binarioParaDecimal(char binario[]) {
    int decimal = 0;
    for (int i = 0; i < 8; i++)
        decimal = decimal * 2 + (binario[i] - '0');

    return decimal;
}

void ipBinarioParaDecimal(char ipBinario[], char ipDecimal[]) {
    char binario[9];
    int decimal;
    ipDecimal[0] = '\0';

    for (int i = 0; i < 4; i++) {
        strncpy(binario, ipBinario + i * 9, 8);
        binario[8] = '\0';
        decimal = binarioParaDecimal(binario);

        char octeto[4];
        sprintf(octeto, "%d", decimal);
        strcat(ipDecimal, octeto);

        if (i < 3)
            strcat(ipDecimal, ".");
    }
}

unsigned int ipParaInt(char *ip) {
    unsigned int a, b, c, d;
    sscanf(ip, "%u.%u.%u.%u", &a, &b, &c, &d);

    return a * 256 * 256 * 256 + b * 256 * 256 + c * 256 + d;
}


void intParaIp(unsigned int ip, char *str) {
    unsigned int a = ip / (256 * 256 * 256);
    unsigned int b = (ip % (256 * 256 * 256)) / (256 * 256);
    unsigned int c = (ip % (256 * 256)) / 256;
    unsigned int d = ip % 256;

    sprintf(str, "%u.%u.%u.%u", a, b, c, d);
}

void listarFaixaEnderecoHost(char enderecoRede[], char enderecoBroadcast[], FILE *arquivo) {
    unsigned int rede = ipParaInt(enderecoRede);
    unsigned int broadcast = ipParaInt(enderecoBroadcast);
    char enderecoAtual[16];

    fprintf(arquivo, "Faixa de endereços:\n");
    for (unsigned int ip = rede; ip <= broadcast; ip++) {
        intParaIp(ip, enderecoAtual);
        fprintf(arquivo, "%s\n", enderecoAtual);
    }
}

void listarFaixasSubredes(char enderecoRede[], int numSubRedes, int hostsPorSubrede, FILE *arquivo) {
    unsigned int rede = ipParaInt(enderecoRede);
    char enderecoAtual[16];

    fprintf(arquivo, "Faixas de subredes:\n");
    for (int i = 0; i < numSubRedes; i++) {
        unsigned int inicioSubrede = rede + i * (hostsPorSubrede + 2);
        unsigned int fimSubrede = inicioSubrede + hostsPorSubrede + 1;
        intParaIp(inicioSubrede, enderecoAtual);
        fprintf(arquivo, "Subrede %d: %s - ", i + 1, enderecoAtual);
        intParaIp(fimSubrede, enderecoAtual);
        fprintf(arquivo, "%s\n", enderecoAtual);
    }
}


