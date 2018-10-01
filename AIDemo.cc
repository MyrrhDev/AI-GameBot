#include "Player.hh"

/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */

#define PLAYER_NAME Demo


// DISCLAIMER: The following Demo player is *not* meant to do anything
// sensible. It is provided just to illustrate how to use the API.
// Please use AINull.cc as a template for your player.


struct PLAYER_NAME : public Player {

  /**
   * Factory: returns a new instance of this class.
   * Do not modify this function.
   */
  static Player* factory () {
    return new PLAYER_NAME;
  }

  /**
   * Types and attributes for your player can be defined here.
   */

  static constexpr int I[8] = { 1, 1, 0, -1, -1, -1,  0,  1 };
  static constexpr int J[8] = { 0, 1, 1,  1,  0, -1, -1, -1 };

  typedef vector<int> VE;

  void play_soldier(int id) {
    Data in = data(id);
    int i = in.pos.i;
    int j = in.pos.j;
    int player = in.player;
    for (int k = 0; k < 8; ++k) {
      int ii = i + I[k];
      int jj = j + J[k];
      if (ii >= 0 and ii < MAX and jj >= 0 and jj < MAX) {
        int id2 = which_soldier(ii, jj);
        // If there is an enemy next to us, we attack.
        if (id2 and data(id2).player != player) {
          command_soldier(id, ii, jj);
          return;
        }
      }
    }
    int ii = i + random(-1, 1);
    int jj = j + random(-1, 1);
    if (ii >= 0 and ii < MAX and jj >= 0 and jj < MAX)
      command_soldier(id, ii, jj);
  }

  void play_helicopter(int id) {

    // If we can, we throw napalm.
    if (data(id).napalm == 0) {
      command_helicopter(id, NAPALM);
      return;
    }

    // With probability 20% we turn counter clockwise,
    // otherwise we try to move forward two steps.
    int c = random(1, 5);
    command_helicopter(id, c == 1 ? COUNTER_CLOCKWISE : FORWARD2);
  }

  void throw_parachuter(int helicopter_id) {
    // We get the data of the helicopter...
    Data in = data(helicopter_id);
    // ... and try to throw a parachuter, without even examining the land.
    int ii = in.pos.i + random(-2, 2);
    int jj = in.pos.j + random(-2, 2);
    if (ii >= 0 and ii < MAX and jj >= 0 and jj < MAX)
      command_parachuter(ii, jj);
  }
  
  /**
   * Play method, invoked once per each round.
   */
  virtual void play () {

    int player = me();
    VE H = helicopters(player); // helicopters of my player
    VE S =    soldiers(player); //    soldiers of my player

    // If in a random helicopter I have parachuters, I throw one.
    int helicopter_id = H[random(0, H.size()-1)];
    if (not data(helicopter_id).parachuters.empty()) throw_parachuter(helicopter_id);

    for (int i = 0; i < (int)H.size(); ++i) play_helicopter(H[i]);
    for (int i = 0; i < (int)S.size(); ++i) play_soldier(   S[i]);
  }

};

constexpr int PLAYER_NAME::I[8];
constexpr int PLAYER_NAME::J[8];

/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
