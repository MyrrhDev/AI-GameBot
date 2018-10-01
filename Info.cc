//////// STUDENTS DO NOT NEED TO READ BELOW THIS LINE ////////  

#include "Info.hh"

const char Info::code[5] = {' ',
                            ':',  // FOREST
                            '.',  // GRASS
                            'O',  // WATER
                            'X'}; // MOUNTAIN

bool Info::ok() {

  if (not(0 <= round() and round() <= GAME_ROUNDS)) {
    cerr << "error: invalid round " << round() << endl;
    return false;
  }

  if (land.size() != MAX or land[0].size() != MAX) {
    cerr << "error: wrong dimensions of land" << endl;
    return false;
  }

  for (int i = 0; i < MAX; ++i)
    for (int j = 0; j < MAX; ++j) 
      if (land[i][j] != FOREST and
          land[i][j] != GRASS  and
          land[i][j] != WATER  and
          land[i][j] != MOUNTAIN) {
        cerr << "error: wrong value in land" << endl;
        return false;
      }

  for (int k = 0; k < MAX; ++k)
    if (land[  k  ][  0  ] != MOUNTAIN or
        land[  k  ][MAX-1] != MOUNTAIN or
        land[  0  ][  k  ] != MOUNTAIN or
        land[MAX-1][  k  ] != MOUNTAIN) {
      cerr << "error: board should be surrounded by mountains" << endl;
      return false;
    }
  
  if (owner.size() != MAX or owner[0].size() != MAX) {
    cerr << "error: wrong dimensions of owner" << endl;
    return false;
  }

  if (value.size() != MAX or value[0].size() != MAX) {
    cerr << "error: wrong dimensions of value" << endl;
    return false;
  }

  int cnt_post;

  cnt_post= 0;
  for (int i = 0; i < MAX; ++i)
    for (int j = 0; j < MAX; ++j) {
      if (owner[i][j] != -2) {
        ++cnt_post;
        if (owner[i][j] != -1 and not player_ok(owner[i][j])) {
          cerr << "error: invalid post owner" << endl;
          return false;
        }
      }
    }
  if (cnt_post != NUM_POSTS) {
    cerr << "error: mismatch in number of posts" << endl;
    return false;
  }

  cnt_post = 0;
  for (int i = 0; i < MAX; ++i)
    for (int j = 0; j < MAX; ++j) {
      if (value[i][j] != -2) {
        ++cnt_post;
        if (value[i][j] != LOW_VALUE and value[i][j] != HIGH_VALUE) {
          cerr << "error: invalid post value" << endl;
          return false;
        }
      }
    }
  if (cnt_post != NUM_POSTS) {
    cerr << "error: mismatch in number of posts" << endl;
    return false;
  }
  if (int(post.size()) != NUM_POSTS) {
    cerr << "error: mismatch in number of posts" << endl;
    return false;
  }
  for (int k = 0; k < int(post.size()); ++k) {
    int player = post[k].player;
    int i = post[k].pos.i;
    int j = post[k].pos.j;
    int v = post[k].value;
    if (owner[i][j] != player) {
      cerr << "error: mismatch in post owner" << endl;
      return false;
    }
    if (value[i][j] != v) {
      cerr << "error: mismatch in post value" << endl;
      return false;
    }
    if (land[i][j] != FOREST and land[i][j] != GRASS) {
      cerr << "error: posts should be in forest or grass" << endl;
      return false;
    }
  }
  
  for (int i = 0; i < MAX; ++i)
    for (int j = 0; j < MAX; ++j) {
      if (fire[i][j] < 0) {
        cerr << "error: fire cannot have negative value " << fire[i][j]
             << " at i = " << i << ", j = " << j << endl;
        return false;
      }
      if (land[i][j] == MOUNTAIN and fire[i][j] > 0) {
        cerr << "error: there cannot be fire in mountains"
             << " at i = " << i << ", j = " << j << endl;
        return false;
      }
      if (land[i][j] == FOREST and fire[i][j] > FOREST_BURNS) {
        cerr << "error: exceeding fire value " << fire[i][j]
             << " at forest cell i = " << i << ", j = " << j << endl;
        return false;
      }
      if (land[i][j] != FOREST and fire[i][j] > OTHER_BURNS) {
        cerr << "error: exceeding fire value " << fire[i][j]
             << " at non-forest cell i = " << i << ", j = " << j << endl;
        return false;
      }
    }

  int cnt_ids = 0;
  int cnt_sky = 0;
  for (int i = 0; i < MAX; ++i)
    for (int j = 0; j < MAX; ++j) {
      int id_soldier = iden[SOLDIER][i][j];
      if (id_soldier != 0) {
        if (id_soldier < 0) {
          cerr << "error: identifiers should be positive" << endl;
          return false;
        }
        auto it = data_.find(id_soldier);
        if (it == data_.end()) {
          cerr << "error: unregistered soldier identifier " << id_soldier << endl;
          return false;
        }
        Data d = it->second;
        if (d.pos != Position(i, j)) {
          cerr << "error: mismatch with position of soldier " << id_soldier << endl;
          return false;
        }
        if (d.type != SOLDIER) {
          cerr << "error: mismatch with type of unit " << id_soldier << endl;
          return false;
        }
        if (land[i][j] == MOUNTAIN or land[i][j] == WATER) {
          cerr << "error: soldier " << id_soldier << " in forbidden land "
               << " at i = " << i << ", j = " << j << endl;
          return false;
        }
        if (owner[i][j] != -2 and owner[i][j] != d.player) {
          cerr << "error: mismatch with conquered post" << endl;
          return false;
        }
        ++cnt_ids;
      }
      int id_helicopter = iden[HELICOPTER][i][j];
      if (id_helicopter != 0) {
        if (id_helicopter < 0) {
          cerr << "error: identifiers should be positive" << endl;
          return false;
        }
        auto it = data_.find(id_helicopter);
        if (it == data_.end()) {
          cerr << "error: unregistered helicopter identifier " << id_helicopter << endl;
          return false;
        }
        Data d = it->second;
        if (d.pos != Position(i, j)) {
          cerr << "error: mismatch with position of helicopter " << id_helicopter << endl;
          return false;
        }
        if (d.type != HELICOPTER) {
          cerr << "error: mismatch with type of unit " << id_helicopter << endl;
          return false;
        }
        for (int di = -2; di <= 2; ++di)
          for (int dj = -2; dj <= 2; ++dj) {
            int ii = i + di;
            int jj = j + dj;
            if (not pos_ok(ii, jj)) {
              cerr << "error: helicopter " << id_helicopter << " out of board"<< endl;
              return false;
            }
            if (land[ii][jj] == MOUNTAIN) {
              cerr << "error: helicopter " << id_helicopter << " in forbidden land "
                   << " at i = " << ii << ", j = " << jj << endl;
              return false;
            }
            if (not sky[ii][jj]) {
                  cerr << "error: mismatch with sky of helicopter " << id_helicopter
                       << " at i = " << ii << ", j = " << jj << endl;
              return false;
            }
            ++cnt_sky;
          }
        ++cnt_ids;
      }
    }
  if (cnt_ids != int(data_.size())) {
    cerr << "error: mismatch in sizes" << endl;
    return false;
  }

  int cnt_sky2 = 0;
  for (int i = 0; i < MAX; ++i)
    for (int j = 0; j < MAX; ++j)
      if (sky[i][j])
        ++cnt_sky2;
  if (cnt_sky != cnt_sky2) {
    cerr << "error: mismatch in number of occupied sky cells" << endl;
    return false;
  }

  for (const auto& p : data_) {
    int id = p.first;
    Data d = p.second;
    if (id != d.id) {
      cerr << "error: mismatch in identifiers" << endl;
      return false;
    }
    if (d.type != SOLDIER and d.type != HELICOPTER) {
      cerr << "error: invalid type" << endl;
      return false;
    }
    if (not player_ok(d.player)) {
      cerr << "error: invalid player" << endl;
      return false;
    }
    if (d.type == HELICOPTER and d.life != -1) {
      cerr << "error: helicopters never die" << endl;
      return false;
    }
    if (d.type == SOLDIER and (d.life < 0 or d.life > LIFE)) {
      cerr << "error: invalid life value" << endl;
      return false;
    }
    if (d.type == HELICOPTER and (d.orientation < 0 or d.orientation > 3)) {
      cerr << "error: invalid orientation value" << endl;
      return false;
    }
    if (d.type == SOLDIER and d.orientation != -1) {
      cerr << "error: soldiers do not have orientation" << endl;
      return false;
    }
    if (d.type == HELICOPTER and (d.napalm < 0 or d.napalm > ROUNDS_NAPALM)) {
      cerr << "error: invalid napalm value" << endl;
      return false;
    }
    if (d.type == SOLDIER and d.napalm != -1) {
      cerr << "error: soldiers do not have napalm" << endl;
      return false;
    }
    if (d.type == SOLDIER and not d.parachuters.empty()) {
      cerr << "error: soldiers do not have parachuters" << endl;
      return false;
    }
    for (int r : d.parachuters)
      if (r < 0 or r > ROUNDS_JUMP) {
        cerr << "error: parachuters with invalid number of rounds for jumping" << endl;
        return false;
      }
  }

  int n_units = 0;
  for (int pl = 0; pl < nb_players(); ++pl)
    n_units += soldier[pl].size() + helicopter[pl].size();

  if (n_units != int(data_.size())) {
    cerr << "error: mismatch in number of units" << endl;
    return false;
  }

  for (int pl = 0; pl < nb_players(); ++pl) {
    for (int id : soldier[pl]) {
      auto it = data_.find(id);
      if (it == data_.end()) {
        cerr << "error: unregistered soldier" << endl;
        return false;
      }
      Data d = it->second;
      if (d.type != SOLDIER) {
        cerr << "error: identifier should be soldier" << endl;
        return false;
      }
      if (pl != d.player) {
        cerr << "error: mismatch in players of soldiers" << endl;
        return false;
      }
    }
    for (int id : helicopter[pl]) {
      auto it = data_.find(id);
      if (it == data_.end()) {
        cerr << "error: unregistered helicopter" << endl;
        return false;
      }
      Data d = it->second;
      if (d.type != HELICOPTER) {
        cerr << "error: identifier should be helicopter" << endl;
        return false;
      }
      if (pl != d.player) {
        cerr << "error: mismatch in players of helicopters" << endl;
        return false;
      }
    }
  }

  int n_soldiers_or_paracs = 0;
  for (int pl = 0; pl < nb_players(); ++pl) {
    n_soldiers_or_paracs += soldier[pl].size();
    for (auto id : helicopter[pl]) {
      auto it = data_.find(id);
      n_soldiers_or_paracs += it->second.parachuters.size();
    }
  }
  if (n_soldiers_or_paracs != nb_players() * NUM_SOLDIERS) {
    cerr << "error: mismatch in the number of soldiers and parachuters" << endl;
    return false;
  }
  
  return true;
}
