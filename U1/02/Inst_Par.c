#include <stdio.h>
#include <stdlib.h>
#include <time.h>


// Calcula tempo decorrido (em segundos com fração)
double tempo_decorrido(struct timespec inicio, struct timespec fim) {
    return (fim.tv_sec - inicio.tv_sec) + (fim.tv_nsec - inicio.tv_nsec) / 1e9;
}

int main() {
    
    srand(time(NULL));

    FILE* arquivo = fopen("temp.csv", "w");
    if (!arquivo) {
        printf("Erro ao abrir arquivo.\n");
        return 1;
    }

    struct timespec inicio, fim;
    int n=100000000;
    int soma,soma_a,soma_b,soma_c,soma_d=0;
    int* x = (int*)malloc(n * sizeof(int));
    double temp;

    fprintf(arquivo, "Inic_Vec,sum_norm,sum_div\n");
    // Excluir a primeira medição por cold cache  
    for (int i = 0; i <= 30; i++) {
        soma = soma_a = soma_b = soma_c = soma_d = 0;
        clock_gettime(CLOCK_MONOTONIC, &inicio);
        for (int i = 0; i < n; i++) {
            x[i] = rand() % 10;
        }
        clock_gettime(CLOCK_MONOTONIC, &fim);
        temp = tempo_decorrido(inicio, fim);
        fprintf(arquivo, "%.9f", temp);

        clock_gettime(CLOCK_MONOTONIC, &inicio);
        for (int i = 0; i < n; i++) {
            soma+=x[i];
        }
        clock_gettime(CLOCK_MONOTONIC, &fim);
        temp = tempo_decorrido(inicio, fim);
        fprintf(arquivo, ",%.9f", temp);

        clock_gettime(CLOCK_MONOTONIC, &inicio);
        for (int i = 0; i < n; i+=4) {
            soma_a+=x[i];
            soma_b+=x[i+1];
            soma_c+=x[i+2];
            soma_d+=x[i+3];
        }
        int res=soma_a+soma_b+soma_c+soma_d;
        clock_gettime(CLOCK_MONOTONIC, &fim);
        temp = tempo_decorrido(inicio, fim);
        fprintf(arquivo, ",%.9f\n", temp);
    }
    free(x);  
    fclose(arquivo);
    printf("Resultados salvos\n");
    return 0;
}