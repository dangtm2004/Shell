#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "string.h"
#include "unistd.h"

#define MAX_ARGS 20000 // includes program name, limits tokens variable
#define MAX_LENGTH 256 // limits max absolute path
#define TRUE 1
#define FALSE 0
// Path array
static char* PATH[MAX_ARGS]; // array storing paths
static int head_pos = 0; // head position
// Param list
static char* params[20000];
// function prototypes
void built_in_exit(); // implementation for built-in exit
void built_in_cd(int argc, char* argv[]); // implementation for built-in cd
void built_in_path(int argc, char* argv[]);// implementation for built-in path
// service functions
void init(); // sets PATH to default, containig only /bin
void throw_error(); // throws error
void tokenize(char* line, char* tokens[], int *token_counter, const char* del); // fills array of tokens
void reset_tokens(char* tokens[], int *token_counter, size_t size); // resets array of tokens
void crop_nl(char* token); // crops new line character
int search_path(char *cmd, char* full_path); // search command path
void remove_spaces(char* s); // removes spaces from c string
void process_line(char* line);

int main(int argc, char* argv[])
{
    // initializing PATH with default value
    init();
    // variables
    char* ifilename; // input filename
    size_t len = 0;
    //size_t *len;
    ssize_t lineSize = 0;
    char* line;
    //char** line = NULL;

    if (argc >= 3) // if more than one argument is supplied prints Usage
    {
        printf("Usage: dash can take no or one argument.\n");
        throw_error();
        exit(1);
    } if (argc == 2) // if file name is supplied, no prompt
    {
        ifilename = argv[1];
        remove_spaces(ifilename);
        FILE * file = fopen(ifilename, "r");
        if (file == NULL)
        {
            exit(1);
        }

        while ((lineSize = getline(&line, &len, file)) != -1) {
            // Processing file line by line
            process_line(line);
        }

        free(line);
        exit(0);

    }

    // interactive shell
    while(1)
    {
        printf("dash> ");
        lineSize = getline(&line, &len, stdin); // gets line from stdin
        //lineSize = getLine(&line, &len, stdin);
        //printf("#1 %s",line); // DEBUG
        process_line(line);
        //printf("%s\n", "End of while 1."); // DEBUG
    }
}

void built_in_exit() // implementation for built-in exit
{
    exit(0); // calls exit(0) system call
}

void built_in_cd(int argc, char* argv[])// implementation for built-in cd
{
    //printf("#3 %d\n", argc);
    int result;

    if(argc != 2)
    {
        throw_error();
        return;
    }
    //printf("#4 %s\n", argv[1]);
    // perform chdir()
    crop_nl(argv[1]);
    result = chdir(argv[1]);

    // check result
    if(result != 0)
    {
        throw_error();
    } else
    {
        printf("Directory succesfully changed to: %s\n", argv[1]);
    }
    return;
}

void built_in_path(int argc, char* argv[])// implementation for built-in path
{
    if(argc < 2) // if passed with no parameters, clear PATH
    {
        int i = 0;
        for(i = 0; i < head_pos; i++)
        {
            PATH[i] = "\0";
            head_pos = 0;
        }
    } else if(argc > MAX_ARGS)
    {
        throw_error();
    } else {
        int i = 0;
        for(i = 1; i < argc; i++) // disregard first element because it is command name not parameter
        {
            PATH[i-1] = (char*)malloc(strlen(argv[i])+1); // allocatng memory
            strcpy(PATH[i-1],argv[i]); // assigning new values to path
            head_pos++;
        }
    }
}

void init()
{
    head_pos = 0;
    int i = 0;
    for(i = 0; i < MAX_ARGS; i++)
    {
        PATH[i] = "\0\n"; // set elements to 0
    }
    PATH[0] = "/bin"; // adds initial value
    head_pos++;
}

void throw_error()
{
    char error_message[30] = "An error has occurred\n";
    int i = write(STDERR_FILENO, error_message, strlen(error_message));
    if(i == -1)
    {
        exit(1);
    }
}

void tokenize(char* line, char* tokens[], int *token_counter, const char* del) // fills array of tokens
{
    char* token; // variable for tokenizing string
    while ((token = strtok_r(line, del, &line)))
    {
        // saving tokens
        // crop new line at the end of token if there is one
        tokens[*token_counter] = token;
        ++*token_counter;
    }
}

void reset_tokens(char* tokens[], int *token_counter, size_t size) // resets array of tokens
{
    memset(tokens,0,size); // reset tokens for reuse
    *token_counter = 0; // reset token counter for reuse
}

void crop_nl(char* token)
{
    size_t len;

    len = strlen(token);
    // if last element is new line, remove it
    if( token[len-1] == '\n' )
    {
        token[len-1] = 0;
    }
}

int search_path(char *cmd, char* full_path) // search command path
{
    remove_spaces(cmd);
    char* abs_path = (char*)malloc(MAX_LENGTH);
    char* temp;
    if(head_pos == 0)
    {
        // PATH is empty
        throw_error();
        return -1;
    }
    int i = 0;
    for(i = 0; i < head_pos; i++)
    {
        strcpy(abs_path, PATH[i]);
        strcat(abs_path, "/");
        strcat(abs_path, cmd);
        temp = (char*)malloc(strlen(abs_path)+1);
        strcpy(temp, abs_path);
        strcpy(full_path, abs_path);
        //printf("Abs: %s, temp: %s\n", abs_path, temp); // DEBUG

        crop_nl(temp);
        if(access(temp, X_OK) == 0)
        {
            free(temp);
            return 0; // returns on success
        }

        abs_path = "\0";
    }
    // if iterated through the path and no success
    // throw error
    throw_error();
    return -1;
}

void remove_spaces(char* s) {
    const char* d = s;
    do {
        while (*d == ' ') {
            ++d;
        }
    } while ((*s++ = *d++));
}

void process_line(char* line)
{
    int token_counter = 0; // token counter
    int token_counterr = 0; // token counter
    char* tokens[MAX_ARGS]; // array of tokens
    char* tokens_r[MAX_ARGS]; // array of tokens for redirection
    char* tokens_m[MAX_ARGS]; // array of tokens for parallel
    char** tokens_p = NULL; // pointer, used to switch between token arrays
    int inFile = FALSE; // redirection flag
    char* ofilename = NULL; // output filename
    int i = 0;
    int res = 0;
    pid_t pids[MAX_ARGS]; // multiple processes
    char* abs_path = (char*)malloc(MAX_LENGTH);

    if(strcmp(line, "exit\n") == 0) // checks if line contains exit
    {
        built_in_exit();
    } else
        if(strstr(line, "cd") != NULL) // checks for built in cd
        {
            tokenize(line,tokens,&token_counter, " ");
            built_in_cd(token_counter, tokens);
            reset_tokens(tokens, &token_counter, sizeof (tokens));

        } else if(strstr(line, "path") != NULL)// checks for built in path
        {
            tokenize(line,tokens,&token_counter, " ");
            built_in_path(token_counter, tokens);
            reset_tokens(tokens, &token_counter, sizeof (tokens));
        } else
        {
            if(strstr(line, "&") != NULL) // checks for parallel
            {
                int parallel_counter = 0;
                //printf("Line: %s", line);
                tokenize(line,tokens_m,&parallel_counter, "&");

                for(i = 0; i < parallel_counter; i++)
                {
                    //printf("Tokens - m: %s\n", tokens_m[i]);
                    if(strstr(tokens_m[i], ">") != NULL) // checks for redirection
                    {
                        token_counter = 0;
                        tokenize(tokens_m[i],tokens,&token_counter, ">");
                        //printf("Redirection: tc %d", token_counter);
                        if(token_counter > 2) // something wrong, more > than one
                        {
                            token_counter = 0;
                            throw_error();
                        } else
                        {
                            inFile = TRUE; // raise the flag
                            ofilename = tokens[1];
                            remove_spaces(ofilename);
                            //printf("Output filename: %s", ofilename);
                            tokenize(tokens[0], tokens_r, &token_counterr, " ");
                            reset_tokens(tokens, &token_counter, sizeof (tokens));
                            // searching abs path in PATH
                            res = search_path(tokens_r[0], abs_path);

                            crop_nl(abs_path);

                            params[0] = abs_path;
                            int i = 1;
                            for(i = 1; i < token_counterr; i++)
                            {
                                params[i] = tokens_r[i];
                                crop_nl(params[i]);
                            }
                            params[token_counterr + 1] = NULL; // designate end of array
							reset_tokens(tokens_r, &token_counterr, sizeof (tokens_r));

                        }

                    } else
                    {
                        tokenize(tokens_m[i],tokens,&token_counter, " ");
                        // searching abs path in PATH
                        res = search_path(tokens[0], abs_path);
                        crop_nl(abs_path);

                        params[0] = abs_path;
                        int i = 0;
                        for(i = 1; i < token_counter; i++)
                        {
                            params[i] = tokens[i];
                            crop_nl(params[i]);
                        }
                        params[token_counter + 1] = NULL; // designate end of array
                        reset_tokens(tokens,&token_counter,sizeof (tokens));
                    }
                      

                    if(res == 0){
                        if((pids[i] = fork()) < 0)
                        {
                            throw_error();
                            //printf("Cannot fork\n");
                        } else
                        { 
                            if(pids[i] == 0)
                            { 
                                if(inFile) // enable redirection to file
                                {
                                    close(STDOUT_FILENO);
                                    crop_nl(ofilename);
                                    open(ofilename, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
                                    inFile = FALSE;
                                }
                                //printf("Child-> %d", (int)getpid());
                                
                                execv(abs_path, params); // no return expected
                            }
                          
                        }
                    } else
                    {
                        throw_error();
                    }

                    memset(params, 0, sizeof (params));
                    int status;

                    //while(parallel_counter>0)
                    //{
                    waitpid(pids[parallel_counter-1],&status, 0);
                    //    --parallel_counter;
                    //}
                }
            } else {
                if(strstr(line, ">") != NULL) // checks for redirection
                {
                    tokenize(line,tokens,&token_counter, ">");
                    //printf("Redirection: tc %d", token_counter);
                    if(token_counter > 2) // something wrong, more > than one
                    {
                        token_counter = 0;
                        throw_error();
                    } else
                    {
                        inFile = TRUE; // raise the flag
                        ofilename = tokens[1];
                        remove_spaces(ofilename);
                        //printf("Output filename: %s", ofilename);
                        token_counter = 0;
                        tokenize(tokens[0], tokens_r, &token_counter, " ");
                        tokens_p = tokens_r;
                    }
                    reset_tokens(tokens, &token_counter, sizeof (tokens));
                } else
                {
                    tokenize(line,tokens,&token_counter, " ");
                    tokens_p = tokens;
                }
                char* abs_path = (char*)malloc(MAX_LENGTH);
                // searching abs path in PATH
                int res = search_path(tokens_p[0], abs_path);
                if (res == 0) // if found path execute program
                {
                   // printf("pre-Child -> abs_path %s\n", abs_path);
                    int rc =fork();
                    if (rc < 0) // fork failed
                    {
                        throw_error();
                    } else
                    {
                        if( rc == 0) // child new process
                        {
                           // printf("Are you there????????????????????????????????????");
                            if(inFile) // enable redirection to file
                            {
                                close(STDOUT_FILENO);
                                crop_nl(ofilename);
                                open(ofilename, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
                            }
                            //printf("Child-> %d", (int)getpid());
                            crop_nl(abs_path);

                            params[0] = abs_path;
                            int i = 1;
                            for(i = 1; i < token_counter; i++)
                            {
                                params[i] = tokens[i];
                                crop_nl(params[i]);
                            }
                            params[token_counter + 1] = NULL; // designate end of array

                            //                            for(int i = 0; i <= token_counter +1; i++)
                            //                            {
                            //                                printf("param: %s at %d", params[i], i);  // DEBUG
                            //                            }

                            execv(abs_path, params); // no return expected
                        } else {
                            wait(NULL);
                            //printf("Parent-> %d", (int)getpid());
                        }
                    }
                } else
                {
                    throw_error();
                }


                reset_tokens(tokens, &token_counter, sizeof (tokens));
            }
        }
}
