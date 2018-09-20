/* -*- coding: utf-8 -*- */

#ifndef FOV_HAMMING_HPP
#define FOV_HAMMING_HPP

/** 
 * Compute a Field of View using ONLY Hamming distance.
 * It can be couter intuitive as "vision" does not travel in straight line.
 *
 * _status[pos] indicates the distance of 'pos' (in Environment coordinate)
 * from this->_pos.
 */


#include <matrix2020/m_def.hpp>
#include <matrix2020/m_environment.hpp>

#include <map>
#include <sstream>
#include <stdexcept>      // std::out_of_range

namespace matrix2020
{
// ***************************************************************************
// **************************************************************** FOVHamming
// ***************************************************************************
/**
 * Environment &env : ref to indicate that this class relies on env to exist
 * but stored as pointer for copy and assignment.
 * same for Pos &pos
 */
class FOVHamming
{
  // A map between a Pos and the dist from center of FOV
  using Status = std::map<Pos,int>;
  
public:
  // **************************************************** FOVHamming::creation
  FOVHamming( Environment &env, Pos& pos, int max_distance ) :
    _env(&env), _status(), _pos(&pos), _max_distance(max_distance)
  {
    refresh();
  }
  // **************************************************** FOVHamming::str_dump
  std::string str_dump () const
  {
    std::stringstream dump;
    dump << "FOVHamming = ";
    for( auto& st: _status) {
      dump << st.first << ":" << st.second << "; ";
    }

    return dump.str();
  }
  std::string str_display () const
  {
    std::stringstream display;
    // size of array
    int size = 2 * (int)_max_distance + 1;

    for( int row = size-1; row >= 0; --row) {
      for( int col = 0; col < size; ++col) {
        auto res = _status.find( Pos( col - _max_distance + _pos->x,
                                      row - _max_distance + _pos->y) );
        // not found
        if (res == _status.end()) {
          display << '.'; 
        }
        else {
          display << res->second;
        }
      }
      display << std::endl;
    }

    return display.str();
  }
  std::string str_view () const
  {
    std::stringstream view;
    // size of array
    int size = 2 * (int)_max_distance + 1;

    for( int row = size-1; row >= 0; --row) {
      for( int col = 0; col < size; ++col) {
        auto res = _status.find( Pos( col - _max_distance + _pos->x,
                                      row - _max_distance + _pos->y) );
        // not found
        if (res == _status.end()) {
          view << '?'; 
        }
        else {
          try {
            if (_env->is_obstacle( res->first )) {
              view << 'X';
            }
            else if (_env->is_cell( res->first )) {
              view << '.';
            }
            else {
              view << '@';
            }
          }
          catch (std::out_of_range& e) {
            std::cout << "caught " << e.what() << std::endl;
            view << '_';
          }
          catch (...) {
            std::cerr << "Unknown exception" << std::endl;
            view << '$';
          }
        }
      }
      view << std::endl;
    }

    return view.str();
  }
  // ***************************************************** FOVHamming::refresh
  void refresh()
  {
    _status.clear();
    expand( *_pos, 0 );  
  }
  // *************************************************** FOVHamming::attributs
  Environment* _env;
  Status _status;
  Pos* _pos;
  int _max_distance;
  
  // ****************************************************** FOVHamming::expand
  void expand( const Pos& pos, int distance )
  {
    //DEBUG std::cout << "  at " << pos;
    try {
      // if current pos has no _status (i.e : never visited ) then
      // set its new status 
      if (_status.count( pos) == 0) {
        _status[pos] = distance;
      }
      // else if current_distance is lower, change it
      else if (_status[pos] > distance) {
        _status[pos] = distance;
        // and expand
      }
      //DEBUG std::cout << " " << str_dump() << std::endl;
      // expand if not already at _max_distance
      // AND current pos is not an obstacle
      //DEBUG std::cout << "  Obstacle=" << (_env->is_obstacle( pos ) ? "true" : "false") << std::endl;
      if (( distance < _max_distance ) and not _env->is_obstacle( pos )) {
        for( auto& dir: AllDir) {
          expand( pos+dir, distance+1 );
        }
      }
    }
    catch (std::out_of_range& e) {
      std::cout << "caught " << e.what() << std::endl;
    }
    catch (...) {
      std::cerr << "Unknown exception" << std::endl;
    }
    
    return;
  }

};
// ********************************************************** FOVHamming - END
// ***************************************************************************

}; // namespace matrix2020

#endif // FOV_HAMMING_HPP
