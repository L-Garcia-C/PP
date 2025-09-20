#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define N 10000000  // Número de pontos para a simulação

int main() {
    double start, end;

    long long t_count = 0;
    int nthreads;
    long long *counts;

    start = omp_get_wtime();
    #pragma omp parallel
    {
        int tid = omp_get_thread_num();

        // Apenas uma thread aloca o vetor
        #pragma omp single
        {
            nthreads = omp_get_num_threads();
            counts = (long long*)calloc(nthreads, sizeof(long long));
        }

        srand(time(NULL)*(omp_get_thread_num()+1));

        #pragma omp for
        for (long long i = 0; i < N; i++) {
            double x = (double)rand() / RAND_MAX;
            double y = (double)rand() / RAND_MAX;
            if (x * x + y * y <= 1.0) {
                counts[tid]++; // cada thread escreve na sua posição
            }
        }
    }

    // soma serial
    for (int i = 0; i < nthreads; i++) {
        t_count += counts[i];
    }

    double pi = 4.0 * t_count / (double)N;
    end = omp_get_wtime();

    printf("%.9f\n", (end - start)*1000);
    
    free(counts);
    return 0;
}
