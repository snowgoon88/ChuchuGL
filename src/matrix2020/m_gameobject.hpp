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
    _name(name), _pos({0,0}), _orient( D_UP )
  {
  }
  // ********************************************************* GameObject::str
  std::string str_dump() const
  {
    std::stringstream dump;
    dump << "GO: " << _name << " Pos= " << _pos << " Dir= " << _orient;

    return dump.str();
  }
  // *************************************************** GameObject::attributs
  Pos pos() const { return _pos; }
  Pos orient() const { return _orient; }

  std::string _name;
  Pos _pos;
  Pos _orient;

};
// ********************************************************** GameObject - END

}; // namespace matrix2020

#endif // M_GAMEOBJECT_HPP
