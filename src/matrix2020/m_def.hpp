/* -*- coding: utf-8 -*- */

#ifndef M_DEF_HPP
#define M_DEF_HPP

/** 
 * Type definitions
 */

#include <memory>                         // std::shared_ptr

namespace matrix2020
{
  // ********************************************************************* Pos
  using Pos = struct s_Pos {
    unsigned int x;
    unsigned int y;
  };
  using Dir = struct s_Dir {
    int x;
    int y;
  };
  static const Dir D_UP( {0,1});
  static const Dir D_DOWN( {0,-1});
  static const Dir D_RIGHT( {1,0});
  static const Dir D_LEFT( {-1,0});
  // ************************************************************* Environment
  class Environment;
  // ************************************************************** GameObject
  class GameObject;
  using GameObjectPtr = std::shared_ptr<GameObject>;
  
}; // namespace matrix2020

#endif // M_DEF_HPP
