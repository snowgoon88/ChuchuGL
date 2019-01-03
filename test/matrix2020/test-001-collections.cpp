/* -*- coding: utf-8 -*- */

/** 
 * Test Collection of Things
 * - keep reference (iterator) on object while adding, moving.
 */

#include <list>
#include <iostream>
#include <string>
#include <algorithm>
// ********************************************************************* Types
using Thing = struct th {
  std::string name;
  double x, y;
};
std::ostream& operator<<( std::ostream& os, const Thing& th )
{
  os << "{" << th.name << ": " << th.x << ", " << th.y << "}";
  return os;
}
bool operator< (const Thing& lhs, const Thing& rhs)
{
  if (lhs.x < rhs.x)
    return true;
  return (lhs.y < rhs.y );
}
using Collec = std::list<Thing>;

Collec _collec;
Collec::iterator it_a, it_b, it_c;

// ************************************************************** display_list
void display_list()
{
  std::cout << "** LIST **" << std::endl;
  for( auto& th: _collec) {
    std::cout << "  " << th << std::endl;
  }
}
// ****************************************************************** Creation
void create()
{
  _collec.clear();

  std::cout << "__INSERT A end" << std::endl;
  it_a = _collec.insert( _collec.end(), {"A", 0.0, 1.0} );
  std::cout << "  A = " << *it_a << std::endl;

  std::cout << "__INSERT B front" << std::endl;
  it_b = _collec.insert( _collec.begin(), {"B", 2.0, 2.0} );
  std::cout << "  A = " << *it_a << std::endl;
  std::cout << "  B = " << *it_b << std::endl;

  std::cout << "__INSERT C end" << std::endl;
  it_c = _collec.insert( _collec.end(), {"C", 1.0, 0.0} );
  std::cout << "  A = " << *it_a << std::endl;
  std::cout << "  B = " << *it_b << std::endl;
  std::cout << "  C = " << *it_c << std::endl;

  std::cout << "__PUSH FRONT" << std::endl;
  _collec.push_front( {"D", 2.0, 2.0} );
  std::cout << "  A = " << *it_a << std::endl;
  std::cout << "  B = " << *it_b << std::endl;
  std::cout << "  C = " << *it_c << std::endl;
                      
}
// ***************************************************************** Transform
void reverse()
{
  std::cout << "__REVERSE" << std::endl;
  _collec.reverse();
  std::cout << "  A = " << *it_a << std::endl;
  std::cout << "  B = " << *it_b << std::endl;
  std::cout << "  C = " << *it_c << std::endl;
}
void sort()
{
  std::cout << "__SORT" << std::endl;
  _collec.sort();
  std::cout << "  A = " << *it_a << std::endl;
  std::cout << "  B = " << *it_b << std::endl;
  std::cout << "  C = " << *it_c << std::endl;
}
void change()
{
  std::cout << "__CHANGE A" << std::endl;
  it_a->x = 1.5;
  std::cout << "  A = " << *it_a << std::endl;
  std::cout << "  B = " << *it_b << std::endl;
  std::cout << "  C = " << *it_c << std::endl;
}
void move_last_up()
{
  std::cout << "__MOVE LAST UP" << std::endl;
  auto itlast = std::prev(_collec.end());
  std::move( itlast, std::next(itlast), std::prev(itlast) );
  std::cout << "  A = " << *it_a << std::endl;
  std::cout << "  B = " << *it_b << std::endl;
  std::cout << "  C = " << *it_c << std::endl;
}
void move_first_down()
{
  std::cout << "__MOVE FIRST DOWN" << std::endl;
  auto itfirst = _collec.begin();
  std::move( itfirst, std::next(itfirst), std::next(itfirst) );
  std::cout << "  A = " << *it_a << std::endl;
  std::cout << "  B = " << *it_b << std::endl;
  std::cout << "  C = " << *it_c << std::endl;
}
void move_2_1()
{
  std::cout << "__MOVE 2 1" << std::endl;
  auto itsrc = std::next(_collec.begin());
  std::move( itsrc, std::next(itsrc), std::prev(itsrc) );
  std::cout << "  A = " << *it_a << std::endl;
  std::cout << "  B = " << *it_b << std::endl;
  std::cout << "  C = " << *it_c << std::endl;
}  
void rotate()
{
  std::cout << "__ROTATE PREMS" << std::endl;
  std::rotate( _collec.begin(), std::next(_collec.begin()),
               std::next( std::next( _collec.begin() )) );
  std::cout << "  A = " << *it_a << std::endl;
  std::cout << "  B = " << *it_b << std::endl;
  std::cout << "  C = " << *it_c << std::endl;
}
void splice()
{
  std::cout << "__SPLICE" << std::endl;
  // pos, other, it
  _collec.splice( _collec.begin(), _collec, std::next( _collec.begin() ));
  std::cout << "  A = " << *it_a << std::endl;
  std::cout << "  B = " << *it_b << std::endl;
  std::cout << "  C = " << *it_c << std::endl;
}
// ********************************************************************** MAIN
int main(int argc, char *argv[])
{
  create();
  display_list();

  reverse();
  display_list();

  sort();
  display_list();

  change();
  display_list();

  sort();
  display_list();

  // move_last_up();
  // display_list();

  // move_first_down();
  // display_list();

  // move_2_1();
  // display_list();

  // rotate();
  // display_list();
  splice();
  display_list();
  

  return 0;
}

  

