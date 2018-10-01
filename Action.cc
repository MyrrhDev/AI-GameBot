//////// STUDENTS DO NOT NEED TO READ BELOW THIS LINE ////////  

#include "Action.hh"

Action::Action (istream& is) {

  commands_soldiers   .clear();
  commands_helicopters.clear();
  commands_parachuters.clear();

  int id;
  while (is >> id and id != -1) {
    int i, j;
    if (is >> i >> j) 
      commands_soldiers[id] = Position(i, j);
    else {
      cerr << "warning: could not read completely data of id " << id << endl;
      return;
    }
  }

  while (is >> id and id != -1) {
    int d;
    if (is >> d) 
      commands_helicopters[id] = d;  
    else {
      cerr << "warning: could not read completely data of id " << id << endl;
      return;
    }
  }

  int i, j;
  while (is >> i and i != -1) {
    if (is >> j) 
      commands_parachuters.push_back(Position(i, j));  
    else {
      cerr << "warning: could not read completely data" << endl;
      return;
    }
  }
}

void Action::print (ostream& os) const {
  for (const auto& p : commands_soldiers)
    os << p.first << " " << p.second.i << " " << p.second.j << endl;
  os << "-1" << endl;

  for (const auto& p : commands_helicopters)
    os << p.first << " " << p.second << endl;
  os << "-1" << endl;

  for (const auto& p : commands_parachuters)
    os << p.i << " " << p.j << endl;
  os << "-1" << endl;
}
