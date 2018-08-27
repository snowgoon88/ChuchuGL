/* -*- coding: utf-8 -*- */


/** 
 * test-004-basiclevel.cpp
 * Simple environment where hacker moves
 */

#include <iostream>                       // std::cout
#include <gl_engine.hpp>

#include <matrix2020/gl_basic_level.hpp>
using namespace matrix2020;

int main(int argc, char *argv[])
{
  std::cout << "__GAME ENGINE" << std::endl;
  GLEngine gl_engine( "Basic Level" );

  std::cout << "__BASIC LEVEL" << std::endl;
  GLBasicLevel level( gl_engine );

  gl_engine.run<GLBasicLevel,bool>( level );
  
  return 0;
}
