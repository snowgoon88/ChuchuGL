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

int main(int argc, char *argv[])
{
  std::cout << "__***** TEST INSTRUCTION" << std::endl;
  test_instruction(argc, argv);

  std::cout << "__***** TEST CORE" << std::endl;
  test_core(argc, argv);

  std::cout << "__***** TEST EXEC IMP" << std::endl;
  test_imp( 2 );
  
  return 0;
}
