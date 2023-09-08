#include "help.h"
#include <stdio.h>

void help(void)
{
    printf("Usage: hex [OPTION]... [FILE]...\n");
    printf("[option]\n");
    printf("  -?, --help\t\t\tDisplay this help and exit\n");
    printf("  -v, --version\t\t\tDisplay version information and exit\n");
    printf("  -h, \t\tSpecify input hex file\n");
    printf("  -s, \t\tSpecify input S-record file\n");
    printf("  [FILE]...\t\t\tSpecify input file\n");
}