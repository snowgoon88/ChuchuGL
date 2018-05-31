/* -*- coding: utf-8 -*- */

/** 
 * Basic tests about Core War.
 */

#include "core.hpp"
#include <iostream>                     // cout

namespace cw = corewar;

/**
 * Creation of a simple mov 0, 1 instruction 
 */
void test_instruction(int argc, char *argv[])
{
  cw::Instruction in1( "mov", "0", "1" );

  std::cout << "in1=" << in1.str_dump() << std::endl;
}

/**
 * Create core and put some 'mov 0, 1" at some places.
 */
void test_core(int argc, char *argv[] )
{
  cw::Core core;

  std::cout << core.str_dump() << std::endl;

  for( unsigned int i = 3; i < 32; i += 7) {
    std::cout << "  put at " << i << std::endl;
    core.put( i, cw::Instruction( "mov", "0", "1") );
    std::cout << core.str_dump() << std::endl;

  }
}

/**
 * Create core with imp at 3.
 * Then run 'nb' times this "program"
 */
void test_imp(unsigned int nb)
{
  cw::Core core;
  int adr = 3;
  core.put( adr, cw::Instruction( "mov", "0", "1" ) );

  std::cout << "** Start" << std::endl;
  std::cout << core.str_dump() << std::endl;

  for( unsigned int i = 0; i < nb; ++i) {
    core.exec_at( adr+i );
    std::cout << "** Exec at " << (adr+i) << std::endl;
    std::cout << core.str_dump() << std::endl;
  }
}

/**
 * Create core with various ADD and test
 */
void test_add()
{
  cw::Core core(4);
  std::cout << "** i:ADD #2,#3 => i:ADD #2,#5" << std::endl; 
  core.put( 1, cw::Instruction( "add", "#2", "#3" ) );
  std::cout << core.str_dump() << std::endl;
  core.exec_at( 1 );
  std::cout << "========" << std::endl <<core.str_dump() << std::endl;

  std::cout << "** i:ADD 2,#3 => NOT" << std::endl; 
  core.put( 1, cw::Instruction( "add", "2", "#3" ) );
  std::cout << core.str_dump() << std::endl;
  core.exec_at( 1 );
  std::cout << "========" << std::endl <<core.str_dump() << std::endl;

  std::cout << "** i:ADD #2,1 => i+1:XXX ,#B+3" << std::endl; 
  core.put( 1, cw::Instruction( "add", "#2", "1" ) );
  core.put( 2, cw::Instruction( "dat", "#4", "#4" ) );
  std::cout << core.str_dump() << std::endl;
  core.exec_at( 1 );
  std::cout << "========" << std::endl <<core.str_dump() << std::endl;

  std::cout << "** i:ADD #2,1 => i+1:XXX ,B+3" << std::endl; 
  core.put( 1, cw::Instruction( "add", "#2", "1" ) );
  core.put( 2, cw::Instruction( "dat", "#4", "4" ) );
  std::cout << core.str_dump() << std::endl;
  core.exec_at( 1 );
  std::cout << "========" << std::endl <<core.str_dump() << std::endl;
  
}
int main(int argc, char *argv[])
{
  std::cout << "__***** TEST INSTRUCTION" << std::endl;
  test_instruction(argc, argv);

  std::cout << "__***** TEST CORE" << std::endl;
  test_core(argc, argv);

  std::cout << "__***** TEST EXEC IMP" << std::endl;
  test_imp( 2 );

  std::cout << "__***** TEST EXEC ADD" << std::endl;
  test_add();

  
  
  return 0;
}
