#include <stdint.h>
#include "header/command-shell/command-shell.h"

int main(void) {
    char name[100] = "\ns0sis@OS-IF2230:";
    char cur_dir[100] = "";
    syscall(18,(uint32_t)2,(uint32_t)"",(uint32_t)true);

    syscall(7,0,0,0);
    
    while(true){
        char command[100] = "";
        int idx = 0;
        char *buf = '\0';

        syscall(21,(uint32_t)&cur_dir,0,0);

        puts(name, 0xA);
        puts(cur_dir, 0x9);
        puts("$ ", 0xF);

        while(*buf!='\n'){
            int idxOld = idx;
            syscall(4,(uint32_t)buf,(uint32_t)command,(uint32_t)&idx);
            if (*buf == '\b') {
                if(idx!=idxOld){
                    syscall(5, (uint32_t)"\b", 0xF, 0);
                }
            }else if(*buf!='\0' && *buf!='\n'){
                syscall(5,(uint32_t)buf,0xF,0);
            }
        }
        if(*buf=='\n'){
            syscall(17,0,0,0);
        }

        char *cmdtyped = '\0';
        getWord(command, 0, cmdtyped);
        puts("\n",0xF);

        if (strcmp(cmdtyped, "rm")) {
            rm(command);
        }
        else if (strcmp(cmdtyped, "cp")) {
            copyPath(command);
        }   
        else if (strcmp(cmdtyped, "mkdir")) {
            mkdir(command);
        }
        else if (strcmp(cmdtyped, "mv")) {
            mv(command);
        }
        else if (strcmp(cmdtyped,"find")) {
            findShell(command);
        }
        else if(strcmp(cmdtyped,"clear")) {
            syscall(8,0,0,0);
        }
        else if(strcmp(cmdtyped,"cd")) { 
            cd(command);
        }
        else if (strcmp(cmdtyped, "ls")) {
            ls();
        }
        else if(strcmp(cmdtyped,"cat")){
            cat(command);
        }
        else if(strcmp(cmdtyped,"exit")){
            break;
        }
        else if(strcmp(cmdtyped,"ps")){
            ps();
        }
        else if(strcmp(cmdtyped,"kill")){
            kill(command);
        }
        else if(strcmp(cmdtyped,"exec")){
            exec(command);
        }
        else {
            puts(cmdtyped, 0x07);
            puts(": command not found\n", 0x07);
        }
        int cur_cluster = 2;
        syscall(19,(uint32_t)&cur_cluster,0,0);
        syscall(18,(uint32_t)cur_cluster,(uint32_t)"",(uint32_t)true);
    }

    return 0;
}