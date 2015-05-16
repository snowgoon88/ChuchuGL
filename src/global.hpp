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

static Direction dir_right = { 1, {1, 0}, "R"};
static Direction dir_left = { 3, {-1, 0}, "L"};

