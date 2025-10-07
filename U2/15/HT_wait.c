#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define N 5000
#define STEPS 500
#define REPEATS 10
const double ALPHA = 0.1;

void initialize(double* u, int local_n) {
    for (int i = 0; i < local_n + 2; i++) u[i] = 0.0;
    u[local_n / 2 + 1] = 100.0;
}

void update(double* u, double* u_new, int local_n) {
    for (int i = 1; i <= local_n; i++) {
        u_new[i] = u[i] + ALPHA * (u[i - 1] - 2 * u[i] + u[i + 1]);
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int local_n = N / size;
    double times_ms[REPEATS];

    for (int r = 0; r < REPEATS; r++) {
        double* u = malloc((local_n + 2) * sizeof(double));
        double* u_new = malloc((local_n + 2) * sizeof(double));
        initialize(u, local_n);

        double start = MPI_Wtime();

        for (int t = 0; t < STEPS; t++) {
            MPI_Request reqs[4];

            // Comunicação não bloqueante
            if (rank > 0) {
                MPI_Irecv(&u[0], 1, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, &reqs[0]);
                MPI_Isend(&u[1], 1, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, &reqs[1]);
            } else {
                reqs[0] = MPI_REQUEST_NULL;
                reqs[1] = MPI_REQUEST_NULL;
            }

            if (rank < size - 1) {
                MPI_Irecv(&u[local_n + 1], 1, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, &reqs[2]);
                MPI_Isend(&u[local_n], 1, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, &reqs[3]);
            } else {
                reqs[2] = MPI_REQUEST_NULL;
                reqs[3] = MPI_REQUEST_NULL;
            }

            MPI_Waitall(4, reqs, MPI_STATUSES_IGNORE);
            update(u, u_new, local_n);

            double* tmp = u;
            u = u_new;
            u_new = tmp;
        }

        double end = MPI_Wtime();
        double elapsed_ms = (end - start) * 1000.0;

        double max_elapsed_ms;
        MPI_Reduce(&elapsed_ms, &max_elapsed_ms, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

        if (rank == 0) times_ms[r] = max_elapsed_ms;

        free(u);
        free(u_new);
    }

    if (rank == 0) {
        FILE* fp = fopen("tempos_nonblocking_wait.csv", "w");
        if (fp == NULL) {
            perror("Erro ao criar arquivo CSV");
            MPI_Finalize();
            return 1;
        }
        fprintf(fp, "Tempo_ms\n");
        for (int i = 0; i < REPEATS; i++) {
            fprintf(fp, "%.6f\n", times_ms[i]);
        }
        fclose(fp);
        printf("Tempos salvos em 'tempos_nonblocking_wait.csv'\n");
    }

    MPI_Finalize();
    return 0;
}