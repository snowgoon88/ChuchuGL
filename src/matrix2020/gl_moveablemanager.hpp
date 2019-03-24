/* -*- coding: utf-8 -*- */

#ifndef GL_MOVEABLEMANAGER_HPP
#define GL_MOVEABLEMANAGER_HPP

/** 
 * Manage Movable (and Sink)
 * - receive mouse click
 * - check if over Moveable
 * - move and drop on Sink
 */

#include <list>
#include <sstream>
#include <memory>

#include <gl_shader.hpp>
#include <SOIL/SOIL.h>               // Load images
#include <matrix2020/gl_def.hpp>
#include <transition_func.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

namespace matrix2020 {


// ***************************************************************************
// ********************************************************************** Sink
// ***************************************************************************
class Sink
{
public:
  using Pos2D = glm::vec2;
  using Color = struct {
    GLfloat r,g,b;
  };
public:
  // ********************************************************** Sink::creation
  Sink( const std::string name,
       const Pos2D& pos, const Pos2D& topleft, const Pos2D& botright ) :
    _name(name),
    _pos( pos ) , _topleft( topleft ), _botright( botright )
  {
  }
  std::string str_dump () const
  {
    std::stringstream dump;
    dump << _name << " at=( " << _pos[0] << ", " << _pos[1] << ")";

    return dump.str();
  }
  // ****************************************************** Sink::add_vertices
  void add_frame_cross( LineVertexV& vec, float depth_z,
                        Color col = {1.f,1.f,1.f} )
  {
    // frame around
    vec.push_back( LineVertex{ _pos[0] + _topleft[0], _pos[1] + _botright[1], depth_z, col.r, col.g, col.b });
    vec.push_back( LineVertex{ _pos[0] + _botright[0], _pos[1] + _botright[1], depth_z, col.r, col.g, col.b  });

    vec.push_back( LineVertex{ _pos[0] + _botright[0], _pos[1] + _botright[1], depth_z, col.r, col.g, col.b  });
    vec.push_back( LineVertex{ _pos[0] + _botright[0], _pos[1] + _topleft[1], depth_z, col.r, col.g, col.b  });

    vec.push_back( LineVertex{ _pos[0] + _botright[0], _pos[1] + _topleft[1], depth_z, col.r, col.g, col.b  });
    vec.push_back( LineVertex{ _pos[0] + _topleft[0], _pos[1] + _topleft[1], depth_z, col.r, col.g, col.b  });

    vec.push_back( LineVertex{ _pos[0] + _topleft[0], _pos[1] + _topleft[1], depth_z, col.r, col.g, col.b  });
    vec.push_back( LineVertex{ _pos[0] + _topleft[0], _pos[1] + _botright[1], depth_z, col.r, col.g, col.b  });

    // inside cross
    vec.push_back( LineVertex{ _pos[0] + _topleft[0], _pos[1] + _botright[1], depth_z, col.r, col.g, col.b  });
    vec.push_back( LineVertex{ _pos[0] + _botright[0], _pos[1] + _topleft[1], depth_z, col.r, col.g, col.b  });

    vec.push_back( LineVertex{ _pos[0] + _botright[0], _pos[1] + _botright[1], depth_z, col.r, col.g, col.b  });
    vec.push_back( LineVertex{ _pos[0] + _topleft[0], _pos[1] + _topleft[1], depth_z, col.r, col.g, col.b  });
  }
  // ********************************************************* Sink::attributs
public:
  Pos2D pos() const { return _pos; }
private:
  std::string _name;
  Pos2D _pos;
  Pos2D _topleft, _botright;
}; // class Sink

// ****************************************************************** Moveable
class Moveable
{
public:
  using Pos2D = glm::vec2;
  using Color = struct {
    GLfloat r,g,b;
  };
  using TFPtr = std::unique_ptr<TransitionFunction>;
public:
  // ****************************************************** Moveable::creation
  Moveable( const std::string name,
            const Pos2D& topleft, const Pos2D& botright,
            const Pos2D& tex_topleft, const Pos2D& tex_botright) :
    _name(name),
    _pos( {0.0, 0.0} ),
    _topleft( topleft ), _botright( botright ),
    _tex_topleft( tex_topleft ), _tex_botright( tex_botright ),
    _target(nullptr), _transition(nullptr)
  {
  }
  std::string str_dump () const
  {
    std::stringstream dump;
    dump << _name << " at=( " << _pos[0] << ", " << _pos[1] << ")";

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
  // ******************************************************** Moveable::moving
  void self_move()
  {
    // if not moving, return
    if (_target == nullptr) return;

    double time_delta = glfwGetTime() - _time_start;
    // End of transition reached ?
    if (time_delta > _transition->delta_t()) {
      stop_self_move();
    }
    else {
      // move according to current value of Transition
      auto deltapos = _target->pos() - _start_pos;
      deltapos = deltapos * (float) _transition->val( time_delta );
      deltapos += _start_pos;
      pos( deltapos );
    }
  }
  void set_transition( Sink* target, double time_length )
  {
    std::cout << "__TRANSITION of " << _name << " " << target->str_dump() << std::endl;
    _target = target;
    _start_pos = pos();
    _time_start = glfwGetTime();
    _transition.reset( new TransitionFunction( 0.f, 1.f, time_length,
                                               1 /* order */ ) );
  }
  void stop_self_move()
  {
    std::cout << "__TRANSITION stop for " << _name << std::endl;
    _target = nullptr;
    _transition.reset( nullptr );
  }
  // ************************************************** Moveable::add_vertices
  void add_texture( TexVertexV& vec, float depth_z )
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
  void add_frame( LineVertexV& vec, float depth_z, Color col={1.f,0.f,0.f} )
  {
    // frame around
    vec.push_back( LineVertex{ _pos[0] + _topleft[0], _pos[1] + _botright[1], depth_z, col.r, col.g, col.b });
    vec.push_back( LineVertex{ _pos[0] + _botright[0], _pos[1] + _botright[1], depth_z, col.r, col.g, col.b  });

    vec.push_back( LineVertex{ _pos[0] + _botright[0], _pos[1] + _botright[1], depth_z, col.r, col.g, col.b  });
    vec.push_back( LineVertex{ _pos[0] + _botright[0], _pos[1] + _topleft[1], depth_z, col.r, col.g, col.b  });

    vec.push_back( LineVertex{ _pos[0] + _botright[0], _pos[1] + _topleft[1], depth_z, col.r, col.g, col.b  });
    vec.push_back( LineVertex{ _pos[0] + _topleft[0], _pos[1] + _topleft[1], depth_z, col.r, col.g, col.b  });

    vec.push_back( LineVertex{ _pos[0] + _topleft[0], _pos[1] + _topleft[1], depth_z, col.r, col.g, col.b  });
    vec.push_back( LineVertex{ _pos[0] + _topleft[0], _pos[1] + _botright[1], depth_z, col.r, col.g, col.b  });
  }
  // ***************************************************** Moveable::attributs
  Pos2D pos() const { return _pos; }
  void pos( const Pos2D& pos ) { _pos = pos; }
private:
  std::string _name;
  Pos2D _pos;
  Pos2D _topleft, _botright;
  
  Pos2D _tex_topleft, _tex_botright;

  // Target
  Sink* _target;                    // nullptr or Sink
  TFPtr _transition;                // nullptr or TF used to move it
  double _time_start;               // glfwTime when Transition started
  Pos2D _start_pos;
}; // class Moveable
// ***************************************************************************

  
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
private:
  // max number of square to draw
  static const unsigned int _max_size = 100;
  // Depth of Moveable on screen
  constexpr static const float _posz = 0.5f;
public:
  using MoveablePtr = Moveable*;
  using ListMoveable = std::list<MoveablePtr>;
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
                  GLMoveableManager::_max_size * sizeof(LineVertex),
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
      mov->self_move();
      mov->add_texture( _moveable_vtx, GLMoveableManager::_posz );
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
      sink.add_frame_cross( _line_vtx, GLMoveableManager::_posz, {1.f,1.f,0.f} );
    }
    // if needed, a frame around current_moveable
    if (_current_moveable != nullptr ) {
      _current_moveable->add_frame( _line_vtx, GLMoveableManager::_posz-0.1f );
    }
    // specify the buffer we are about to update
    glBindBuffer( GL_ARRAY_BUFFER, _line_vbo );
    // ask for reallocation, glBufferData with NULL and same parameters
    // see https://www.khronos.org/opengl/wiki/Buffer_Object_Streaming)
    glBufferData( GL_ARRAY_BUFFER,
                  GLMoveableManager::_max_size * sizeof(LineVertex),
                  NULL, 
                  GL_DYNAMIC_DRAW );
    // will call glBufferSubData on the entire buffer
    glBufferSubData( GL_ARRAY_BUFFER,
                     0,                                   //start of sub
                     _line_vtx.size() * sizeof(LineVertex), //length sub
                     _line_vtx.data() );                // data
  }
  // ********************************************* GLMoveableManager::Moveable
  void add_moveable( MoveablePtr moveable )
  {
    _moveables.push_back( moveable );
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
  void grab( const Pos2D& pos )
  {
    // Look in ListMoveable for a suitable Moveable to move
    // by default, _current_sink will be the Sink of the Moveable
    for( auto& mov: _moveables) {
      if (mov->is_inside( pos )) {
        std::cout << "++ inside " << mov->str_dump() << std::endl;
        _current_moveable = mov;
        _offset = pos - _current_moveable->pos();
        // stop self_move
        _current_moveable->stop_self_move();
        std::cout << "  offset=(" << _offset[0] << ", " << _offset[1] << ")" << std::endl;
        return;
      }
    }
  }
  void release_to_sink( const Pos2D& pos )
  {
    if (_current_moveable) {
      // Set a target for self move
      _current_moveable->set_transition( _current_sink, 2.0 /*delta_t*/ );
      // If has _current_moveable, set in potential Sink
      _current_moveable = nullptr;
      // TODO: could only update texture pos, and pop out last of _line_vtx and
      // TODO: add a new frame..
      //update_vbo();
    }
  }
  void release_in_place( const Pos2D& pos )
  {
    // release current (in place)
    _current_moveable = nullptr;
  }
  void move( const Pos2D& pos )
  {
    if (_current_moveable) {
      _current_moveable->pos( pos-_offset );
      // TODO: could only update texture pos, and pop out last of _line_vtx and
      // TODO: add a new frame..
      //update_vbo();
      // check for new _current_sink
    }
  }
  bool is_grabbing()
  {
    return (_current_moveable != nullptr );
  }
private:
  // ******************************************** GLMoveableManager::attributs
  ListMoveable _moveables;
  ListSink    _sinks;
public:
  Moveable* _current_moveable;
  Sink*    _current_sink;
private:
  Pos2D _offset;  // offset between mouse Pos and Moveable Pos
  
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
  LineVertexV _line_vtx;

}; // class GLMoveableManager

};  // matrix2020


#endif // GL_MOVEABLEMANAGER_HPP
