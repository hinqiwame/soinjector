#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dlfcn.h>

#define BUF_SIZE 256
#define RTLD_NEXT ((void *) -1l)

// better error handler
void die(char* message) {
    perror(message);
    exit(EXIT_FAILURE);
}

int main(int argc, char** argv) {
    // sudo check!
    if (geteuid() != 0) {
        printf("[!] please run this program as sudo!\n");
        exit(EXIT_FAILURE);
    }

    if (argc < 3) {
        printf("usage: %s <lib_path> <pid>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char lib_path[BUF_SIZE];
    strncpy(lib_path, argv[1], BUF_SIZE);
    int pid = atoi(argv[2]);

    // attach to the target process
    if (ptrace(PTRACE_ATTACH, pid, 0, 0) == -1) {
        die("[!] failed to attach to process");
    }

    // wait for the process to stop
    waitpid(pid, NULL, 0);

    // get the address of the dlopen function in the target process
    void* dlopen_addr = dlsym(RTLD_NEXT, "dlopen");
    if (dlopen_addr == NULL) {
        die("[!] failed to locate dlopen function in target process");
    }
    printf("[*] dlopen memory address: %p\n", dlopen_addr);

    // allocate space in the target process for the path to the library
    void* lib_path_addr = (void*) calloc(BUF_SIZE, sizeof(char));
    if (lib_path_addr == NULL) {
        die("[!] failed to allocate memory in target process");
    }

    // write the library path to the allocated space in the target process
    strncpy((char*) lib_path_addr, lib_path, BUF_SIZE);
    printf("[*] found library path: %s\n", (char*) lib_path_addr);

    // get the register values from the target process
    struct user_regs_struct regs64;
    if (ptrace(PTRACE_GETREGS, pid, 0, &regs64) == -1) {
        die("[!] failed to get register values from target process");
    }

    // set the arguments for the dlopen function call in the target process
    regs64.rdi = (unsigned long long) lib_path_addr;
    regs64.rsi = RTLD_NOW;
    regs64.rip = (unsigned long long) dlopen_addr;

    // set the register values in the target process
    if (ptrace(PTRACE_SETREGS, pid, 0, &regs64) == -1) {
        die("[!] failed to set register values in target process");
    }

    // continue execution of the target process
    if (ptrace(PTRACE_CONT, pid, 0, 0) == -1) {
        die("[!] failed to continue target process");
    }

    // wait for the process to stop again
    waitpid(pid, NULL, 0);

    // detach from the target process
    if (ptrace(PTRACE_DETACH, pid, NULL, NULL) == -1) {
        die("[!] failed to detach from target process");
    }

    printf("[*] library injected successfully!\n");

    return 0;
}
