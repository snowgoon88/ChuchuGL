/* -*- coding: utf-8 -*- */

#ifndef M_GAMEOBJECT_HPP
#define M_GAMEOBJECT_HPP

/** 
 * GameObject has one Pos.
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
    _pos({0,0}), _name(name)
  {
  }
  GameObject( const GameObject& other ) :
    _pos(other._pos), _name(other._name)
  {
  }
  GameObject& operator=( const GameObject& other )
  {
    if (this != &other ) { // protect against invalid self-assignment
      _pos = other._pos;
      _name = other._name;
    }
    return *this;
  }
  // ************************************************* GameObject::destruction
  virtual ~GameObject()
  {
  }
  // ********************************************************* GameObject::str
  std::string str_dump() const
  {
    std::stringstream dump;
    dump << "GO: " << _name << " Pos= " << _pos;

    return dump.str();
  }
  // *************************************************** GameObject::attributs
  Pos pos() const { return _pos; }

  Pos _pos;
  std::string _name;
};

};

#endif // M_GAMEOBJECT_HPP
