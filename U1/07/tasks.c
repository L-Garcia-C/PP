#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#define NUM_FILES 100

// Estrutura do nó da lista encadeada
typedef struct Node {
    char filename[50];
    struct Node* next;
} Node;

// Função para criar um novo nó
Node* create_node(const char* filename) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    strcpy(new_node->filename, filename);
    new_node->next = NULL;
    return new_node;
}

// Função para liberar a lista
void free_list(Node* head) {
    Node* temp;
    while (head) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

int main() {
    // Criação da lista encadeada
    Node* head = NULL;
    Node* tail = NULL;
    char fname[50];

    for (int i = 0; i < NUM_FILES; i++) {
        sprintf(fname, "arquivo_%02d.txt", i);
        Node* new_node = create_node(fname);
        if (head == NULL) {
            head = new_node;
            tail = head;
        } else {
            tail->next = new_node;
            tail = new_node;
        }
    }

    // Região paralela com tarefas
    #pragma omp parallel
    {
        #pragma omp single
        {
            Node* current = head;
            while (current != NULL) {
                Node* node = current; // evitar race condition
                #pragma omp task firstprivate(node)
                {
                    printf("Arquivo: %s - Thread: %d\n", node->filename, omp_get_thread_num());
                }
                current = current->next;
            }
        }
    }

    // Liberação da lista
    free_list(head);

    return 0;
}