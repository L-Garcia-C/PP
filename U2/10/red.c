#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <omp.h>

#define n 100000000// Número de pontos para a simulação
int main() {
    double start, end;

    start = omp_get_wtime();

    long long count = 0;
    #pragma omp parallel reduction(+:count)
    {
        unsigned int seed = time(NULL) * (omp_get_thread_num() + 1);
        
        #pragma omp for
        for (long long i = 0; i < n; i++) {
            double x = (double)rand_r(&seed) / RAND_MAX;
            double y = (double)rand_r(&seed) / RAND_MAX;
            if (x * x + y * y <= 1.0) {
                count++;
            }
        }
    }
    // Estima π
    double pi = 4.0 * count / n;

    end = omp_get_wtime();
    printf( "%.9f\n", (end - start)*1000);

    return 0;
}