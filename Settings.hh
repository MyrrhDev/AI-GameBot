#ifndef Settings_hh
#define Settings_hh

#include "Structs.hh"

/**
 * Contains a class to store all the game settings that do not change
 * during a game, except the names of the players.
 */

/**
 * Stores most of the game settings.
 */
class Settings {

public:

  static const int NUM_PLAYERS =   4;     // Number of players.
  static const int GAME_ROUNDS = 200;     // Rounds that the game lasts.
  static const int MAX         =  60;     // Number of rows and columns of the board.

  static const int NUM_SOLDIERS    = 20;  // Initial number of soldiers per player.
  static const int NUM_HELICOPTERS =  2;  // Number of helicopters per player.
  static const int NUM_POSTS       = 16;  // Total number of posts.

  static const int LIFE          = 100;   // Initial life points of soldiers.
  static const int FOREST_DAMAGE =  20;   // Minimum damage per attack at forest (maximum damage: the double).
  static const int GRASS_DAMAGE  =  50;   // Minimum damage per attack at grass  (maximum damage: the double).

  static const int ROUNDS_NAPALM = 30;    // Rounds before napalm is available again.
  static const int REACH         =  2;    // Napalm is thrown in the square (2*REACH+1)x(2*REACH+1)
                                          // centered at the helicopter.

  static const int FOREST_BURNS = 10;     // Rounds that  a forest cell burns.
  static const int OTHER_BURNS  =  5;     // Rounds that any other cell burns.
  static const int PROB_FIRE    = 10;     // Probability of propagation of fire.

  static const int ROUNDS_JUMP  = 20;     // Maximum number of rounds to jump.
  static const int MAX_JUMP     =  4;     // Maximum number of parachuters of a player
                                          // who can jump in the same round.

  static const int HIGH_VALUE  = 100;     // Value of the  most valuable posts.
  static const int LOW_VALUE   =  50;     // Value of the least valuable posts.


  /**
   * Returns a string with the game name and version.
   */
  static string version ();

  /**
   * Returns the number of players in the game.
   */
  int nb_players () const;

  /**
   * Returns the number of rounds of the game.
   */
  int nb_rounds () const;

  /**
   * Returns whether pl is a valid player identifier.
   */
  bool player_ok (int pl) const;

  /**
   * Returns whether (i, j) is a position inside the board.
   */
  bool pos_ok (int i, int j) const;

  /**
   * Returns whether p is a position inside the board.
   */
  bool pos_ok (const Position& p) const;


  //////// STUDENTS DO NOT NEED TO READ BELOW THIS LINE ////////


private:

  friend Data;
  friend class Board;
  friend class Game;
  friend class SecGame;
  friend class Player;

  /**
   * Reads the settings from a stream.
   */
  static Settings read_settings (istream& is);

};

inline string Settings::version () {
  return string(GAME_NAME) + " " + string(VERSION);
}

inline int Settings::nb_players () const {
  return NUM_PLAYERS;
}

inline int Settings::nb_rounds () const {
  return GAME_ROUNDS;
}

inline bool Settings::player_ok (int pl) const {
  return pl >= 0 and pl < NUM_PLAYERS;
}

inline bool Settings::pos_ok (int i, int j) const {
  return i >= 0 and i < MAX and j >= 0 and j < MAX;
}

inline bool Settings::pos_ok (const Position& p) const {
  return pos_ok(p.i, p.j);
}


#endif
