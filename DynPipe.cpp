// file: DynPipe.cpp
// author: Qays Hawwar
// date: 11/16/2025
// purpose: CS3377 - Assignment 5
// description:
//   Dynamic version of a pipeline program.
//   This program takes between 2 and 5 commands as arguments and
//   executes them as a pipeline.
//   Example:
//      dynpipe "ls -ltr" "grep 3377"
//      dynpipe "ls -ltr" "grep 3377" "grep hi" "wc -l"
//   This corresponds to:
//      ls -ltr | grep 3377
//      ls -ltr | grep 3377 | grep hi | wc -l
//
//   Each argument (quoted) is a full command with its parameters.
//   The program uses pipes and fork() calls to connect the commands.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_CMDS   5          // maximum number of commands
#define MAX_PIPES  (MAX_CMDS - 1)
#define MAX_TOKENS 20         // max tokens per command when splitting on spaces

int main(int argc, char **argv)
{
  int i, j;
  int status;

  // Number of commands is argc - 1 (since argv[0] is program name)
  int num_cmds = argc - 1;

  // Check number of commands
  if (num_cmds < 2 || num_cmds > MAX_CMDS) {
    fprintf(stderr,
            "Usage: %s \"cmd1\" \"cmd2\" ... (min 2, max 5 commands)\n",
            argv[0]);
    exit(1);
  }

  // Create needed pipes. For N commands, we need N-1 pipes.
  int pipes[MAX_PIPES][2];

  for (i = 0; i < num_cmds - 1; i++) {
    if (pipe(pipes[i]) == -1) {
      perror("Error creating pipe");
      exit(1);
    }
  }

  // Create one child process per command
  for (i = 0; i < num_cmds; i++) {

    pid_t childpid = fork();

    if (childpid == -1) {
      perror("Error creating child process");
      exit(1);
    }

    if (childpid == 0) {
      // Child process i

      // If not first command, connect stdin to previous pipe's read end
      if (i > 0) {
        if (dup2(pipes[i - 1][0], 0) == -1) {
          perror("dup2 error (stdin)");
          exit(1);
        }
      }

      // If not last command, connect stdout to current pipe's write end
      if (i < num_cmds - 1) {
        if (dup2(pipes[i][1], 1) == -1) {
          perror("dup2 error (stdout)");
          exit(1);
        }
      }

      // Close all pipe ends in the child (we are done with the original fds)
      for (j = 0; j < num_cmds - 1; j++) {
        close(pipes[j][0]);
        close(pipes[j][1]);
      }

      // Parse the command string argv[i+1] into tokens
      // We assume tokens are separated by spaces.
      char *cmd_string = strdup(argv[i + 1]);
      if (cmd_string == NULL) {
        perror("strdup failed");
        exit(1);
      }

      char *args[MAX_TOKENS];
      int   token_count = 0;

      char *token = strtok(cmd_string, " ");
      while (token != NULL && token_count < MAX_TOKENS - 1) {
        args[token_count++] = token;
        token = strtok(NULL, " ");
      }
      args[token_count] = NULL;  // terminate argument list

      if (token_count == 0) {
        fprintf(stderr, "Empty command provided.\n");
        exit(1);
      }

      // Execute the command
      execvp(args[0], args);
      perror("Error executing command");
      exit(1);
    }
  }

  // Parent process: close all pipe ends
  for (i = 0; i < num_cmds - 1; i++) {
    close(pipes[i][0]);
    close(pipes[i][1]);
  }

  // Wait for all children to finish
  for (i = 0; i < num_cmds; i++) {
    wait(&status);
  }

  return 0;
}
