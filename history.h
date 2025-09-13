#include <string.h>

#include <iostream>

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
  void StoreHistoryDeque(std::deque<std::string> history_buffer);

  /**
   * Loads the history from `.shell_history` file into a deque:
   * @code
   * std::deque<std::string> historyBuffer;
   * @endcode
   * This deque is then used as to fetch history commands from.
   */
  std::deque<std::string> LoadHistoryDeque();
};
