/* -*- coding: utf-8 -*- */

/** 
 * Modulus on float and rounding to int
 */
#include <iostream>
#include <math.h>

int main(int argc, char *argv[])
{
  for (float val=0.0; val < 10; val+=0.1) {
    std::cout << "val= " << val;
    std::cout << " -> " << static_cast<int>(val);
    std::cout << " %= " << static_cast<int>(val) % 3  << std::endl;
  }
  
  return 0;
}
