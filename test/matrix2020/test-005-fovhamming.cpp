/* -*- coding: utf-8 -*- */

/** 
 * test Field of View using hamming distance
 */

#include <matrix2020/fov_hamming.hpp>
#include <iostream>

using namespace matrix2020;


/** 
 * Print some FoV of various max_dist
 */
void test_no_env()
{
  Environment env;
  env.load_from_txt( "data/matrix00.txt" );
  std::cout << env.str_display() << std::endl;

  Pos pos(5,3);
  
  FOVHamming fov( env, pos, 1 /* max dist */);

  std::cout << "__INIT" << std::endl;
  std::cout << fov.str_dump() << std::endl;

  for( int max_dist = 0; max_dist < 4; ++max_dist) {
    std::cout << "__FOV at " << pos << " max=" << max_dist << std::endl;
    fov = FOVHamming( env, pos, max_dist );
    std::cout << fov.str_dump() << std::endl;
    std::cout << fov.str_display() << std::endl;
    std::cout << fov.str_view() << std::endl;
  }

  pos = {3,3};
  std::cout << "__POS " << pos << std::endl;
  for( int max_dist = 0; max_dist < 4; ++max_dist) {
    std::cout << "__FOV at " << pos << " max=" << max_dist << std::endl;
    fov = FOVHamming( env, pos, max_dist );
    std::cout << fov.str_dump() << std::endl;
    std::cout << fov.str_display() << std::endl;
    std::cout << fov.str_view() << std::endl;
  }

}

int main(int argc, char *argv[])
{
  test_no_env();
  
  return 0;
}
