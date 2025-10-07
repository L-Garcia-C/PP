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

void update_inner(double* u, double* u_new, int local_n) {
    for (int i = 2; i <= local_n - 1; i++) {
        u_new[i] = u[i] + ALPHA * (u[i - 1] - 2 * u[i] + u[i + 1]);
    }
}

void update_border(double* u, double* u_new, int i) {
    u_new[i] = u[i] + ALPHA * (u[i - 1] - 2 * u[i] + u[i + 1]);
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
            int flag0 = 0, flag1 = 0;

            if (rank > 0) {
                MPI_Irecv(&u[0], 1, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, &reqs[0]);
                MPI_Isend(&u[1], 1, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, &reqs[1]);
            } else {
                reqs[0] = MPI_REQUEST_NULL;
                reqs[1] = MPI_REQUEST_NULL;
                flag0 = 1;
            }

            if (rank < size - 1) {
                MPI_Irecv(&u[local_n + 1], 1, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, &reqs[2]);
                MPI_Isend(&u[local_n], 1, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, &reqs[3]);
            } else {
                reqs[2] = MPI_REQUEST_NULL;
                reqs[3] = MPI_REQUEST_NULL;
                flag1 = 1;
            }

            // Atualiza região interna enquanto espera comunicação
            update_inner(u, u_new, local_n);

            // Espera bordas chegarem e atualiza
            if (!flag0) {
                MPI_Test(&reqs[0], &flag0, MPI_STATUS_IGNORE);
                if (flag0) update_border(u, u_new, 1);
                else MPI_Wait(&reqs[0], MPI_STATUS_IGNORE), update_border(u, u_new, 1);
            }

            if (!flag1) {
                MPI_Test(&reqs[2], &flag1, MPI_STATUS_IGNORE);
                if (flag1) update_border(u, u_new, local_n);
                else MPI_Wait(&reqs[2], MPI_STATUS_IGNORE), update_border(u, u_new, local_n);
            }

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
        FILE* fp = fopen("tempos_nonblocking_test.csv", "w");
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
        printf("Tempos salvos em 'tempos_nonblocking_test.csv'\n");
    }

    MPI_Finalize();
    return 0;
}