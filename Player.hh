#ifndef Player_hh
#define Player_hh


#include "Info.hh"
#include "Action.hh"
#include "Random.hh"
#include "Registry.hh"


/***
 * Abstract base class for players.
 * *
 * This class uses multiple inheritance from Info and Action,
 * so that their public operations can been used from within Player.
 *
 * In order to create new players, inherit from this class and register them.
 * See for example AINull.cc and AIDemo.cc.
 */
class Player : public Info, public Random_generator, public Action {

public:

  /**
   * Play intelligence. Will be overwritten, thus declared virtual.
   */
  virtual void play () {
  };

  /**
   * Identifier of my player.
   */
  int me ();


  // Methods to give instructions to your units.

  // Commands the soldier with identifier id to move (or attack) position (i, j).
  // The instruction is ignored if there is no soldier with identifier id,
  // or if the soldier does not belong to the commanding player,
  // or if the soldier has already received an instruction in this round,
  // or if position (i, j) is not adjacent to the current position of the soldier.
  void command_soldier(int id, int i, int j);

  // Commands the helicopter with identifier id
  // to follow the instruction codified with instruction.
  // The instruction is ignored if there is no helicopter with identifier id,
  // or if the helicopter does not belong to the commanding player,
  // or if the helicopter has already received an instruction in this round,
  // or if the code is none of:
  // FORWARD1,
  // FORWARD2,
  // COUNTER_CLOCKWISE,
  // CLOCKWISE,
  // NAPALM.
  // (these constants are predefined)
  void command_helicopter(int id, int instruction);

  // Commands a parachuter to jump to position (i, j).
  // The instruction is ignored if in this round
  // the number of given jump instructions included this one is greater than
  //    the number of available parachuters in the corresponding helicopter
  //    or the maximum number of allowed jumps in each round,
  // or if (i, j) is outside the board.
  void command_parachuter(int i, int j);


  //////// STUDENTS DO NOT NEED TO READ BELOW THIS LINE ////////


private:

  friend class Game;
  friend class SecGame;

  int me_;

  inline void reset (const Info& info) {
    *static_cast<Action*>(this) = Action();
    *static_cast<State*>(this) = (State)info;
  }

  void reset (ifstream& is);

  bool  check_command_soldier(int id, int i, int j, int player);
  string code_command_soldier(int id, int i, int j, int player);

  bool  check_command_helicopter(int id, int codi, int player);
  string code_command_helicopter(int id, int codi, int player);

  bool  check_command_parachuter(int player, int i, int j);
  string code_command_parachuter(int player, int i, int j);

  bool enough_parachuters(int player, int i, int j);
};


inline int Player::me () {
  return me_;
};


inline void Player::command_soldier(int id, int i, int j) {
  if (not check_command_soldier(id, i, j, me())) {
    cerr << "warning: command_soldier(int id, int i, int j)"
         << " invoked by player " << me() << " with"
         << " id = " << id
         << " i = " << i
         << " j = " << j
         << ": " << code_command_soldier(id, i, j, me()) << endl;
    return;
  }
  Action::command_soldier(id, i, j);
}


inline void Player::command_helicopter(int id, int instruction) {
  if (not check_command_helicopter(id, instruction, me())) {
    cerr << "warning: command_helicopter(int id, int instruction) "
         << "invoked by player " << me() << " with"
         << " id = " << id
         << " instruction = " << instruction
         << ": " << code_command_helicopter(id, instruction, me()) << endl;
    return;
  }
  Action::command_helicopter(id, instruction);
}


inline void Player::command_parachuter(int i, int j) {
  if (not check_command_parachuter(me(), i, j)) {
    cerr << "warning: command_parachuter(int i, int j) "
         << "invoked by player " << me() << " with"
         << " i = " << i
         << " j = " << j
         << ": " << code_command_parachuter(me(), i, j) << endl;
    return;
  }
  Action::command_parachuter(i, j);
}


inline bool Player::check_command_soldier(int id, int i, int j, int player) {
  auto p = data_.find(id);
  return not (p == data_.end() or p->second.player != player
              or p->second.type != SOLDIER
              or commands_soldiers.find(id) != commands_soldiers.end()
              or abs(p->second.pos.i - i) > 1
              or abs(p->second.pos.j - j) > 1);
}

inline bool Player::check_command_helicopter(int id, int instruction, int player) {
  auto p = data_.find(id);
  return not (p == data_.end() or p->second.player != player
              or p->second.type != HELICOPTER
              or commands_helicopters.find(id) != commands_helicopters.end()
              or (    instruction != FORWARD1  and instruction != FORWARD2
                  and instruction != CLOCKWISE and instruction != COUNTER_CLOCKWISE
                  and instruction != NAPALM));
}

inline bool Player::check_command_parachuter(int player, int i, int j) {
  return enough_parachuters(player, i, j) and
    not (int(commands_parachuters.size()) >= MAX_JUMP
         or i < 0 or i >= MAX or j < 0 or j >= MAX);
}

inline string Player::code_command_soldier(int id, int i, int j, int player) {
  auto p = data_.find(id);
  if (p == data_.end())             return "identifier does not exist";
  if (p->second.player != player)   return "invalid player";
  if (p->second.type != SOLDIER)    return "invalid type";
  if (commands_soldiers.find(id) != commands_soldiers.end())
    return "more than one instruction in this round";
  if (abs(p->second.pos.i - i) > 1 or abs(p->second.pos.j - j) > 1)
    return "trying to move to a non adjacent cell";
  return "???";
}

inline string Player::code_command_helicopter(int id, int instruction, int player) {
  auto p = data_.find(id);
  if (p == data_.end())             return "identifier does not exist";
  if (p->second.player != player)   return "invalid player";
  if (p->second.type != HELICOPTER) return "invalid type";
  if (commands_helicopters.find(id) != commands_helicopters.end())
    return "more than one instruction in this round";
  if (    instruction != FORWARD1  and instruction != FORWARD2
      and instruction != CLOCKWISE and instruction != COUNTER_CLOCKWISE
      and instruction != NAPALM)
    return "unknown instruction";
  return "???";
}

inline string Player::code_command_parachuter(int player, int i, int j) {
  if (not enough_parachuters(player, i, j))
    return "too many jump instructions for an helicopter";
  if (int(commands_parachuters.size()) >= MAX_JUMP)
    return "too many jump instructions in the same round";
  if (i < 0 or i >= MAX or j < 0 or j >= MAX)
    return "position outside the board";
  return "???";
}

inline bool Player::enough_parachuters(int player, int i, int j) {
  for (int helicopter_id : helicopter[player]) {
    int i2 = data_[helicopter_id].pos.i;
    int j2 = data_[helicopter_id].pos.j;

    if (abs(i2 - i) <= 2 and abs(j2 - j) <= 2) {

      int cnt = 1;
      for (int k = 0; k < int(commands_parachuters.size()) and cnt <= int(data_[helicopter_id].parachuters.size()); ++k) {
        int i1 = commands_parachuters[k].i;
        int j1 = commands_parachuters[k].j;
        if (abs(i2 - i1) <= 2 and abs(j2 - j1) <= 2)
          ++cnt;
      }
      return cnt <= int(data_[helicopter_id].parachuters.size());
    }
  }
  return false;
}

#endif
