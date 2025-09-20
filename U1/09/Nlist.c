#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <time.h>

#define NUM_INSERCOES 10000

typedef struct Node {
    int value;
    struct Node* next;
} Node;

// Cria e insere novo nó no inicio (mais facil para não ter que administrar 2 ponteiros oara cada lista)
void create_insert_node(Node** head, const int value) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    new_node->value = value;
    new_node->next = *head;
    *head = new_node;
}

void free_list(Node* head) {
    Node* temp;
    while (head) {
        temp = head;
        head = head->next;
        free(temp);
    }
}
int main() {
    int num_listas;

    printf("Digite o número de listas: ");
    scanf("%d", &num_listas);

    if (num_listas <= 0) {
        printf("Número inválido de listas.\n");
        return 1;
    }

    Node** listas = malloc(num_listas * sizeof(Node*));
    omp_lock_t* locks = malloc(num_listas * sizeof(omp_lock_t));

    for (int i = 0; i < num_listas; i++) {
        listas[i] = NULL;
        omp_init_lock(&locks[i]);
    }
    
    #pragma omp parallel
    {
        #pragma omp single
        {
            srand(time(NULL));
            for (int i = 0; i < NUM_INSERCOES; i++) {
                int idx = rand() % num_listas;
                #pragma omp task firstprivate(idx)
                {
                    omp_set_lock(&locks[idx]);
                    create_insert_node(&listas[idx], i);
                    omp_unset_lock(&locks[idx]);
                }
            }
        }
        
    }

    // Contagem dos elementos
    int total = 0;
    for (int i = 0; i < num_listas; i++) {
        int count = 0;
        Node* temp = listas[i];
        while (temp) {
            count++;
            temp = temp->next;
        }
        printf("Lista %d: %d Nós\n", i, count);
        total += count;
    }

    printf("Total inserções: %d\n", total);

    // Liberação
    for (int i = 0; i < num_listas; i++) {
        omp_destroy_lock(&locks[i]);
        free_list(listas[i]);
    }

    free(locks);
    free(listas);

    return 0;
}
