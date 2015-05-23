/* -*- coding: utf-8 -*- */

#ifndef GL_WORLD_HPP
#define GL_WORLD_HPP

/** 
 * Viewer OpenGL : le monde comme une grille.
 */
#include <gl_utils.hpp>                   // GLUtils

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <world.hpp>

// ***************************************************************************
// ******************************************************************* GLWorld
// ***************************************************************************
class GLWorld
{
public:
  // ****************************************************************** creation
  GLWorld( World& world ) : _model(world)
  {
    // VBO pour les lignes : 4 float par ligne
    GLfloat line_vtx[(_model.nb_row()+1+_model.nb_col()+1)*4];
    for( unsigned int row = 0; row < _model.nb_row()+1; ++row) {
      line_vtx[row*4] = (float) -0.5f;
      line_vtx[row*4+1] = (float) row - 0.5f;
      line_vtx[row*4+2] = (float) _model.nb_col() - 0.5f;
      line_vtx[row*4+3] = (float) row - 0.5f;
    }
    for( unsigned int col = 0; col < _model.nb_col()+1; ++col) {
      line_vtx[(_model.nb_row()+1)*4+col*4] = (float) col - 0.5f;
      line_vtx[(_model.nb_row()+1)*4+col*4+1] = (float) - 0.5f;
      line_vtx[(_model.nb_row()+1)*4+col*4+2] = (float) col - 0.5f;
      line_vtx[(_model.nb_row()+1)*4+col*4+3] = (float) _model.nb_row() - 0.5f;
      }
    _vbo_lines_size = 2*(_model.nb_row()+1 + _model.nb_col()+1);
    // Un VBO
    glGenBuffers(1, &_vbo_lines);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_lines);
    // Pousse les points dans le VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(line_vtx), 
		 line_vtx, GL_STATIC_DRAW);
    // Delier les VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // TODO VBO avec les points des murs

    // Charger les Shaders
    GLint link_ok = GL_FALSE;
    GLuint vs, fs;
    /** Vertex Shader */
    if ((vs = GLUtils::create_shader("../src/shaders/line.v.glsl", GL_VERTEX_SHADER))   == 0)
      exit( EXIT_FAILURE );
    /** Fragment Shader */
    if ((fs = GLUtils::create_shader("../src/shaders/line.f.glsl", GL_FRAGMENT_SHADER)) == 0)
      exit( EXIT_FAILURE );
    /** GLSL program */
    // link Vertex et Fragment Shaders
    // program est une variable GLOBALE
    _program = glCreateProgram();
    glAttachShader(_program, vs);
    glAttachShader(_program, fs);
    glLinkProgram(_program);
    glGetProgramiv(_program, GL_LINK_STATUS, &link_ok);
    if (!link_ok) {
      std::cerr << "__GLWorld: Pb avec glLinkProgam "<< std::endl;
      std::cerr << GLUtils::str_compiler_log(_program) << std::endl;
      exit( EXIT_FAILURE );;
    }

    // Lier les variables des shaders
    // l_color : couleur des lignes
    const char* uniform_name = "l_color";
    _uniform_l_color = glGetUniformLocation(_program, uniform_name);
    if (_uniform_l_color == -1) {
      std::cerr <<  "Pb pour lier l'uniform " << uniform_name << std::endl;
      exit( EXIT_FAILURE );
    }
    // la variable uniforme mvp
    uniform_name = "mvp";
    _uniform_mvp = glGetUniformLocation(_program, uniform_name);
    if (_uniform_mvp == -1) {
      std::cerr <<  "Pb pour lier l'uniform " << uniform_name << std::endl;
      exit( EXIT_FAILURE );
    }
    // coord2d : coordonnées du vertex
    const char* attribute_name = "coord2d";
    _attribute_coord2d= glGetAttribLocation(_program, attribute_name);
    if (_attribute_coord2d == -1) {
      std::cerr <<  "Pb pour lier l'attribut " << attribute_name << std::endl;
      exit( EXIT_FAILURE );
    }
    
  };
  // *************************************************************** destruction
  virtual ~GLWorld()
  {
    // Détruit le programme GLSL
    glDeleteProgram(_program);
    // Et les vbo
    glDeleteBuffers(1, &_vbo_lines);
  };
  // ******************************************************************** render
  void render () 
  {
    glUseProgram( _program );
    // Projection (to 2D screen)
    glm::mat4 projection = glm::ortho( -1.0f, 10.0f,
				       -1.0f, 10.0f,
				       -1.0f, 1.0f );
    glUniformMatrix4fv(_uniform_mvp, 1, GL_FALSE,
     		       glm::value_ptr(projection));
    
    // Couleur des lignes : BLACK
    glUniform3f( _uniform_l_color, 0.f, 0.f, 0.f );
    // Dessiner la grille
    glBindBuffer( GL_ARRAY_BUFFER, _vbo_lines );
    glEnableVertexAttribArray( _attribute_coord2d );
    /* Describe our vertices array to OpenGL (it can't guess its format automatically) */
    glVertexAttribPointer(
      _attribute_coord2d, // attribute
      2,                 // number of elements per vertex, here (x,y)
      GL_FLOAT,          // the type of each element
      GL_FALSE,          // take our values as-is
      0,                 // stride
      0                  // offset of first element
			  );
    /* Push each element in buffer_vertices to the vertex shader */
    glDrawArrays(GL_LINES, 0, _vbo_lines_size);
    glDisableVertexAttribArray(_attribute_coord2d);
  };
  // ***************************************************************** attributs
private:
  /** Model : World */
  World& _model;
  /** Program GLSL */
  GLuint _program;
  /** Variables globale du Programme GLSL */
  GLint _attribute_coord2d;
  /** Uniform var */
  GLint _uniform_l_color, _uniform_mvp;
  /** Vertex Buffer Object pour lines */
  GLuint _vbo_lines;
  int _vbo_lines_size;
};
#endif // GL_WORLD_HPP
