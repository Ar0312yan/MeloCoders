#include "mem_leak_detector.h"

int main() {
    atexit(report_leaks);  // Automatically call at program exit

    // Intentional memory leaks
    int* leak1 = malloc(sizeof(int) * 5);          // Leak #1
    char* leak2 = strdup("Memory Leak Example");   // Leak #2

    // Properly allocated and freed memory
    char* buffer = calloc(10, sizeof(char));
    free(buffer);

    // realloc test
    int* numbers = malloc(2 * sizeof(int));
    numbers = realloc(numbers, 5 * sizeof(int));
    free(numbers);

    // Double free test (intentional)
    char* temp = malloc(50);
    free(temp);
    free(temp);  // This should warn about double free

    // Invalid free test (intentional)
    int x;
    free(&x);  // This should warn about invalid free

    return 0;
}
