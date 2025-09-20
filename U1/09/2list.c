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
    Node* lista1 = NULL;
    Node* lista2 = NULL;

    #pragma omp parallel
    {
        #pragma omp single
        {
            srand(time(NULL));
            for (int i = 0; i < NUM_INSERCOES; i++) {
                int lista_idx = rand() % 2;
                #pragma omp task firstprivate(lista_idx)
                {
                    if (lista_idx == 0) {
                        #pragma omp critical(lista1)
                        create_insert_node(&lista1, i);
                    } else {
                        #pragma omp critical(lista2)
                        create_insert_node(&lista2, i);
                    }
                }
            }
        }
        
    }

    // Contagem dos elementos
    int count1 = 0, count2 = 0;
    Node* temp;

    for (temp = lista1; temp != NULL; temp = temp->next) count1++;
    for (temp = lista2; temp != NULL; temp = temp->next) count2++;

    printf("Lista 1: %d Nós\n", count1);
    printf("Lista 2: %d Nós\n", count2);
    printf("Total inserções: %d\n", count1 + count2);

    // Liberação de memória
    free_list(lista1);
    free_list(lista2);

    return 0;
}
