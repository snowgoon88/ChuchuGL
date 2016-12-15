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
  const Action select( DecisionNode& node, double K )
  {
    std::vector<Action> best_actions;
    Action a;
    double max_qval = - std::numeric_limits<double>::max();
    for( const auto& n: node._children) {
      //std::cout << n.str_display();
      if( n._nb > 0 ) {
	double qval = n._rec_sum / static_cast<double>(n._nb);
	//std::cout << " qval=" << qval;
	double bonus= K * sqrt( log( static_cast<double>(node._nb) ) / log( static_cast<double>(n._nb) ));
	//std::cout << " b=" << bonus << std::endl;
	if( (qval+bonus) > max_qval ) {
	  max_qval = qval+bonus;
	  a = n.get_label();
	}
      }
      else {
	//std::cout << " never visited" << std::endl;
	best_actions.push_back( n.get_label());
      }
    }

    if( best_actions.empty() ) {
      return a;
    }
    else {
    // One random Act from best_actions
    auto unif = std::uniform_int_distribution<>(0, best_actions.size()-1);
    return best_actions[unif(_rnd_engine)];
    }
  }

  // **************************************************************** growtree
  /**
   * GROWTREE
   */
  void grow_tree( DecisionNode& tree, Bot& bot, double K=1.0)
  {
    // selection of a leaf node
    DecisionNode* node = &tree;
    do {
      // select an action
      auto act = select( *node, K);
      auto child = node->add_child( act );
      // generate next state
      auto state = node->get_label();
      auto new_state = bot.play( state, act );
      // add state to children of child
      auto& new_node = child.add_child( new_state );

      std::cout << "  select " << act << " for " << node->str_display() << std::endl;
      std::cout << "    -> child " << child.str_display() << std::endl;
      std::cout << "    -> new " << new_node.str_display() << std::endl;
    
      // update rec of child
      auto winner = bot.winner( new_state );
      if( winner == 'O' ) child._rec = 10;
      else if( winner == 'X' ) child._rec = -10;
      else child._rec = 0;
      // set current node
      node = &new_node;
    }
    while( node->_nb != 0 and node->_children.size() > 0 );
  
    std::cout << "LEAF node is" << node->str_dump() << std::endl;
  }
  // ********************************************************* Algo::attributs
private:
  std::random_device _random_seeder; 
  std::default_random_engine _rnd_engine;
  
}; // class Algo
}; // namespace MCTS
  
#endif // MCTS_ALGO_HPP
