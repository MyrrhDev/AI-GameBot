#ifndef Action_hh
#define Action_hh

#include "Structs.hh"


/**
 * Class that stores the commands requested by a player in a round.
 */

class Action {

public:

  // Commands a soldier with identifier id to move to (i, j).
  void command_soldier(int id, int i, int j);

  // Commands a helicopter with identifier id to execute the action
  // codified with code.
  void command_helicopter(int id, int code);

  // Commands a parachuter to jump to (i, j).
  void command_parachuter(int i, int j);


  //////// STUDENTS DO NOT NEED TO READ BELOW THIS LINE ////////
  
  // Creates an empty action.
  Action ()
  {   }

private:

  friend class Game;
  friend class SecGame;
  friend class Board;
  friend class Player;

  map<int, Position> commands_soldiers;    // Commands for soldiers.
  map<int, int>      commands_helicopters; // Commands for helicopters.
  vector<Position>   commands_parachuters; // Commands for parachuters.

  /**
   * Constructor reading one action object from a stream.
   */
  Action (istream& is);

  /**
   * Print the action object to a stream.
   */
  void print (ostream& os) const;

};

inline void Action::command_soldier(int id, int i, int j) {
  commands_soldiers[id] = Position(i, j);
}

inline void Action::command_helicopter(int id, int code) {
  commands_helicopters[id] = code;
}

inline void Action::command_parachuter(int i, int j) {
  commands_parachuters.push_back(Position(i, j));
}

#endif
