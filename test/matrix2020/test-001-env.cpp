/* -*- coding: utf-8 -*- */

/** 
 * matrix2020/test-001-env.cpp
 *
 * - Load a env.txt file and display it
 */

#include <matrix2020/m_environment.hpp>

using namespace matrix2020;

void load_and_display( const std::string& filename )
{
  Environment env;

  env.load_from_txt( filename );
  std::cout << env.str_dump() << std::endl;
}
int main(int argc, char *argv[])
{
  load_and_display( "data/matrix00.txt" );
  return 0;
}

