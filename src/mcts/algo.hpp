/* -*- coding: utf-8 -*- */

#ifndef MCTS_ALGO_HPP
#define MCTS_ALGO_HPP

/** 
 * Algorithms used in MCST
 */

#include <tree.hpp>
#include <limits>                    // max dbl
#include <math.h>
#include <random>                     // std::uniform_int...
#include <chrono>                      // std::chrono

// SPECIFIC
#include <tictactoe.hpp>

namespace MCTS {

// ***************************************************************************
// ********************************************************************** Algo
// ***************************************************************************
  template<typename State, typename Action, typename Bot>
class Algo
{
  using DecisionNode = Node<State,Action>;
  using RandomNode   = Node<Action,State>;
public:
  // ********************************************************** Algo::creation
  Algo() : _random_seeder(), _rnd_engine(_random_seeder())
  {
  }

  // ****************************************************************** select
  /**
   * SELECT : choose one Act among the _childreng Nodes of 'node'.
   *
   * argmax_{children} ( rec_sum(x,a)/n-x,a) + K sqrt( log(n(x)) / log(n(x,a))) )
   */
  const Action select( DecisionNode& node, double K, bool verb=false )
  {
    if(verb){
      std::cout << "__SELECT for "<< node.str_dump() << std::endl;
    }
    std::vector<Action> best_actions;
    Action a;
    double max_qval = - std::numeric_limits<double>::max();
    for( const auto& n: node._children) {
      if(verb) std::cout << n.str_display();
      if( n._nb > 0 ) {
	double qval = n._rec_sum / static_cast<double>(n._nb);
	if(verb) {
	  std::cout << " qval=" << qval;
	  std::cout << " node._nb=" << node._nb << " n._nb=" << n._nb << std::endl;
	}
	
	double bonus= K * sqrt( log( static_cast<double>(node._nb) ) / static_cast<double>(n._nb) );
	if(verb) std::cout << " b=" << bonus << std::endl;
	if( (qval+bonus) > max_qval ) {
	  if(verb) std::cout << "  new best is " << n.get_label() << " with " << qval+bonus << std::endl;
	  max_qval = qval+bonus;
	  a = n.get_label();
	}
      }
      else {
	if(verb) std::cout << " never visited" << std::endl;
	//std::cout << "  infinity with " << n.get_label() << std::endl;
	best_actions.push_back( n.get_label());
      }
    }

    if( best_actions.empty() ) {
      if(verb) std::cout << "  best_action is empty => return a" << std::endl;
      return a;
    }
    else {
    // One random Act from best_actions
      if(verb) std::cout << "  return one from best_action" << std::endl;
      auto unif = std::uniform_int_distribution<>(0, best_actions.size()-1);
      return best_actions[unif(_rnd_engine)];
    }
  }
  // ******************************************************************** eval
  double eval( const DecisionNode& node, Bot& bot)
  {
    double r_sum = 0;
    State state = node.get_label();
    while( bot.winner( state ) == '.' ) {
      // chose an action
      auto list_act = bot.legal_moves( state );
      auto unif = std::uniform_int_distribution<>(0, list_act.size()-1);
      auto act = list_act[unif(_rnd_engine)];

      // generate next state
      auto next_state = bot.play( state, act );
      auto win = bot.winner( next_state );
      if( win == '.' ) r_sum += 0.0;
      else if( win == '-' ) r_sum += 1.0;
      else if( win == 'O' ) r_sum += 5.0;
      else r_sum += (-5.0);

      state = next_state;
    }
    //std::cout << "__EVAL ends at " <<  state << " with r_sum=" << r_sum << std::endl;
    return r_sum;
  }

  // **************************************************************** growtree
  /**
   * GROWTREE
   */
  void grow_tree( DecisionNode& tree, Bot& bot, double K=1.0, bool verb=false)
  {
    // cumulative reward
    double cum_rew = 0.0;

    // selection of a leaf node
    DecisionNode* node = &tree;
    do {
      // select an action
      auto act = select( *node, K, verb);
      auto& child = node->add_child( act );
      // generate next state
      auto state = node->get_label();
      auto new_state = bot.play( state, act );
      // add state to children of child
      auto& new_node = child.add_child( new_state );
      // if no children (ie, "new" node), add them
      if( new_node._children.size() == 0 ) {
	auto actions = bot.legal_moves( new_node.get_label() );
	for( auto& act: actions) {
	  new_node.add_child( act );
	}
      }

      if(verb) {
	std::cout << "  select " << act << " for " << node->str_display() << std::endl;
	std::cout << "    -> child " << child.str_display() << std::endl;
	std::cout << "    -> new " << new_node.str_display() << std::endl;
	//std::cout << "  tree" << std::endl << node->str_tree() << std::endl;
      }
    
      // update rec of child
      auto winner = bot.winner( new_state );
      if( winner == 'O' ) child._rec = 10;
      else if( winner == 'X' ) child._rec = -10;
      else child._rec = 0;
      // set current node
      node = &new_node;
    }
    while( node->_nb != 0 and node->_children.size() > 0 );
    //std::cout << "LEAF node is" << node->str_dump() << std::endl;

    // Eval
    cum_rew = eval( *node, bot );

    // Backpropagate cumulative reward
    while( node->get_father() != nullptr ) {
      // update nb seen
      node->_nb += 1;
      // update father
      auto* father = node->get_father();
      cum_rew += father->_rec;
      father->_rec_sum += cum_rew;
      father->_nb += 1;

      // DEBUG
      // std::cout << father->str_display() << std::endl;
      // if( father->get_father() == nullptr ) {
      // 	std::cerr << "Error: " << father->str_display() << " should have a father !!" << std::endl;
      // }
      // and prepare to do that with father of father
      node = father->get_father();
    }
    // update also root node
    node->_nb += 1;
  }
  // ******************************************************* Algo::best_action
  const Action best_action( const DecisionNode& node, bool verb=false )
  {
    std::vector<Action> best_actions;
    int best_nb = -1;
    for( const auto& n: node._children) {
      if( n._nb > best_nb ) {
	best_actions.clear();
	best_actions.push_back( n.get_label() );
	best_nb = n._nb;
      }
      else if( n._nb == best_nb ) {
	best_actions.push_back( n.get_label() );
      }
    }
    if( verb ) {
      std::cout << "__BEST_ACT = ";
      for( auto& act: best_actions) {
	std::cout << act << ", ";
      }
      std::cout  << std::endl;
    }
    // chose one at random from best_actions
    auto unif = std::uniform_int_distribution<>(0, best_actions.size()-1);
    return best_actions[unif(_rnd_engine)];
  }
  // ************************************************************* Algo::solve
  const Action solve(const State& s_init, Bot& bot,
		     unsigned int nb_run = 100,
		     double K=1.0,
		     bool verb = false)
  {
    // initialise tree
    DecisionNode nroot( s_init );
    // Add all possible RandomNodes
    auto actions = bot.legal_moves( nroot.get_label() );
    for( auto& act: actions) {
      nroot.add_child( act );
    }
    if( verb ) {
      std::cout << "__START" << std::endl;
      std::cout << nroot.str_tree() << std::endl;
    }

    // make nb_run runs
    for( unsigned int i = 0; i < nb_run; ++i) {
      grow_tree(nroot, bot, K);
    }
    if( verb ) {
      std::cout << "__FINAL TREE" << std::endl;
      std::cout << nroot.str_tree(2) << std::endl;
    }

    return best_action( nroot, verb );
  }
  /** With time constraint */
  const Action solve(const State& s_init, Bot& bot,
		     std::chrono::duration<double> max_seconds = std::chrono::milliseconds(20),
		     double K=1.0,
		     bool verb = false)
  {
    // clock now
    auto start_proc = std::chrono::steady_clock::now();
    
    // initialise tree
    DecisionNode nroot( s_init );
    // Add all possible RandomNodes
    auto actions = bot.legal_moves( nroot.get_label() );
    for( auto& act: actions) {
      nroot.add_child( act );
    }
    if( verb ) {
      std::cout << "__START" << std::endl;
      std::cout << nroot.str_tree() << std::endl;
    }

    // wait period 
    std::chrono::duration<double> elapsed_seconds = std::chrono::steady_clock::now() - start_proc;
    unsigned int count = 0;
    while( elapsed_seconds < max_seconds ) {
      grow_tree(nroot, bot, K);
      count ++;
      elapsed_seconds = std::chrono::steady_clock::now() - start_proc;
    }
    std::cout << "__mcts " << count << " eval : " << best_action( nroot ) << std::endl;
    if( verb ) {
      std::cout << "__FINAL TREE" << std::endl;
      std::cout << nroot.str_tree(2) << std::endl;
    }

    return best_action( nroot, verb );
  }
  // ********************************************************* Algo::attributs
private:
  std::random_device _random_seeder; 
  std::default_random_engine _rnd_engine;
  
}; // class Algo
}; // namespace MCTS
  
#endif // MCTS_ALGO_HPP
