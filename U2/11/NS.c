#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define NX 64
#define NY 64
#define NZ 64
#define n 5000
#define DX 0.01
#define DY 0.01
#define DZ 0.01
#define DT 0.0001
#define NU 0.1

double u[NX][NY][NZ], u_new[NX][NY][NZ];

void inicializar(){
    for (int i = 0; i < NX; i++) {
        for (int j = 0; j < NY; j++) {
            for (int k = 0; k < NZ; ++k){
                u[i][j][k] = 0.0;
            }
        }
    }


    // Perturbação no centro
    u[NX/2][NY/2][NZ/2] = 1.0;
}

void update() {
    #pragma omp parallel for collapse(3)
    for (int i = 1; i < NX - 1; ++i){
        for (int j = 1; j < NY - 1; ++j){
            for (int k = 1; k < NZ - 1; ++k) {
                u_new[i][j][k] = u[i][j][k] + NU * DT * (
                    (u[i+1][j][k] - 2*u[i][j][k] + u[i-1][j][k]) / (DX*DX) +
                    (u[i][j+1][k] - 2*u[i][j][k] + u[i][j-1][k]) / (DY*DY) +
                    (u[i][j][k+1] - 2*u[i][j][k] + u[i][j][k-1]) / (DZ*DZ)
                );
            }
        }
    }
    
    #pragma omp parallel for collapse(3)
    for (int i = 1; i < NX - 1; ++i){
        for (int j = 1; j < NY - 1; ++j){
            for (int k = 1; k < NZ - 1; ++k){
                u[i][j][k] = u_new[i][j][k];
            }
        }
    }
}

int main() {
    inicializar();

    double start = omp_get_wtime();
    for (int t = 0; t < n; ++t)
        update();
    double end = omp_get_wtime();

    printf("Simulação concluída em %.9f segundos\n", end - start);

    // Exporta uma fatia do centro para visualização (por exemplo, plano Z = NZ/2)
    FILE *f = fopen("slice_xy.csv", "w");
    for (int i = 0; i < NX; ++i) {
        for (int j = 0; j < NY; ++j)
            fprintf(f, "%f,", u[i][j][NZ/2]);
        fprintf(f, "\n");
    }
    fclose(f);

    return 0;
}