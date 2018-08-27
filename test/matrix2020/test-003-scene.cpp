/* -*- coding: utf-8 -*- */

/** 
 * Test Scene, adding GameObjects
 */

#include <matrix2020/m_def.hpp>
#include <matrix2020/m_scene.hpp>

using namespace matrix2020;

int main(int argc, char *argv[])
{
  Environment env;
  env.load_from_txt( "data/matrix00.txt" );

  Scene scene( env );
  std::cout << "__START" << std::endl;
  std::cout << scene.str_dump() << std::endl;

  GameObjectPtr go( new GameObject( "hacker" ) );
  std::cout << "__GO " << go->str_dump() << std::endl;

  std::cout << "__WRONG WALL" << std::endl;
  go->_pos = {2, 2};
  scene.add( go );
  std::cout << scene.str_dump() << std::endl;

  std::cout << "__WRONG EMPTY" << std::endl;
  go->_pos = {1, 0};
  scene.add( go );
  std::cout << scene.str_dump() << std::endl;

  go->_pos = {3, 2};
  scene.add( go );
  std::cout << "__RIGHT" << std::endl;
  std::cout << scene.str_dump() << std::endl;

  
  return 0;
}
