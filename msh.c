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

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 5     // Mav shell only supports four arguments

int main()
{

  char * command_string = (char*) malloc( MAX_COMMAND_SIZE );

  while( 1 )
  {
    // Print out the msh prompt
    printf ("msh> ");

    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while( !fgets (command_string, MAX_COMMAND_SIZE, stdin) );

    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    int   token_count = 0;                                 
                                                           
    // Pointer to point to the token
    // parsed by strsep
    char *argument_ptr;                                         
                                                           
    char *working_string  = strdup( command_string );                

    // we are going to move the working_string pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *head_ptr = working_string;

    // Tokenize the input strings with whitespace used as the delimiter
    while ( ( (argument_ptr = strsep(&working_string, WHITESPACE ) ) != NULL) && 
              (token_count<MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup( argument_ptr, MAX_COMMAND_SIZE );
      if( strlen( token[token_count] ) == 0 )
      {
        token[token_count] = NULL;
      }
        token_count++;
    }

    // Now print the tokenized input as a debug check
    // Huh?
    
    // Part 3 If the user enters nothing, continue the prompt again 
    if(token[0] == NULL) {
      continue;
    }
    // Part 4 If the user enters quit or exit, exit with 0
    else if(strcmp(token[0], "quit") == 0) {
      exit(0);
    }
    else if (strcmp(token[0], "exit") == 0) {
      exit(0);
    }
    // Part 9 handle cd
    else if (strcmp(token[0], "cd") == 0) {
      chdir(token[1]);
    }
    // Part 2 If the command is not supported your shell shall print the invalid command followed by “: Command not found.”
    // Check for the 5 built ins, call exec, if it returns -1, print command found
//     else if (strcmp(token[0], "exit") == 0 ||
//     strcmp(token[0], "quit") == 0 ||
//     strcmp(token[0], "history") == 0 ||
//     strcmp(token[0], "!") == 0 ||
//     strcmp(token[0], "fork") == 0 ||
//     strcmp(token[0], "execvp") == 0) {
    
//     // Execvp needs more arguments?
//     int check_validity = execvp(token[0]);
//     if (check_validity == -1) {
//         printf("%s, Command not found.\n", token[0]);
//     }
// }

    // Part 6 Your version of Mav shell shall support up to 10 command line parameters in addition to the command.
    //if( argc > 11) {
    //  printf("Too many arguments");
    //  continue;
    //}

    // Part 8 Mav shell shall be implemented using fork(), wait() and one of the exec family of functions. Your Mav shell shall not use system(). Use of system() will result in a grade of 0.


    // Cleanup allocated memory
    for( int i = 0; i < MAX_NUM_ARGUMENTS; i++ )
    {
      if( token[i] != NULL )
      {
        free( token[i] );
      }
    }

    free( head_ptr );

  }
  return 0;
  // e2520ca2-76f3-90d6-0242ac120003
}
