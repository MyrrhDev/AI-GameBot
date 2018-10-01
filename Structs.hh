#ifndef Structs_hh
#define Structs_hh

#include "Utils.hh"

/** 
 * Contains the definition of some constants,
 * the Position struct,
 * the Post struct,
 * and the Data struct.
 */

// The concrete values of the constants below are irrellevant.

const int FOREST   = 1;
const int GRASS    = 2;
const int WATER    = 3;
const int MOUNTAIN = 4;

const int SOLDIER    = 1;
const int HELICOPTER = 2;

const int FORWARD1          = 1;
const int FORWARD2          = 2;
const int COUNTER_CLOCKWISE = 3;
const int CLOCKWISE         = 4;
const int NAPALM            = 5;


// Struct defining a position.

struct Position {

  int i, j;

  // Uninitialized position.
  Position() { }

  // Position (i, j).
  Position(int i, int j) : i(i), j(j) { }

  // Origin.
  Position(int) : i(0), j(0) { }

  // Comparison of positions.
  bool operator!=(const Position &p) const {
    return i != p.i or j != p.j;
  }
};


// Struct defining a post.

struct Post {
  int     player;  // Player that owns the post, -1 if nobody.
  Position   pos;  // Position of the post.
  int      value;  // Value    of the post.

  // Uninitialized post.
  Post() { }
  
  // Post of the given player at (i, j) with value v.
  Post(int player, int i, int j, int v) : player(player), pos(Position(i, j)), value(v) { }

  // The three methods below are not likely to be useful for students.
  Post(int) : player(0), pos(0), value(0) { }

  Post(const Post &p) : player(p.player), pos(p.pos), value(p.value) { }

  bool operator!=(const Post &p) const {
    return player != p.player or pos != p.pos or value != p.value;
  }
};


// Struct defining the information of a soldier or helicopter.

struct Data {
  int id;              // Identifier.
  int type;            // SOLDIER or HELICOPTER.
  int player;          // 0, 1, 2 or 3.
  Position pos;        // Position (of the center, if helicopter) where it is.
  int life;            // Life points (-1 if helicopter).
  int orientation;     // Where the helicopter is heading:
                       // 0 = South, 1 = East, 2 = North, 3 = West
                       // (-1 if soldier).
  int napalm;          // Rounds before napalm is available again (-1 if soldier).

  vector<int> parachuters;  // Vector of counters for parachuters, sorted increasingly.
                            // The counters are the rounds before the parachuter dies.
                            // (empty if soldier).

  // Constructor for Data with all defined fields.
  Data(int id, int type, int player, int i, int j, int life, int orientation,
       int napalm, const vector<int>& parachuters) :
    id(id),     type(type),               player(player),     pos(i, j),
    life(life), orientation(orientation), napalm(napalm), parachuters(parachuters)
  { }

  // The two methods below are not likely to be useful for students.
  Data() : id(0), type(0), player(0), pos(0), life(0), orientation(0),
           napalm(0), parachuters() { }

  bool operator!=(const Data &d) const {
    return
      id          != d.id          or
      type        != d.type        or
      player      != d.player      or
      pos         != d.pos         or
      life        != d.life        or
      orientation != d.orientation or
      napalm      != d.napalm      or
      parachuters != d.parachuters;
  }
};

//////// STUDENTS DO NOT NEED TO READ BELOW THIS LINE ////////

// Data that is returned to indicate that a procedure call was not OK.
const Data NUL(-1, -1, -1, -1, -1, -1, -1, -1, vector<int>());

#endif
