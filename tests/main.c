#include <stdio.h>

unsigned long __passed_tests;
unsigned long __failed_tests;

int main(void)
{
    printf("\npassed tests: %lu\nfailed tests: %lu\ntotal:        %lu\n",
        __passed_tests, __failed_tests, __passed_tests + __failed_tests);

    return 0;
}
