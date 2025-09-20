#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define n 100000000// Número de pontos para a simulação

///*
int main() {
    long long t_count = 0;
    long long count = 0;
    #pragma omp parallel default(none) shared(t_count) firstprivate(count)
    {
        srand(time(NULL)*(omp_get_thread_num()+1));
        
        #pragma omp for
        for (long long i = 0; i < n; i++) {
            // Gera dois números aleatórios entre 0 e 1
            double x = (double)rand() / RAND_MAX;
            double y = (double)rand() / RAND_MAX;
            
            // Verifica se o ponto está dentro do quarto de círculo
            if (x * x + y * y <= 1.0) {
                count++;
            }
        }

        #pragma omp critical
        t_count += count;
    }
    // Estima π
    double pi = 4.0 * t_count / n;

    printf("Estimativa de pi: %f\n", pi);

    return 0;
}
//*/
/*  Da errado por concorrencia (quanto mais threads mais concorrencia, maior chance de erros)
int main() {
    long long count = 0;
    
    srand(time(NULL));
    
    #pragma omp parallel for
    for (long long i = 0; i < n; i++) {
        // Gera dois números aleatórios entre 0 e 1
        double x = (double)rand() / RAND_MAX;
        double y = (double)rand() / RAND_MAX;
            
        // Verifica se o ponto está dentro do quarto de círculo
        if (x * x + y * y <= 1.0) {
            count++;
        }
    }
    
    // Estima π
    double pi = 4.0 * count / n;

    printf("Estimativa de pi: %f\n", pi);

    return 0;
}
//*/