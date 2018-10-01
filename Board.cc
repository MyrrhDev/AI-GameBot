//////// STUDENTS DO NOT NEED TO READ BELOW THIS LINE ////////  

#include "Board.hh"
#include "Action.hh"


void Board::compute_scores () {
  for (int pl = 0; pl < nb_players(); ++pl) {
    int p = 0;
    for (int k = 0; k < int(post.size()); ++k)
      if (post[k].player == pl) p += post[k].value;
    p += soldier[pl].size();
    total_score_[pl] += p;
  }
}


void Board::initialize_auxiliary_vectors() {

  int ci[4] = { 1,  0, -1,  0 };
  int cj[4] = { 0,  1,  0, -1 };
  int di[4] = { 1, -1, -1,  1 };
  int dj[4] = { 1,  1, -1, -1 };

  Icros.clear();
  Jcros.clear();
  Ineig.clear();
  Jneig.clear();
  Idiag.clear();
  Jdiag.clear();

  for (int k = 0; k < 4; ++k) {
    Icros.push_back(ci[k]);
    Jcros.push_back(cj[k]);
    Ineig.push_back(ci[k]);
    Jneig.push_back(cj[k]);
    Idiag.push_back(di[k]);
    Jdiag.push_back(dj[k]);
    Ineig.push_back(di[k]);
    Jneig.push_back(dj[k]);
  }
  Ineig.push_back(0);
  Jneig.push_back(0);
}


Board::Board (istream& is, int seed) {

  new_id = 0;
  
  set_random_seed(seed);
  *static_cast<Settings*>(this) = Settings::read_settings(is);

  names_ = vector<string>(nb_players());
  initialize_auxiliary_vectors();

  read_generator_and_grid(is);
  
  round_ = 0;
  total_score_ = vector<int>   (nb_players(), 0);
  cpu_status_  = vector<double>(nb_players(), 0);

  compute_scores();
  
  _my_assert(ok(), "Invariants are not satisfied when leaving Board::Board (istream& is, int seed).");
}


void Board::print_settings (ostream& os) const {
  // Should match the format of *.cnf files, except for the last line of board generation.
  os << version() << endl;
}


void Board::print_names (ostream& os) const {
  os << "names         ";
  for (int pl = 0; pl < nb_players(); ++pl) os << ' ' << name(pl);
  os << endl;
}


void Board::print_state (ostream& os) const {

  // Should start with the same format of Info::read_grid.
  // Then other data describing the state.

  os << "land" << endl;
  os << "   ";
  for (int j = 0; j < MAX; ++j) os << (j / 10);
  os << endl;

  os << "   ";
  for (int j = 0; j < MAX; ++j) os << (j % 10);
  os << endl;
  os << endl;

  for (int i = 0; i < MAX; ++i) {
    os << (i / 10) << (i % 10) << " "; // Adding col numbers.
    for (int j = 0; j < MAX; ++j)
      os << code[land[i][j]];
    os << endl;
  }

  os << endl;
  os << "posts" << endl;
  os << "player" << "\t"
      << "i"      << "\t"
      << "j"      << "\t"
      << "value"  << "\t"
      << endl;
  for (const auto& p : post)
    os << p.player
        << "\t" << p.pos.i 
        << "\t" << p.pos.j 
        << "\t" << p.value
        << endl;
  os << endl;

  os << "units" << endl;
  os << "id"          << "\t"
      << "type"        << "\t"
      << "player"      << "\t"
      << "i"           << "\t"
      << "j"           << "\t"
      << "life"        << "\t"
      << "ori"         << "\t"
      << "napalm"      << "\t"
      << "parachuters" << "\t"
      << endl;
  for (const auto& p: data_) {
    int id     = p.second.id;
    int type   = p.second.type;
    int player = p.second.player;
    int i      = p.second.pos.i;
    int j      = p.second.pos.j;
    int life   = p.second.life;
    int ori    = p.second.orientation;
    int napalm = p.second.napalm;
    int psize  = p.second.parachuters.size();
    os << id 
        << "\t" << type
        << "\t" << player 
        << "\t" << i
        << "\t" << j 
        << "\t" << life 
        << "\t" << ori 
        << "\t" << napalm 
        << "\t" << psize;
    for (int x : p.second.parachuters)
      os << "\t" << x;

    os << "\t" << endl;
  }
  os << "-1" << endl;
  os << endl;

  os << "fire" << endl;
  os << "rounds" << "\t" << "i" << "\t" << "j" << endl;
  for (int i = 0; i < MAX; ++i)
    for (int j = 0; j < MAX; ++j)
      if (fire[i][j] != 0)
        os << fire[i][j] << "\t" << i << "\t" << j << endl;
  os << "-1" << endl << endl;

  os << "round " << round() << endl << endl;
  
  os << "total_score" << endl;
  for (int pl = 0; pl < nb_players(); ++pl)
    os << total_score(pl) << "\t";
  os << endl << endl;

  os << "status" << endl;
  for (int pl = 0; pl < nb_players(); ++pl)
    os << status(pl) << "\t";
  os << endl << endl;
}


void Board::print_results () const {
  int max_score = 0;
  vector<int> v;
  for (int pl = 0; pl < nb_players(); ++pl) {
    cerr << "info: player " << name(pl)
         << " got score " << total_score(pl) << endl;
    if (total_score(pl) > max_score) {
      max_score = total_score(pl);
      v = vector<int>(1, pl);
    }
    else if (total_score(pl) == max_score) v.push_back(pl);
  }

  cerr << "info: player(s)";
  for (int pl : v) cerr << " " << name(pl);
  cerr << " got top score" << endl;
}


void Board::next(const vector<Action>& act, ostream& os) {

  _my_assert(ok(), "Invariants are not satisfied when entering Board::next(const vector<Action>& act, ostream& os).");

  new_id = 1 + nb_players() * round() * (NUM_SOLDIERS + NUM_HELICOPTERS);
  
  vector<vector<Position>> commands_parachuters(nb_players());
  for (int pl = 0; pl < nb_players(); ++pl)
    commands_parachuters[pl] = act[pl].commands_parachuters;
  parachuters_jump(commands_parachuters);

  vector<pair<int,int>> commands_helicopters;
  for (int pl = 0; pl < nb_players(); ++pl)
    for (auto& p : act[pl].commands_helicopters)
      commands_helicopters.push_back(p);
  do_helicopters(commands_helicopters);

  vector<pair<int,Position>> commands_soldiers;
  for (int pl = 0; pl < nb_players(); ++pl)
    for (auto& p : act[pl].commands_soldiers)
      commands_soldiers.push_back(p);
  do_soldiers(commands_soldiers);

  propagate_fire();
  compute_scores();

  ++round_;

  _my_assert(ok(), "Invariants are not satisfied when leaving Board::next(const vector<Action>& act, ostream& os).");
}


// Decrements the fire counters, transforms burned forests into grass,
// propagates fires, and checks if any soldier is burned in the propagation.
inline void Board::propagate_fire() {

  vector<vector<int>> B(MAX, vector<int>(MAX, 0));
  for (int i = 0; i < MAX; ++i)
    for (int j = 0; j < MAX; ++j)
      if (fire[i][j]) {
        --fire[i][j];
        if (land[i][j] == FOREST) {
          if (fire[i][j] == 0) land[i][j] = GRASS;
          for (int k = 0; k < 4; ++k) {
            int ii = i + Icros[k];
            int jj = j + Jcros[k];
            if (land[ii][jj] == FOREST and fire[ii][jj] == 0)
              B[ii][jj] = true;
          }
        }
      }

  for (int i = 0; i < MAX; ++i)
    for (int j = 0; j < MAX; ++j)
      if (B[i][j] and random(1, 100) <= PROB_FIRE) {
        fire[i][j] = FOREST_BURNS;
        int id = iden[SOLDIER][i][j];
        if (id) {
          int new_player = different(data_[id].player);
          int helicopter_id = helicopter[new_player][random(0, int(helicopter[new_player].size())-1)];
          data_[helicopter_id].parachuters.push_back(ROUNDS_JUMP);
          kill_soldier(id);
        }
      }
}


// Executes the given instruction to a soldier, checking that he goes
// to a valid place, he is still alive, whether he wants to go into a
// fire, and if there is another soldier where he wants to go.
// Otherwise he moves and checks whether there is a post which was not
// already owned by the same player, and updates data consistently.
// If yes, and he is from another player, computes the damage
// according to the land, and if live points reach 0, kills it.
void Board::do_soldier(int id, int i2, int j2) {

  if (i2 < 0 or i2 >= MAX or j2 < 0 or j2 >= MAX) return;
  if (land[i2][j2] != FOREST and land[i2][j2] != GRASS) return;

  auto p = data_.find(id);
  if (p == data_.end()) return;

  int player = p->second.player;
  if (fire[i2][j2]) {
    int new_player = different(player);
    int helicopter_id = helicopter[new_player][random(0, int(helicopter[new_player].size())-1)];
    data_[helicopter_id].parachuters.push_back(ROUNDS_JUMP);
    kill_soldier(id);
    return;
  }

  int i = p->second.pos.i;
  int j = p->second.pos.j;
  int id2 = iden[SOLDIER][i2][j2];

  if (id2 == id) return;

  if (id2 == 0) {
    swap(iden[SOLDIER][i][j], iden[SOLDIER][i2][j2]);
    p->second.pos = Position(i2, j2);
    if (owner[i2][j2] == -2 or owner[i2][j2] == player) return;

    owner[i2][j2] = player;
    int k = 0;
    while (post[k].pos != Position(i2, j2)) ++k;
    post[k].player = player;
    return;
  }

  auto q = data_.find(id2);
  int player2 = q->second.player;
  if (player2 == player) return;

  int a = (land[i2][j2] == FOREST ? FOREST_DAMAGE : GRASS_DAMAGE);
  q->second.life -= random(a, 2*a);
  if (q->second.life > 0) return;

  int helicopter_id = helicopter[player][random(0, int(helicopter[player].size())-1)];
  data_[helicopter_id].parachuters.push_back(ROUNDS_JUMP);
  kill_soldier(id2);
  return;
}


// Executes all the instructions given to soldiers in a random order.
void Board::do_soldiers(vector<pair<int, Position>>& V) {

  int n = V.size();
  vector<int> perm = random_permutation(n);
  for (int k = 0; k < n; ++k) {
    int z = perm[k];
    do_soldier(V[z].first, V[z].second.i, V[z].second.j);
  }
}


// Executes the instruction given to a helicopter. If the instruction
// is to throw napalm, we have to check it can do it, mark the cells
// with fire (the +1 is because propagate_fire removes 1 to all cells
// at the end), kill soldiers, and assign them to the corresponding
// player. If the instruction is to move, first we compute how many
// steps are asked (1 or 2) and how many can be done, and we keep the minimum.
void Board::do_helicopter(int id, int ins) {

  auto p = data_.find(id);
  int i = p->second.pos.i;
  int j = p->second.pos.j;
  int ori = p->second.orientation;

  if (ins == COUNTER_CLOCKWISE) {
    p->second.orientation = (ori + 1)%4;
    return;
  }

  if (ins == CLOCKWISE) {
    p->second.orientation = (ori + 3)%4;
    return;
  }

  if (ins == NAPALM) {
    if (p->second.napalm) return;

    int player = p->second.player;
    for (int k = -REACH; k <= REACH; ++k)
      for (int l = -REACH; l <= REACH; ++l) {
        int ii = i + k;
        int jj = j + l;
        if (ii >= 0 and ii < MAX and jj >= 0 and jj < MAX) {
          int t = land[ii][jj];
          if (t != MOUNTAIN) {
            fire[ii][jj] = 1 + (t == FOREST ? FOREST_BURNS : OTHER_BURNS);
            int id2 = iden[SOLDIER][ii][jj];
            if (id2) {
              int player2 = data_[id2].player;
              if (player2 != player) {
                int helicopter_id = helicopter[player][random(0, int(helicopter[player].size())-1)];
                data_[helicopter_id].parachuters.push_back(ROUNDS_JUMP);
              }
              else {
                int new_player = different(player);
                int helicopter_id = helicopter[new_player][random(0, int(helicopter[new_player].size())-1)];
                data_[helicopter_id].parachuters.push_back(ROUNDS_JUMP);
              }
              kill_soldier(id2);
            }
          }
        }
      }
    p->second.napalm = ROUNDS_NAPALM + 1;
    return;
  }

  bool ok = true;
  int k = 2;
  int fi = (ins == FORWARD1 ? 3 : 4);
  while (ok and ++k <= fi) {
    for (int l = -2; ok and l <= 2; ++l) {
      int ii = i + k*Icros[ori] + l*Jcros[ori];
      int jj = j + k*Jcros[ori] + l*Icros[ori];
      if (land[ii][jj] == MOUNTAIN or sky[ii][jj]) ok = false;
    }
  }
  int i2 = i + (k - 3)*Icros[ori];
  int j2 = j + (k - 3)*Jcros[ori];
  p->second.pos.i = i2;
  p->second.pos.j = j2;
  unmark_helicopter(i, j);
  mark_helicopter(i2, j2);
  iden[HELICOPTER][i][j] = 0;
  iden[HELICOPTER][i2][j2] = id;
}


// Executes all instructions given to helicopters in a random order.
// Also decrements all the counters of napalm.
void Board::do_helicopters(vector<pair<int,int>>& V) {

  int n = V.size();
  vector<int> perm = random_permutation(n);
  for (int k = 0; k < n; ++k) {
    int z = perm[k];
    int id = V[z].first;
    do_helicopter(id, V[z].second);
  }

  for (int pl = 0; pl < nb_players(); ++pl)
    for (int k = 0; k < (int)helicopter[pl].size(); ++k) {
      auto p = data_.find(helicopter[pl][k]);
      if (p->second.napalm) --p->second.napalm;
    }
}


// Executes all instructions given to parachuters. There is no need to
// do it in a random order because there are no possible
// interferences. Start decrementing the counters of the parachuters
// who do not jump in this round. (and killing those that have
// expired). After, it is checked the target cell is forest or grass,
// that there is no fire, that there is no one, and that it is below
// one of the two helicopters of the player. If this fails, the
// parachuter dies and goes to another player. If he jumps, we check
// if he goes to a post.
void Board::parachuters_jump(const vector<vector<Position>>& commands_parachuters) {

  vector<int> new_para(nb_players(), 0);
  for (int pl = 0; pl < nb_players(); ++pl) {
    for (int l = 0; l < (int)helicopter[pl].size(); ++l) {
      int helicopter_id = helicopter[pl][l];
      int i2 = data_[helicopter_id].pos.i;
      int j2 = data_[helicopter_id].pos.j;

      int cnt = 0;
      for (int k = 0; k < int(commands_parachuters[pl].size()); ++k) {
        int i = commands_parachuters[pl][k].i;
        int j = commands_parachuters[pl][k].j;
        if (abs(i2 - i) <= 2 and abs(j2 - j) <= 2)
          ++cnt;
      }
      vector<int> B;
      while (cnt < int(data_[helicopter_id].parachuters.size())) {
        int a = data_[helicopter_id].parachuters[cnt] - 1;
        if (a) B.push_back(a);
        else ++new_para[different(pl)];
        ++cnt;
      }
      swap(data_[helicopter_id].parachuters, B);
    }
  }

  for (int pl = 0; pl < nb_players(); ++pl)
    for (int r = 0; r < new_para[pl]; ++r) {
      int helicopter_id = helicopter[pl][random(0, int(helicopter[pl].size())-1)];
      data_[helicopter_id].parachuters.push_back(ROUNDS_JUMP);
    }


  for (int pl = 0; pl < nb_players(); ++pl)
    for (int k = 0; k < (int)commands_parachuters[pl].size(); ++k) {
      int i = commands_parachuters[pl][k].i;
      int j = commands_parachuters[pl][k].j;
      if (i < 0 or i >= MAX or j < 0 or j >= MAX) {
        int new_player = different(pl);
        int helicopter_id = helicopter[new_player][random(0, int(helicopter[new_player].size())-1)];
        data_[helicopter_id].parachuters.push_back(ROUNDS_JUMP);
      }
      else {
        int t = land[i][j];
        int id2 = iden[SOLDIER][i][j];
        int pl2 = (id2 ? data_[id2].player : -1);
        if ((t != FOREST and t != GRASS) or fire[i][j] or pl2 == pl) {
          int new_player = different(pl);
          int helicopter_id = helicopter[new_player][random(0, int(helicopter[new_player].size())-1)];
          data_[helicopter_id].parachuters.push_back(ROUNDS_JUMP);
        }
        else if (pl2 != -1) {
          int helicopter_id = helicopter[pl2][random(0, int(helicopter[pl2].size())-1)];
          data_[helicopter_id].parachuters.push_back(ROUNDS_JUMP);
        }
        else {
          bool ok = false;
          for (int l = 0; not ok and l < (int)helicopter[pl].size(); ++l) {
            int id = helicopter[pl][l];
            int i2 = data_[id].pos.i;
            int j2 = data_[id].pos.j;
            if (abs(i2 - i) <= 2 and abs(j2 - j) <= 2) ok = true;
          }

          if (ok) {
            soldier[pl].push_back(new_soldier(pl, i, j));
            if (owner[i][j] != -2 and owner[i][j] != pl) {
              owner[i][j] = pl;
              int p = 0;
              while (post[p].pos != Position(i, j)) ++p;
              post[p].player = pl;
            }
          }
          else {
            int new_player = different(pl);
            int helicopter_id = helicopter[new_player][random(0, int(helicopter[new_player].size())-1)];
            data_[helicopter_id].parachuters.push_back(ROUNDS_JUMP);
          }
        }
      }
    }
}
