/* -*- coding: utf-8 -*- */


/** 
 * test-004-movelevel.cpp
 * Simple environment where cards can be moved as Moveable.
 */

#include <iostream>                       // std::cout
#include <gl_engine.hpp>

#include <matrix2020/gl_moveable_level.hpp>
using namespace matrix2020;

int main(int argc, char *argv[])
{
  std::cout << "__GAME ENGINE" << std::endl;
  GLEngine gl_engine( "Move Level", 800 /*width*/, 800 /*height*/ );

  std::cout << "__MOVE LEVEL" << std::endl;
  GLMoveableLevel level( gl_engine );

  gl_engine.run<GLMoveableLevel,bool>( level );
  
  return 0;
}
