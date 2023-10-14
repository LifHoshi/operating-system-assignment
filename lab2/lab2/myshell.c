/**
 * CS2106 AY22/23 Semester 2 - Lab 2
 *
 * This file contains function definitions. Your implementation should go in
 * this file.
 */

#include "myshell.h"

#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define EXITED 1
#define RUNNING 2
#define TERMINATING 3
#define STOPPED 4

const char *status[] = {NULL, "Exited", "Running", "Terminating", "Stopped"};
void pcb_update();
struct PCBTable pcbArray[50];
int pcb_cnt = 0;
int parentPid;
bool isWaiting;


void pcb_update()
{
    pid_t pid;
    pid_t exitCode;
    if ((pid = waitpid(-1, &exitCode, WNOHANG)) > 0)
    {
        for (int i = 0; i < pcb_cnt; i++)
        {
            if (pcbArray[i].pid == pid)
            {
                if (pcbArray[i].status == 3)
                {
                    pcbArray[i].exitCode = WTERMSIG(exitCode);
                }
                else
                {
                    pcbArray[i].exitCode = WEXITSTATUS(exitCode);
                }
                pcbArray[i].status = 1;
                break;
            }
        }
    }
}

/*******************************************************************************
 * Signal handler : ex4
 ******************************************************************************/

static void proc_update_status(pid_t pid, int status, int exitCode)
{
    for (int i = 0; i < pcb_cnt; i++)
    {
        if (pcbArray[i].pid == pid)
        {
            pcbArray[i].status = status;
            pcbArray[i].exitCode = exitCode;
        }
    }
}

static void signal_handler(int signo)
{
    //printf("Enter the this block");
    if (signo == SIGINT && getpid() == parentPid && isWaiting)
    {   
        //printf("Enter the this block for ctrlc")
        printf("[%d] interrupted\n", pcbArray[pcb_cnt].pid);
        pid_t pid = pcbArray[pcb_cnt].pid;
        if (pcbArray[pcb_cnt].status == RUNNING)
        {
            kill(pid, SIGINT);
        }
    }
    else if (signo == SIGTSTP && getpid() == parentPid && isWaiting)
    {
        printf("[%d] stopped\n", pcbArray[pcb_cnt - 1].pid);
        pid_t pid2 = pcbArray[pcb_cnt - 1].pid;
        if (pcbArray[pcb_cnt - 1].status == RUNNING)
        {
            kill(pid2, SIGTSTP);
        }

        proc_update_status(pid2, STOPPED, -1);
    }
}


/*******************************************************************************
 * Built-in Commands
 ******************************************************************************/

static void command_info(char *number)
{
    int option = atoi(number);

    if (option == 0)
    {
        for (int i = 0; i < pcb_cnt; i++)
        {
            if (pcbArray[i].status != 1)
            {
                printf("[%d] %s\n", pcbArray[i].pid, status[pcbArray[i].status]);
            }
            else
            {
                printf("[%d] %s %d\n", pcbArray[i].pid, status[pcbArray[i].status], pcbArray[i].exitCode);
            }
        }
    }
    else if (option == 1)
    {
        int exited_cnt = 0;
        for (int i = 0; i < pcb_cnt; i++)
        {
            if (pcbArray[i].status == 1)
            {
                exited_cnt++;
            }
        }
        printf("Total exited processes: %d", exited_cnt);
        printf("\n");
    }
    else if (option == 2)
    {
        int running_cnt = 0;
        for (int i = 0; i < pcb_cnt; i++)
        {
            if (pcbArray[i].status == RUNNING)
            {
                running_cnt++;
            }
        }
        printf("Total running processes: %d", running_cnt);
        printf("\n");
    }
    else if (option == 3)
    {
        int terminating_cnt = 0;
        for (int i = 0; i < MAX_PROCESSES; i++)
        {
            if (pcbArray[i].status == 3)
            {
                terminating_cnt++;
            }
        }
        // printf("number of pcb_cnt: %d", pcb_cnt);
        printf("Total terminating processes: %d", terminating_cnt);
        printf("\n");
    }
    else if (option == 4)
    {
        int stopped_cnt = 0;
        for (int i = 0; i < pcb_cnt; i++)
        {
            if (pcbArray[i].status == STOPPED)
            {
                stopped_cnt++;
            }
        }
        printf("Total stopped processes: %d", stopped_cnt);
        printf("\n");
    }
    else
    {
        fprintf(stderr, "Wrong command\n");
    }
}

static void command_wait(char *number)
{

    int pidwait = atoi(number);

    for (int i = 0; i < pcb_cnt && pcbArray[i].pid != -1; i++)
    {
        if (pcbArray[i].pid == pidwait)
        {
            if (pcbArray[i].status == RUNNING)
            {
                int exitcode;
                isWaiting = true;
                pid_t pid = waitpid(pidwait, &exitcode, WUNTRACED);
                isWaiting = false;
                if (pid > 0 && WIFEXITED(exitcode))
                {
                    // pcbArray[i].exitCode = WEXITSTATUS(exitcode);
                    int exit_code = WEXITSTATUS(exitcode); // for update function
                    proc_update_status(pidwait, EXITED, exit_code);
                }
            }
        }

        /******* FILL IN THE CODE *******/
    }
    // Find the {PID} in the PCBTable
    // If the process indicated by the process id is RUNNING, wait for it (can use waitpid()).
    // After the process terminate, update status and exit code (call proc_update_status())
    // Else, continue accepting user commands.
}

static void command_terminate(char **tokens)
{
    pid_t pidkill = atoi(tokens[1]);
    for (int i = 0; i < pcb_cnt; i++)
    {
        if (pidkill == pcbArray[i].pid)
        {
            if (pcbArray[i].status == RUNNING)
            {
                kill(pcbArray[i].pid, SIGTERM);
                pcbArray[i].status = 3;
                proc_update_status(pidkill, TERMINATING, -1);
            }
            break;
        }
    }
}

static void command_fg(char *number)
{
    pid_t fgPid = atoi(number);
    for (int i = 0; i < pcb_cnt; i++)
    {
        if (pcbArray[i].pid == fgPid)
        {
            if (pcbArray[i].status == STOPPED)
            {
                isWaiting = true;
                printf("[%d] resumed\n", fgPid);
                proc_update_status(fgPid, RUNNING, -1);
                kill(fgPid, SIGCONT);
            }
        }
    }
}

/*******************************************************************************
 * Program Execution
 ******************************************************************************/

static void command_exec(char *command, char **process, int token_change, char **tokens, size_t num_tokens)
{
    int index = 0;
    if (access(command, F_OK) != 0)
    {
        printf("%s not found\n", tokens[0]);
        return;
    }
    // fork a subprocess and execute the program

    if (strcmp(process[token_change - 2], "&") == 0)
    {
        index = 1;
        process[token_change - 2] = NULL;
    }

    int file_in = 0;
    int file_out = 0;
    int file_err = 0;
    for (int i = 0; i < token_change; i++)
    { // don't know if need type wrap
        if (process[i])
        {
            if (strcmp(process[i], "<") == 0)
            {
                file_in = i + 1;
            }
            else if (strcmp(process[i], ">") == 0)
            {
                file_out = i + 1;
            }
            else if (strcmp(process[i], "2>") == 0)
            {
                file_err = i + 1;
            }
        }
    }

    pid_t childpid;
    if ((childpid = fork()) == 0)
    {
        if (file_err != 0)
        {
            FILE *file;
            file = fopen(process[file_err], "w");
            dup2(fileno(file), 2);
            fclose(file);
            process[file_err - 1] = NULL;
        }
        if (file_out != 0)
        {
            FILE *file;
            file = fopen(process[file_out], "w");
            dup2(fileno(file), 1);
            fclose(file);
            process[file_out - 1] = NULL;
        }
        if (file_in != 0)
        {
            FILE *file;
            file = fopen(process[file_in], "r");
            if (file == NULL)
            {
                // fprintf(stderr, "%s does not exist\n", to_proc[file_in]);
                printf("%s does not exist\n", process[file_in]);
                exit(1);
            }
            dup2(fileno(file), 0);
            fclose(file);
            process[file_in - 1] = NULL;
        }
        execv(command, process);
    }
    else
    {
        struct PCBTable *child = &pcbArray[pcb_cnt];
        child->pid = childpid;
        child->status = 2;
        child->exitCode = -1;

        int result;

        if (index == 0)
        {   
            isWaiting = true;
            waitpid(childpid, &result, WUNTRACED);
            isWaiting = false;
            child->exitCode = result >> 8;
            child->status = 1;
        }
        else
        {
            printf("Child [%d] in background\n", child->pid);
            //waitpid(childpid, NULL, WNOHANG)
        }

        pcb_cnt++;
        tokens = &tokens[token_change];
        if (num_tokens != 0)
        {
            my_process_command(num_tokens, tokens);
        }
        return;
    }
}

/*******************************************************************************
 * Command Processor
 ******************************************************************************/

static void command(char **process, int token_change, char **tokens, size_t num_tokens)
{
    char *command = process[0];
    char *number = process[1];
    // if command is "info" call command_info()             : ex1
    if (strcmp(command, "info") == 0)
    {
        if (number == NULL)
        {
            printf("Wrong command\n");
            return;
        }
        else
        {
            command_info(number);
            return;
        }
    }
    // if command is "wait" call command_wait()             : ex2
    if (strcmp(command, "wait") == 0)
    {
        if (number == NULL)
        {
            printf("Wrong command\n");
            return;
        }
        else
        {
            command_wait(number);
            return;
        }
    }
    // if command is "terminate" call command_terminate()   : ex2
    if (strcmp(command, "terminate") == 0)
    {
        if (number == NULL)
        {
            printf("Wrong command\n");
            return;
        }
        else
        {
            command_terminate(tokens);
            return;
        }
    }
    if (strcmp(command, "fg") == 0)
    {
        if (number == NULL)
        {
            printf("Wrong command\n");
            return;
        }
        else
        {
            command_fg(number);
            return;
        }
    }

    command_exec(command, process, token_change, tokens, num_tokens); // for all other commands           : ex1, ex2, ex3
}

/*******************************************************************************
 * High-level Procedure
 ******************************************************************************/

void my_init(void)
{
    pcb_cnt = 0;
    parentPid = getpid();
    isWaiting = false;
    signal(SIGTSTP, signal_handler);
    signal(SIGINT, signal_handler);
    signal(SIGCHLD, pcb_update);
    
}

void my_process_command(size_t num_tokens, char **tokens)
{
    size_t token_change = num_tokens;
    // size_t tokenIndex = num_tokens;
    size_t i = 0;
    char **process = tokens;

    for (i = 0; i < num_tokens; i++)
    {
        if (tokens[i])
        {
            if (strcmp(tokens[i], ";") == 0)
            {
                token_change = i + 1;
                break;
            }
        }
    }

    /*
    while (tokenIndex < num_tokens)
    {
        if (tokens[tokenIndex])
        {
            if (strcmp(tokens[tokenIndex], ";") == 0)
            {
                token_change = tokenIndex + 1;
                break;
            }
        }
        tokenIndex++;
    }*/

    num_tokens = num_tokens - token_change;
    process[token_change - 1] = NULL;

    command(process, token_change, tokens, num_tokens);
    // Split tokens at NULL or ; to get a single command (ex1, ex2, ex3, ex4(fg command))

    // for example :  /bin/ls ; /bin/sleep 5 ; /bin/pwd
    // split the above line as first command : /bin/ls , second command: /bin/pwd  and third command:  /bin/pwd
    // Call command() and pass each individual command as arguements
}

void my_quit(void)
{
    for (int i = 0; i < pcb_cnt; i++)
    {
        if (pcbArray[i].status == RUNNING)
        {
            kill(pcbArray[i].pid, SIGTERM);
            printf("Killing [%d]\n", pcbArray[i].pid);
        }
    }

    /******* FILL IN THE CODE *******/
    // Kill every process in the PCB that is either stopped or running

    printf("\nGoodbye\n");
}
