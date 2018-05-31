/* -*- coding: utf-8 -*- */

#ifndef CORE_HPP
#define CORE_HPP

/** 
 * Basic CoreWar implementation.
 * @see : http://vyznev.net/corewar/guide.html#start_instr
 */

#include <string>
#include <sstream>

#include <vector>
#include <iostream>                     // cout

namespace corewar {
// ***************************************************************************
// *************************************************************** Instruction
// ***************************************************************************
class Instruction
{
public:
  // *************************************************** Instruction::creation
  Instruction( const std::string& op,
               const std::string& a,
               const std::string& b ):
    op_str(op), a_adr_str(a), b_adr_str(b)
  {
  };
  // ************************************************ Instruction::destruction
  virtual ~Instruction()
  {
  };
  // **************************************************** Instruction::display
  std::string str_dump()
  {
    return op_str+" "+a_adr_str+", "+b_adr_str;
  };
  // ************************************************** Instruction::attributs
  std::string op_str;
  std::string a_adr_str;
  std::string b_adr_str;
};

// ***************************************************************************
// ********************************************************************** Core
// ***************************************************************************
class Core
{
public:
  // ********************************************************** Core::creation
  /**
   * Create a core of given 'size', filled with "nop -,-" Instructions
   */
  Core( const unsigned int size=16 )
  {
    for( unsigned int i = 0; i < size; ++i) {
      _core.push_back( Instruction( "nop", "-", "-" ) );
    }
  };
  // *********************************************************** Core::put/get
  void put( int address, const Instruction& ins )
  {
    _core[ address2index( address ) ] = ins;
  };
  Instruction get( const int address ) const {
    return _core[ address2index( address ) ];
  }
  // *********************************************************** Core::exec_at
  /**
   * Exec the instruction at given 'address'
   */
  void exec_at( int address )
  {
    // current instruction is not mutable
    const Instruction& ins = get( address );

    if( ins.op_str.compare( "mov" ) == 0 ) {
      int a_adr = std::stoi( ins.a_adr_str );
      int b_adr = std::stoi( ins.b_adr_str );
      put( address+b_adr, get( address+a_adr ) );
    }
    else if( ins.op_str.compare( "nop" ) == 0 ) {
    }
    else {
      std::cout << "CW::Core::exec_at unknown instruction" << std::endl;
    }
    
  }
  // ***************************************************** Core::address2index
  /**
   * Transform an 'address' to a valid index of the _core.
   */
  int address2index( int const address ) const
  {
    // Take the modulo of the address
    return (address % _core.size());
  }
  // *********************************************************** Core::display
  std::string str_dump()
  {
    std::stringstream ss;

    for (auto it = _core.begin(); it != _core.end(); ++it) {
      ss << (it - _core.begin());
      ss << "\t" << it->str_dump();
      ss << std::endl;
    }
    
    return ss.str();
  };
  // ********************************************************* Core::attributs
  std::vector<Instruction> _core;
};

}; // namespace corewar

#endif // CORE_HPP
