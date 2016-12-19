/* -*- coding: utf-8 -*- */

/** 
 * Test MCTS tree
 */

#include <iostream>
#include <string>

#include <tree.hpp>
#include <algo.hpp>
#include <tictactoe.hpp>
#include <chrono>                      // std::chrono

using DecisionNode = MCTS::Node<std::string,int>;
using RandomNode   = MCTS::Node<int,std::string>;
using DNodeTTT = MCTS::Node<TTT::State,TTT::Action>;
using RNodeTTT = MCTS::Node<TTT::Action,TTT::State>;
std::ostream& operator<< (std::ostream& os, const TTT::State& state)
{
  os << TTT::str_rep(state);
  return os;
}


// ******************************************************** creation and print
void creation_and_print()
{
  std::cout << "__ CREATION" << std::endl;
  DecisionNode nr( "root" );
  //NEED undelete copy-constructor:DecisionNode nn = nr;

  std::cout << nr.str_dump() << std::endl;
  //NEED undelete copy-constructor:std::cout << nn.str_dump() << std::endl;
}
// ************************************************************** add_children
void add_children()
{
  std::cout << "__ ADD" << std::endl;
  DecisionNode nroot( "root" );
  std::cout << nroot.str_dump() << std::endl;

  std::cout << "** ADD 1" << std::endl;
  auto& n1 = nroot.add_child( 1 );
  
  n1._nb = 11;
  std::cout << nroot.str_dump() << std::endl;

  std::cout << "** ADD 2" << std::endl;
  auto& n2 = nroot.add_child( 2 );
  n2._nb = 12;
  std::cout << nroot.str_dump() << std::endl;

  std::cout << "** ADD 1" << std::endl;
  auto& n3 = nroot.add_child( 1 );
  n3._nb = 13;
  std::cout << nroot.str_dump() << std::endl;
}
// *************************************************************** test_select
void test_select()
{
  DecisionNode nroot( "root" );
  auto& n1 = nroot.add_child( 0 );
  n1._nb = 2;
  n1._rec_sum = 2.0;
  auto& n2 = nroot.add_child( 1 );
  n2._nb = 4;
  n2._rec_sum = 2.2;
  auto& n3 = nroot.add_child( 2 );
  n3._nb = 0;
  n3._rec_sum = 1.8;
  nroot._nb = 6;

  std::cout << nroot.str_dump() << std::endl;

  // Selection Algorithm
  MCTS::Algo<std::string,int,TTT::Bot> alg;

  std::cout << "__BASE" << std::endl;
  auto a = alg.select( nroot, 0.0 );
  std::cout << " -> " <<  a << std::endl;

  std::cout << "__K=1" << std::endl;
  a = alg.select( nroot, 10.0 );
  std::cout << " -> " <<  a << std::endl;

  std::cout << "__n(root,2) = 10" << std::endl;
  n3._nb = 10;
  a = alg.select( nroot, 10.0 );
  std::cout << " -> " <<  a << std::endl;

  std::cout << "__n(root,0)=5" << std::endl;
  n1._nb = 5;
  a = alg.select( nroot, 10.0 );
  std::cout << " -> " <<  a << std::endl; 
}
// ************************************************************* test_growtree
void test_growtree()
{
  TTT::Bot bot;
  DNodeTTT nroot( bot.starting_state() );
  // Add all possible RandomNodes
  auto actions = bot.legal_moves( nroot.get_label() );
  for( auto& act: actions) {
    nroot.add_child( act );
  }
  std::cout << "__START" << std::endl;
  std::cout << nroot.str_dump() << std::endl;

  // Selection Algorithm
  MCTS::Algo<TTT::State,TTT::Action,TTT::Bot> alg;

  std::string answer = "";
  while( answer.compare("n") != 0 ) {
    alg.grow_tree(nroot, bot, 1, true);

    std::cout << "__After GROWTREE " << std::endl;
    std::cout << nroot.str_tree() << std::endl;

    std::cout << "Continuer [On] ?: ";
    std::cin >> answer;
    
  }
  // for( unsigned int i = 0; i < 100; ++i) {
  //   alg.grow_tree(nroot, bot, 1);
  // }
  // std::cout << "__After GROWTREE " << std::endl;
  // std::cout << nroot.str_tree() << std::endl;

  // alg.grow_tree(nroot, bot, 1, true); 
}
// ***************************************************************** test_mcts
void test_mcts( bool verb = false)
{
  TTT::Bot bot;
  // Selection Algorithm
  MCTS::Algo<TTT::State,TTT::Action,TTT::Bot> alg;

  auto state_init = bot.starting_state();

  auto act = alg.solve( state_init, bot, 1000, 10.0, verb);

  std::cout << "In state " << state_init << " => " << act << std::endl;

}
// ***************************************************************** test_game
void test_game( bool verb = false)
{
  TTT::Bot bot;
  // Selection Algorithm
  MCTS::Algo<TTT::State,TTT::Action,TTT::Bot> alg;

  auto state = bot.starting_state();
  std::cout << "*** STARTING STATE ***" << std::endl;
  std::cout << TTT::str_nice(state)  << std::endl;
  while( bot.winner( state) == '.' ) {
    auto act = alg.solve( state, bot, std::chrono::milliseconds(500), 15.0, false );
    state = bot.play( state, act );
  }

  std::cout << "*** FINAL STATE ***" << std::endl;
  std::cout << TTT::str_nice(state)  << std::endl;

  std::cout << "And the winner is :";
  if( bot.winner(state) == 'O' ) {
    std::cout << "MCTS !" << std::endl;
  }
  else if( bot.winner(state) == 'X' ) {
    std::cout <<  "RANDOMNESS..." << std::endl;
  }
  else {
    std::cout << "NOBODY." << std::endl;
  }

  bot.winner(state, true);
  
}
// ********************************************************************** main
int main(int argc, char *argv[])
{
  //creation_and_print();
  //add_children();
  //test_select();
  //test_growtree();
  //test_mcts( true );
  test_game();
  
  return 0;
}
