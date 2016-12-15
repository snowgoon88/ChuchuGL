/* -*- coding: utf-8 -*- */

#ifndef TICTACTOE_HPP
#define TICTACTOE_HPP

/** 
 * Generative model for titactoe.
 *
 * State : X or O or . in every 9 cases
 * 'bot' will use 'X' by default.
 *
 * If nobody wins, winner() returns '-'
 *
 * => vectot<Char> with std::string representation
 * Action : indice
 */
#include <array>
#include <vector>
#include <string>                     // std::string
#include <sstream>                    // std::stringdtream
#include <random>                     // std::uniform_int...

namespace TTT {

// ******************************************************************** GLOBAL
using State = std::array<char,9>; 
using StateRep = std::string;
using Action = int;

// ******************************************************************* str_rep
/** StateRep is aggregation of Char of State */
StateRep str_rep(const State& state)
{
  std::stringstream rep;
  rep << state[0] << state[1] << state[2];// << '\n';
  rep << state[3] << state[4] << state[5];// << '\n';
  rep << state[6] << state[7] << state[8];

  return rep.str();
}
// ***************************************************************************
// *********************************************************************** Bot
// ***************************************************************************
class Bot
{
public:
  // *********************************************************** Bot::creation
  Bot() : _random_seeder(), _rnd_engine(_random_seeder())
  {
    // Create a Random generator between 0 and 9
    // Seed with a real random value, if available
    //_rnd_engine(random_seeder());
    // then, we ca use int alea = _uniform_dist(_rnd_engine());
  }

  // ***************************************************** Bot::starting_state
  State void_state()
  {
    return State {'.','.','.', '.','.','.', '.','.','.'};
  }
  /** Bot begins with proba=0.5 */
  State starting_state ()
  {
    State start = {'.','.','.', '.','.','.', '.','.','.'};
    // Who starts ?
    auto uni0_1 = std::uniform_int_distribution<>(0,1);
    int alea = uni0_1(_rnd_engine);
    // while( alea == 4 ) {
    //   alea = _uniform_dist(_rnd_engine);
    // }
    
    if( alea == 0 ) { // external player starts
      return start;
    }
    else {           // Bot starts
      auto uni0_8 = std::uniform_int_distribution<>(0,8);
      Action act = uni0_8(_rnd_engine);
      start[act] = 'X';
      return start;
    }
  }
  // ********************************************************* Bot:legal_moves
  std::vector<Action> legal_moves( const State& s)
  {
    std::vector<Action> actions;
    for( unsigned int i = 0; i < s.size(); ++i) {
      if( s[i] == '.' ) actions.push_back(i);
    }
    return actions;
  }
  // *************************************************************** Bot::play
  /** Plays randomly in a cell with '.' */
  State play( const State& state, const Action& act )
  {
    State new_state = state;
    // apply external action
    new_state[act] = 'O';

    // Any legal moves left ?
    auto actions = legal_moves( new_state );
    if( not actions.empty() ) {
      auto unif = std::uniform_int_distribution<>(0,actions.size()-1);
      Action botact = actions[unif(_rnd_engine)]; 
    // while( state[act] != '.' ) {
    //   botact = _uniform_dist(_rnd_engine);
    // }
      new_state[botact] = 'X';
    }
    return new_state;
  }
  // ************************************************************* Bot::winner
  char winner(const State& s)
  {
    char winner = '.';
    for( unsigned int id = 0; id < 3; ++id) {
      if( s[id] == s[id+1] and s[id+1] == s[id+2] and s[id] != winner ) return s[id];
      if( s[id] == s[id+3] and s[id+3] == s[id+6] and s[id] != winner ) return s[id];
    }
    if( s[0] == s[4] and s[4] == s[8] and s[0] != winner) return s[0];
    if( s[2] == s[4] and s[4] == s[6] and s[2] != winner) return s[2];

    // no winners ?
    for( auto& c: s) {
      if( c == '.' ) return winner;
    }
    
    return '-'; // no winner
  }
  // ********************************************************** Bot::attributs
private:
  std::random_device _random_seeder; 
  std::default_random_engine _rnd_engine;
};
  
}; // namespace TTT

#endif // TICTACTOE_HPP
