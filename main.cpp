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
 * TODO: Stage 4 - changing directory, still requires better error handling.
 * TODO: Think about a elegant way to stirp away \n from the userInput. Its
 * added in the fgets.
 *
 * */


// Constants for the program are now defined here, unsure if this is the correct place to have them.
const int BUFFERLEN = 512;
const string STARTPATH = filesystem::current_path();
const char *HOMEENVIRONMENT = getenv("HOME");

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
              // Exit pid after command is executed.
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

    /*
     * TODO: Add proper error handling. Look over tests with simple shell.
     *
     * */
    void changeDirectory(string const &formattedUserInput) { 
        int stringLen = formattedUserInput.length() - 1;
        int firstNonCmdCharacter = formattedUserInput.find(" ") + 1;
        string currentPath = filesystem::current_path();

        if(stringLen == 2 || stringLen == 3) {
                filesystem::current_path(HOMEENVIRONMENT);
        } else {
            // Concatinate string to get full path and directory
            string directoryCmdArgument = formattedUserInput.substr(
                        firstNonCmdCharacter, 
                        stringLen);

            // Replace final \n with \0 to pass effectively to path.
            std::replace(directoryCmdArgument.begin(), 
                    directoryCmdArgument.end(), '\n', '\0');

            filesystem::path path = directoryCmdArgument;
            if(filesystem::is_directory(path)) {
                filesystem::current_path(path);
            } else {
                cout << "cd: no such file or directory: " 
                    << directoryCmdArgument << endl;
            } 
        }
    }

    /* 
     * This function removes any trailing or unnecessary whitespaces
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

        // Lowercase all the letter
        for(char c : formattedString) {
            c = tolower(c);
        }

        return formattedString;
    }

    public:
        int runShell() {
            filesystem::current_path(HOMEENVIRONMENT);
            int exit = 0;
            char userInput[BUFFERLEN];
            commandCase procedure;
            string formattedUserInput;

            while (exit == 0) {
                printf("$ ");
                if (fgets(userInput, BUFFERLEN, stdin) == NULL) {
                    restorePath(STARTPATH);
                    return -1;
                } else {
                    // First we format the string, removing unnecessary whitespace.
                    formattedUserInput = removeExtraWhiteSpace(userInput);

                    // Then we get what switch statement to use.
                    procedure = commandChoice(formattedUserInput);
                }

                switch (procedure) {
                    case exitCmd:
                        exit = restorePath(STARTPATH);
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
            filesystem::current_path(STARTPATH);
            return -1;
        }
};

int main() {
    SimpleShell shell;
    shell.runShell();
    return 0;
}
