/*
 * dsh.c
 *
 *  Created on: Aug 2, 2013
 *      Author: chiu
 */
#include "dsh.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <sys/types.h>
#include <errno.h>
#include <err.h>
#include <sys/stat.h>
#include <string.h>


// TODO: Your function definitions below (declarations in dsh.h)

void dsh(){
    while(1){
        printf("dsh> ");
        char *input = (char*) malloc(MAXBUF * sizeof(char));
        if(fgets(input, MAXBUF, stdin) == NULL){
            exit(0);
        }

        int numTokens = 1;
        int length = strlen(input);
        int newline = strcspn(input, "\n");//cut off newline to start
        input[newline] = '\0';
        for(int i = 0; i < length; i++){//get the amount of delims in str
        if(input[i] == ' '){
            numTokens++;
        }
    }
        char** array = split(input, " ", numTokens, length);
        
        if(strcmp(array[0], "exit") == 0){//exit
            free(input);
            freearray(array,numTokens);
            break;
        }

        if(strcmp(array[0], "pwd") == 0){//print working directory
            char *wd = (char*) malloc(MAXBUF * sizeof(char));
            getcwd(wd, MAXBUF);
            printf("%s\n", wd);
            free(wd);
        }

        //cd
        if(strcmp(array[0], "cd") == 0){//cd function that changes to the given directory or the HOME if none given
            char *currDir;
            currDir = array[1];
            if(array[1] == NULL){
                currDir = getenv("HOME");
            }
            if(chdir(currDir) != 0){
                printf("Could not change dir to %s", currDir);
            }
        }
    }
}


//splits a string
char** split(char *str, char *delim, int numTokens, int length){
    char **array = (char**) malloc((numTokens + 1) * sizeof(char*));//malloc space for array and its indicies
    for(int i = 0; i < numTokens; i++){
        array[i] = (char*) malloc(length * sizeof(char));
    }

    char *token = strtok(str, delim);
    int i = 0;
    while(token != NULL){//copies the tokens into the array
        strcpy(array[i++], token);
        token = strtok(NULL, delim);
    }
    array[numTokens] = NULL;
    return array;
}

void freearray(char** array, int length){//frees all the spacce from the array
    for(int i = 0; i < length; i++){
        if(array[i] == NULL){
            break;
        }
        free(array[i]);
    }
    free(array);
}
