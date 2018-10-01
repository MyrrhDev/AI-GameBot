#ifndef State_hh
#define State_hh

#include "Structs.hh"

/**
 * Contains a class to store the current state of a game.
 */


/**
 * Stores the game state.
 */
class State {

public:

  // Returns the current round.
  // (0 for the first one, 199 for the last one)
  int round() const;

  // Returns the type of land at (i, j).
  // May be FOREST, GRASS, WATER or MOUNTAIN (predefined constants).
  // Returns -1 if the position is incorrect.
  int what(int i, int j) const;

  // Returns the time that (at least) fire will burn at (i, j).
  // Returns 0 if at (i, j) there is no fire.
  // Returns -1 if the position is incorrect.
  int fire_time(int i, int j) const;

  // Returns the identifier of the soldier at (i, j).
  // Returns 0 if there is none.
  // Returns -1 if the position is incorrect.
  int which_soldier(int i, int j) const;

  // Returns the identifier of the helicopter centered at (i, j).
  // Returns 0 if there is none.
  // Returns -1 if the position is incorrect.
  int which_helicopter(int i, int j) const;

  // Returns whose player the post at (i, j) is.
  // Returns -2 if at (i, j) there is no post.
  // Returns -1 if the post has not been conquered yet,
  // or if the position is incorrect.
  int post_owner(int i, int j) const;

  // Returns the value of the post at (i, j):
  // LOW_VALUE or HIGH_VALUE (predefined constants).
  // Returns -2 if at (i, j) there is no post.
  // Returns -1 if the position is incorrect.
  int post_value(int i, int j) const;

  // Returns the identifiers of the soldiers of player pl.
  // Returns an empty vector if the player is incorrect.
  vector<int> soldiers(int pl) const;

  // Returns the identifiers of the helicopters of player pl.
  // Returns an empty vector if the player is incorrect.
  vector<int> helicopters(int pl) const;

  // Returns the information of all posts
  // (the current owner, their location, and their value).
  vector<Post> posts() const;

  // Returns the information of the soldier or helicopter with identifier id.
  // The fields are:
  //    * id           (identifier),
  //    * type         (SOLDIER o HELICOPTER, predefined constants),
  //    * player       (between 0 and 3),
  //    * pos          (position of the soldier or central position of the helicopter, depending on the type),
  //    * life         (life points for the soldier, -1 for helicopters),
  //    * orientation  (where the helicopter is heading to: 0 -> S, 1 -> E, 2 -> N, 3 -> W, -1 for soldiers),
  //    * napalm       (rounds left before napalm is available again, -1 for soldiers).
  //                   (so 0 means napalm can be thrown)
  //    * parachuters  (rounds before each of the parachuters die, sorted increasingly, empty if soldier)
  //                   (so when 0 for a parachuter this is the last round he can jump )
  // The method data(id) returns NUL (a predefined constant of type Data, with all fields at -1)
  // if there is no unit with this identifier.
  Data data(int id) const;

  // Returns the total score of player pl.
  // Returns -1 if the player is incorrect.
  int total_score(int pl) const;

  // Returns the CPU time that has been consumed till now (over the total available time) of player pl.
  // It is a value from 0 (0%) to 1 (100%), or -1 if timed out.
  // Returns -1 if the player is incorrect.
  // NOTE: only returns a sensible value in server executions.
  // In local executions the returned value is irrelevant.
  double status(int pl) const;


  //////// STUDENTS DO NOT NEED TO READ BELOW THIS LINE ////////  

private:

  friend class Info;
  friend class Board;
  friend class Game;
  friend class SecGame;
  friend class Player;

  vector<vector<int>>           land; // land at (i, j): FOREST, GRASS, WATER or MOUNTAIN
  vector<vector<int>>          owner; // owner of post at (i, j): -2 -> none, -1-> free, 0-3-> player
  vector<vector<int>>          value; // value of post at (i, j): -2 -> none, otherwise LOW_VALUE or HIGH_VALUE
  vector<Post>                  post; // information of all posts

  
  vector<vector<int>>            sky; // sky cells that are currently occupied (0 free, 1 occupied)
  vector<vector<int>>           fire; // rounds that the fire at (i, j) will last: 0 -> there is none
  vector<vector<vector<int>>>   iden; // who is at (level, i, j): 0 -> nobody

  map<int, Data>               data_; // data of each id

  vector<vector<int>>        soldier; // for each player, vector of the identifiers of the soldiers
  vector<vector<int>>     helicopter; // for each player, vector of the identifiers of the helicopters

  int                         round_;
  vector<int>           total_score_;
  vector<double>         cpu_status_; // -1 -> dead, 0..1 -> % of cpu time limit
};

inline int State::round() const {
  return round_;
}

inline int State::what(int i, int j) const {
  if (i < 0 or i >= int(land.size()) or j < 0 or j >= int(land[i].size())) {
    cerr << "warning: what(int i, int j) invoked with i = " << i << " and j = " << j << endl;
    return -1;
  }
  return land[i][j];
}

inline int State::fire_time(int i, int j) const {
  if (i < 0 or i >= int(fire.size()) or j < 0 or j >= int(fire[i].size())) {
    cerr << "warning: fire_time(int i, int j) invoked with i = " << i << " and j = " << j << endl;
    return -1;
  }
  return fire[i][j];
}

inline int State::which_soldier(int i, int j) const {
  if (i < 0 or i >= int(iden[SOLDIER].size()) or j < 0 or j >= int(iden[SOLDIER][i].size())) {
    cerr << "warning: which_soldier(int i, int j) invoked with i = " << i << " and j = " << j << endl;
    return -1;
  }
  return iden[SOLDIER][i][j];
}

inline int State::which_helicopter(int i, int j) const {
  if (i < 0 or i >= int(iden[HELICOPTER].size()) or j < 0 or j >= int(iden[HELICOPTER][i].size())) {
    cerr << "warning: which_helicopter(int i, int j) invoked with i = " << i << " and j = " << j << endl;
    return -1;
  }
  return iden[HELICOPTER][i][j];
}

inline int State::post_owner(int i, int j) const {
  if (i < 0 or i >= int(owner.size()) or j < 0 or j >= int(owner[i].size())) {
    cerr << "warning: post_owner(int i, int j) invoked with i = " << i << " and j = " << j << endl;
    return -1;
  }
  return owner[i][j];
}

inline int State::post_value(int i, int j) const {
  if (i < 0 or i >= int(value.size()) or j < 0 or j >= int(value[i].size())) {
    cerr << "warning: post_value(int i, int j) invoked with i = " << i << " and j = " << j << endl;
    return -1;
  }
  return value[i][j];
}

inline vector<int> State::soldiers(int pl) const {
  if (pl < 0 or pl >= int(soldier.size())) {
    cerr << "warning: soldiers(int pl) invoked with pl = " << pl << endl;
    return vector<int>(0);
  }
  return soldier[pl];
}

inline vector<int> State::helicopters(int pl) const {
  if (pl < 0 or pl >= int(helicopter.size())) {
    cerr << "warning: helicopters(int pl) invoked with pl = " << pl << endl;
    return vector<int>(0);
  }
  return helicopter[pl];
}

inline vector<Post> State::posts() const {
  return post;
}

inline Data State::data(int id) const {
  auto p = data_.find(id);
  if (p == data_.end()) {
    cerr << "warning: data(int id) invoked with id = " << id << endl;
    return NUL;
  }
  return p->second;
}

inline int State::total_score(int pl) const {
  if (pl < 0 or pl >= int(total_score_.size())) {
    cerr << "warning: total_score(int pl) invoked with pl = " << pl << endl;
    return -1;
  }
  return total_score_[pl];
}

inline double State::status(int pl) const {
  if (pl < 0 or pl >= int(cpu_status_.size())) {
    cerr << "warning: status(int pl) invoked with pl = " << pl << endl;
    return -1;
  }
  return cpu_status_[pl];
}

#endif
