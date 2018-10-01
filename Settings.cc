#include "Settings.hh"

const int Settings::NUM_PLAYERS;
const int Settings::GAME_ROUNDS;
const int Settings::MAX;
const int Settings::NUM_SOLDIERS;
const int Settings::NUM_HELICOPTERS;
const int Settings::NUM_POSTS;
const int Settings::LIFE;
const int Settings::FOREST_DAMAGE;
const int Settings::GRASS_DAMAGE;
const int Settings::ROUNDS_NAPALM;
const int Settings::REACH;
const int Settings::FOREST_BURNS;
const int Settings::OTHER_BURNS;
const int Settings::PROB_FIRE;
const int Settings::ROUNDS_JUMP;
const int Settings::MAX_JUMP;
const int Settings::HIGH_VALUE;
const int Settings::LOW_VALUE;


Settings Settings::read_settings (istream& is) {
  // Should match the format of *.cnf files, except for the last line of board generation.
  Settings r;
  string s, v;

  // Version, compared part by part.
  istringstream vs(version());
  while (!vs.eof()) {
    is >> s;
    vs >> v;
    _my_assert(s == v, "Problems when reading.");
  };

  // In this game nothing else, because all parameters are embedded
  // into the Settings class.

  return r;
}
