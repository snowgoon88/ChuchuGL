/* -*- coding: utf-8 -*- */

#ifndef GL_DEF_HPP
#define GL_DEF_HPP

/** 
 * Type definition for usual GLShaders.
 */

#include <vector>

namespace matrix2020 {
// ******************************************************************** Vertex
using TexVertex = struct s_TexVertex {
  GLfloat x, y, z; // pos screen (+ depth)
  GLfloat s, t;    // pos texture
};
using TexVertexV = std::vector<TexVertex>;

using LineVertex = struct s_LineVertex {
  GLfloat x, y, z; // pos
  GLfloat r, g, b; // color
};
using LineVertexV = std::vector<LineVertex>;
// ***************************************************************************

};  // matrix2020

#endif // GL_DEF_HPP
