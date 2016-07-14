/* -*- coding: utf-8 -*- */

#ifndef GL_TEXT_SHADERS_HPP
#define GL_TEXT_SHADERS_HPP

/** 
 * Fonctions pour afficher des textes simples avec des shaders.
 *
 * https://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_Text_Rendering_01
 */

#include <stdexcept>              // std::runtime_error
#include <iostream>               // std::cerr

#include <gl_utils.hpp>
#include <string>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

// ******************************************************************** GLOBAL
#define FONT_PATH std::string("/home/dutech/Fonts/Consolas.ttf")
#define FONT_SIZE 12
#define SHADERS_PATH std::string( "src/shaders/text_fade" )
// ***************************************************************************
// ************************************************************* GLTextShaders
// ***************************************************************************
class GLTextShaders
{
  struct point { GLfloat x; GLfloat y; GLfloat s; GLfloat t; };
public:
  // ************************************************* GLTextShaders::creation
  GLTextShaders()
  {
    // Charger les Shaders : texture+fade
    GLint link_ok = GL_FALSE;
    GLuint vs, fs;
    /** Vertex Shader */
    if ((vs = GLUtils::create_shader(SHADERS_PATH+std::string(".v.glsl"), GL_VERTEX_SHADER))   == 0)
      exit( EXIT_FAILURE );
    /** Fragment Shader */
    if((fs = GLUtils::create_shader(SHADERS_PATH+std::string(".f.glsl"), GL_FRAGMENT_SHADER)) == 0)
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
      std::cerr << "__GLTextShaders: Pb avec glLinkProgam "<< std::endl;
      std::cerr << GLUtils::str_compiler_log(_program) << std::endl;
      exit( EXIT_FAILURE );
    }

    // Lier les variables des Shaders
    // coord 2D
    const char* attribute_name = "coord";
    _attribute_coord = glGetAttribLocation(_program, attribute_name);
    if (_attribute_coord == -1) {
      std::cerr <<  "Pb pour lier l'attribut " << attribute_name << std::endl;
      exit( EXIT_FAILURE );
    }
    // uniform tex
    const char* uniform_name = "tex";
    _uniform_tex = glGetUniformLocation(_program, uniform_name);
    if (_uniform_tex == -1) {
      std::cerr <<  "Pb pour lier l'uniform " << uniform_name << std::endl;
      exit( EXIT_FAILURE );
    }
    // uniform color
    uniform_name = "color";
    _uniform_color = glGetUniformLocation(_program, uniform_name);
    if (_uniform_color == -1) {
      std::cerr <<  "Pb pour lier l'uniform " << uniform_name << std::endl;
      exit( EXIT_FAILURE );
    }

    // Un VBO
    glGenBuffers(1, &_vbo_tex );
    
    // Init FreeType
    if( FT_Init_FreeType( &_ft )) {
      throw std::runtime_error( "GLTextShaders cannot init FreeType Library");
    }

    // Load Fonts :
    // create a FT_Face object, with an active FT_Size
    if( FT_New_Face( _ft, FONT_PATH.c_str(), 0, &_face)) {
      throw std::runtime_error( "GLTextShaders cannot open "+FONT_PATH );
    }
    // set nominal size in pixels
    if( FT_Set_Pixel_Sizes( _face, 0, FONT_SIZE) ) {
      throw std::runtime_error( "GLTextShaders cannot set size at "+ FONT_SIZE );
    }

    // Test si on peut charger un 'X'
    if( FT_Load_Char( _face, 'X', FT_LOAD_RENDER)) {
      throw std::runtime_error( "GLTextShaders cannot load 'X'" );
    }
  }
  // ******************************************** GLTextShaders::destruction
  ~GLTextShaders()
  {
    // Détruit le programme GLSL
    glDeleteProgram(_program);
    // Destroy FT_Face object
    FT_Done_Face( _face );
  };
  // ********************************************* GLTextShaders::pre_render
  void pre_render()
  {
    glUseProgram(_program);

    /* Enable blending, necessary for our alpha texture */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  };
  // ************************************************ GLTextShaders::set_color
  void set_color( const glm::vec4& fg_color = {1,0,0,1} )
  {
    glUniform4f( _uniform_color, fg_color.r, fg_color.g, fg_color.b, fg_color.a );
  };
  // ************************************************* GLTextShaders::set_size
  void set_size( const unsigned int font_size=FONT_SIZE )
  {
    FT_Set_Pixel_Sizes( _face, 0, font_size );
  };
  // ********************************************** GLTextShaders::render_text
  /**
   * Render text using the currently loaded font and currently set font size.
   * Rendering starts at coordinates (x, y), z is always 0.
   * The pixel coordinates that the FreeType2 library uses are scaled by (sx, sy).
   */
  void render_text(const char *text, float x, float y, float sx, float sy)
  {
    const char *p;
    FT_GlyphSlot g = _face->glyph;

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &_tex);
    glBindTexture(GL_TEXTURE_2D, _tex);
    glUniform1i(_uniform_tex, 0);

    /* We require 1 byte alignment when uploading texture data */
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    /* Clamping to edges is important to prevent artifacts when scaling */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    /* Linear filtering usually looks best for text */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /* Set up the VBO for our vertex data */
    glEnableVertexAttribArray(_attribute_coord);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_tex);
    glVertexAttribPointer(_attribute_coord, 4, GL_FLOAT, GL_FALSE, 0, 0);

    /* Loop through all characters */
    for (p = text; *p; p++) {
      /* Try to load and render the character */
      if (FT_Load_Char(_face, *p, FT_LOAD_RENDER))
	continue;
      
      /* Upload the "bitmap", which contains an 8-bit grayscale image, as an alpha texture */
      glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, g->bitmap.width, g->bitmap.rows, 0, GL_ALPHA, GL_UNSIGNED_BYTE, g->bitmap.buffer);

      /* Calculate the vertex and texture coordinates */
      float x2 = x + g->bitmap_left * sx;
      float y2 = -y - g->bitmap_top * sy;
      float w = g->bitmap.width * sx;
      float h = g->bitmap.rows * sy;

      point box[4] = {
	{x2, -y2, 0, 0},
	{x2 + w, -y2, 1, 0},
	{x2, -y2 - h, 0, 1},
	{x2 + w, -y2 - h, 1, 1},
      };
      
      /* Draw the character on the screen */
      glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
      
      /* Advance the cursor to the start of the next character; 
	 in 1/64th of pixels */
      x += (g->advance.x >> 6) * sx;
      y += (g->advance.y >> 6) * sy;
    }
  };
  /** This version takes UTF-8 (char16_t), ie. u"Enervé" */ 
  void render_text(const char16_t *text, float x, float y, float sx, float sy)
  {
    const char16_t *p;
    FT_GlyphSlot g = _face->glyph;

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &_tex);
    glBindTexture(GL_TEXTURE_2D, _tex);
    glUniform1i(_uniform_tex, 0);

    /* We require 1 byte alignment when uploading texture data */
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    /* Clamping to edges is important to prevent artifacts when scaling */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    /* Linear filtering usually looks best for text */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /* Set up the VBO for our vertex data */
    glEnableVertexAttribArray(_attribute_coord);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_tex);
    glVertexAttribPointer(_attribute_coord, 4, GL_FLOAT, GL_FALSE, 0, 0);

    /* Loop through all characters */
    for (p = text; *p; p++) {

      FT_UInt glyph_index = FT_Get_Char_Index( _face, *p );
      /* Try to load and render the character */
      if (FT_Load_Glyph(_face, glyph_index, FT_LOAD_RENDER))
	continue;
      /* convert to an anti-aliased bitmap */
      if( FT_Render_Glyph( _face->glyph, FT_RENDER_MODE_NORMAL ))
	continue;
      
      /* Upload the "bitmap", which contains an 8-bit grayscale image, as an alpha texture */
      glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, g->bitmap.width, g->bitmap.rows, 0, GL_ALPHA, GL_UNSIGNED_BYTE, g->bitmap.buffer);

      /* Calculate the vertex and texture coordinates */
      float x2 = x + g->bitmap_left * sx;
      float y2 = -y - g->bitmap_top * sy;
      float w = g->bitmap.width * sx;
      float h = g->bitmap.rows * sy;

      point box[4] = {
	{x2, -y2, 0, 0},
	{x2 + w, -y2, 1, 0},
	{x2, -y2 - h, 0, 1},
	{x2 + w, -y2 - h, 1, 1},
      };
      
      /* Draw the character on the screen */
      glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
      
      /* Advance the cursor to the start of the next character; 
	 in 1/64th of pixels */
      x += (g->advance.x >> 6) * sx;
      y += (g->advance.y >> 6) * sy;
    }
  };
  // ********************************************** GLTExtShaders::post_render
  void post_render()
  {
    glDisableVertexAttribArray(_attribute_coord);
    glDeleteTextures(1, &_tex);
  };
  // ********************************************** GlTextShaders::attributs
  FT_Library _ft;
  FT_Face _face;
  const GLuint& program() const{ return _program; };
  const GLint&  attribute_coord() const { return _attribute_coord; };
protected:
  /** Program GLSL */
  GLuint _program;
  /** Variables globale du Programme GLSL */
  GLint _attribute_coord;
  /** Uniform var */
  GLint _uniform_tex, _uniform_color;
  /** Vertex Buffer Object */
  GLuint _vbo_tex;
  /* Create a texture that will be used to hold one "glyph" */
  GLuint _tex;
};

#endif // GL_TEXT_SHADERS_HPP
