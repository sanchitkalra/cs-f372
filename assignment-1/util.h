#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <unistd.h>

FILE* openMenu() {
    FILE* fptr;
    fptr = fopen("menu.txt", "r");
    char fileContent[1024];
    return fptr;
}
