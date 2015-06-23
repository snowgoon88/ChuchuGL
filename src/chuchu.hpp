/* -*- coding: utf-8 -*- */

#ifndef CHUCHU_CPP
#define CHUCHU_CPP

/** 
 * Un Chuchu qui avance dans des Cell.
 * en sortant d'une Cell, obtient sa nouvelle Direction.
 *
 * _pos : position sur le World => toujours positif. Un chuchu dont la 
 *        _pos.x et _pos.y sont entre 0 et 1 est dans la case (0.0).
 * 
 */

#include "global.hpp"
#include "cell.hpp"

#include <string>                      // std::string

// ***************************************************************************
// ******************************************************************** CHUCHU
// ***************************************************************************
class Chuchu {
public:
  // **************************************************************** creation
  /** Creation avec une position et une vitesse */
  Chuchu( Vec2 position = {0,0}, Direction* dir = &_dir_right, Spd speed = 0.1 );
  // ********************************************************************* str
  /** Dump avec string */
  std::string str_dump() const;
  // ****************************************************************** update
  /** Mise à jour de la position */
  void update( double delta_t );
  // *************************************************************** attributs
  /** _pos */
  const Vec2& pos() const {return _pos;};
  void set_pos( const Pos& x, const Pos& y ) {_pos.x = x; _pos.y=y;};
  /** 
   * Quand on lui attribue une Cell, le Chuchu regarde s'il ENTER
   * ou EXIT cette CELL. 
   * En cas de EXIT, déterminer sa nouvelle _dir
   *
   * @return : true si le chuchu vient de changer de Cell
   */
  bool set_cell( Cell* cell );
  /** dir */
  const Direction& dir() const {return *_dir;};
private:
  /** Position */
  Vec2 _pos;
  /** Direction actuelle */
  Direction* _dir;
  /** Vitesse */
  Spd _spd;
  /** Cell sur laquelle se trouve le Chuchu */
  Cell* _cell;
  /** Est-ce que le Chuchu a été maj en sortant de sa Cell */
  bool _is_exiting;
};
// ***************************************************************************
#endif // CHUCHU_CPP
