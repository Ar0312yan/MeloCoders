#ifndef MEM_LEAK_DETECTOR_H
#define MEM_LEAK_DETECTOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define RESET   "\033[0m"

typedef struct MemNode {
    void* ptr;
    size_t size;
    const char* file;
    int line;
    int is_freed;
    struct MemNode* next;
} MemNode;

static MemNode* head = NULL;
static size_t total_allocated = 0;
static size_t total_freed = 0;
FILE* log_file;

void add_mem(void* ptr, size_t size, const char* file, int line) {
    MemNode* new_node = (MemNode*)malloc(sizeof(MemNode));
    new_node->ptr = ptr;
    new_node->size = size;
    new_node->file = file;
    new_node->line = line;
    new_node->is_freed = 0;
    new_node->next = head;
    head = new_node;
    total_allocated += size;
}

MemNode* find_mem(void* ptr) {
    MemNode* temp = head;
    while (temp != NULL) {
        if (temp->ptr == ptr) return temp;
        temp = temp->next;
    }
    return NULL;
}

void my_free(void* ptr, const char* file, int line) {
    MemNode* node = find_mem(ptr);
    if (node == NULL) {
        printf(YELLOW "[Warning] Attempt to free untracked pointer at %s:%d\n" RESET, file, line);
        return;
    }
    if (node->is_freed) {
        printf(YELLOW "[Warning] Double free detected at %s:%d (originally freed at %s:%d)\n" RESET,
               file, line, node->file, node->line);
        return;
    }
    node->is_freed = 1;
    total_freed += node->size;
    free(ptr);
}

void* my_malloc(size_t size, const char* file, int line) {
    void* ptr = malloc(size);
    add_mem(ptr, size, file, line);
    return ptr;
}

void* my_calloc(size_t n, size_t size, const char* file, int line) {
    void* ptr = calloc(n, size);
    add_mem(ptr, n * size, file, line);
    return ptr;
}

void* my_realloc(void* ptr, size_t size, const char* file, int line) {
    MemNode* node = find_mem(ptr);
    if (node != NULL && !node->is_freed) {
        total_freed += node->size;
        node->is_freed = 1;
    }
    void* new_ptr = realloc(ptr, size);
    add_mem(new_ptr, size, file, line);
    return new_ptr;
}

char* my_strdup(const char* s, const char* file, int line) {
    char* ptr = strdup(s);
    add_mem(ptr, strlen(s) + 1, file, line);
    return ptr;
}

void report_leaks() {
    MemNode* temp = head;
    int leak_found = 0;
    log_file = fopen("memleak_report.txt", "w");
    fprintf(log_file, "{\"leaks\": [\n");

    while (temp != NULL) {
        if (!temp->is_freed) {
            printf(RED "Leak: %zu bytes from %s:%d\n" RESET, temp->size, temp->file, temp->line);
            fprintf(log_file, "  {\"file\": \"%s\", \"line\": %d, \"size\": %zu},\n",
                    temp->file, temp->line, temp->size);
            leak_found = 1;
        }
        temp = temp->next;
    }

    fprintf(log_file, "]}\n");
    fclose(log_file);

    printf(GREEN "Total allocated: %zu bytes\n" RESET, total_allocated);
    printf(GREEN "Total freed:     %zu bytes\n" RESET, total_freed);
    printf(RED   "Memory leaked:   %zu bytes\n" RESET, total_allocated - total_freed);

    if (!leak_found)
        printf(GREEN "No memory leaks detected! 🎉\n" RESET);
    else
        printf(RED "Memory leaks found! ⚠️\n" RESET);
}

#define malloc(size)        my_malloc(size, __FILE__, __LINE__)
#define calloc(n, size)     my_calloc(n, size, __FILE__, __LINE__)
#define realloc(ptr, size)  my_realloc(ptr, size, __FILE__, __LINE__)
#define strdup(s)           my_strdup(s, __FILE__, __LINE__)
#define free(ptr)           my_free(ptr, __FILE__, __LINE__)

#endif // MEM_LEAK_DETECTOR_H


