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
#include <stdbool.h>


// TODO: Your function definitions below (declarations in dsh.h)

void dsh(){
    while(1){
        printf("dsh> ");
        char *input = (char*) malloc(1 + MAXBUF * sizeof(char) + 2); // 1 for newline
        if(fgets(input, MAXBUF, stdin) == NULL){
            exit(0);
        }

        int numTokens = 1;
        int length = strlen(input);
        if(length <= 0 ){
            continue;
        }
        int newline = strcspn(input, "\n");//cut off newline to start
        input[newline] = '\0';
        for(int i = 0; i < length; i++){//get the amount of delims in str
        if(input[i] == ' '){
            numTokens++;
        }
        }
        char** array = split(input, " ", numTokens, length);
       free(input);

        if(strcmp(array[0], "exit") == 0){//exit
            freearray(array,numTokens);
            break;
        }
        else if(strcmp(array[0], "pwd") == 0){//print working directory
            char *wd = (char*) malloc(MAXBUF * sizeof(char));
            getcwd(wd, MAXBUF);
            printf("%s\n", wd);
            free(wd);
        }
        else if(strcmp(array[0], "cd") == 0){//cd function that changes to the given directory or the HOME if none given
            char *currDir;
            currDir = array[1];
            if(array[1] == NULL){
                currDir = getenv("HOME");
            }
            if(chdir(currDir) != 0){
                printf("Could not change dir to %s\n", currDir);
            }
        }
        else{
             //full path
            if(array[0][0] == '/'){
                mode1(array, numTokens);
             }
             else{
            //path construction
                mode2(array, numTokens);
             }
        }

       
        freearray(array, numTokens);
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

void freearray(char** array, int length){//frees all the space from the array
    for(int i = 0; i < length; i++){
        if(array[i] == NULL){
            break;
        }
        free(array[i]);
    }
    free(array);
}
//full path mode
void mode1(char** array, int numTokens){
     if(array[0][0] == '/'){
            if(access(array[0], F_OK | X_OK) == 0){
                if(strcmp(array[numTokens - 1], "&") == 0){// checks for the & and changes it to null so it isnt passed in the execv args
                array[numTokens - 1] = NULL;
                
                int t_pid = fork();
                if(t_pid == 0){//is child
                execv(array[0], array);
                }
                } 
                else{//theres no &
                int t_pid = fork();
                if(t_pid == 0){//is child
                execv(array[0], array);
                }
                wait(NULL);//must wait
                }     
            }
            else{
                printf("%s is not an executable or does not exist\n" ,array[0]);
            }
        }
        return;
}
//partial path builder mode
void mode2(char** array, int numTokens){
    char* wd = (char*) malloc(MAXBUF * sizeof(char));
    if(getcwd(wd, MAXBUF) != NULL){//make sure you get the cwd
        char* array0 = (char*) malloc(1+MAXBUF * sizeof(char));
        strcpy(array0, array[0]);
        free(array[0]);
        array[0] = (char*) malloc(MAXBUF * sizeof(char));
        strcpy(array[0], array0);
        

        strcat(wd, "/");
        strcat(wd, array[0]);
        strcpy(array[0], wd);

        if(access(array[0], F_OK | X_OK) == 0){//if this is what we want treat it like mode 1 and pass it there
            mode1(array, numTokens);
        }
        else{
            strcpy(wd, getenv("PATH"));//reuse wd variable to put full path in
            strcpy(array[0], array0);
            int locations = 1;
            int length = strlen(wd);

            for(int i = 0; i < length; i++){//get the amount of delims in str
                if(wd[i] == ':'){
                locations++;
            }
            }
            char** path = split(wd, ":", locations, length);//do split on path to get it in pieces
            for(int i = 0; i < locations; i++){
                strcat(path[i], "/");
                strcat(path[i], array[0]);//concatenate / and the command
            }
            bool foundPath = false;
            for(int i = 0; i < locations; i++){//check each command if its the working one
                if(access(path[i] , F_OK | X_OK) == 0){
                    foundPath = true;//set bool true
                    strcpy(array[0], path[i]);//copy in the working path
                    break;
                }
            }
            
            if(foundPath){//if we found the right command
                int t_pid = fork();
                if(t_pid == 0){//in child
                execv(array[0], array);
                }
                wait(NULL);
                return;
            }
            else{//if not
                printf("Command not Found\n");
            }   
            freearray(path, locations);//memory free
        }
        free(array0);
    }
    free(wd);
    return;
}