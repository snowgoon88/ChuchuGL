/* -*- coding: utf-8 -*- */

#pragma once

/** 
 * Une Cell avec
 * - flèche : _arrow = NULL ou &Direction
 * - murs : TODO ---> tableau de 4 boolean ??
 */

#include <sstream>                        // std::stringstream

#include <global.hpp>

// ***************************************************************************
// ********************************************************************** CASE
// ***************************************************************************
class Cell
{
public:
  // ****************************************************************** CREATION
  /** Avec position */
  Cell( TVec2 position = {0,0} ) :
    _pos(position), _arrow(NULL) 
  {};

  // *********************************************************************** STR
  /** Dump avec string */
  std::string str_dump()
  {
    std::stringstream dump;
    
    dump << "CELL";
    dump << " at (" << _pos.x << "; " << _pos.y << ")";
    if( _arrow ) dump << " ARROW to " << _arrow->str;

    return dump.str();
  };
  // ******************************************************************* LEAVING
  /** Calcule la direction que l'on a en sortant de la Cell
   * quand on avait la direction dir.
   */
  Direction* dir_arrive_from( Direction* dir )
  {
    // Par défaut on va tout droit
    Direction* next_dir = dir;
    // Mais _arrow est prioritaire
    if(_arrow) {
      next_dir = _arrow;
    }
    // Ensuite, on regarde s'il y a des murs
    
    return next_dir;
  };
  // ****************************************************************** VARIABLE
  TVec2& pos() {return _pos;};
  void set_arrow( Direction* dir) {_arrow = dir;}
private:
  /** Position */
  TVec2 _pos;
  /** Arrow : NULL ou ref vers Direction */
  Direction* _arrow;
};
