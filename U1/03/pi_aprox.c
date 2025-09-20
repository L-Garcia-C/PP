#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#define Tamanho 10000000000

// Série de Leibniz
double pi_aprox(long n) {
    double pi = 0.0;
    for (int i = 0; i < n; i++) {
        double termo = (i % 2 == 0 ? 1.0 : -1.0) / (2.0 * i + 1.0);
        pi += termo;
    }
    return pi * 4.0;
}

// Calcula tempo decorrido (em segundos com fração)
double tempo_decorrido(struct timespec inicio, struct timespec fim) {
    return (fim.tv_sec - inicio.tv_sec) + (fim.tv_nsec - inicio.tv_nsec) / 1e9;
}

int main() {
    
    srand(time(NULL));

    FILE* arquivo = fopen("Aprox.csv", "w");
    if (!arquivo) {
        printf("Erro ao abrir arquivo.\n");
        return 1;
    }

    fprintf(arquivo, "Termos,Aproximacao,Erro,Tempo(s)\n");
    
    struct timespec inicio, fim;
    double temp;

    
    for (long n = 1; n <= Tamanho; n *= 10) { // escala logarítmica
        clock_gettime(CLOCK_MONOTONIC, &inicio);
        double pi = pi_aprox(n);
        clock_gettime(CLOCK_MONOTONIC, &fim);

        double erro = fabs(pi - M_PI);
        temp = tempo_decorrido(inicio, fim);

        fprintf(arquivo, "%ld,%.15f,%.15f,%.6f\n", n, pi, erro, temp);
    }
    


    fclose(arquivo);
    printf("Resultados salvos em Aprox.csv\n");
    
    return 0;
}