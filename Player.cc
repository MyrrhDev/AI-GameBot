#include "Player.hh"

void Player::reset (ifstream& is) {

  // Should read what Board::print_state() prints.
  // Should fill the same data structures as
  // Board::Board (istream& is, int seed), except for settings and names.

  *(Action*)this = Action();

  read_grid(is);

  string s;
  is >> s;
  _my_assert(s == "round", "read " + s + " instead of round");
  is >> round_;
  _my_assert(round_ >= 0 and round_ < nb_rounds(), "round is not ok.");

  is >> s;
  _my_assert(s == "total_score", "read " + s + " instead of total_score");
  total_score_ = vector<int>(nb_players(), 0);
  for (auto& ts : total_score_) {
    is >> ts;
    _my_assert(ts >= 0, "read invalid score " + int_to_string(ts));
  }

  is >> s;
  _my_assert(s == "status", "read " + s + " instead of status");
  cpu_status_ = vector<double>(nb_players(), 0);
  for (auto& st : cpu_status_) {
    is >> st;
    _my_assert(st == -1 or (st >= 0 and st <= 1), "read invalid status" + double_to_string(st));
  }

  _my_assert(ok(), "Invariants are not satisfied when leaving Player::reset (ifstream& is)");
}
