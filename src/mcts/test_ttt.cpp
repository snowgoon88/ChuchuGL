/* -*- coding: utf-8 -*- */

/** 
 * Test TicTacToe
 */
#include <iostream>
#include <string>

#include <tictactoe.hpp>

// ****************************************************************** creation
void creation()
{
  TTT::Bot bot;
  auto state = bot.void_state();

  std::cout << TTT::str_rep(state) << std::endl;
}
void creation_rand()
{
  for( unsigned int i = 0; i < 10; ++i) {
    TTT::Bot bot;
    auto state = bot.starting_state();

    std::cout << "__CREATION " << i << std::endl;
    std::cout << TTT::str_rep(state) << std::endl;
  }
}
// ****************************************************************** equality
void equality()
{
  TTT::Bot bot;
  auto state = bot.void_state();
  auto other = bot.void_state();

  std::cout << "__VOID equality" << std::endl;
  std::cout << (state == other) << std::endl;

  TTT::State s1 {'X','.','.', '.','.','.', '.','.','.'};
  TTT::State s2 {'X','.','.', '.','.','.', '.','.','.'};
  TTT::State s3 {'O','.','.', '.','.','.', '.','.','.'};
  TTT::State s4 {'O','.','.', '.','.','.', '.','.','O'};

  std::cout << TTT::str_rep(s1) << "== " << TTT::str_rep(s1) << " " << (s1 == s1) << std::endl;
  std::cout << TTT::str_rep(s1) << "== " << TTT::str_rep(s2) << " " << (s1 == s2) << std::endl;
  std::cout << TTT::str_rep(s1) << "== " << TTT::str_rep(s3) << " " << (s1 == s3) << std::endl;
  std::cout << TTT::str_rep(s2) << "== " << TTT::str_rep(s3) << " " << (s2 == s3) << std::endl;
  std::cout << TTT::str_rep(s3) << "== " << TTT::str_rep(s4) << " " << (s3 == s4) << std::endl;
  s4[8] = '.';
    std::cout << TTT::str_rep(s3) << "== " << TTT::str_rep(s4) << " " << (s3 == s4) << std::endl;
}
// ********************************************************************** play
void play()
{
  TTT::Bot bot;
  auto state = bot.starting_state();
  std::cout << "__START" << std::endl;
  std::cout << TTT::str_rep(state) << std::endl;

  // plays first legal move
  auto actions = bot.legal_moves( state );
  state = bot.play( state, actions.front() );
  std::cout << "__FIRST" << std::endl;
  std::cout << TTT::str_rep(state) << std::endl;
  
  actions = bot.legal_moves( state );
  state = bot.play( state, actions.front() );
    std::cout << "__SECOND" << std::endl;
  std::cout << TTT::str_rep(state) << std::endl;
}
// ********************************************************************** game
void game()
{
   TTT::Bot bot;
  auto state = bot.starting_state();
  std::cout << "__START" << std::endl;
  std::cout << TTT::str_rep(state) << std::endl;
  char player = 'O';
  while( bot.winner( state ) == '.' ) {
    state = bot.play( state, player);
    std::cout << "==> " << player << std::endl;
    std::cout << TTT::str_rep(state) << std::endl;
    player = (player == 'O' ? 'X' : 'O');
  }
  std::cout << "==> " << bot.winner( state ) << " wins !!!" << std::endl;
}
// ********************************************************************** main
int main(int argc, char *argv[])
{
  // creation();
  // creation_rand();
  //play();
  equality();
  game();
  
  return 0;
}
