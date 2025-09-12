#include "history.h"

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
    cout << "the file is here!" << endl;
  }
}
string History::GetHistoryCommand() { return "history"; }

void History::StoreHistoryQueue(queue<string> historyVector) {
  ofstream file(kHistoryFileName, std::ios::trunc);

  while (!historyVector.empty()) {
    file << historyVector.front();
    historyVector.pop();
  }
  file.close();
}

queue<string> History::LoadHistoryQueue() {
  queue<string> historyQueue;
  string line;
  ifstream file(kHistoryFileName);

  while (getline(file, line)) {
    historyQueue.push(line.append("\n"));
  }
  file.close();

  return historyQueue;
}
