//////// STUDENTS DO NOT NEED TO READ BELOW THIS LINE ////////  

#ifndef Info_hh
#define Info_hh


#include "Settings.hh"
#include "State.hh"


/**
 * Contains a class to store most of the information of the game.
 */


/**
 * Stores all the information of the game,
 * except the names of the vectors and the random generator of the board.
 */
class Info : public Settings, public State {
  
  friend class Game;
  friend class SecGame;

public:

  static const char code[5];
  
  /**
   * Reads the grid of the board.
   * Should fill the same data structures as a board generator.
   */
  void read_grid (istream& is) {

    string s;
    is >> s;
    _my_assert(s == "land", "read " + s + " instead of land");

    is >> s; // Read 1st line of column labels.
    _my_assert(s == "000000000011111111112222222222333333333344444444445555555555",
               "expected 1st line of column labels");

    is >> s; // Read 2nd line of column labels.
    _my_assert(s == "012345678901234567890123456789012345678901234567890123456789",
               "expected 2nd line of column labels");
    
    land = vector<vector<int>>(MAX, vector<int>(MAX));

    for (int i = 0; i < MAX; ++i) {
      int aux;
      is >> aux;
      _my_assert(aux == i, "read " + int_to_string(aux) + " instead of " + int_to_string(i));
      for (int j = 0; j < MAX; ++j) {
        char c;
        is >> c;
        if      (c == code[FOREST  ]) land[i][j] = FOREST;
        else if (c == code[GRASS   ]) land[i][j] = GRASS;
        else if (c == code[WATER   ]) land[i][j] = WATER;
        else if (c == code[MOUNTAIN]) land[i][j] = MOUNTAIN;
        else _unreachable();
      }
    }

    owner = vector<vector<int>>(MAX, vector<int>(MAX, -2));
    value = vector<vector<int>>(MAX, vector<int>(MAX, -2));
    post  = vector<Post>(NUM_POSTS);
    
    is >> s; _my_assert(s == "posts",  "read " + s + " instead of posts" );
    is >> s; _my_assert(s == "player", "read " + s + " instead of player");
    is >> s; _my_assert(s == "i",      "read " + s + " instead of i"     );
    is >> s; _my_assert(s == "j",      "read " + s + " instead of j"     );
    is >> s; _my_assert(s == "value",  "read " + s + " instead of value" );
    for (int k = 0; k < NUM_POSTS; ++k) {
      int player, i, j, v;
      is >> player >> i >> j >> v;
      _my_assert(-1 <= player and player <= 3, "read wrong player " + int_to_string(player));
      _my_assert(pos_ok(i, j),
                 string("read wrong position")
                        + " i = " + int_to_string(i)
                        + " j = " + int_to_string(j));
      post[k].player = player;
      post[k].pos.i = i;
      post[k].pos.j = j;
      post[k].value = v;
      owner[i][j] = player;
      value[i][j] = v;
    }

    sky  = vector<vector<int>>(MAX, vector<int>(MAX, 0));
    fire = vector<vector<int>>(MAX, vector<int>(MAX, 0));
    iden = vector<vector<vector<int>>>(3, vector<vector<int>>(MAX, vector<int>(MAX, 0)));

    soldier    = vector<vector<int>>(nb_players());
    helicopter = vector<vector<int>>(nb_players());

    is >> s; _my_assert(s == "units",       "read " + s + " instead of units"      );
    is >> s; _my_assert(s == "id",          "read " + s + " instead of id"         );
    is >> s; _my_assert(s == "type",        "read " + s + " instead of type"       );
    is >> s; _my_assert(s == "player",      "read " + s + " instead of player"     );
    is >> s; _my_assert(s == "i",           "read " + s + " instead of i"          );
    is >> s; _my_assert(s == "j",           "read " + s + " instead of j"          );
    is >> s; _my_assert(s == "life",        "read " + s + " instead of life"       );
    is >> s; _my_assert(s == "ori",         "read " + s + " instead of ori"        );
    is >> s; _my_assert(s == "napalm",      "read " + s + " instead of napalm"     );
    is >> s; _my_assert(s == "parachuters", "read " + s + " instead of parachuters");

    data_.clear();
    int id;
    while (is >> id and id != -1) {
      int type, player, i, j, life, ori, napalm, parac_size;
      is >> type >> player >> i >> j >> life >> ori >> napalm >> parac_size;
      vector<int> par(parac_size);
      for (int k = 0; k < parac_size; ++k)
        is >> par[k];

      data_[id] = Data(id, type, player, i, j, life, ori, napalm, par);
      iden[type][i][j] = id;
      if (type == HELICOPTER) {
        for (int di = -2; di <= 2; ++di)
          for (int dj = -2; dj <= 2; ++dj) {
            int ii = i + di;
            int jj = j + dj;
            if (pos_ok(ii, jj)) sky[ii][jj] = 1;
          }
        helicopter[player].push_back(id);
      }
      else soldier[player].push_back(id);
    }

    is >> s; _my_assert(s == "fire",   "read " + s + " instead of fire"  );
    is >> s; _my_assert(s == "rounds", "read " + s + " instead of rounds");
    is >> s; _my_assert(s == "i",      "read " + s + " instead of i"     );
    is >> s; _my_assert(s == "j",      "read " + s + " instead of j"     );
    int rounds;
    while (is >> rounds and rounds != -1) {
      int i, j;
      is >> i >> j;
      _my_assert(0 < rounds, "read fire lasts " + int_to_string(rounds) + "rounds");
      _my_assert(pos_ok(i, j), string("read wrong position")
                 + " i = " + int_to_string(i)
                 + " j = " + int_to_string(j));
      fire[i][j] = rounds;
    }
  }


  /**
   * Checks invariants are preserved.
   */
  bool ok();

};


#endif
