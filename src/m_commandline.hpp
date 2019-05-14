/* -*- coding: utf-8 -*- */

#ifndef M_COMMANDLINE_HPP
#define M_COMMANDLINE_HPP

/** 
 * Model for a command line of utf16+color+police.
 * A sequence (deque) of
 * - char16_t : caracter
 * - Color * : nullptr if no change
 * - Face * : nullptr if no change 
 */
#include <string>     // string, wstring
#include <sstream>    // stringstream, wstringstream
#include <deque>
#include <iterator>   // insert_iterator

// ***************************************************************************
// ************************************************************** MCommandLine
// ***************************************************************************
class MCommandLine
{
public:
  // ****************************************************** MCommandLine::type
  using Color = struct { float r,g,b; };
  using Face = void;
  using MEvent = struct s_mevent {
    char16_t c;
    Color*   color;
    Face*    face;
    s_mevent( char16_t c) : c(c), color(nullptr), face(nullptr) {};
    s_mevent( char16_t c, const Color& col ) :
      c(c),
      color( new Color{col.r,col.g, col.b} ),
      face(nullptr)
    {};
    ~s_mevent()
    { if( color ) delete color; }
  };
  std::wstring wstr_event ( const MEvent& ev ) const
  {
    std::u16string event;
    
    if (ev.color) {
      event.append( u"[C]" );
    }
    event.append( 1, ev.c ); 
    std::wstring wevent( event.begin(), event.end() );
    return wevent;
  }
  using MEventSeq = std::deque<MEvent>;
  // ************************************************** MCommandLine::creation
  MCommandLine() :
    _cmd(), _cursor(_cmd.end())
  {
  }

  // ************************************************** MCommandLine::str_dump
  std::wstring wstr_dump () const
  {
    std::wstringstream dump;
    dump << "CMD (" << _cursor-_cmd.begin() << "/" << _cmd.size() << "): ";
    for( auto& ev: _cmd) {
      dump << wstr_event( ev );
    }

    return dump.str();
  }
  // ****************************************************** MCommandLine::init
  void init()
  {
  }
  // ******************************************************** MCommandLine:add
  void add( char16_t c )
  {
    _cursor = _cmd.insert( _cursor, MEvent( c ));
    _cursor++;
  }
  void add( char16_t c, const Color& col )
  {
    _cursor = _cmd.insert( _cursor, MEvent(c, col) );
    _cursor++;
  }

public:
  // ************************************************* MCommandLine::attributs
  MEventSeq _cmd;
  MEventSeq::iterator _cursor;
  
}; // MCommandLine

#endif // M_COMMANDLINE_HPP
