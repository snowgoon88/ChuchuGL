/* -*- coding: utf-8 -*- */

#ifndef GL_ARROW_HPP
#define GL_ARROW_HPP

/** 
 * Viewer d'une arrow,
 * change de taille en fonction de l'index d'animation
 * TODO traînée de même couleur, fading...
 */

#include <gl_utils.hpp>

#include <SOIL/SOIL.h>               // Load images

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

// ******************************************************************** GLOBAL
#define ANIM_LENGTH 30
#define IDX_CURSOR 0
#define IDX_ARROW  1
#define IDX_CROSS  2
#define NB_SPRITE_ROW (IDX_CROSS+1)
#define NB_SPRITE_COL _col_size

// ***************************************************************************
// ******************************************************************* GLArrow
// ***************************************************************************
class GLArrow
{
public:
  // ******************************************************* GLArrow::creation
  GLArrow()
  {
    // VBO pour un carré : 2 x triangle de 3 vertex
    // Le centre du carré est en 0.0 
    GLfloat carre_vtx[] = {
      -1.0, -1.0,      1.0, -1.0,     1.0, 1.0,
      1.0, 1.0,       -1.0, 1.0,      -1.0, -1.0 
    };
    _vbo_carre_size = 2 * 3; // 2 triangles de 3 pts;
    // Un VBO 
    glGenBuffers(1, &_vbo_carre_vtx);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_carre_vtx);
    // Pousse les points dans le VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(carre_vtx), 
		 carre_vtx, GL_STATIC_DRAW);
    // Delier les VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Les coordonnées des différents sprites dans la texture
    // 3 x 4 sprites (de 64x64)
    unsigned int nb_row = NB_SPRITE_ROW;
    unsigned int nb_col = NB_SPRITE_COL;
    GLfloat arrow_texcoords[ nb_row*nb_col * 2*6 ];
    for( unsigned int row = 0; row < nb_row; ++row) {
      for( unsigned int col = 0; col < nb_col; ++col) {
	// Premier triangle
	arrow_texcoords[(col+row*nb_col)*12+0] = col * 1.0f/ (float) nb_col;
	arrow_texcoords[(col+row*nb_col)*12+1] = (nb_row-1-row) * 1.0f/(float) nb_row;
	arrow_texcoords[(col+row*nb_col)*12+2] = (col+1) * 1.0f/ (float) nb_col;
	arrow_texcoords[(col+row*nb_col)*12+3] = (nb_row-1-row) * 1.0f/(float) nb_row;
	arrow_texcoords[(col+row*nb_col)*12+4] = (col+1) * 1.0f/ (float) nb_col;
	arrow_texcoords[(col+row*nb_col)*12+5] = (nb_row-1-row+1) * 1.0f/(float) nb_row;
	// Deuxième triangle
	arrow_texcoords[(col+row*nb_col)*12+6] = (col+1) * 1.0f/ (float) nb_col;
	arrow_texcoords[(col+row*nb_col)*12+7] = (nb_row-1-row+1) * 1.0f/(float) nb_row;
	arrow_texcoords[(col+row*nb_col)*12+8] = col * 1.0f/ (float) nb_col;
	arrow_texcoords[(col+row*nb_col)*12+9] = (nb_row-1-row+1) * 1.0f/(float) nb_row;
	arrow_texcoords[(col+row*nb_col)*12+10] = col * 1.0f/ (float) nb_col;
	arrow_texcoords[(col+row*nb_col)*12+11] = (nb_row-1-row) * 1.0f/(float) nb_row;
      }
    };
    glGenBuffers(1, &_vbo_arrow_texcoords);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_arrow_texcoords);
    glBufferData(GL_ARRAY_BUFFER, sizeof(arrow_texcoords),
		 arrow_texcoords, GL_STATIC_DRAW);


    // Charger la texture
    glActiveTexture(GL_TEXTURE0);
    _texture_id = SOIL_load_OGL_texture
      (
       "../Images/tex_arrow.png",           // pathfile
       SOIL_LOAD_AUTO,                           // 
       SOIL_CREATE_NEW_ID,                       //
       SOIL_FLAG_INVERT_Y                        // Corrige les Y upside/down
       );
    if(_texture_id == 0)
      std::cerr << "SOIL loading error: '" << SOIL_last_result() << "' (" << "../Images/tex_arrow.png" << ")" << std::endl;

    // Charger les Shaders CARRE + TEXTURE
    GLint link_ok = GL_FALSE;
    GLuint vs, fs;
    /** Vertex Shader */
    if ((vs = GLUtils::create_shader("src/shaders/chuchu.v.glsl", GL_VERTEX_SHADER))   == 0)
      exit( EXIT_FAILURE );
    /** Fragment Shader */
    if ((fs = GLUtils::create_shader("src/shaders/chuchu.f.glsl", GL_FRAGMENT_SHADER)) == 0)
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
      exit( EXIT_FAILURE );
    }

    // Lier les variables des Shaders
    // coord 2D
    const char* attribute_name = "coord2d";
    _attribute_coord2d= glGetAttribLocation(_program, attribute_name);
    if (_attribute_coord2d == -1) {
      std::cerr <<  "Pb pour lier l'attribut " << attribute_name << std::endl;
      exit( EXIT_FAILURE );
    }
    // la variable de textute
    attribute_name = "texcoord";
    _attribute_texcoord = glGetAttribLocation(_program, attribute_name);
    if (_attribute_texcoord == -1) {
      std::cerr <<  "Pb pour lier l'attribut " << attribute_name << std::endl;
      exit( EXIT_FAILURE );
    }
    // 'm'(+vp) matrix => on peut pas pop/push !!
    const char* uniform_name = "mvp";
    _uniform_mvp = glGetUniformLocation(_program, uniform_name);
    if (_uniform_mvp == -1) {
      std::cerr <<  "Pb pour lier l'uniform " << uniform_name << std::endl;
      exit( EXIT_FAILURE );
    }

  };
  // ***************************************************** GLArrow::destruction
  virtual ~GLArrow()
  {
    // Détruit le programme GLSL
    glDeleteProgram(_program);
    // Et les vbo
    glDeleteBuffers( 1, &_vbo_carre_vtx);
    glDeleteBuffers( 1, &_vbo_arrow_texcoords);
    glDeleteTextures( 1, &_texture_id);
  };
  // ************************************************** GLArrow::render_cursor
  /** 'anim_idx' indique l'index de l'animation 
   * on s'en sert pour scaler la flèche.
   */
  void render_cursor( glm::mat4& proj, const Vec2& pos, 
		      unsigned int color_idx, unsigned int anim_idx )
  {
    // Calculer la Translation
    // Model : translation
    glm::mat4 trans = glm::translate(glm::mat4(1.0f),
				     glm::vec3( pos.x + 0.5,
						pos.y - 0.5,
						0.2));
    // Suivant anim, on scale entre 0.4 et 0.6
    
    double anim_scale = 0.4;
    if( anim_idx < ANIM_LENGTH/2 ) anim_scale += anim_idx * 0.01;
    else anim_scale += (ANIM_LENGTH - anim_idx) * 0.01;
    glm::mat4 scal = glm::scale( glm::mat4(1.0f),
				 glm::vec3( anim_scale, anim_scale, 1.0));
    // Et finalement
    glm::mat4 mvp = proj * trans * scal;
    
    // Draw
    glUseProgram(_program);
    // Carre avec Chuchu
    glBindBuffer( GL_ARRAY_BUFFER, _vbo_carre_vtx );
    glEnableVertexAttribArray(_attribute_coord2d);
    /* Describe our vertices array to OpenGL (it can't guess its format automatically) */
    glVertexAttribPointer(
      _attribute_coord2d, // attribute
      2,                 // number of elements per vertex, here (x,y)
      GL_FLOAT,          // the type of each element
      GL_FALSE,          // take our values as-is
      0,                 // stride
      0                  // offset of first element
			  );
    // Texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _texture_id);
    _uniform_mytexture = glGetUniformLocation(_program, "mytexture");
    glUniform1i(_uniform_mytexture, /*GL_TEXTURE*/0);
    // index de la texture dépend du type (cursor/arrow/cross)
    // et de la couleur du joueur
    int idtexture = IDX_CURSOR*NB_SPRITE_COL + color_idx; // cursor + color
    glEnableVertexAttribArray(_attribute_texcoord);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_arrow_texcoords);
    glVertexAttribPointer(
       _attribute_texcoord, // attribute
       2,                  // number of elements per vertex, here (x,y)
       GL_FLOAT,           // the type of each element
       GL_FALSE,           // take our values as-is
       0,                  // no extra data between each position
       (GLvoid*) (idtexture * 12 * sizeof(GLfloat)) // offset of first element (as Ptr!!)
			  );

    // Transform
    glUniformMatrix4fv(_uniform_mvp, 1, GL_FALSE,
     		       glm::value_ptr(mvp));
 
    /* Push each element in buffer_vertices to the vertex shader
     * according to index */
    glDrawArrays(GL_TRIANGLES, 0, _vbo_carre_size );

    glDisableVertexAttribArray(_attribute_coord2d);
  };
  // ************************************************** GLArrow::render_arrow
  void render_arrow( glm::mat4& proj, const Vec2& pos, const Direction& dir, 
		     unsigned int color_idx )
  {
    // Calculer la Translation
    // Model : translation
    glm::mat4 trans = glm::translate(glm::mat4(1.0f),
				     glm::vec3( pos.x + 0.5,
						pos.y + 0.5,
						0.2));
    glm::mat4 rot = glm::rotate( glm::mat4(1.0f),
				 (float) -M_PI/2 * dir.index,
				 glm::vec3( 0.f, 0.f, 1.0f));
    glm::mat4 scal = glm::scale( glm::mat4(1.0f),
				 glm::vec3( 0.5f, 0.5f, 1.0f));
    // Et finalement
    glm::mat4 mvp = proj * trans * rot * scal;
    
    // Draw
    glUseProgram(_program);
    // Carre avec Chuchu
    glBindBuffer( GL_ARRAY_BUFFER, _vbo_carre_vtx );
    glEnableVertexAttribArray(_attribute_coord2d);
    /* Describe our vertices array to OpenGL (it can't guess its format automatically) */
    glVertexAttribPointer(
      _attribute_coord2d, // attribute
      2,                 // number of elements per vertex, here (x,y)
      GL_FLOAT,          // the type of each element
      GL_FALSE,          // take our values as-is
      0,                 // stride
      0                  // offset of first element
			  );
    // Texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _texture_id);
    _uniform_mytexture = glGetUniformLocation(_program, "mytexture");
    glUniform1i(_uniform_mytexture, /*GL_TEXTURE*/0);
    // index de la texture dépend du type (cursor/arrow/cross)
    // et de la couleur du joueur
    int idtexture = IDX_ARROW*NB_SPRITE_COL + color_idx; // cursor + color
    glEnableVertexAttribArray(_attribute_texcoord);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_arrow_texcoords);
    glVertexAttribPointer(
       _attribute_texcoord, // attribute
       2,                  // number of elements per vertex, here (x,y)
       GL_FLOAT,           // the type of each element
       GL_FALSE,           // take our values as-is
       0,                  // no extra data between each position
       (GLvoid*) (idtexture * 12 * sizeof(GLfloat)) // offset of first element (as Ptr!!)
			  );

    // Transform
    glUniformMatrix4fv(_uniform_mvp, 1, GL_FALSE,
     		       glm::value_ptr(mvp));
 
    /* Push each element in buffer_vertices to the vertex shader
     * according to index */
    glDrawArrays(GL_TRIANGLES, 0, _vbo_carre_size );

    glDisableVertexAttribArray(_attribute_coord2d);
  };

  // ************************************************** GLArrow::render_cross
  void render_cross( glm::mat4& proj, const Vec2& pos )  
  {
    // Calculer la Translation
    // Model : translation
    glm::mat4 trans = glm::translate(glm::mat4(1.0f),
				     glm::vec3( pos.x + 0.5,
						pos.y + 0.5,
						0.0));
    glm::mat4 scal = glm::scale( glm::mat4(1.0f),
				 glm::vec3( 0.5, 0.5, 1.0));
    // Et finalement
    glm::mat4 mvp = proj * trans * scal;
    
    // Draw
    glUseProgram(_program);
    // Carre avec Chuchu
    glBindBuffer( GL_ARRAY_BUFFER, _vbo_carre_vtx );
    glEnableVertexAttribArray(_attribute_coord2d);
    /* Describe our vertices array to OpenGL (it can't guess its format automatically) */
    glVertexAttribPointer(
      _attribute_coord2d, // attribute
      2,                 // number of elements per vertex, here (x,y)
      GL_FLOAT,          // the type of each element
      GL_FALSE,          // take our values as-is
      0,                 // stride
      0                  // offset of first element
			  );
    // Texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _texture_id);
    _uniform_mytexture = glGetUniformLocation(_program, "mytexture");
    glUniform1i(_uniform_mytexture, /*GL_TEXTURE*/0);
    // index de la texture dépend du type (cursor/arrow/cross)
    // et de la couleur du joueur
    int idtexture = IDX_CROSS*NB_SPRITE_COL; // cross
    glEnableVertexAttribArray(_attribute_texcoord);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_arrow_texcoords);
    glVertexAttribPointer(
       _attribute_texcoord, // attribute
       2,                  // number of elements per vertex, here (x,y)
       GL_FLOAT,           // the type of each element
       GL_FALSE,           // take our values as-is
       0,                  // no extra data between each position
       (GLvoid*) (idtexture * 12 * sizeof(GLfloat)) // offset of first element (as Ptr!!)
			  );

    // Transform
    glUniformMatrix4fv(_uniform_mvp, 1, GL_FALSE,
     		       glm::value_ptr(mvp));
 
    /* Push each element in buffer_vertices to the vertex shader
     * according to index */
    glDrawArrays(GL_TRIANGLES, 0, _vbo_carre_size );

    glDisableVertexAttribArray(_attribute_coord2d);
  };

  // ******************************************************** GLArrow::attributs
private:
  /** Program GLSL */
  GLuint _program;
  /** Variables globale du Programme GLSL */
  GLint _attribute_coord2d, _attribute_texcoord;
  GLuint _texture_id;
  /** uniform var */
  GLint _uniform_mvp, _uniform_mytexture;
  /** Vertex Buffer Objects */
  GLuint _vbo_carre_vtx, _vbo_arrow_texcoords;
  unsigned int _vbo_carre_size;
};

#endif // GL_ARROW_HPP
