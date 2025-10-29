#include "history.h"

#include <deque>
#include <filesystem>
#include <fstream>
#include <iostream>

using namespace std;

const string kCurrentPath = filesystem::current_path();
const string kHistoryFileName = kCurrentPath + "/.shell_history";

void History::ShowHistory() { cout << "history is shown" << endl; }

void History::CreateHistoryFile() {
  if (!filesystem::exists(kHistoryFileName)) {
    cout << kHistoryFileName << endl;
    ofstream Myfile(kHistoryFileName);
  } else {
    cout << "the file is here!" << '\n';
  }
}
string History::GetHistoryCommand() { return "history"; }

void History::StoreHistoryDeque(deque<string> history_buffer) {
  ofstream file(kHistoryFileName, std::ios::trunc);

  while (!history_buffer.empty()) {
    file << history_buffer.front();
    history_buffer.pop_front();
  }
  file.close();
}

deque<string> History::LoadHistoryDeque() {
  deque<string> history_deque;
  string line;
  ifstream file(kHistoryFileName);

  while (getline(file, line)) {
    history_deque.push_back(line.append("\n"));
  }
  file.close();

  return history_deque;
}
