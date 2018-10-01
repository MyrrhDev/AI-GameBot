//////// STUDENTS DO NOT NEED TO READ BELOW THIS LINE ////////  

#ifndef Board_hh
#define Board_hh


#include "Info.hh"
#include "Action.hh"
#include "Random.hh"


/**
 * Contains the Board class, with all the game information,
 * plus the names of the players and a random generator.
 */


/**
 * Manages a random generator and the information of the board.
 */
class Board : public Info, public Random_generator {

  friend class Game;
  friend class SecGame;

  vector<string> names_;


  // Fresh identifier.
  
  int new_id;

  
  // Auxiliary vectors.
  
  vector<int> Icros, Jcros;  // Neighbours in cross.
  vector<int> Idiag, Jdiag;  // Neighbours in diagonal.
  vector<int> Ineig, Jneig;  // All neighbours, including the center.

  
  /**
   * Reads the generator method, and generates or reads the grid.
   */
  void read_generator_and_grid (istream& is) {
    string gen;
    is >> gen;
    _my_assert(gen == "FIXED", "Only fixed generator is allowed.");
    read_grid(is);
  }

  /**
   * Computes the scores of all players.
   */
  void compute_scores ();


  // Auxiliary methods.
  void initialize_auxiliary_vectors();

  int  new_identifier();
  int  new_soldier   (int pl, int i, int j);
  int  new_helicopter(int pl, int i, int j, int orientation);

  void   mark_helicopter(int i, int j);
  void unmark_helicopter(int i, int j);

  void kill_soldier(int id);
  int  different(int e);
  void propagate_fire();

  void do_soldier(int id, int i2, int j2);
  void do_soldiers(vector<pair<int, Position>>& V);
  void do_helicopter(int id, int ins);
  void do_helicopters(vector<pair<int,int>>& V);
  void parachuters_jump(const vector<vector<Position>>& commands_parachuters);
  
public:

  /**
   * Construct a board by reading information from a stream.
   */
  Board (istream& is, int seed);

  /**
   * Returns the name of a player.
   */
  inline string name (int player) const {
    _my_assert(player_ok(player), "Player is not ok.");
    return names_[player];
  }

  /**
   * Prints the board settings to a stream.
   */
  void print_settings (ostream& os) const;

  /**
   * Prints the name players to a stream.
   */
  void print_names (ostream& os) const;

  /**
   * Prints the state of the board to a stream.
   */
  void print_state (ostream& os) const;

  /**
   * Prints the results and the names of the winning players.
   */
  void print_results () const;

  /**
   * Computes the next board aplying the given actions to the current board.
   * It also prints to os the actual actions performed.
   */
  void next (const vector<Action>& act, ostream& os);

};


inline int Board::new_identifier() {
  return new_id++;
}

inline int Board::new_soldier(int pl, int i, int j) {
  int id = new_identifier();
  _my_assert(iden[SOLDIER][i][j] ==  0, "Position is full.");
  iden[SOLDIER][i][j] = id;
  _my_assert(not data_.count(id), "Identifier is not fresh.");
  data_[id] = Data(id, SOLDIER, pl, i, j, LIFE, -1, -1, vector<int>());
  return id;
}

inline void Board::mark_helicopter(int i, int j) {
  for (int k = -2; k <= 2; ++k)
    for (int l = -2; l <= 2; ++l) sky[i+k][j+l] = true;
}

inline void Board::unmark_helicopter(int i, int j) {
  for (int k = -2; k <= 2; ++k)
    for (int l = -2; l <= 2; ++l) sky[i+k][j+l] = false;
}

inline int Board::new_helicopter(int pl, int i, int j, int orientation) {
  int id = new_identifier();
  _my_assert(iden[HELICOPTER][i][j] == 0, "Position is full.");
  iden[HELICOPTER][i][j] = id;
  _my_assert(not data_.count(id), "Identifier is not fresh.");
  data_[id] = Data(id, HELICOPTER, pl, i, j, -1, orientation, ROUNDS_NAPALM, vector<int>());
  mark_helicopter(i, j);
  return id;
}

inline void Board::kill_soldier(int id) {
  auto p = data_.find(id);
  int player = p->second.player;
  int i = p->second.pos.i;
  int j = p->second.pos.j;
  iden[SOLDIER][i][j] = 0;
  data_.erase(p);

  vector<int> B;
  for (int k = 0; k < (int)soldier[player].size(); ++k) {
    int a = soldier[player][k];
    if (a != id) B.push_back(a);
  }
  swap(soldier[player], B);
}

inline int Board::different(int player) {
  int pl;
  do pl = random(0, 3);
  while (pl == player);
  return pl;
}

#endif
