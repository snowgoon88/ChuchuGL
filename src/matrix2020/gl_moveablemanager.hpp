/* -*- coding: utf-8 -*- */

#ifndef GL_MOVEABLEMANAGER_HPP
#define GL_MOVEABLEMANAGER_HPP

/** 
 * Manage Movable (and Sink)
 * - receive mouse click
 * - check if over Moveable
 * - move and drop on Sink
 */

#include <vector>
#include <list>
#include <sstream>

#include <gl_shader.hpp>
#include <SOIL/SOIL.h>               // Load images

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

namespace matrix2020 {

// ****************************************************************** Moveable
class Moveable
{
public:
  using Pos2D = glm::vec2;
  using TexVertex = struct s_TexVertex {
    GLfloat x, y, z; // pos screen (+ depth)
    GLfloat s, t;    // pos texture
  };
  using TexVertexV = std::vector<TexVertex>;
public:
  // ****************************************************** Moveable::creation
  Moveable( const Pos2D& topleft, const Pos2D& botright,
            const Pos2D& tex_topleft, const Pos2D& tex_botright) :
    _pos( {0.0, 0.0} ),
    _topleft( topleft ), _botright( botright ),
    _tex_topleft( tex_topleft ), _tex_botright( tex_botright )
  {
  }
  std::string str_dump () const
  {
    std::stringstream dump;
    dump << "at=( " << _pos[0] << ", " << _pos[1] << ")";

    return dump.str();
  }
  // ***************************************************** Moveable::is_inside
  bool is_inside( const Pos2D& pos )
  {
    // Is given Pos inside the Moveable shape ?
    if ( (pos[0] > _pos[0]+_topleft[0] and pos[0] < _pos[0]+_botright[0]) and
         (pos[1] < _pos[1]+_topleft[1] and pos[1] > _pos[1]+_botright[1]) ) {
      return true;
    }
    return false;
  }
  // ************************************************** Moveable::add_vertices
  void add_vertices( TexVertexV& vec, float depth_z )
  {
    // First triangle 0,0 -> 0,1 -> 1,1
    vec.push_back( TexVertex{ _pos[0] + _topleft[0], _pos[1] + _botright[1], depth_z, _tex_topleft[0], _tex_botright[1] } );
    vec.push_back( TexVertex{ _pos[0] + _botright[0], _pos[1] + _botright[1], depth_z, _tex_botright[0], _tex_botright[1] } );
    vec.push_back( TexVertex{ _pos[0] + _botright[0], _pos[1] + _topleft[1], depth_z, _tex_botright[0], _tex_topleft[1] } );

    // Second triangle 0,0 -> 1,1 -> 1,0
    vec.push_back( TexVertex{ _pos[0] + _topleft[0], _pos[1] + _botright[1], depth_z, _tex_topleft[0], _tex_botright[1] } );
    vec.push_back( TexVertex{ _pos[0] + _botright[0], _pos[1] + _topleft[1], depth_z, _tex_botright[0], _tex_topleft[1] } );
    vec.push_back( TexVertex{ _pos[0] + _topleft[0], _pos[1] + _topleft[1], depth_z, _tex_topleft[0], _tex_topleft[1] } );
  }
  // ***************************************************** Moveable::attributs
  Pos2D pos() const { return _pos; }
  void pos( const Pos2D& pos ) { _pos = pos; }
private:
  Pos2D _pos;
  Pos2D _topleft, _botright;
  
  Pos2D _tex_topleft, _tex_botright;
  
}; // class Moveable
// ***************************************************************************

// ***************************************************************************
// ********************************************************************** Sink
// ***************************************************************************
class Sink
{
public:
  using Pos2D = glm::vec2;
  using Vertex = struct s_Vertex {
    GLfloat x, y, z; // pos
    GLfloat r, g, b; // color
  };
  using VertexV = std::vector<Vertex>;
  using Color = struct {
    GLfloat r,g,b;
};
public:
  // ********************************************************** Sink::creation
  Sink( const Pos2D& pos, const Pos2D& topleft, const Pos2D& botright ) :
    _pos( pos ) , _topleft( topleft ), _botright( botright )
  {
  }
  std::string str_dump () const
  {
    std::stringstream dump;
    dump << "at=( " << _pos[0] << ", " << _pos[1] << ")";

    return dump.str();
  }
  // ****************************************************** Sink::add_vertices
  void add_vertices( VertexV& vec, float depth_z, Color col = {1.f,1.f,1.f} )
  {
    // frame around
    vec.push_back( Vertex{ _pos[0] + _topleft[0], _pos[1] + _botright[1], depth_z, col.r, col.g, col.b });
    vec.push_back( Vertex{ _pos[0] + _botright[0], _pos[1] + _botright[1], depth_z, col.r, col.g, col.b  });

    vec.push_back( Vertex{ _pos[0] + _botright[0], _pos[1] + _botright[1], depth_z, col.r, col.g, col.b  });
    vec.push_back( Vertex{ _pos[0] + _botright[0], _pos[1] + _topleft[1], depth_z, col.r, col.g, col.b  });

    vec.push_back( Vertex{ _pos[0] + _botright[0], _pos[1] + _topleft[1], depth_z, col.r, col.g, col.b  });
    vec.push_back( Vertex{ _pos[0] + _topleft[0], _pos[1] + _topleft[1], depth_z, col.r, col.g, col.b  });

    vec.push_back( Vertex{ _pos[0] + _topleft[0], _pos[1] + _topleft[1], depth_z, col.r, col.g, col.b  });
    vec.push_back( Vertex{ _pos[0] + _topleft[0], _pos[1] + _botright[1], depth_z, col.r, col.g, col.b  });

    // inside cross
    vec.push_back( Vertex{ _pos[0] + _topleft[0], _pos[1] + _botright[1], depth_z, col.r, col.g, col.b  });
    vec.push_back( Vertex{ _pos[0] + _botright[0], _pos[1] + _topleft[1], depth_z, col.r, col.g, col.b  });

    vec.push_back( Vertex{ _pos[0] + _botright[0], _pos[1] + _botright[1], depth_z, col.r, col.g, col.b  });
    vec.push_back( Vertex{ _pos[0] + _topleft[0], _pos[1] + _topleft[1], depth_z, col.r, col.g, col.b  });
  }
  // ***************************************************** Moveable::attributs
private:
  Pos2D _pos;
  Pos2D _topleft, _botright;
}; // class Sink

// ***************************************************************************
// ********************************************************* GLMoveableManager
// ***************************************************************************
class GLMoveableManager
{
// private:
//   using Pos2D = glm::vec2;
//   using TexVertex = struct s_TexVertex {
//     GLfloat x, y, z; // pos screen (+ depth)
//     GLfloat s, t;    // pos texture
//   };
//   using TexVertexV = std::vector<TexVertex>;
private:
  using Pos2D = Moveable::Pos2D;
  using TexVertex = Moveable::TexVertex;
  using TexVertexV = Moveable::TexVertexV;
  using Vertex = Sink::Vertex;
  using VertexV = Sink::VertexV;
private:
  // max number of square to draw
  static const unsigned int _max_size = 100;
  // Depth of Moveable on screen
  constexpr static const float _posz = 0.5f;
public:
  using MoveablePtr = Moveable*;
  using ListMoveable = std::list<Moveable>;
  using SinkPtr = Sink *;
  using ListSink = std::list<Sink>;
  // ********************************************* GLMoveableManager::creation
  GLMoveableManager() :
    _current_moveable(nullptr), _offset( {0.0, 0.0} ), _current_sink(nullptr)
  {
    // TEXTURES
    _texture_shader = new GLShader( "src/shaders/texture.v330.glsl",
                                    "src/shaders/texture.f330.glsl" );
    _proj_view_loc_tex = _texture_shader->getUniformLocation( "proj_view" );
    _texture_loc_tex = _texture_shader->getUniformLocation( "texture_id" );

    // load and create texture
    //std::string filename{"data/tex_mvm_463x210.png"};
    //std::string filename{"data/tex_mvm_462x210.png"};
    //std::string filename{"data/tex_mvm_512x210.png"};
    //std::string filename{"data/tex_mvm_160x210.png"};
    //std::string filename{"data/tex_mvm_156x210.png"};
    std::string filename{"data/tex_mvm_464x210.png"};
    //std::string filename{"data/tex_mvm_512x512.png"};
    std::cout << "__SOIL with " << filename  << std::endl;
    //glActiveTexture(GL_TEXTURE0);
    std::cout << "  active texture" << std::endl;
    _texture_id = SOIL_load_OGL_texture
      (
       filename.c_str(),                         // pathfile
       SOIL_LOAD_AUTO,                           // 
       SOIL_CREATE_NEW_ID,                       //
       SOIL_FLAG_INVERT_Y                        // Corrige les Y upside/down
       );
    std::cout << "  soil finished with id=" << _texture_id << std::endl;
    if(_texture_id == 0)
      std::cerr << "GLSprite: SOIL loading error: '" << SOIL_last_result() << "' (" << filename << ")" << std::endl;

    // A dynamical vbo in vao with x,y,z s,t (texture coordinates)
    glGenVertexArrays( 1, &_moveable_vao );
    glGenBuffers( 1, &_moveable_vbo );

    glBindVertexArray( _moveable_vao );
    glBindBuffer( GL_ARRAY_BUFFER, _moveable_vbo );
    glBufferData( GL_ARRAY_BUFFER,
                  GLMoveableManager::_max_size * sizeof(TexVertex),
                  NULL, //not linked to _moveables() as it might be empty
                  GL_DYNAMIC_DRAW );

    // position attribute
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE,
                           (3+2) * sizeof(float), // stride
                           (void*)0); // offset
    glEnableVertexAttribArray(0);
    // texture coordinate
    glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE,
                           (3+2) * sizeof(float), // stride
                           (void*)(3 * sizeof(float)) ); // offset
    glEnableVertexAttribArray(1);

    // Lines with Colors
    _line_shader = new GLShader( "src/shaders/base3D.vert330.glsl",
                                 "src/shaders/base3D.frag330.glsl" );
    _proj_view_loc_line = _line_shader->getUniformLocation( "proj_view" );
    _model_loc_line = _line_shader->getUniformLocation( "model" );

    glGenVertexArrays( 1, &_line_vao );
    glGenBuffers( 1, &_line_vbo );
    glBindVertexArray( _line_vao );

    glBindBuffer( GL_ARRAY_BUFFER, _line_vbo );
    // glBufferData( GL_ARRAY_BUFFER, sizeof(square_vtx), square_vtx,
    //               GL_STATIC_DRAW );
    glBufferData( GL_ARRAY_BUFFER,
                  GLMoveableManager::_max_size * sizeof(Vertex),
                  NULL, //not _line_vtx.data() as it might be empty
                  GL_DYNAMIC_DRAW );

    // position attribute
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE,
                           (3+3) * sizeof(float), // stride
                           (void*)0); // offset
    glEnableVertexAttribArray(0);
    // color coordinate
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE,
                           (3+3) * sizeof(float), // stride
                           (void*)(3 * sizeof(float)) ); // offset
    glEnableVertexAttribArray(1); 
  }
  // ****************************************** GLMoveableManager::destruction
  virtual ~GLMoveableManager()
  {
    if ( _texture_id > 0 ) {
      glDeleteTextures( 1, &_texture_id);
    }
    glDeleteVertexArrays( 1, &_moveable_vao);
    glDeleteBuffers( 1, &_moveable_vbo);
    delete _texture_shader;

    glDeleteVertexArrays( 1, &_line_vao);
    glDeleteBuffers( 1, &_line_vbo);
    delete _line_shader;
  }
  // ************************************************** GLMoveableManager::str
  std::string str_dump () const
  {
    std::stringstream dump;
    dump << "GLMoveableManager : ";
    dump << "move=";
    if (_current_moveable) {
      dump << _current_moveable->str_dump();
    }
    else {
      dump << "NULL";
    }
    dump << "sink=";
    if (_current_sink) {
      dump << _current_sink->str_dump();
    }
    else {
      dump << "NULL";
    }
    dump << "\nVertices";
    for( auto& vtx: _moveable_vtx) {
      dump << "  (" << vtx.x << "/" << vtx.y << "/" << vtx.z;
      dump << " : " << vtx.s << "/" << vtx.t << ")";
    }
    return dump.str();
  }
  // ******************************************* GLMoveableManager::update_vbo
  void update_vbo()
  {
    // update list of Moveable Position
    // TODO: could only update positions, as Tex Coord should be ok
    _moveable_vtx.clear();
    for( auto& mov: _moveables) {
      mov.add_vertices( _moveable_vtx, GLMoveableManager::_posz );
    }
    
    // specify the buffer we are about to update
    glBindBuffer( GL_ARRAY_BUFFER, _moveable_vbo );
    // use std::vector to store data ?
    // ask for reallocation, glBufferData with NULL and same parameters
    // see https://www.khronos.org/opengl/wiki/Buffer_Object_Streaming)
    glBufferData( GL_ARRAY_BUFFER,
                  GLMoveableManager::_max_size * sizeof(TexVertex),
                  NULL, 
                  GL_DYNAMIC_DRAW );
    
    // will call glBufferSubData on the entire buffer
    glBufferSubData( GL_ARRAY_BUFFER,
                     0,                                   //start of sub
                     _moveable_vtx.size() * sizeof(TexVertex), //length sub
                     _moveable_vtx.data() );                // data

    // update list of Sinks
    _line_vtx.clear();
    for( auto& sink: _sinks) {
      sink.add_vertices( _line_vtx, GLMoveableManager::_posz, {1.f,1.f,0.f} );
    }
    // specify the buffer we are about to update
    glBindBuffer( GL_ARRAY_BUFFER, _line_vbo );
    // ask for reallocation, glBufferData with NULL and same parameters
    // see https://www.khronos.org/opengl/wiki/Buffer_Object_Streaming)
    glBufferData( GL_ARRAY_BUFFER,
                  GLMoveableManager::_max_size * sizeof(Vertex),
                  NULL, 
                  GL_DYNAMIC_DRAW );
    // will call glBufferSubData on the entire buffer
    glBufferSubData( GL_ARRAY_BUFFER,
                     0,                                   //start of sub
                     _line_vtx.size() * sizeof(Vertex), //length sub
                     _line_vtx.data() );                // data
  }
  // ********************************************* GLMoveableManager::Moveable
  void add_moveable( MoveablePtr moveable )
  {
    _moveables.push_back( *moveable );
  }
  // ************************************************* GLMoveableManager::Sink
  void add_sink( SinkPtr sink )
  {
    _sinks.push_back( *sink );
  }
  // *********************************************** GLMOveableManager::render
  void render( const glm::mat4& projview )
  {
    // Transparence
    // Enable alpha
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // draw sinks using lines
    _line_shader->use();
    // create transformations
    glm::mat4 model( 1.0f );
    // pass transformation matrices to the shader
    glUniformMatrix4fv( _proj_view_loc_line, 1, GL_FALSE,
                        glm::value_ptr( projview ) );
    glUniformMatrix4fv( _model_loc_line, 1, GL_FALSE,
                        glm::value_ptr(model) );

    glBindVertexArray(_line_vao);
    glDrawArrays(GL_LINES, 0, _line_vtx.size() ); // mode, first, count

    // draw moveable usin textures
    _texture_shader->use();
    // pass transformation matrices to the shader
    glUniformMatrix4fv( _proj_view_loc_tex, 1, GL_FALSE,
                        glm::value_ptr( projview ) );

    glBindTexture(GL_TEXTURE_2D, _texture_id);
    glBindVertexArray(_moveable_vao);
    glDrawArrays(GL_TRIANGLES, 0, _moveable_vtx.size()); // mode, first, count
  }
  // *********************************************** GLMoveableManager::on_xxx
  void on_click( const Pos2D& pos )
  {
    // Look in ListMoveable for a suitable Moveable to move
    // by default, _current_sink will be the Sink of the Moveable
  }
  void on_release( const Pos2D& pos )
  {
    // If has _current_moveable, set in potential Sink
  }
  void on_move( const Pos2D& pos )
  {
    // if has _current_moveable, move and check for new _current_sink
  }
private:
  // ******************************************** GLMoveableManager::attributs
  ListMoveable _moveables;
  ListSink    _sinks;
  Moveable* _current_moveable;
  Pos2D _offset;  // offset between mouse Pos and Moveable Pos
  Sink*    _current_sink;
  
  // For graphics
  GLShader* _texture_shader;
  GLuint _texture_loc_tex;
  GLuint _proj_view_loc_tex;
  GLuint _texture_id;
  GLuint _moveable_vao, _moveable_vbo;
  TexVertexV _moveable_vtx;

  GLShader* _line_shader;
  GLuint _proj_view_loc_line;
  GLuint _model_loc_line;
  GLuint _line_vao, _line_vbo;
  VertexV _line_vtx;

}; // class GLMoveableManager

};  // matrix2020


#endif // GL_MOVEABLEMANAGER_HPP
