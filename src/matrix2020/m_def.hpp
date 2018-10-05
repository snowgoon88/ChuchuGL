/* -*- coding: utf-8 -*- */

#ifndef M_DEF_HPP
#define M_DEF_HPP

/** 
 * Type definitions
 */

#include <memory>                         // std::shared_ptr
#include <iostream>
#include <vector>

#include <matrix2020/m_pos.hpp>
namespace matrix2020
{
// ***************************************************************************
static const Pos D_UP( {0,1});
static const Pos D_DOWN( {0,-1});
static const Pos D_RIGHT( {1,0});
static const Pos D_LEFT( {-1,0});
static const std::vector<Pos> AllDir {D_UP, D_RIGHT, D_DOWN, D_LEFT};
  unsigned int get_dir_index( const Pos& dir );
Pos turn_right( const Pos& dir );
Pos turn_left( const Pos& dir );
float angle_from_dir( const Pos& dir );

// ********************************************************************** Move
using Move = struct s_Move {
    Pos pos;
    Pos dir;
};
// ************************************************************* Environment
class Environment;
// ************************************************************** GameObject
class GameObject;
using GameObjectPtr = std::shared_ptr<GameObject>;
  
}; // namespace matrix2020

#endif // M_DEF_HPP
