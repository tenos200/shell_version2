#include "history.h"

#include <filesystem>
#include <iostream>

using namespace std;

const string kHistoryFileName = ".shell_history";

void History::ShowHistory() { cout << "history is shown" << endl; }
void History::CreateHistoryFile() {
  if (!filesystem::exists(kHistoryFileName)) {
    cout << "file doesn't exist" << endl;
  } else {
    cout << "the file is here!" << endl;
  }
}
string History::GetHistoryCommand() { return "history"; }
