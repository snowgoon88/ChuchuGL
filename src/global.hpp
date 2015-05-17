/* -*- coding: utf-8 -*- */

#pragma once

/** 
 * Quelques types et variables utiles partout
 */
// ******************************************************************* TYPEDEF
typedef double TPos;
typedef double TSpd;
typedef struct {
  TPos x,y;
} TVec2;

// ***************************************************************** DIRECTION
typedef struct {
  unsigned int index;
  TVec2 vec;
  std::string str;
} Direction;

static Direction _dir_up = { 0, {0, 1}, "U" };
static Direction _dir_right = { 1, {1, 0}, "R"};
static Direction _dir_down = { 2, {0, -1}, "D"};
static Direction _dir_left = { 3, {-1, 0}, "L"};

static const unsigned int _dir_size = 4;
static Direction _l_dir[_dir_size] = {_dir_up, _dir_right, _dir_down, _dir_left};

static Direction* _dir_rotate( Direction* dir )
{
  return &(_l_dir[ (dir->index+1) % _dir_size ]);
}
static std::string _dir_str( unsigned int index )
{
  return _l_dir[index].str;
}
