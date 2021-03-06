#include <cassert>
#include <sstream>
using namespace std;

#include "dfa.hpp"
#include "nfa.hpp"

void DFA::insert(size_t s, char c, size_t t) {
  assert(s < pool.size());
  assert(pool[s].trans.find(c) == pool[s].trans.end());
  pool[s].trans[c] = t;
}

string DFA::to_dot() const {
  stringstream ss;
  ss << "digraph {" << endl;
  ss << "  node[shape=circle];" << endl;
  ss << "  edge[arrowhead=vee];" << endl;
  ss << "  START[shape=point, color=white];" << endl;
  for (size_t i = 0; i < pool.size(); i++)
    if (pool[i].final)
      ss << "  " << i << "[shape=doublecircle];" << endl;
  ss << "  START -> " << init << " [label=start];" << endl;
  for (size_t i = 0; i < pool.size(); i++)
    for (auto j = pool[i].trans.begin(); j != pool[i].trans.end(); j++)
      ss << "  " << i << " -> " << j->second << " [label=\"\\" << j->first
         << "\"];" << endl;
  ss << "}" << endl;
  return ss.str();
}

NFA DFA::reverse() const {
  NFA nfa;
  const DFA &dfa = *this;
  nfa.init.clear();
  nfa.pool.assign(dfa.pool.size(), NFA::State());
  for (size_t i = 0; i < dfa.pool.size(); i++) {
    for (auto j = dfa.pool[i].trans.begin(); j != dfa.pool[i].trans.end(); j++)
      nfa.insert(j->second, j->first, i);
    if (dfa.pool[i].final)
      nfa.init.insert(i);
  }
  nfa.pool[dfa.init].final = 1;
  return nfa;
}

// Brzozowski's algorithm
template <class I> DFA DFA::from_regex(I lo, I hi) {
  return move(NFA::from_regex(lo, hi)
                  .determinize()
                  .reverse()
                  .determinize()
                  .reverse()
                  .determinize());
}
template DFA DFA::from_regex(const char *, const char *);
template DFA DFA::from_regex(string::const_iterator, string::const_iterator);
