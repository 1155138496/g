#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>

#define WRITE_END 1     // pipe write end
#define READ_END 0      // pipe read end

/*	shell_execute - Function to execute the command with specified command content and command arguments. 
*	@args	String array with command and its arguments. 
*	@argc	Total number of strings including the command, its arguments and the NULL in the end
*/

	 


int shell_execute(char ** args, int argc){


  int child_pid, wait_return, status;

	/* Execute built-in commands */
	/* exit */
	if(strcmp(args[0], "exit") == 0 ){
		return -1; 
	}
	/* TODO: cd */
	if(strcmp(args[0], "cd")== 0){
		if(chdir(args[1])<0){
			printf("cd: %s: %s ", args[1],strerror(errno));
		}
		return 0;
	}
	
  /* Non-built-in commands. These commands should be executed in a child process so the parent process can continue to invoke other commands */	
	/* One command without pipe */
	if((child_pid = fork()) < 0){
		printf("fork() error \n");
	}
	else if(child_pid == 0 ){
		/* TODO: execute the command and check if the command is correctly executed */
		if(execvp(args[0],args)<0){
			printf("execute error\n");
			exit(-1);
		}
	}else{
		if ((wait_return=wait(&status))<0)
		printf("wait error\n");
		
	}
	
	
	/* TODOs: one pipe and two pipes */

    /* One pipe */
	int pipefd[2];
	int pipepid;
	char * left[argc];
	char * right[argc];
	int leftc = 0;
	int rightc = 0;

	// Check if there's a pipe symbol and split the commands
	for (int i = 0; i < argc; i++) {
		if (strcmp(args[i], "|") == 0) {
			for (int j = 0; j < i; j++) {
				left[j] = args[j];
			}
			left[i] = NULL;
			for (int j = i + 1; j < argc; j++) {
				right[rightc] = args[j];
				rightc++;
			}
			right[rightc] = NULL;
			break;
		} else {
			left[leftc] = args[i];
			leftc++;
		}
	}

	if (rightc > 0) {
		if (pipe(pipefd) < 0) {
			printf("pipe() error\n");
			return -1;
		}
		if ((pipepid = fork()) < 0) {
			printf("fork() error\n");
		} else if (pipepid == 0) {
			// Child process - execute right command
			close(pipefd[READ_END]);
			dup2(pipefd[WRITE_END], STDOUT_FILENO);
			close(pipefd[WRITE_END]);
			if (execvp(right[0], right) < 0) {
				printf("execute error\n");
				exit(-1);
			}
		} else {
			// Parent process - execute left command and pass result to right command
			close(pipefd[WRITE_END]);
			dup2(pipefd[READ_END], STDIN_FILENO);
			close(pipefd[READ_END]);
            if(execvp(left[0],left)<0){
                printf("execute error\n");
                exit(-1);
            }
            }
            }

            /* Two pipes */
int pipefd1[2], pipefd2[2];
int pipepid1, pipepid2;
char * cmd1[argc];
char * cmd2[argc];
char * cmd3[argc];
int cmd1c = 0;
int cmd2c = 0;
int cmd3c = 0;

// Check if there are two pipe symbols and split the commands
for (int i = 0; i < argc; i++) {
    if (strcmp(args[i], "|") == 0 && strcmp(args[i+1], "|") == 0) {
        for (int j = 0; j < i; j++) {
            cmd1[cmd1c] = args[j];
            cmd1c++;
        }
        cmd1[cmd1c] = NULL;
        for (int j = i+2; j < argc; j++) {
            cmd3[cmd3c] = args[j];
            cmd3c++;
        }
        cmd3[cmd3c] = NULL;
        for (int j = i+1; j < i+2+cmd3c; j++) {
            cmd2[cmd2c] = args[j];
            cmd2c++;
        }
        cmd2[cmd2c] = NULL;
        break;
    }
}

if (cmd3c > 0) {
    if (pipe(pipefd1) < 0 || pipe(pipefd2) < 0) {
        printf("pipe() error\n");
        return -1;
    }
    if ((pipepid1 = fork()) < 0) {
        printf("fork() error\n");
    } else if (pipepid1 == 0) {
        // Child process - execute cmd2 and pass result to cmd3
        close(pipefd1[READ_END]);
        close(pipefd2[WRITE_END]);
        dup2(pipefd1[WRITE_END], STDOUT_FILENO);
        dup2(pipefd2[READ_END], STDIN_FILENO);
        close(pipefd1[WRITE_END]);
        close(pipefd2[READ_END]);
        if (execvp(cmd2[0], cmd2) < 0) {
            printf("execute error\n");
            exit(-1);
        }
    } else {
        if ((pipepid2 = fork()) < 0) {
            printf("fork() error\n");
        } else if (pipepid2 == 0) {
            // Child process - execute cmd3
            close(pipefd1[WRITE_END]);
            close(pipefd2[READ_END]);
            dup2(pipefd1[READ_END], STDIN_FILENO);
            close(pipefd1[READ_END]);
            dup2(pipefd2[WRITE_END], STDOUT_FILENO);
            close(pipefd2[WRITE_END]);
            if (execvp(cmd3[0], cmd3) < 0) {
                printf("execute error\n");
                exit(-1);
            }
        } else {
            // Parent process - execute cmd1 and pass result to cmd2
            close(pipefd1[READ_END]);
            close(pipefd1[WRITE_END]);
            close(pipefd2[READ_END]);
            dup2(pipefd2[WRITE_END], STDOUT_FILENO);
            close(pipefd2[WRITE_END]);
            if (execvp(cmd1[0], cmd1) < 0) {
                printf("execute error\n");
                exit(-1);
            }
        }
    }
}









 /* wait for child process to terminate */
	while((wait_return = wait(&status)) > 0);
			
 return 0;

}