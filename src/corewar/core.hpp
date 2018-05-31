/* -*- coding: utf-8 -*- */

#ifndef CORE_HPP
#define CORE_HPP

/** 
 * Basic CoreWar implementation.
 * @see : http://vyznev.net/corewar/guide.html
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
  Instruction (const Instruction& i) :
    op_str(i.op_str), a_adr_str(i.a_adr_str), b_adr_str(i.b_adr_str)
  {
  };
  Instruction& operator=(const Instruction& i) 
  { 
    if(this != &i) { // protect against invalid self-assignment
      op_str = i.op_str;
      a_adr_str = i.a_adr_str;
      b_adr_str = i.b_adr_str;
      
    }
    return *this;
  };
  // ************************************************ Instruction::destruction
  virtual ~Instruction()
  {
    //std::cout << str_dump() << " deleted"  << std::endl;
  }
  // ******************************************************** Instruction::get
  int get_val_a() const { return get_val( a_adr_str ); };
  int get_val_b() const { return get_val( b_adr_str ); };
  int get_val ( const std::string& str ) const
  {
    // Check direct (#), (@)
    if( (str.find( "#" ) == 0) or (str.find( "@") == 0)) {
      return std::stoi( str.substr( 1 ));
    }
    else {
      return std::stoi( str );
    }
  };
  // ******************************************************** Instruction::put
  void put_b( int val )
  {
    b_adr_str = std::to_string( val );
  };
  void put_direct_b( int val )
  {
    b_adr_str = "#"+std::to_string( val );
  };
  //**************************************** Instruction::is_direct_addressing
  bool is_direct_addressing_a() const { return is_direct( a_adr_str ); };
  bool is_direct_addressing_b() const { return is_direct( b_adr_str ); };
  bool is_direct( const std::string& str ) const {
    return (str.find( "#") == 0);
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
      int a_adr = ins.get_val_a();
      int b_adr = ins.get_val_b();
      put( address+b_adr, get( address+a_adr ) );
    }
    else if( ins.op_str.compare( "add" ) == 0 ) {
      int a_val = ins.get_val_a();
      int b_val = ins.get_val_b();
      
      // if a_adr is immediate_adressing
      if( ins.is_direct_addressing_a() ) {
        // if b_adr is immediate_adressing
        if( ins.is_direct_addressing_b() ) {
          // add a_val to b_val
          Instruction new_ins = ins;
          new_ins.put_direct_b( a_val + b_val );
          put( address, new_ins );
        }
        else {
          // add a_val to b_adr of Instruction at (address+b_val) 
          Instruction new_ins( get( address+b_val ) );
          if( new_ins.is_direct_addressing_b() ) {
            new_ins.put_direct_b( a_val+new_ins.get_val_b() );
          }
          else {
            new_ins.put_b( a_val+new_ins.get_val_b() );
          }
          put( address+b_val, new_ins );
        }
      }
      else {
        std::cout << "CW::Core::exec_at ADD A, x not implemented" << std::endl;
      }
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
