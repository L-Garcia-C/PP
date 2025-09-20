#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define n 100000000  // 100 milhões

/* Limitado por CPU
int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Uso: %s <num_threads>\n", argv[0]);
        return 1;
    }

    int num_threads = atoi(argv[1]);
    omp_set_num_threads(num_threads);

    double *x = malloc(n * sizeof(double));
    double *y = malloc(n * sizeof(double));

    for (long i = 0; i < n; i++) {
        x[i] = (double)i / 1000.0;
    }

    double start = omp_get_wtime();
    #pragma omp parallel for
    for (long i = 0; i < n; i++) {
        y[i] = sin(x[i]) * cos(x[i]) + log(x[i] + 1.0) + sqrt(x[i]);
    }
    double end = omp_get_wtime();
    
    printf("Compute-bound | Threads: %d | Tempo: %.3f segundos\n", num_threads, end - start);

    free(x);
    free(y);
    return 0;
}
//*/
///*  Limitado por Memoria
int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Uso: %s <num_threads>\n", argv[0]);
        return 1;
    }

    int num_threads = atoi(argv[1]);
    omp_set_num_threads(num_threads);

    double *a = malloc(n * sizeof(double));
    double *b = malloc(n * sizeof(double));
    double *c = malloc(n * sizeof(double));

    for (long i = 0; i < n; i++) {
        a[i] = 1.0;
        b[i] = 2.0;
    }

    double start = omp_get_wtime();
    #pragma omp parallel for
    for (long i = 0; i < n; i++) {
        c[i] = a[i] + b[i];
    }
    double end = omp_get_wtime();

    printf("Memory-bound | Threads: %d | Tempo: %.3f segundos\n", num_threads, end - start);

    free(a);
    free(b);
    free(c);
    return 0;
}
//*/