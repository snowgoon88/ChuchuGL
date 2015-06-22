/* -*- coding: utf-8 -*- */

#ifndef GL_TEXT_HPP
#define GL_TEXT_HPP

/** 
 * Utilise FTGL pour afficher du texte en OpenGL.
 * Pas hyper compatible avec les SHADERS.
 *
 * Ne pas oublier de :
 * // do_ortho() - Needed by _gl_text
 * glMatrixMode(GL_PROJECTION);
 * glLoadIdentity();
 *
 * glOrtho(-1.0f, 10.0f, -1.0f, 10.0f, 1.f, -1.f);
 *    
 * // Needed by _gl_text
 * glMatrixMode(GL_MODELVIEW);
 * glLoadIdentity();
 * 
 * // Remove any programm so that glText can "work"
 * glUseProgram(0);
 */

#include <GL/gl.h>           // OpenGL
#include <FTGL/ftgl.h>       // Fontes dans OpenGL

// ******************************************************************** GLOBAL
#define FONT_PATH std::string("/home/dutech/Fonts/Consolas.ttf")
#define FONT_SIZE 12
// Scale par défaut des fontes : écran de 800 de large, axe de -1 à 1.
#define FONT_SCALE ((1.0 - -1.0) / 800.0)
// ***************************************************************************
// ******************************************************************** GLText
// ***************************************************************************
class GLText
{
public:
  // ******************************************************** GLText::creation
  GLText( std::string fontpath = FONT_PATH,
	  unsigned int fontsize = FONT_SIZE) :
    _font(nullptr), _scale_x(FONT_SCALE), _scale_y(FONT_SCALE)
  {
    std::cout << "__Gl_TEXT" << std::endl;
    _font = new FTGLTextureFont( fontpath.c_str() );
    if( _font->Error() != 0 ) {
      std::cerr << "GLText: Unable to open file " << fontpath << std::endl;
    }
    else {
      std::cout << "  size" << std::endl;
      _font->FaceSize( fontsize );
      if( _font->Error() != 0 ) {
	std::cerr << "GLText: Unable to set font face size " << fontsize << std::endl;
      }
    }
    std::cout << "  end creation" << std::endl;
  }
  ~GLText()
  {
    if( _font) delete _font;
  }
  // ************************************************************ set_fontsize
  void set_fontsize( unsigned int fontsize )
  {
    _font->FaceSize( fontsize );
    if( _font->Error() != 0 ) {
      std::cerr << "GLText: Unable to set font face size " << fontsize << std::endl;
    }
  }
  // ********************************************************** GLText::render
  /** A appeler, en connaissant la taille de screen, avant render.
   * Habituellement, scale = size_opengl / size_screen.
   */
  void set_scale( float scale_x =  FONT_SCALE,
		  float scale_y = FONT_SCALE)
  {
    _scale_x = scale_x;
    _scale_y = scale_y;
  };
  void render( const std::string& text, float x=0, float y=0, float z=0 )
  {
    glPushMatrix();
    glTranslatef( x, y, z );
    glScalef( _scale_x, _scale_y, 1.0 );
    _font->Render( text.c_str() );
    glPopMatrix();
  };
  void set_glstate()
  {
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }
  void unset_glstate()
  {
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
  };
  // ******************************************************* GLText::attributs
private:
  /** Des Fontes pour écrire */
  FTFont* _font;
  /** Scale pour les lettres */
  float _scale_x, _scale_y;
};

#endif // GL_TEXT_HPP
