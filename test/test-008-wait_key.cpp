/* -*- coding: utf-8 -*- */

/** 
 * Test wait_keypressed.
 */

#include <key_pressed.h>

int main(int argc, char *argv[])
{
  std::cout << "__PREMS" << std::endl;
  wait_keypressed();

  std::cout << "__DEUS" << std::endl;
  wait_keypressed();
  
  std::cout << "__TRES" << std::endl;
  wait_keypressed();
  
  return 0;
}
