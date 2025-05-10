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

// Better error handler
void die(char* message, int sp_err) // sp_err for special errors 
{
    if (sp_err == 1)
    {
	    perror("Usage: %s <lib_path> <pid>\n", argv[0]);
	    exit(EXIT_FAILURE);
    }
    perror(message);
    exit(EXIT_FAILURE);
}

int main(int argc, char** argv) 
{
    // Root check!
    if (geteuid() != 0) {
        die("[!] Please run this program with root privileges (by using the sudo command or directly running as root)\n");
    }

    if (argc < 3) {
        die(NULL, 1);
    }

    char lib_path[BUF_SIZE];
    strncpy(lib_path, argv[1], BUF_SIZE);
    int pid = atoi(argv[2]);

    // Lib path check
    if (access(lib_path, F_OK) == -1) {
        die("[!] Invalid library path");
    }

    // Pid check
    if (kill(pid, 0) == -1) {
        die("[!] Invalid pid");
    }

    // Attach to the target process
    if (ptrace(PTRACE_ATTACH, pid, 0, 0) == -1) {
        die("[!] Failed to attach to process");
    }

    // Wait for the process to stop
    waitpid(pid, NULL, 0);

    // Get the address of the dlopen function in the target process
    void* dlopen_addr = dlsym(RTLD_NEXT, "dlopen");
    if (dlopen_addr == NULL) {
        die("[!] Failed to locate dlopen function in target process");
    }

    printf("[*] Dlopen memory address: %p\n", dlopen_addr);

    // Allocate space in the target process for the path to the library
    void* lib_path_addr = (void*) calloc(BUF_SIZE, sizeof(char));
    if (lib_path_addr == NULL) {
        die("[!] Failed to allocate memory in target process");
    }

    // Write the library path to the allocated space in the target process
    strncpy((char*) lib_path_addr, lib_path, BUF_SIZE);
    printf("[*] Found library path: %s\n", (char*) lib_path_addr);

    // Get the register values from the target process
    struct user_regs_struct regs64;
    if (ptrace(PTRACE_GETREGS, pid, 0, &regs64) == -1) {
        die("[!] Failed to get register values from target process");
    }

    // Set the arguments for the dlopen function call in the target process
    regs64.rdi = (unsigned long long) lib_path_addr;
    regs64.rsi = RTLD_NOW;
    regs64.rip = (unsigned long long) dlopen_addr;

    // Set the register values in the target process
    if (ptrace(PTRACE_SETREGS, pid, 0, &regs64) == -1) {
        die("[!] Failed to set register values in target process");
    }

    // Continue execution of the target process
    if (ptrace(PTRACE_CONT, pid, 0, 0) == -1) {
        die("[!] Failed to continue target process");
    }

    // Wait for the process to stop again
    waitpid(pid, NULL, 0);
	
    // Detach from the target process
    if (ptrace(PTRACE_DETACH, pid, NULL, NULL) == -1) {
        die("[!] Failed to detach from target process");
    }

    printf("[*] Library injected successfully!\n");

    // Restore the target process's state
    if (kill(pid, SIGCONT) == -1) {
        printf("[!] Failed to resume the target process: %d\n", pid);
    } else {
        printf("[*] Resumed the target process: %d\n", pid);
    }

    return 0;
}
