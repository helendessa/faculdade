#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "calculadora-ip.h"

int main() {

    char ipDecimal[16], ipBinario[40], mascaraBool[40], redeBinaria[40], broadcastBinario[40];
    char redeDecimal[16], broadcastDecimal[16];
    int mascara;

    printf("Insira o endereço IP (com pontos): ");
    fgets(ipDecimal, sizeof(ipDecimal), stdin);
    printf("Insira a mascara: ");
    scanf("%d", &mascara);
    //printf("ip lido: %s\n", ipDecimal);

    char classe = identificaClasse(ipDecimal);
    printf("Classe %c\n", classe);

    int numSubRedes = numeroSubRedes(classe, mascara);
    printf("%d subredes\n", numSubRedes);

    int hosts = hostsPorSubrede(mascara);
    printf("%d hosts por subrede\n", hosts);

    //decimalParaBinario(10, ipBinario);
    //printf("%s", ipBinario);

    //ipDecimalParaBinario(ipDecimal, ipBinario);
    //printf("%s\n", ipBinario);
    //char bin[] = "0011";
    //ipBinarioParaDecimal(ipBinario, ipDecimal);
    //printf("%s\n", ipDecimal);

    //mascaraBooleana(mascara, mascaraBool);
    //printf("%s\n", mascaraBool);

    ipDecimalParaBinario(ipDecimal, ipBinario);
    mascaraBooleana(mascara, mascaraBool);
    geraEnderecoRede(ipBinario, mascaraBool, redeBinaria);
    geraEnderecoBroadcast(ipBinario, mascaraBool, broadcastBinario);

    printf("IP de rede em binário: %s\n", redeBinaria);
    ipBinarioParaDecimal(redeBinaria, redeDecimal);
    printf("IP de rede em decimal: %s\n", redeDecimal);

    printf("IP de broadcast em binário: %s\n", broadcastBinario);
    ipBinarioParaDecimal(broadcastBinario, broadcastDecimal);
    printf("IP de broadcast em decimal: %s\n", broadcastDecimal);

    FILE *arquivo = fopen("faixas_enderecos.txt", "w");
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo");
        return 1;
    }

    listarFaixaEnderecoHost(redeDecimal, broadcastDecimal, arquivo);
    listarFaixasSubredes(redeDecimal, numSubRedes, hosts, arquivo);

    fclose(arquivo);

    printf("Lista com faixas de endereço de host e subredes no arquivo gerado em anexo.\n");

    return 0;
}
