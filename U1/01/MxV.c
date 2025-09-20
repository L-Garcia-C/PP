#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_SIZE 5000
#define STEP_SIZE 25

// Aloca matriz n x n
int** aloca_matriz(int n) {
    int** mat = (int**)malloc(n * sizeof(int*));
    for (int i = 0; i < n; i++) {
        mat[i] = (int*)malloc(n * sizeof(int));
    }
    return mat;
}

// Libera matriz
void libera_matriz(int** mat, int n) {
    for (int i = 0; i < n; i++) {
        free(mat[i]);
    }
    free(mat);
}

// Multiplicação MxV - acesso por linhas
void mxv_linhas(int** A, int* x, int* y, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            y[i] += A[i][j] * x[j];
        }
    }
}

// Multiplicação MxV - acesso por colunas
void mxv_colunas(int** A, int* x, int* y, int n) {
    for (int j = 0; j < n; j++) {
        for (int i = 0; i < n; i++) {
            y[i] += A[i][j] * x[j];
        }
    }
}

// Inicializa matriz e vetor com inteiros aleatórios
void inicializa_dados(int** A, int* x, int n) {
    for (int i = 0; i < n; i++) {
        x[i] = rand() % 10;
        for (int j = 0; j < n; j++) {
            A[i][j] = rand() % 10;
        }
    }
}

// Calcula tempo decorrido (em segundos com fração)
double tempo_decorrido(struct timespec inicio, struct timespec fim) {
    return (fim.tv_sec - inicio.tv_sec) + (fim.tv_nsec - inicio.tv_nsec) / 1e9;
}

// Zerar vetor
void zerar_vetor(int* y, int n) {
    for (int i = 0; i < n; i++) {
        y[i] = 0;
    }
}

int main() {
    srand(time(NULL));

    FILE* arquivo = fopen("temp.csv", "w");
    if (!arquivo) {
        printf("Erro ao abrir arquivo.\n");
        return 1;
    }

    fprintf(arquivo, "Tamanho,Tempos_Linhas(s),Tempos_Colunas(s)\n");

    for (int n = 0; n <= MAX_SIZE; n += STEP_SIZE) {
        int** A = aloca_matriz(n);
        int* x = (int*)malloc(n * sizeof(int));
        int* y = (int*)malloc(n * sizeof(int));

        inicializa_dados(A, x, n);

        struct timespec inicio, fim;
        double tempo_linha, tempo_coluna;

        fprintf(arquivo, "%d", n);


        // Tempos - acesso por linhas
        for (int i = 0; i < 30; i++) {
            zerar_vetor(y, n);
            clock_gettime(CLOCK_MONOTONIC, &inicio);
            mxv_linhas(A, x, y, n);
            clock_gettime(CLOCK_MONOTONIC, &fim);
            tempo_linha = tempo_decorrido(inicio, fim);
            fprintf(arquivo, ",%.9f", tempo_linha);
        }

        // Tempos - acesso por colunas
        for (int i = 0; i < 30; i++) {
            zerar_vetor(y, n);
            clock_gettime(CLOCK_MONOTONIC, &inicio);
            mxv_colunas(A, x, y, n);
            clock_gettime(CLOCK_MONOTONIC, &fim);
            tempo_coluna = tempo_decorrido(inicio, fim);
            fprintf(arquivo, ",%.9f", tempo_coluna);
        }

        fprintf(arquivo, "\n");

        libera_matriz(A, n);
        free(x);
        free(y);
    }

    fclose(arquivo);
    printf("\nResultados salvos em 'temp.csv'\n");
    return 0;
}