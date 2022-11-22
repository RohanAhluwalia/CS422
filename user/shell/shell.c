#include "shell.h"
#include <gcc.h>
#include <stdio.h>
#include <syscall.h>
#include <x86.h>


 int main(int argc, char *argv[])
 {
    char line[100];
     while (1) {
        readline(line);
     }
 }