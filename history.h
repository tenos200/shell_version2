#include <string.h>

#include <iostream>
#include <vector>

class History {
 public:
  // Shows the shell history fetched from .shell_history
  void ShowHistory();
  // Creates the history file on the machine in form of .shell_history
  void CreateHistoryFile();
  std::string GetHistoryCommand();
  /** Stores history queue in .shell_history file, overwrites each time a
   * new file is stored.
   */
  void StoreHistoryQueue(std::queue<std::string> commandToStore);

  /**
   * Loads the history from `.shell_history` file into a queue:
   * @code
   * std::queue<std::string> historyQueue;
   * @endcode
   * This queue is then used as to fetch history commands from.
   */
  std::queue<std::string> LoadHistoryQueue();
};
