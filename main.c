#include "mem_leak_detector.h"

int main() {
    atexit(report_leaks);  // Call at program exit

    int* a = malloc(sizeof(int) * 5); // Not freed (intentional leak)
    char* b = malloc(20);
    free(b);

    return 0;
}

