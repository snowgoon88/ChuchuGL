/* -*- coding: utf-8 -*- */

/** 
 * Test Dir and Pos
 */

#include <iostream>       // std::cout

#include <matrix2020/m_def.hpp>

using namespace matrix2020;

void print( const std::string& name, const Pos& d )
{
  std::cout << name << d << std::endl;
}

int main(int argc, char *argv[])
{
  Pos d1 {1,2};
  print( "d1", d1 );
  Pos d2 {2,3};
  print( "d2", d2 );
  Pos d3 = d2+d1;
  std::cout << "__PLUS" << std::endl;
  print( "d3", d3 );
  print( "d2", d2 );
  std::cout << "__PLUS" << std::endl;
  d3 = d2+d2;
  print( "d3", d3 );
  print( "d2", d2 );
  std::cout << "__PLUS" << std::endl;
  d1 = d2+d1;
  print( "d1", d1 );
  print( "d2", d2 );
  std::cout << "__MOINS" << std::endl;
  d3 = d2-d1;
  print( "d1", d1 );
  print( "d2", d2 );
  print( "d3", d3 );
  std::cout << "d1<d2 = " << (d1<d2) << std::endl;
  std::cout << "d2<d1 = " << (d2<d1) << std::endl;
  std::cout << "d2>d1 = " << (d2>d1) << std::endl;
  std::cout << "d2==d1 = " << (d2==d1) << std::endl;
  std::cout << "d1==d1 = " << (d1==d1) << std::endl;
  std::cout << "d2<=d1 = " << (d2<=d1) << std::endl;
  std::cout << "d2<=d2 = " << (d2<=d2) << std::endl;
  std::cout << "d1<=d2 = " << (d1<=d2) << std::endl;
  std::cout << "d1<=d1 = " << (d1<=d1) << std::endl;

  std::cout << "__d2 += d1" << std::endl;
  d2 += d1;
  print( "d1", d1 );
  print( "d2", d2 );
  std::cout << "__d2 -= d1" << std::endl;
  d2 -= d1;
  print( "d1", d1 );
  print( "d2", d2 );
  return 0;
}
