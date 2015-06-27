/* -*- coding: utf-8 -*- */

#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include <string>

/** 
 * Quelques types et variables utiles partout
 */
// ******************************************************************* TYPEDEF
typedef double Pos;
typedef double Spd;
typedef struct {
  Pos x,y;
} Vec2;

// ***************************************************************** DIRECTION
typedef struct {
  unsigned int index;
  Vec2 vec;
  std::string str;
} Direction;

static Direction _dir_up = { 0, {0, 1}, "U" };
static Direction _dir_right = { 1, {1, 0}, "R"};
static Direction _dir_down = { 2, {0, -1}, "D"};
static Direction _dir_left = { 3, {-1, 0}, "L"};

static const unsigned int _dir_size = 4;
static Direction _l_dir[_dir_size] = {_dir_up, _dir_right, _dir_down, _dir_left};

static const Direction* _dir_rotate( const Direction* dir )
{
  return &(_l_dir[ (dir->index+1) % _dir_size ]);
}
static std::string _dir_str( unsigned int index )
{
  return _l_dir[index].str;
}
// ******************************************************************** MColor
typedef struct {
  unsigned int index;
  std::string str;
} MColor;

static MColor _col_blue = { 0, "B" };
static MColor _col_yellow = { 0, "J" };
static MColor _col_red = { 0, "R" };
static MColor _col_green = { 0, "V" };

static const unsigned int _col_size = 4;
static MColor _l_col[_col_size] = {_col_blue, _col_yellow, _col_red, _col_green};
static std::string _col_str( unsigned int index )
{
  return _l_col[index].str;
}

#endif // GLOBAL_HPP
