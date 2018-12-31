/* -*- coding: utf-8 -*- */

/** 
 * Modulus on float and rounding to int
 */
#include <iostream>
#include <math.h>

void test_for_square()
{
  for( int i=0; i<4; ++i ) {
    std::cout << "x : x%2 (x+1)%2 (x/2) (x/2)%2 (x-1)/2 ((x-1)/2)%2) ((x+1)/2)%2" << std::endl;
    std::cout << i << " : ";
    std::cout << (i%2) << "   ";
    std::cout << (i+1)%2 << "       ";
    std::cout << (i/2) << "     ";
    std::cout << (i/2)%2 << "       ";
    std::cout << (i-1)/2 << "       ";
    std::cout << ((i-1)/2)%2 << "            ";
    std::cout << (i+1)/2 << "       ";
    std::cout << ((i+1)/2)%2;
    std::cout << std::endl;
  }
}

int main(int argc, char *argv[])
{
  for (float val=0.0; val < 10; val+=0.1) {
    std::cout << "val= " << val;
    std::cout << " -> " << static_cast<int>(val);
    std::cout << " %= " << static_cast<int>(val) % 3  << std::endl;
  }

  std::cout << "__INT modulus" << std::endl;
  test_for_square();
  
  return 0;
}
