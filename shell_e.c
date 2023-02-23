#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64

/* Splits the string by space and returns the array of tokens
 *
 */


char **tokenize(char *line)
{
    char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
    char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
    int i, tokenIndex = 0, tokenNo = 0;

    for (i = 0; i < strlen(line); i++)
    {

        char readChar = line[i];

        if (readChar == ' ' || readChar == '\n' || readChar == '\t')
        {
            token[tokenIndex] = '\0';
            if (tokenIndex != 0)
            {
                tokens[tokenNo] = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
                strcpy(tokens[tokenNo++], token);
                tokenIndex = 0;
            }
        }
        else
        {
            token[tokenIndex++] = readChar;
        }
    }

    free(token);
    tokens[tokenNo] = NULL;
    return tokens;
}
int main(int argc, char *argv[]) {
    char line[MAX_INPUT_SIZE];
    char **tokens;
    int i, j;
    
    while (1) {
        /* BEGIN: TAKING INPUT */
        bzero(line, sizeof(line));
        printf("$ ");
        scanf("%[^\n]", line);
        getchar();

        /* END: TAKING INPUT */

        line[strlen(line)] = '\n'; // terminate with new line

        // Split the input line into multiple commands separated by "&&"
        char *command = strtok(line, "&");
        while (command != NULL) {
            tokens = tokenize(command);
            
            int background = 0;
            int last_token = 0;
            while (tokens[last_token] != NULL) {
                last_token++;
            }
            last_token--;
            if (strcmp(tokens[last_token], "&") == 0) {
                background = 1;
                tokens[last_token] = NULL;
            }

            // fork a new process to run the command
            pid_t pid = fork();

            if (pid == 0) {
                // Call the command using the execvp function
                execvp(tokens[0], tokens);
                perror("Error");
                exit(1);
            }
            else if (pid > 0) {
                // parent process
                if (!background) {
                    int status;
                    waitpid(pid, &status, 0);
                    // If the command returns a non-zero exit status, stop executing the rest of the commands
                    if (status != 0) {
                        break;
                    }
                }
            }
            else {
                perror("Error");
            }
            
            // Free the tokens for the current command
            for (j = 0; tokens[j] != NULL; j++) {
                free(tokens[j]);
            }
            free(tokens);
            
            // Get the next command
            command = strtok(NULL, "&");
        }
    }
    
    return 0;
}
