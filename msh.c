// The MIT License (MIT)
//
// Copyright (c) 2016 Trevor Bakker
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>

#define WHITESPACE " \t\n" // We want to split our command line up into tokens
                           // so we need to define what delimits our tokens.
                           // In this case  white space
                           // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255 // The maximum command-line size

#define MAX_NUM_ARGUMENTS 11 // Mav shell supports 10 arguments 

//Block the SIGINT and SIGTSTP signals:
static void handle_signal(int sig) {
}

int main() {
  struct sigaction act;
  char history_arr[50][255] = {0};
  int history_counter = 0;
 
  /*
    Zero out the sigaction struct
  */ 
  memset (&act, '\0', sizeof(act));
 
  /*
    Set the handler to use the function handle_signal()
  */ 
  act.sa_handler = &handle_signal;
 
  /* 
    Install the handler and check the return value.
  */ 
  if (sigaction(SIGTSTP , &act, NULL) < 0) {
    perror ("sigaction: ");
    return 1;
  }
 
  /*
    Zero out the sigaction struct
  */ 
  memset (&act, '\0', sizeof(act));
 
  /*
    Set the handler to use the function handle_signal()
  */ 
  act.sa_handler = &handle_signal;
 
  /* 
    Install the handler and check the return value.
  */ 
  if (sigaction(SIGINT , &act, NULL) < 0) {
    perror ("sigaction: ");
    return 1;
  }


  char *command_string = (char *)malloc(MAX_COMMAND_SIZE);

  while (1) {
    int redirect_found = 0;
    int redirect_file_index = 0;
    int pipe_found = 0;
    int pipe_index = 0;
    int pfd[2];

    // Print out the msh prompt
    printf("msh> ");

    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while (!fgets(command_string, MAX_COMMAND_SIZE, stdin));


    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    int i;
    for( i = 0 ; i < MAX_NUM_ARGUMENTS; i++ ) {
      token[i] = NULL;
    }

    int token_count = 0;

    // Pointer to point to the token
    // parsed by strsep
    char *argument_ptr;
    char *working_string = NULL;

    // If they are rerunning a command, use that instead
    if(command_string[0] == '!') {
      int num = atoi(&command_string[1]);
      working_string = strdup(history_arr[num]);
    }
    else {
      working_string = strdup(command_string);
    }
     //Keeping track of history
    strcpy(history_arr[history_counter], working_string);
    history_counter++;

    // we are going to move the working_string pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *head_ptr = working_string;

    // Tokenize the input strings with whitespace used as the delimiter
    while (((argument_ptr = strsep(&working_string, WHITESPACE)) != NULL) &&
           (token_count < MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup(argument_ptr, MAX_COMMAND_SIZE);
      if (strlen(token[token_count]) == 0)
      {
        token[token_count] = NULL;
      }
      else
      {
        token_count++;
      }
    }

      //Search the token array for |:
       for(int i = 0; i < token_count; i++) {
          if (strcmp(token[i], "|") == 0) {
            pipe_found = 1;
            pipe_index = i+1;
            //Trim off the end of the line
            token[i] = NULL;
            break;
        }
      }

      //Search the token array for > 
      for(int i = 0; i < token_count; i++) {

        //i might be NULL if we previously found a pipe
        //it is safe to skip over it because we already handled the pipe
        if (token[i] == NULL) {
          continue;
        }

        if (strcmp(token[i], ">") == 0) {
          redirect_found = 1;
          redirect_file_index = i+1;

          // Trim off the end of the line
          token[i] = NULL;
          break;
        }
      }

    //If the user enters nothing, continue the prompt again:
    if (token[0] == NULL)
    {
      continue;
    }

    //If the user enters quit or exit, exit with 0:
    else if (strcmp(token[0], "quit") == 0)
    {
      exit(0);
    }
    else if (strcmp(token[0], "exit") == 0)
    {
      exit(0);
    }
    //Handle cd:
    else if (strcmp(token[0], "cd") == 0)
    {
      chdir(token[1]);
    }

    //Printing history
    else if (strcmp(token[0], "history") == 0) {
      for(int i = 0; i < history_counter; i++) {
        printf("[%d] %s", i, history_arr[i]);
      }
    }

    //Running general commands
    else {
      if(pipe_found == 1) {
        // Open the pipe
        if ( pipe(pfd) == -1) { 
          perror("pipe"); 
          exit(EXIT_FAILURE);
        } 
      }

      pid_t pid = fork();

      //Child process
      if (pid == 0) { 
        if(redirect_found == 1) {
          int fd = open(token[redirect_file_index], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR );
          if(fd < 0) {
            perror("Can't open output file. " );
            exit(0);
          }
          dup2(fd, 1);
          close(fd);
        }

        if (pipe_found == 1) {
          dup2(pfd[1], 1);
        }
        
        int check_validity = execvp(token[0], &token[0]);
        if (check_validity == -1) {
          printf("%s, Command not found.\n", token[0]);
          exit(0);
        }
      }

      //Parent process
      else { 

        //Fork again from the parent for piping
        if (pipe_found == 1) {

          pid_t pid2 = fork();

          //Second child process
          if (pid2 == 0) {
            if(redirect_found == 1) {
              int fd = open(token[redirect_file_index], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR );
              if(fd < 0) {
                perror("Can't open output file. " );
                exit(0);
              }

              dup2(fd, 1);
              close(fd);
            }

            //Replace stdin with the read end of the pipe
            if (pipe_found == 1) {
              dup2(pfd[0], 0);
            }

            int check_validity = execvp(token[pipe_index], &token[pipe_index]);
            if (check_validity == -1) {
              printf("%s, Command not found.\n", token[0]);
              exit(0);
            }
          }
        }

        int status;
        wait(&status);
        if (pipe_found == 1) {
          wait(&status);
        }
      }
    }


    //Cleanup allocated memory
    for (int i = 0; i < MAX_NUM_ARGUMENTS; i++) {
      if (token[i] != NULL) {
        free(token[i]);
      }
    }

    free(head_ptr);
  }
  return 0; 
  // e2520ca2-76f3-90d6-0242ac120003
}
