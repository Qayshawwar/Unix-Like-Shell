// file: TwoPipesThreeChildren.cpp
// author: Qays Hawwar
// date: 11/16/2025
// purpose: CS3377 - Assignment 5
// description:
//   This program executes the shell command
//      "ls -ltr | grep 3377 | wc -l"
//   using two pipes and three child processes.
//   The parent only creates the children and then waits.
//   Child1 executes "ls -ltr".
//   Child2 executes "grep 3377".
//   Child3 executes "wc -l".

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{
  int status;
  int childpid1;
  int childpid2;
  int childpid3;

  // Commands and their arguments
  char *ls_args[]   = { (char *)"ls",   (char *)"-ltr", NULL };
  char *grep_args[] = { (char *)"grep", (char *)"3377", NULL };
  char *wc_args[]   = { (char *)"wc",   (char *)"-l",   NULL };

  // Two pipes:
  // pipe1: between "ls -ltr" and "grep 3377"
  // pipe2: between "grep 3377" and "wc -l"
  int pipe1[2];
  int pipe2[2];

  if (pipe(pipe1) == -1) {
    perror("Error creating pipe1");
    exit(1);
  }

  if (pipe(pipe2) == -1) {
    perror("Error creating pipe2");
    exit(1);
  }

  // Child1: "ls -ltr"
  if ((childpid1 = fork()) == -1) {
    perror("Error creating first child process");
    exit(1);
  }

  if (childpid1 == 0) {
    // Replace stdout with write end of pipe1
    dup2(pipe1[1], 1);

    // Close all pipe ends
    close(pipe1[0]);
    close(pipe1[1]);
    close(pipe2[0]);
    close(pipe2[1]);

    execvp(*ls_args, ls_args);
    perror("Error executing ls");
    exit(1);
  }

  // Child2: "grep 3377"
  if ((childpid2 = fork()) == -1) {
    perror("Error creating second child process");
    exit(1);
  }

  if (childpid2 == 0) {
    // stdin from read end of pipe1
    dup2(pipe1[0], 0);
    // stdout to write end of pipe2
    dup2(pipe2[1], 1);

    // Close all pipe ends
    close(pipe1[0]);
    close(pipe1[1]);
    close(pipe2[0]);
    close(pipe2[1]);

    execvp(*grep_args, grep_args);
    perror("Error executing grep");
    exit(1);
  }

  // Child3: "wc -l"
  if ((childpid3 = fork()) == -1) {
    perror("Error creating third child process");
    exit(1);
  }

  if (childpid3 == 0) {
    // stdin from read end of pipe2
    dup2(pipe2[0], 0);

    // Close all pipe ends
    close(pipe1[0]);
    close(pipe1[1]);
    close(pipe2[0]);
    close(pipe2[1]);

    execvp(*wc_args, wc_args);
    perror("Error executing wc");
    exit(1);
  }

  // Parent: close all pipe ends, then wait for children
  close(pipe1[0]);
  close(pipe1[1]);
  close(pipe2[0]);
  close(pipe2[1]);

  // Wait for all three children to finish
  wait(&status);
  wait(&status);
  wait(&status);

  return 0;
}
