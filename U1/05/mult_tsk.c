#include <stdio.h>
#include <stdbool.h>
#include <omp.h>
#include <math.h>

int eh_primo(int n) {
    if (n <= 1) { return false; }
    if (n <= 3) { return true; }
    if (n % 2 == 0 || n % 3 == 0) { return false; }
    int limit = sqrt(n);
    for (int i = 5; i <= limit; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) { return false; } // 6k +/- 1 , tal que k é um número natural
    }
    return true;
}

int cont_p_seq(int n) {
    int contador = 0;
    for (int i = 2; i <= n; i++) {
        if (eh_primo(i)){
            contador++;
        }
    }
    return contador;
}

int cont_p_par(int n) {
    int contador = 0;
    #pragma omp parallel for
    for (int i = 2; i <= n; i++) {
        if (eh_primo(i)){
            contador++;
        }
    }
    return contador;
}

int main() {
    
    FILE* arquivo = fopen("Times.csv", "w");
    if (!arquivo) {
        printf("Erro ao abrir arquivo.\n");
        return 1;
    }

    fprintf(arquivo, "N°,Tempo Sequencial(s),Tempo Paralelo(s),Erro\n"); 
    double start, end;
    int erro;
    for (int i=100; i<10000; i+=100) {
        start = omp_get_wtime();
        erro = cont_p_seq(i);
        end = omp_get_wtime();
        fprintf(arquivo, "%d,%.9f",i, end - start);
        start = omp_get_wtime();
        erro -= cont_p_par(i);
        end = omp_get_wtime();
        fprintf(arquivo, ",%.9f,%d\n", end - start, erro);
    }

    fclose(arquivo);
    printf("Resultados salvos\n");
    return 0;
}




//    #pragma omp parallel for reduction(+:contador) //clausula reduction equivalente a private + critical para salvar o resoltado das variaveis privadas numa "global" 
    