/* -*- coding: utf-8 -*- */

#include <sstream>
#include <matrix2020/m_pos.hpp>

using namespace matrix2020;

// ********************************************************** Pos::str_display
std::string Pos::str_display() const
{
  std::stringstream disp;
  disp << *this;
  
  return disp.str();
}
