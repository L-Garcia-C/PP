#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define MAX_MESSAGE_SIZE 4194304 // 4 MB
#define NUM_TROCAS 1000

int main(int argc, char** argv) {
    int rank, size;
    double start_time, end_time;
    int message_size;
    char *message;
    
    // Inicializa o MPI
    MPI_Init(&argc, &argv);
    
    // Obtem o número de processos e o rank do processo
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Verifica se há exatamente 2 processos
    if (size != 2) {
        if (rank == 0) {
            printf("Este programa deve ser executado com exatamente dois processos.\n");
        }
        MPI_Finalize();
        return 1;
    }

    // Abre o arquivo CSV para salvar os resultados
    FILE *file;
    if (rank == 0) {
        file = fopen("resultados.csv", "w");
        if (file == NULL) {
            printf("Erro ao abrir o arquivo para escrita!\n");
            MPI_Finalize();
            return 1;
        }
        // Escreve o cabeçalho do CSV
        fprintf(file, "Tamanho da Mensagem (bytes),Tempo (ms)\n");
    }

    // A variável message_size vai variar dependendo do loop
    for (message_size = 8; message_size <= MAX_MESSAGE_SIZE; message_size = (int)(message_size * 1.5)) {
        message = (char*) malloc(message_size * sizeof(char));

        // Preenche a mensagem com dados fictícios
        for (int i = 0; i < message_size; i++) {
            message[i] = 'A';
        }

        // Processos de envio e recepção
        if (rank == 0) {
            // O processo 0 envia a mensagem para o processo 1
            start_time = MPI_Wtime();
            for (int i = 0; i < NUM_TROCAS; i++) {
                MPI_Send(message, message_size, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
                MPI_Recv(message, message_size, MPI_CHAR, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            end_time = MPI_Wtime();

            double tempo_medio = (end_time - start_time) * 1000.0 / NUM_TROCAS;  // em milissegundos
            fprintf(file, "%d,%f\n", message_size, tempo_medio);
;
        } else if (rank == 1) {
            for (int i = 0; i < NUM_TROCAS; i++) {
                MPI_Recv(message, message_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Send(message, message_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
            }
        }

        // Libera a memória alocada para a mensagem
        free(message);
    }

    // Fecha o arquivo CSV
    if (rank == 0) {
        fclose(file);
    }

    // Finaliza o MPI
    MPI_Finalize();
    return 0;
}