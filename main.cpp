#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <unistd.h>

using namespace std;

const int BUFFERLEN = 512;


class SimpleShell {

    int processInput(char userInput[512]) {
        //here we should write a function that gets the input, checks it
        return -1;
    }

    int executeCommand() {
        //here we should fork the process and run the command in shell
        return -1;
    }

    public: 
        int runShell() {
            //here we should have the while loop that is running the shell
            int exit = 1;
            char userInput[512];
            while(exit == 1) {
                printf("$ ");
                if(fgets(userInput, BUFFERLEN, stdin) == NULL) {
                    return -1;
                } else if(strcmp(userInput, "exit\n") == 0) {
                    return -1;
                } else {
                    //we process the input
                    processInput(userInput);
                }
            }
            return 0;
        }

};

int main() {
    //initialise the shell class
    SimpleShell shell;
    shell.runShell();
    return 0;
}









