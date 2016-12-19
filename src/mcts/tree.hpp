/* -*- coding: utf-8 -*- */

#ifndef MCTS_TREE_HPP
#define MCTS_TREE_HPP

/** 
 * Tree representation for MCTS.
 */

#include <string>
#include <sstream>                   // std::stringstream
#include <list>

namespace MCTS {

//using LabelNode = std::string;

/** LabelChild must be comparable */
template<typename LabelNode, typename LabelChild>
class Node
{

// ************************************************************ Node::creation
public:
  Node( const LabelNode& label) :
    _label(label), _nb(0), _rec(0.0), _rec_sum(0.0),
    _father(nullptr)
  {
    _children.clear();
  }
  //Node( const Node& other ) = delete;
  // Node& operator=( const Node& other)
  // {
  //   // check for self-assignment
  //   if(&other == this)
  //     return *this;
  //   _label = std::string( other.get_label() );
  //   _rec = other._rec;
  //   _rec_sum = other._rec_sum;
    
  //   // reuse storage when possible
  //       if(size != other.size)
  //       {
  //           data.reset(new int[other.size]);
  //           size = other.size;
  //       }
  //       std::copy(&other.data[0], &other.data[0] + size, &data[0]);
  //       return *this;
  // }
  // ******************************************************** Node::comparison
  // bool operator ==(const Node& other)
  // {
  //   std::cout << _label << "=?=" << other._label << std::endl;
  //   return (_label == other._label);
  // }
  // *********************************************************** Node::display
  std::string str_display() const
  {
    std::stringstream dump;
    dump << "N:" << _label << " (" << _nb << ")" << " r=" << _rec << "/" << _rec_sum;
    
    return dump.str();
  }
  std::string str_dump() const
  {
    std::stringstream dump;
    dump << str_display() << std::endl;

    for( const auto& child: _children) {
      dump << "  +" << child.str_display() << std::endl; 
    }
    
    return dump.str();
  }
  /** maxdepth = -1 : no limit */
  std::string str_tree(  int max_depth=-1, std::string indent = "") const
  {
    if( max_depth != 0 ) {
      std::stringstream tree;
      tree << indent << "+" << str_display() << std::endl;
      for( auto& child: _children) {
	tree << child.str_tree( max_depth-1, indent + "  " );
      }
      return tree.str();
    }
    else {
      return "";
    } 
  }
  // ********************************************************* Node::add_child
  /** Insert an element if not already in _children */
  Node<LabelChild,LabelNode>& add_child( const LabelChild& lab )
  {
    for( auto& child: _children) {
      if( child.get_label() == lab ) return child;
    }

    // create a new child
    Node<LabelChild,LabelNode> child{lab};
    child._father = this;
    
    _children.push_back( child );
    return _children.back();
  }
// ************************************g*********************** Node::attributs
  const LabelNode& get_label() const{ return _label; };
  Node<LabelChild,LabelNode>* get_father() const { return _father; };
  
private:
  const LabelNode _label;
public:
  int _nb;
  double _rec;
  double _rec_sum;
  std::list<Node<LabelChild,LabelNode>> _children;
  Node<LabelChild,LabelNode>* _father;
};

}; // namespace MCTS

#endif // MCTS_TREE_HPP
