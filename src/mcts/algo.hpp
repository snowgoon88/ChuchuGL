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
    std::cout << "__EVAL ends at " <<  state << " with r_sum=" << r_sum << std::endl;
    return r_sum;
  }

  // **************************************************************** growtree
  /**
   * GROWTREE
   */
  void grow_tree( DecisionNode& tree, Bot& bot, double K=1.0)
  {
    // cumulative reward
    double cum_rew = 0.0;

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

    // Eval
    cum_rew = eval( *node, bot );

    // 
    
  }
  // ********************************************************* Algo::attributs
private:
  std::random_device _random_seeder; 
  std::default_random_engine _rnd_engine;
  
}; // class Algo
}; // namespace MCTS
  
#endif // MCTS_ALGO_HPP
