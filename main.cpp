#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>
#include <string> 
using namespace std;

/*
 * TODO: Ensure that proper string cleaning is carried out before necessary
 * TODO: Stage 4 - changing directory
 * TODO: Think about a different way to handle strings so we can compare
 * what the strings starts with i.e., "cd ." for example.
 * commands.
 * */


//Constants for the program are now defined here, unsure if this is the correct place to have them.
const int BUFFERLEN = 512;
// Get and set the home environment
const string startPath = filesystem::current_path();
const string homeEnvironment = getenv("HOME");

// Enum to decide how to proceed with command execution
enum commandCase {
    exitCmd,
    changeDirectoryCmd,
    executableCmd,
};

class SimpleShell {


    vector<char*> processInput(char userInput[BUFFERLEN]) {
        // Here we should write a function that gets the input, checks it
        vector<char*> args;

        // Replace \n with \0 from the fgets, this allows us to use execvp later
        size_t len = strlen(userInput);
        if (userInput[len - 1] == '\n') {
          userInput[len - 1] = '\0';
        }

        char *token = strtok(userInput, " \t|><;&");
        while (token != nullptr) {
          args.push_back(token);
          token = strtok(nullptr, " ");
        }

        // For execvp the last element of the array must be nullptr
        args.push_back(nullptr);
        return args;
    }

    int executeCommand(vector<char*> args) {
        pid_t pid = fork();

          if (pid < 0) {
              perror("fork failed");
          } else if (pid == 0) {
              execvp(args[0], args.data());

              // Added exit statement to fix problem with command not exiting
              exit(1);
          } else {
              int status;

              // Wait or the child process to complete
              waitpid(pid, &status, 0);

              // Debugging statement that is here for now
              cout << "Parent process: child completed" << endl;
          }
          return 1;
    }

    commandCase commandChoice(string &formattedUserInput) {

        if(formattedUserInput.compare("exit\n") == 0) {
            return exitCmd;
        } else if(formattedUserInput.starts_with("cd")) {
            return changeDirectoryCmd;
        } else {
            return executableCmd;
        }
    }

    int restorePath(string startPath) {
        filesystem::current_path(startPath);
        return 1;
    }

    int changeDirectory(string formattedUserInput) { 
        cout << "CD" << endl;
        return 0; 
    }

    /* This function removes any trailing or unnecessary whitespaces
     * TODO: We should work on a better way to actually remove the first
     * whitespace, rather than carrying out a algorithm and the remove the
     * first withspace. We will come back to this later.
     *
     * */
    string removeExtraWhiteSpace(char userInput[BUFFERLEN]) {
        string userInputStr(userInput);
        string formattedString = "";
        string temp = "x";

        for(char c : userInputStr) {
            if(temp.back() != ' ' && c == ' ') {
                formattedString.push_back(' ');
            } 
            temp.push_back(c);
            if(c != ' ') {
                formattedString.push_back(c);
            }
        }

        // Given that the first character is a whitespace, remove the space
        int end = formattedString.size() - 1;
        if(formattedString.at(0) == ' ') {
            formattedString = formattedString.substr(1, end);
        }

        // Lower-case all the letters
        for(char &c: formattedString) {
            c = tolower(c);
        }
        return formattedString;
    }

    public:
        int runShell() {
            filesystem::current_path(homeEnvironment);
            int exit = 0;
            char userInput[BUFFERLEN];
            commandCase procedure;
            string formattedUserInput;

            while (exit == 0) {
                printf("$ ");
                if (fgets(userInput, BUFFERLEN, stdin) == NULL) {
                    restorePath(startPath);
                    string test = filesystem::current_path();
                    cout << test << endl;
                    return -1;
                } else {
                    // First we format the string, removing unnecessary whitespace.
                    formattedUserInput = removeExtraWhiteSpace(userInput);
                    // Then we get what switch statement to use.
                    procedure = commandChoice(formattedUserInput);
                }

                switch (procedure) {
                    case exitCmd:
                        exit = restorePath(startPath);
                        break;
                    case changeDirectoryCmd:
                        changeDirectory(formattedUserInput);
                        break;
                    case executableCmd:
                        vector<char*> args = processInput(userInput);
                        executeCommand(args);
                        break;
                }
            }
            filesystem::current_path(startPath);
            return 0;
        }
};

int main() {
    SimpleShell shell;
    shell.runShell();
    return 0;
}
