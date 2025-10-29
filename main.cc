#include <unistd.h>

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "history.h"

using namespace std;

/*
 * TODO: Stage 5 - history implementation, should include, !! command,
 * !N command.
 * TODO: Think about a elegant way to stirp away \n from the userInput. Its
 * added in the fgets.
 * */

// Constants for the program are now defined here, unsure if this is the correct
// place to have them.
const int kBufferlen = 512;
const string kStartPath = filesystem::current_path();
const char* kHomeEnvironment = getenv("HOME");
const size_t kMaxHistoryLength = 1000;

// Enum to decide how to proceed with command execution
enum CommandCase {
  kExitCmd,
  kChangeDirectoryCmd,
  kExecutableCmd,
  kShowHistoryCmd,
  kExecutePreviousCmd,
};

class SimpleShell {
  // History class that handles all history output
 private:
  History history_handler;

  vector<char*> ProcessInput(char user_input[]) {
    // Here we should write a function that gets the input, checks it
    vector<char*> args;

    // Replace \n with \0 from the fgets, this allows us to use execvp later
    size_t len = strlen(user_input);
    if (user_input[len - 1] == '\n') {
      user_input[len - 1] = '\0';
    }

    char* token = strtok(user_input, " \t|><;&");
    while (token != nullptr) {
      args.push_back(token);
      token = strtok(nullptr, " ");
    }

    // For execvp the last element of the array must be nullptr
    args.push_back(nullptr);
    return args;
  }

 private:
  int ExecuteCommand(vector<char*> args) {
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

 private:
  CommandCase CommandChoice(string& formatted_user_input) {
    if (formatted_user_input.compare("exit\n") == 0 ||
        formatted_user_input.compare("exit \n") == 0) {
      return kExitCmd;
    } else if (formatted_user_input.compare("history\n") == 0 ||
               formatted_user_input.compare("history \n") == 0) {
      return kShowHistoryCmd;
    } else if (formatted_user_input.compare("!!\n") == 0 ||
               formatted_user_input.compare("!! \n") == 0) {
      return kExecutePreviousCmd;
    } else if (formatted_user_input.starts_with("cd")) {
      return kChangeDirectoryCmd;
    } else {
      return kExecutableCmd;
    }
  }

 private:
  int RestorePath(string start_path) {
    filesystem::current_path(start_path);
    return 1;
  }

 private:
  void ChangeDirectory(string const& formatted_user_input) {
    int string_len = formatted_user_input.length() - 1;
    int first_non_cmd_char = formatted_user_input.find(" ") + 1;
    string current_path = filesystem::current_path();

    if (string_len == 2 || string_len == 3) {
      filesystem::current_path(kHomeEnvironment);
    } else {
      // Concatinate string to get full path and directory
      string directoryCmdArgument =
          formatted_user_input.substr(first_non_cmd_char, string_len);

      // Replace final \n with \0 to pass effectively to path.
      std::replace(directoryCmdArgument.begin(), directoryCmdArgument.end(),
                   '\n', '\0');

      if (filesystem::is_directory(directoryCmdArgument)) {
        filesystem::path path = directoryCmdArgument;
        filesystem::current_path(path);
      } else if (filesystem::is_regular_file(directoryCmdArgument)) {
        cout << "cd: not a directory: " << directoryCmdArgument << '\n';
      } else {
        cout << "cd: no such file or directory: " << directoryCmdArgument
             << '\n';
      }
    }
  }

  /*
   * This function removes any trailing or unnecessary whitespaces
   * */
 private:
  string RemoveExtraWhiteSpace(char user_input[kBufferlen]) {
    string user_input_str(user_input);
    string formatted_str = "";
    string temp = "x";

    for (char c : user_input_str) {
      if (temp.back() != ' ' && c == ' ') {
        formatted_str.push_back(' ');
      }
      temp.push_back(c);
      if (c != ' ') {
        formatted_str.push_back(c);
      }
    }

    // Given that the first character is a whitespace, remove the space
    int end = formatted_str.size() - 1;
    if (formatted_str.at(0) == ' ') {
      formatted_str = formatted_str.substr(1, end);
    }

    // Lowercase all the letter
    for (char c : formatted_str) {
      c = tolower(c);
    }

    return formatted_str;
  }

  void ShowHistory() { history_handler.ShowHistory(); };

  /** Function that adds to deque, given that its not greater than 1000
   * if it is, then we remove pop the first element and add new to back of
   * queue
   */
 private:
  void AddEntry(const string& user_input, deque<string>& history_buffer) {
    if (history_buffer.size() == kMaxHistoryLength) {
      history_buffer.pop_front();
    }
    history_buffer.push_back(user_input);
  }

 public:
  int RunShell() {
    filesystem::current_path(kHomeEnvironment);

    history_handler.CreateHistoryFile();
    int exit = 0;
    char user_input[kBufferlen];
    CommandCase procedure;
    string formatted_user_input;
    string previous_command;

    deque<string> history_buffer = history_handler.LoadHistoryDeque();

    while (exit == 0) {
      cout << "$ ";
      if (fgets(user_input, kBufferlen, stdin) == NULL) {
        RestorePath(kStartPath);
        break;
      } else {
        // First we format the string, removing unnecessary whitespace.
        formatted_user_input = RemoveExtraWhiteSpace(user_input);

        // Then we get what switch statement to use.
        procedure = CommandChoice(formatted_user_input);
      }

      switch (procedure) {
        case kExitCmd:
          exit = RestorePath(kStartPath);
          break;
        case kChangeDirectoryCmd:
          ChangeDirectory(formatted_user_input);
          break;
        case kExecutePreviousCmd:
          previous_command = history_buffer.back();
          cout << previous_command;
          procedure = CommandChoice(previous_command);
          // TODO: We need some way to feed back this command yo.
          break;
        case kShowHistoryCmd:
          ShowHistory();
          break;
        case kExecutableCmd:
          vector<char*> args = ProcessInput(user_input);
          ExecuteCommand(args);
          break;
      }

      // Then we should add the command to history queue
      AddEntry(formatted_user_input, history_buffer);
    }
    history_handler.StoreHistoryDeque(history_buffer);
    filesystem::current_path(kStartPath);
    return -1;
  }
};

int main() {
  SimpleShell shell;
  shell.RunShell();
  return 0;
}
