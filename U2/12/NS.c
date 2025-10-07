#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define n 10000
#define DX 0.01
#define DY 0.01
#define DZ 0.01
#define DT 0.0001
#define NU 0.1

int order;
double ***u, ***u_new;

// Função para alocar matriz 3D dinamicamente
double ***aloca3D(int nd) {
    double ***array = (double ***)malloc(nd * sizeof(double **));
    for (int i = 0; i < nd; ++i) {
        array[i] = (double **)malloc(nd * sizeof(double *));
        for (int j = 0; j < nd; ++j) {
            array[i][j] = (double *)malloc(nd * sizeof(double));
        }
    }
    return array;
}

// Liberação da matriz 3D
void libera3D(double ***array, int nd) {
    for (int i = 0; i < nd; ++i) {
        for (int j = 0; j < nd; ++j) {
            free(array[i][j]);
        }
        free(array[i]);
    }
    free(array);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <order>\n", argv[0]);
        return 1;
    }

    int size = atoi(argv[1]);
    if (size < 3) {
        fprintf(stderr, "Erro: o tamanho deve ser >= 3 para permitir vizinhança\n");
        return 1;
    }

    order = size;

    // Alocação das matrizes
    u = aloca3D(order);
    u_new = aloca3D(order);

    // Inicialização
    #pragma omp parallel for collapse(3)
    for (int i = 0; i < order; i++) {
        for (int j = 0; j < order; j++) {
            for (int k = 0; k < order; ++k){
                u[i][j][k] = 0.0;
            }
        }
    }

    // Perturbação no centro
    u[order/2][order/2][order/2] = 1.0;

    double start = omp_get_wtime();
    
    for (int t = 0; t < n; ++t){
        #pragma omp parallel for collapse(3) schedule(guided, 20)
        for (int i = 1; i < order - 1; ++i){
            for (int j = 1; j < order - 1; ++j){
                for (int k = 1; k < order - 1; ++k) {
                    u_new[i][j][k] = u[i][j][k] + NU * DT * (
                        (u[i+1][j][k] - 2*u[i][j][k] + u[i-1][j][k]) / (DX*DX) +
                        (u[i][j+1][k] - 2*u[i][j][k] + u[i][j-1][k]) / (DY*DY) +
                        (u[i][j][k+1] - 2*u[i][j][k] + u[i][j][k-1]) / (DZ*DZ)
                    );
                }
            }
        }

       // Atualização da solução
        #pragma omp parallel for collapse(3) schedule(guided, 20)
        for (int i = 1; i < order - 1; ++i){
            for (int j = 1; j < order - 1; ++j){
                for (int k = 1; k < order - 1; ++k){
                    u[i][j][k] = u_new[i][j][k];
                }
            }
        }
    }
    double end = omp_get_wtime();

    printf("Simulação concluída em %.9f segundos\n", end - start);

    // Liberação da memória
    libera3D(u, order);
    libera3D(u_new, order);

    return 0;
}
