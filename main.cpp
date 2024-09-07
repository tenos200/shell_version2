#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>

using namespace std;

int runShell() {
    string setEnvironment = "/bin/";
    char userInput[512];
    int bufferLen = 512;
    string start = "";
    //max counter to prevent error
    int count = 0;
    //use a vector to decompose and store all commands
    vector<string> allCommands;

    while(count < 100) {
        //clear the vector, for command purposes
        allCommands.clear();
        //showcase the first symbol  
        printf("$");
        //should we encounter ctrl-d check this to prevent segfault
        if(fgets(userInput, bufferLen, stdin) == NULL) {
                break;
        } else {
            start = userInput;
            //if the user inputs exit we want to quit, otherwise we process tokens
            if(start.compare("exit\n") == 0) {
                break;
            } else {
                //split all tokens that are do not contain necessary variables
                char *token = strtok(userInput, " \t|><;&");

                //print out the valid tokens
                while(token != NULL) {
                    //get the next token in the list of split tokens
                    printf("%s\n", token);
                    if(token != NULL) {
                        allCommands.push_back(token);
                    }
                    token = strtok(NULL, " \t|><;&");
                }
                //this is fucking shit, needs to be redone
                if(fork() == 0) {
                    //we start a new child process and then execute command
                    //construct command
                    string command = allCommands[0];
                    string s = "/bin/" + command;
                    char *arg_list[] = {"ls", "-a", NULL};
                    //check if command is valid
                    int statusCode = execvp(s.c_str(), arg_list);
                    if(statusCode == -1) {
                        printf("Process did not terminate correctly\n");
                    }
                }
            }
        }
        count++;
    }
    return 0;

}

int main() {
    runShell();
    return 0;

}







