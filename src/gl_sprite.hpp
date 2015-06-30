/* -*- coding: utf-8 -*- */

#ifndef GL_SPRITES_HPP
#define GL_SPRITES_HPP

/** 
 * Viewer de Sprite
 */

#include <global.hpp>
#include <gl_utils.hpp>

#include <SOIL/SOIL.h>               // Load images

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

// ******************************************************************** GLOBAL
// #define ANIM_LENGTH 30
// #define IDX_CURSOR 0
// #define IDX_ARROW  1
// #define IDX_CROSS  2
// #define NB_SPRITE_ROW (IDX_CROSS+1)
// #define NB_SPRITE_COL _col_size

// ***************************************************************************
// ******************************************************************* GLArrow
// ***************************************************************************
class GLSprite
{
public:
  // ****************************************************** GLSprite::creation
  /** 
   * 'filename' contient une texture avec nb_row x nb_col versions
   * du sprite, numéroté de gauche à droit et de bas en haut.
   * Ils ont tous la même taille, rectangle donnée par son point 
   * en bas à gauche (bl_pt) et en haut à droite (ur_pt)
   */
  GLSprite( GLTexture& gl_texture, std::string filename,
	    unsigned int nb_row, unsigned int nb_col,
	    const Vec2& bl_pt = {-1,-1},
	    const Vec2& ur_pt = {1, 1} ) :
    _gl_texture(gl_texture)
  {
    // VBO pour un carré : 2 x triangle de 3 vertex
    // Le centre du carré est en 0.0 
    GLfloat carre_vtx[] = {
      (float)bl_pt.x, (float)bl_pt.y,
      (float)ur_pt.x, (float)bl_pt.y,
      (float)ur_pt.x, (float)ur_pt.y,

      (float)ur_pt.x, (float)ur_pt.y,
      (float)bl_pt.x, (float)ur_pt.y,
      (float)bl_pt.x, (float)bl_pt.y
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
    // nb_row x nb_col sprites
    GLfloat sprite_texcoords[ nb_row*nb_col * 2*6 ];
    for( unsigned int row = 0; row < nb_row; ++row) {
      for( unsigned int col = 0; col < nb_col; ++col) {
	// Premier triangle
	sprite_texcoords[(col+row*nb_col)*12+0] = col * 1.0f/ (float) nb_col;
	sprite_texcoords[(col+row*nb_col)*12+1] = (nb_row-1-row) * 1.0f/(float) nb_row;
	sprite_texcoords[(col+row*nb_col)*12+2] = (col+1) * 1.0f/ (float) nb_col;
	sprite_texcoords[(col+row*nb_col)*12+3] = (nb_row-1-row) * 1.0f/(float) nb_row;
	sprite_texcoords[(col+row*nb_col)*12+4] = (col+1) * 1.0f/ (float) nb_col;
	sprite_texcoords[(col+row*nb_col)*12+5] = (nb_row-1-row+1) * 1.0f/(float) nb_row;
	// Deuxième triangle
	sprite_texcoords[(col+row*nb_col)*12+6] = (col+1) * 1.0f/ (float) nb_col;
	sprite_texcoords[(col+row*nb_col)*12+7] = (nb_row-1-row+1) * 1.0f/(float) nb_row;
	sprite_texcoords[(col+row*nb_col)*12+8] = col * 1.0f/ (float) nb_col;
	sprite_texcoords[(col+row*nb_col)*12+9] = (nb_row-1-row+1) * 1.0f/(float) nb_row;
	sprite_texcoords[(col+row*nb_col)*12+10] = col * 1.0f/ (float) nb_col;
	sprite_texcoords[(col+row*nb_col)*12+11] = (nb_row-1-row) * 1.0f/(float) nb_row;
      }
    };
    glGenBuffers(1, &_vbo_sprite_texcoords);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_sprite_texcoords);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sprite_texcoords),
		 sprite_texcoords, GL_STATIC_DRAW);


    // Charger la texture
    glActiveTexture(GL_TEXTURE0);
    _texture_id = SOIL_load_OGL_texture
      (
       filename.c_str(),                         // pathfile
       SOIL_LOAD_AUTO,                           // 
       SOIL_CREATE_NEW_ID,                       //
       SOIL_FLAG_INVERT_Y                        // Corrige les Y upside/down
       );
    if(_texture_id == 0)
      std::cerr << "GLSprite: SOIL loading error: '" << SOIL_last_result() << "' (" << filename << ")" << std::endl;


    glEnableVertexAttribArray(_gl_texture.attribute_texcoord() );
    glEnableVertexAttribArray( _gl_texture.attribute_coord2d() );
  };
  // **************************************************** GLSprite::destruction
  ~GLSprite()
  {
    // Et les vbo
    glDeleteBuffers( 1, &_vbo_carre_vtx);
    glDeleteBuffers( 1, &_vbo_sprite_texcoords);
    glDeleteTextures( 1, &_texture_id);
  };
  // **************************************************** GLSprite::pre_render
  void pre_render()
  {
    // Select Program
    glUseProgram( _gl_texture.program() );

    // TODO en faire une fonction de GLTexture avec _vbo_carre_vtx et 
    // je sais pas quoi en argument ??
    // Carre avec Chuchu
    glBindBuffer( GL_ARRAY_BUFFER, _vbo_carre_vtx );
    //glEnableVertexAttribArray( _gl_texture.attribute_coord2d() );
    /* Describe our vertices array to OpenGL (it can't guess its format automatically) */
    glVertexAttribPointer(
      _gl_texture.attribute_coord2d(), // attribute
      2,                 // number of elements per vertex, here (x,y)
      GL_FLOAT,          // the type of each element
      GL_FALSE,          // take our values as-is
      0,                 // stride
      0                  // offset of first element
			  );
    // Texture
    // TODO vérifier qu'on ne pourrait pas le binder _uniform_mytexture
    //      dans GLTexture.
    // TODO est-ce qu'il faut le faire forcément après glBindTexture ?
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _texture_id);
    _uniform_mytexture = glGetUniformLocation(_gl_texture.program(),
					      "mytexture");
    glUniform1i(_uniform_mytexture, /*GL_TEXTURE*/0);


    // vbo sprite
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_sprite_texcoords);
    
  };
  // ******************************************************** GLSprite::render
  /** 
   * @param 'proj' : matrice de projection
   * @param 'pos' : position du centre du Sprite
   * @param 'scale' : change taille du Sprite
   * @param 'idx_sprite' : index du Sprite sur la texture.
   */
  void render( glm::mat4& proj, const Vec2& pos, double scale,
	       unsigned int idx_sprite )
  {
    // Calculer la Translation
    // Model : translation
    glm::mat4 trans = glm::translate(glm::mat4(1.0f),
				     glm::vec3( pos.x,
						pos.y,
						0.0));
    // Model : scale
    glm::mat4 scal = glm::scale( glm::mat4(1.0f),
				 glm::vec3( scale, scale, 1.0));
    // Et finalement
    glm::mat4 mvp = proj * trans * scal;
    
    // TODO => pareil, vérifier ordre
    //glBindBuffer(GL_ARRAY_BUFFER, _vbo_sprite_texcoords);
    //glEnableVertexAttribArray(_gl_texture.attribute_texcoord() );

    glVertexAttribPointer(
       _gl_texture.attribute_texcoord(), // attribute
       2,                  // number of elements per vertex, here (x,y)
       GL_FLOAT,           // the type of each element
       GL_FALSE,           // take our values as-is
       0,                  // no extra data between each position
       (GLvoid*) (idx_sprite * 12 * sizeof(GLfloat)) // offset of first element (as Ptr!!)
			  );

    // Transform
    glUniformMatrix4fv(_gl_texture.uniform_mvp(), 1, GL_FALSE,
     		       glm::value_ptr(mvp));
 
    /* Push each element in buffer_vertices to the vertex shader
     * according to index */
    glDrawArrays(GL_TRIANGLES, 0, _vbo_carre_size );
  };
  // *************************************************** GLSprite::post_render
  void post_render()
  {
    //glDisableVertexAttribArray(_gl_texture.attribute_coord2d());
  };
  // ******************************************************** GLArrow::attributs
private:
  /** Program GLSL */
  GLTexture &_gl_texture;
  /** id de notre texture */
  GLuint _texture_id;
  /** uniform var */
  GLint _uniform_mytexture;
  /** Vertex Buffer Objects */
  GLuint _vbo_carre_vtx, _vbo_sprite_texcoords;
  unsigned int _vbo_carre_size;
};

#endif // GL_SPRITES_HPP
