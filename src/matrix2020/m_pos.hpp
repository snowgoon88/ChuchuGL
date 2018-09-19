/* -*- coding: utf-8 -*- */

#ifndef M_POS_HPP
#define M_POS_HPP

/** 
 * Pos hold x,y int coordinates.
 */

#include <iostream>       // std::cout
#include <string.h>       

namespace matrix2020
{
// ***************************************************************************
// *********************************************************************** Pos
// ***************************************************************************
class Pos
{
public:
  // *********************************************************** Pos::creation
  Pos( int x, int y) : x(x), y(y) {}
  // copy constructor not needed as no ressources to manage : implicit ok
  //Pos( const Pos& other ) : x(other.x), y(other.y) {}
  // destructor not needed as no ressources to manage : implicit ok
  //virtual ~Pos() {}
  // assignment creator : implicit
  // **************************************************************** Pos::str
  std::string str_display() const;
  // ***************************************************** Pos::unary operator
  /** Binary will use that outside of class */
  Pos& operator+=( const Pos& rh )
  {
    x += rh.x; y += rh.y;
    return *this;
  }
  Pos& operator-=( const Pos& rh )
  {
    x -= rh.x; y -= rh.y;
    return *this;
  }
public:
  // ********************************************************** Pos::attributs
  int x;
  int y;
};

// *****************************************************Pos - stream operator
/** if it where using private member, declared as friend IN class and inlined
 * public:
 *  friend std::ostream& operator<<( std::ostream&, const Pos& obj)
 *  {
 *    // stream obj's data into os
 *    return os;
 *  }
 */
inline std::ostream& operator<<(std::ostream& os, const Pos& obj)
{
  os << "( " << obj.x << ", " << obj.y << ")";
  return os;
}

// ***************************************************** Pos - binary operator
inline Pos operator+( Pos lhs, const Pos& rhs)
{
  lhs += rhs;
  return lhs;
}
inline Pos operator-( Pos lhs, const Pos& rhs)
{
  lhs -= rhs;
  return lhs;
}
// *************************************************** Pos - binary comparison
inline bool operator==(const Pos& lhs, const Pos& rhs)
{
  /* do actual comparison */
  return (lhs.x == rhs.x ) && (lhs.y == rhs.y);
}
inline bool operator!=(const Pos& lhs, const Pos& rhs)
{return !operator==(lhs,rhs);}
inline bool operator< (const Pos& lhs, const Pos& rhs)
{
  // To get Complete order p1 < p2 <=>
  //  x1+y1 < x2+y2 OR (x1+y1==x2+y2 AND y1<y2)
  // open semi-line from pos with dir 1,-1
  // plus close semi-ine from pos-(0,1) with direction -1,1
  
  return (lhs.x+lhs.y < rhs.x+rhs.y ) ||
    ((lhs.x + lhs.y == rhs.x+rhs.y) && (lhs.y < rhs.y));
}
inline bool operator> (const Pos& lhs, const Pos& rhs)
{return  operator< (rhs,lhs);}
inline bool operator<=(const Pos& lhs, const Pos& rhs)
{return !operator> (lhs,rhs);}
inline bool operator>=(const Pos& lhs, const Pos& rhs)
{return !operator< (lhs,rhs);}
// ***************************************************************** Pos - END
// ***************************************************************************

}; // namespace matrix2020

#endif // M_POS_HPP
