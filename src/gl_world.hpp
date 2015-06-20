/* -*- coding: utf-8 -*- */

#ifndef GL_WORLD_HPP
#define GL_WORLD_HPP

/** 
 * Viewer OpenGL : le monde comme une grille.
 */
#include "gl_utils.hpp"                   // GLUtils

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "world.hpp"
#include "gl_chuchu.hpp"
#include "gl_cell.hpp"
#include "gl_arrow.hpp"

// ******************************************************************** GLOBAL
/** épaisseur d'un mur et nb de point pour dessiner un mur*/
#define WALL_WIDTH 0.1f
#define WALL_SIZE  (4*6)

// ***************************************************************************
// ******************************************************************* GLWorld
// ***************************************************************************
class GLWorld
{
public:
  // **************************************************************** creation
  GLWorld( World& world ) : _model(world)
  {
    // VBO pour les lignes : 4 float par ligne
    GLfloat line_vtx[(_model.nb_row()+1+_model.nb_col()+1)*4];
    for( unsigned int row = 0; row < _model.nb_row()+1; ++row) {
      line_vtx[row*4] = (float) 0.f;
      line_vtx[row*4+1] = (float) row;
      line_vtx[row*4+2] = (float) _model.nb_col();
      line_vtx[row*4+3] = (float) row;
    }
    for( unsigned int col = 0; col < _model.nb_col()+1; ++col) {
      line_vtx[(_model.nb_row()+1)*4+col*4] = (float) col;
      line_vtx[(_model.nb_row()+1)*4+col*4+1] = (float) 0.f;
      line_vtx[(_model.nb_row()+1)*4+col*4+2] = (float) col;
      line_vtx[(_model.nb_row()+1)*4+col*4+3] = (float) _model.nb_row();
      }
    _vbo_lines_size = 2*(_model.nb_row()+1 + _model.nb_col()+1);
    // Un VBO
    glGenBuffers(1, &_vbo_lines);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_lines);
    // Pousse les points dans le VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(line_vtx), 
		 line_vtx, GL_STATIC_DRAW);

        // VBO avec les points des murs
    GLfloat wall_vtx[(_model.walls().size())*WALL_SIZE];
    _vbo_walls_size = 0;
    for( auto& wall : _model.walls() ) {
      if( wall.idd == 0 ) {
    	_vbo_walls_size += add_wall_hor( (GLfloat) wall.x,
    					 (GLfloat) wall.y + 1.0f,
    					 &(wall_vtx[_vbo_walls_size]) );
      }
      else if( wall.idd == 1 ) {
    	_vbo_walls_size += add_wall_vert( (GLfloat) wall.x + 1.0f,
					  (GLfloat) wall.y,
					  &(wall_vtx[_vbo_walls_size]) );
      }
      else if( wall.idd == 2 ) {
    	_vbo_walls_size += add_wall_hor( (GLfloat) wall.x,
    					 (GLfloat) wall.y,
    					 &(wall_vtx[_vbo_walls_size]) );
      }
      else if ( wall.idd == 3 ) {
    	_vbo_walls_size += add_wall_vert( (GLfloat) wall.x,
    					  (GLfloat) wall.y,
    					  &(wall_vtx[_vbo_walls_size]) );
      } 
    }
    _vbo_walls_size = _vbo_walls_size/2;
    // Un VBO
    glGenBuffers(1, &_vbo_walls);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_walls);
    // Pousse les points dans le VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(wall_vtx), 
		 wall_vtx, GL_STATIC_DRAW);

    // Delier les VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    
    // Charger les Shaders LINE + TRIANGLES
    GLint link_ok = GL_FALSE;
    GLuint vs, fs;
    /** Vertex Shader */
    if ((vs = GLUtils::create_shader("src/shaders/line.v.glsl", GL_VERTEX_SHADER))   == 0)
      exit( EXIT_FAILURE );
    /** Fragment Shader */
    if ((fs = GLUtils::create_shader("src/shaders/line.f.glsl", GL_FRAGMENT_SHADER)) == 0)
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
  // ************************************************************* destruction
  virtual ~GLWorld()
  {
    // Détruit le programme GLSL
    glDeleteProgram(_program);
    // Et les vbo
    glDeleteBuffers(1, &_vbo_lines);
    glDeleteBuffers(1, &_vbo_walls);
  };
  // ****************************************************************** render
  void render( unsigned int anim_idx = 0 )
  {
    // Projection (to 2D screen)
    glm::mat4 projection = glm::ortho( -1.0f, 10.0f,
				       -1.0f, 10.0f,
				       -1.0f, 1.0f );

    glUseProgram( _program );
    glUniformMatrix4fv(_uniform_mvp, 1, GL_FALSE,
     		       glm::value_ptr(projection));

    // LIGNES **************************************
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

    // MURS
    // Couleur des murs : RED
    glUniform3f( _uniform_l_color, 1.f, 0.f, 0.f );
    // Dessiner les murs
    glBindBuffer( GL_ARRAY_BUFFER, _vbo_walls );
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
    glDrawArrays(GL_TRIANGLES, 0, _vbo_walls_size);

    glDisableVertexAttribArray(_attribute_coord2d);
    // Les Rockets
    // TODO changer orientation en fonction de la position dans la grille
    for( auto& rocket : _model.rocket()) {
      _rocket_viewer.render( projection, rocket->pos(), 0 );
    }
    // Tous les chuchu
    for( auto& chuchu: _model.chuchu()) {
      _chuchu_viewer.render( projection, chuchu->pos(), chuchu->dir() );
    }
    // Les curseur des joueurs
    _arrow_viewer.render( projection, Vec2({2, 3.3}), anim_idx );
    // Un Chuchu vers la droite en (2,2)
    //_chuchu_viewer.render( projection, Vec2({2,2}), _dir_right );
  };
  // *************************************************************** attributs
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
  GLuint _vbo_lines, _vbo_walls;
  unsigned int _vbo_lines_size, _vbo_walls_size;
  /** Sous-viewer */
  GLChuchu _chuchu_viewer;
  GLRocket _rocket_viewer;
  GLArrow  _arrow_viewer;
  // **************************************************************** add_wall
  /**
   * Mur vertical en 4 triangles
   * @return nb de point ajoutés
   */
  unsigned int add_wall_vert( GLfloat x, GLfloat y, GLfloat* ptr )
  {
    // extrémité basse
    *ptr++ = x;
    *ptr++ = y;
    *ptr++ = x + WALL_WIDTH/2.f;
    *ptr++ = y + WALL_WIDTH/2.f;
    *ptr++ = x - WALL_WIDTH/2.f;
    *ptr++ = y + WALL_WIDTH/2.f;
    // droite du mur
    *ptr++ = x + WALL_WIDTH/2.f;
    *ptr++ = y + WALL_WIDTH/2.f;
    *ptr++ = x + WALL_WIDTH/2.f;
    *ptr++ = y + 1.f - WALL_WIDTH/2.f;
    *ptr++ = x - WALL_WIDTH/2.f;
    *ptr++ = y + WALL_WIDTH/2.f;
    // gauche du mur
    *ptr++ = x - WALL_WIDTH/2.f;
    *ptr++ = y + WALL_WIDTH/2.f;
    *ptr++ = x + WALL_WIDTH/2.f;
    *ptr++ = y + 1.f - WALL_WIDTH/2.f;
    *ptr++ = x - WALL_WIDTH/2.f;
    *ptr++ = y + 1.f - WALL_WIDTH/2.f;
    // extrémité haute
    *ptr++ = x + WALL_WIDTH/2.f;
    *ptr++ = y + 1.f - WALL_WIDTH/2.f;
    *ptr++ = x - WALL_WIDTH/2.f;
    *ptr++ = y + 1.f - WALL_WIDTH/2.f;
    *ptr++ = x;
    *ptr++ = y + 1.f;

    return 4*6;
  };
  /**
   * Mur horizontal en 4 triangles
   * @return nb de point ajoutés
   */
  unsigned int add_wall_hor( GLfloat x, GLfloat y, GLfloat* ptr )
  {
    // extrémité gauche
    *ptr++ = x;
    *ptr++ = y;
    *ptr++ = x + WALL_WIDTH/2.f;
    *ptr++ = y - WALL_WIDTH/2.f;
    *ptr++ = x + WALL_WIDTH/2.f;
    *ptr++ = y + WALL_WIDTH/2.f;
    // bas du mur
    *ptr++ = x + WALL_WIDTH/2.f;
    *ptr++ = y - WALL_WIDTH/2.f;
    *ptr++ = x + 1.f - WALL_WIDTH/2.f;
    *ptr++ = y - WALL_WIDTH/2.f;
    *ptr++ = x + WALL_WIDTH/2.f;
    *ptr++ = y + WALL_WIDTH/2.f;
    // haut du mur
    *ptr++ = x + WALL_WIDTH/2.f;
    *ptr++ = y + WALL_WIDTH/2.f;
    *ptr++ = x + 1.f - WALL_WIDTH/2.f;
    *ptr++ = y - WALL_WIDTH/2.f;
    *ptr++ = x + 1.f - WALL_WIDTH/2.f;
    *ptr++ = y + WALL_WIDTH/2.f;
    // extrémité droite
    *ptr++ = x + 1.f - WALL_WIDTH/2.f;
    *ptr++ = y + WALL_WIDTH/2.f;
    *ptr++ = x + 1.f - WALL_WIDTH/2.f;
    *ptr++ = y - WALL_WIDTH/2.f;
    *ptr++ = x + 1.f;
    *ptr++ = y;
    
    return 4*6;
  };

};
#endif // GL_WORLD_HPP
