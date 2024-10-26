#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <filesystem>

/* TODO:
 * Set the start directory to be home directory.
 * */


using namespace std;

const int BUFFERLEN = 512;

class SimpleShell {

    vector<char*> processInput(char userInput[512]) {
        // Here we should write a function that gets the input, checks it
        vector<char*> args;

        // Replace \n with \0 from the fgets, this allows us to use execvp later
        size_t len = strlen(userInput);
        if(userInput[len - 1] == '\n') {
            userInput[len - 1] = '\0';
        }

        char* token = strtok(userInput, " \t|><;&");
        while(token != nullptr) {
            args.push_back(token);
            token = strtok(nullptr, " ");
        }

        // For execvp the last element of the array must be nullptr
        args.push_back(nullptr);
        return args;
    }

    int executeCommand(vector<char*> args) {
        pid_t pid = fork();

        if(pid < 0) {
            perror("fork failed");
        } else if(pid == 0) {
            execvp(args[0], args.data());
            // Added exit statement to fix problem with command not exiting 
            exit(1);
        } else {
            int status;
            // Wait or the child process to complete
            waitpid(pid, &status, 0);  
            // Debugging statement that is here for now
            std::cout << "Parent process: child completed" << std::endl;
        }
        return 1;
    }

    int restorePath() {
        return 0;
    }

    public: 
        int runShell() {
            // Get and set the home environment 
            const string homeEnvironment = getenv("HOME");
            filesystem::current_path(homeEnvironment);

            // Here we should have the while loop that is running the shell
            int exit = 1;
            char userInput[512];
            while(exit == 1) {
                printf("$ ");
                if(fgets(userInput, BUFFERLEN, stdin) == NULL) {
                    return -1;
                } else if(strcmp(userInput, "exit\n") == 0) {
                    return 0;
                } else {
                    // We process the input
                    vector<char*> args = processInput(userInput);
                    // After that we pass and execute command
                    executeCommand(args);
                }
            }
            return 0;
        }
};


int main() {
    // Initialise the shell class
    SimpleShell shell;
    shell.runShell();
    return 0;
}
