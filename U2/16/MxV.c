#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char** argv) {
    int rank, size;
    int N = 100; // Ordem da matriz (quadrada)

    // Inicialização do MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Permitir passar a ordem da matriz por argumento (opcional)
    if (argc > 1) {
        N = atoi(argv[1]);
    }

    // Verifica se a ordem da matriz é divisível pelo número de processos
    if (N % size != 0) {
        if (rank == 0) {
            printf("Erro: N (%d) deve ser divisível por número de processos (%d)\n", N, size);
        }
        MPI_Finalize();
        return -1;
    }

    int local_rows = N / size;
    int* A = NULL;
    int* x = (int*) malloc(N * sizeof(int));
    int* y = NULL;

    if (rank == 0) {
        A = (int*) malloc(N * N * sizeof(int));
        y = (int*) malloc(N * sizeof(int));

        unsigned int seed = time(NULL);
        for (int i = 0; i < N * N; i++) {
            A[i] = rand_r(&seed) % 10;
        }
        for (int i = 0; i < N; i++) {
            x[i] = rand_r(&seed) % 10;
        }

    }

    // Aloca memória para porções locais
    int* local_A = (int*) malloc(local_rows * N * sizeof(int));
    int* local_y = (int*) malloc(local_rows * sizeof(int));

    MPI_Barrier(MPI_COMM_WORLD);
    double start_time = MPI_Wtime();

    // Distribui partes da matriz A
    MPI_Scatter(A, local_rows * N, MPI_INT,
                local_A, local_rows * N, MPI_INT,
                0, MPI_COMM_WORLD);

    // Distribui o vetor x
    MPI_Bcast(x, N, MPI_INT, 0, MPI_COMM_WORLD);

    // Cálculo local do produto
    for (int i = 0; i < local_rows; i++) {
        local_y[i] = 0;
        for (int j = 0; j < N; j++) {
            local_y[i] += local_A[i * N + j] * x[j];
        }
    }

    // Coleta resultados
    MPI_Gather(local_y, local_rows, MPI_INT,
               y, local_rows, MPI_INT,
               0, MPI_COMM_WORLD);

    double end_time = MPI_Wtime();

    if (rank == 0) {
        double elapsed_ms = (end_time - start_time) * 1000.0;

        // Abre arquivo CSV em modo append
        FILE* file = fopen("tempos_execucao.csv", "a");
        if (file == NULL) {
            fprintf(stderr, "Erro ao abrir o arquivo CSV.\n");
        } else {
            fprintf(file, "%d,%.6f\n", N, elapsed_ms);
            fclose(file);
        }

        free(A);
        free(y);
    }

    // Liberação de memória
    free(local_A);
    free(local_y);
    free(x);

    MPI_Finalize();
    return 0;
}