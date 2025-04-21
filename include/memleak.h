#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct MemNode {
    void* ptr;
    size_t size;
    const char* file;
    int line;
    struct MemNode* next;
} MemNode;

MemNode* head = NULL;

void add_mem(void* ptr, size_t size, const char* file, int line) {
    MemNode* new_node = (MemNode*)malloc(sizeof(MemNode));
    new_node->ptr = ptr;
    new_node->size = size;
    new_node->file = file;
    new_node->line = line;
    new_node->next = head;
    head = new_node;
}

void remove_mem(void* ptr) {
    MemNode *temp = head, *prev = NULL;
    while (temp != NULL) {
        if (temp->ptr == ptr) {
            if (prev == NULL) head = temp->next;
            else prev->next = temp->next;
            free(temp);
            return;
        }
        prev = temp;
        temp = temp->next;
    }
}

void report_leaks() {
    MemNode* temp = head;
    printf("\n=== Memory Leak Report ===\n");
    int leak_found = 0;
    while (temp != NULL) {
        printf("Leaked %zu bytes at %s:%d\n", temp->size, temp->file, temp->line);
        leak_found = 1;
        temp = temp->next;
    }
    if (!leak_found)
        printf("No memory leaks detected! 🎉\n");
    else
        printf("Memory leaks found! ⚠️\n");
}

void* my_malloc(size_t size, const char* file, int line) {
    void* ptr = malloc(size);
    add_mem(ptr, size, file, line);
    return ptr;
}

void my_free(void* ptr, const char* file, int line) {
    remove_mem(ptr);
    free(ptr);
}

#define malloc(size) my_malloc(size, __FILE__, __LINE__)
#define free(ptr) my_free(ptr, __FILE__, __LINE__)

