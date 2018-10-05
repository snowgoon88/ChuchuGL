/* -*- coding: utf-8 -*- */

#ifndef M_GAMEOBJECT_HPP
#define M_GAMEOBJECT_HPP

/** 
 * GameObject has string name, Pos pos, Pos dir.
 */

#include <matrix2020/m_def.hpp>
#include <sstream>

namespace matrix2020 {

// ***************************************************************************
// **************************************************************** GameObject
// ***************************************************************************
class GameObject
{
public:
  // **************************************************** GameObject::creation
  GameObject( const std::string name ) :
    _name(name), _pos({0,0}), _orient( D_UP ), _action( {{0,0}, D_UP} )
  {
  }
  // ************************************************** GameObject::destructor
  virtual ~GameObject()
  {
  }
  // ********************************************************* GameObject::str
  std::string str_dump() const
  {
    std::stringstream dump;
    dump << "GO: " << _name << " Pos= " << _pos << " Dir= " << _orient;

    return dump.str();
  }
  // *********************************************** GameObject::take_decision
  virtual void take_decision( bool debug=false )
  {
  }
  // *************************************************** GameObject::attributs
  void pos( const Pos& pos) { _pos = pos; }
  Pos pos() const { return _pos; }
  void orient( const Pos& dir ) { _orient=dir; }
  Pos orient() const { return _orient; }
  Move action() const { return _action; }
  void action( const Move& action ) { _action = action; }

  std::string _name;
  Pos _pos;
  Pos _orient;
  Move _action;
};
// ********************************************************** GameObject - END

}; // namespace matrix2020

#endif // M_GAMEOBJECT_HPP
