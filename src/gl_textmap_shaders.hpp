/* -*- coding: utf-8 -*- */

#ifndef GL_TEXTMAP_SHADERS_HPP
#define GL_TEXTMAP_SHADERS_HPP

/** 
 * Display texts using shaders AND a map of most frequent characters.
 * Can render ascii (char) or unicode (char16_t) or string.
 *
 * By default, the screen coordinate go from (-1,-1) to (1,1).
 *
 * https://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_Text_Rendering_01
 * https://learnopengl.com/In-Practice/Text-Rendering
 */

#include <stdexcept>              // std::runtime_error
#include <iostream>               // std::cerr
#include <sstream>
#include <map>

#include <gl_shader.hpp>

#include <gl_utils.hpp>
#include <string>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

// ******************************************************************** GLOBAL
#define FONT_PATH std::string("ressources/Consolas.ttf")
#define FONT_SIZE 48
#define SHADERS_PATH std::string( "src/shaders/text" )
#define CLASS std::string( "GLTextmapShader" )
// Maximum texture width
#define MAXWIDTH 512
// ***************************************************************************
// ********************************************************** GLTextmapShaders
// ***************************************************************************

class GLTextmapShaders
{
  struct GLGlyph {
    glm::ivec2 size;
    glm::ivec2 bearing;
    GLuint     advance;
  };
  std::string str_glyph( const GLGlyph& gg) const
  {
    std::stringstream dump;
    dump << "s=" << gg.size[0] << ", " << gg.size[1];
    dump << " b=" << gg.bearing[0] << ", " << gg.bearing[1];
    dump << " ad=" << gg.advance;

    return dump.str();
  }
    
  struct point { GLfloat x; GLfloat y; GLfloat s; GLfloat t; };
public:
  // ************************************************* GLTextShaders::creation
  GLTextmapShaders() :
    _sx( 1.f ), _sy( 1.f )
  {
    // Load Shaders : texture with RED component
    _tex_shader = new GLShader( SHADERS_PATH+std::string(".v330.glsl"),
                            SHADERS_PATH+std::string(".f330.glsl") );
    _proj_view_loc = _tex_shader->getUniformLocation( "proj_view" );
    _tex_loc = _tex_shader->getUniformLocation( "tex" );
    _text_color_loc = _tex_shader->getUniformLocation( "text_color" );
    
    // Init FreeType
    if( FT_Init_FreeType( &_ft )) {
      throw std::runtime_error( CLASS+" cannot init FreeType Library");
    }
    // Load Fonts :
    // create a FT_Face object, with an active FT_Size
    if( FT_New_Face( _ft, FONT_PATH.c_str(), 0, &_face)) {
      throw std::runtime_error( CLASS+" cannot open "+FONT_PATH );
    }
    // set nominal size in pixels
    if( FT_Set_Pixel_Sizes( _face, 0, FONT_SIZE) ) {
      throw std::runtime_error(  CLASS+" cannot set size at "+ std::to_string(FONT_SIZE) );
    }

    // Test if can access an 'X'
    if( FT_Load_Char( _face, 'X', FT_LOAD_RENDER)) {
      throw std::runtime_error( CLASS+" cannot load 'X'" );
    }
    compute_lineheight();

    // A VBO/VAO
    glGenVertexArrays( 1, &_vao_tex ); 
    glGenBuffers(1, &_vbo_tex );
    // build map
    build_map();
    
  }
  // ******************************************* GLTextmapShaders::destruction
  ~GLTextmapShaders()
  {
    if (_tex_shader) delete _tex_shader;
    // Destroy FT_Face object
    FT_Done_Face( _face );
    
    glDeleteBuffers( 1, &_vbo_tex);
  }
  // ********************************************* GLTextmapShaders::build_map
  /** create "large" texture to hold all the glyph raster
   */
  void build_map()
  {
    FT_GlyphSlot g = _face->glyph;
    
    auto text = std::u16string( u"ABÉ" );

    // Find size to hold map
    unsigned int map_w = 0;
    unsigned int map_h = 0;

    unsigned int row_w = 0;
    unsigned int row_h = 0;
    for (auto itt = text.begin(); itt != text.end(); ++itt) {
      std::cout << "size for " << *itt << std::endl;
      FT_UInt glyph_index = FT_Get_Char_Index( _face, *itt );
      /* Try to load and render the character */
      if (FT_Load_Glyph(_face, glyph_index, FT_LOAD_RENDER))
	continue;
      // TODO: antialiasing ??
      std::cout << "  index="<< glyph_index;
      std::cout << " " << g->bitmap.width << "x" << g->bitmap.rows << std::endl;
      
      if( row_w + g->bitmap.width + 1 >= MAXWIDTH ) {
        // create a new row in texture
        map_w = std::max( map_w, row_w );
        map_h += row_h;
        row_w = 0;
        row_h = 0;
      }
      row_w += g->bitmap.width+1;
      row_h = std::max( row_h, g->bitmap.rows );
    }
    map_w = std::max( map_w, row_w );
    map_h += row_h;

    std::cout << "Map size = " << map_w << " x " << map_h << std::endl;

    // Generate texture
    glActiveTexture(GL_TEXTURE0);
    glGenTextures( 1, &_textmap ); // generate 1 texture name
    glBindTexture( GL_TEXTURE_2D, _textmap );
    // Create mutable (texture size and format) storage
    glTexImage2D( GL_TEXTURE_2D, 0 /*level*/, GL_RED /*format*/,
                  map_w, map_h, 0, /*width x height, border */
                  GL_RED, GL_UNSIGNED_BYTE, NULL /* format, type, pixels */ );
    // We require 1 byte alignment when uploading texture data
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    // Set texture options TODO : could use CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);       
                  
    
    // Put glyphs into _textmap
    // // TODO: iterate on string
    // const char16_t* text = u"ABÉ";
    GLint gx = 0;
    GLint gy = 0;
    row_h = 0;
    
    for (auto itt = text.begin(); itt != text.end(); ++itt) {
      std::cout << "Pasting  " << *itt << std::endl;
      FT_UInt glyph_index = FT_Get_Char_Index( _face, *itt );
      /* Try to load and render the character */
      if (FT_Load_Glyph(_face, glyph_index, FT_LOAD_RENDER))
	continue;
      
      if (gx + g->bitmap.width + 1 >= MAXWIDTH) {
        gy += row_h; 				
        row_h = 0;
        gx = 0;
      }

      glTexSubImage2D( GL_TEXTURE_2D /*target*/, 0 /*level*/,
                       gx, gy, /*offset*/
                       g->bitmap.width, g->bitmap.rows, /*size*/
                       GL_RED, GL_UNSIGNED_BYTE, /*format, type*/
                       g->bitmap.buffer /* pixels */);

      row_h = std::max( row_h, g->bitmap.rows);
      gx += g->bitmap.width + 1;
    }
    std::cout << "__TEX generated" << std::endl;

    //for( const char16_t* p = text; *p; p++  ) {
    //   // Load Glyph
    //   if (FT_Load_Char( _face, *p, FT_LOAD_RENDER)) {
    //     throw std::runtime_error( "GLTextmapShaders cannot load Glyph" );
    //   }
    //   // glTexSubImage2D( GL_TEXTURE_2D /*target*/, 0 /*level*/,
    //   //                  (*p-65) * 20, 0, /*offset*/
    //   //                  g->bitmap.width, g->bitmap.rows, /*size*/
    //   //                  GL_ALPHA, GL_UNSIGNED_BYTE, /*format, type*/
    //   //                  g->bitmap.buffer );

    //   //Store in GLGlyphmap
    //   GLGlyph gg = {
    //     glm::ivec2( g->bitmap.width, g->bitmap.rows ),
    //     glm::ivec2( _face->glyph->bitmap_left, _face->glyph->bitmap_top ),
    //     static_cast<GLuint>(_face->glyph->advance.x )
    //   };
    //   _glyphs.insert( std::pair<FT_UInt, GLGlyph>( *p, gg ));
    // }

    // glBindTexture(GL_TEXTURE_2D, 0);
    // // Destroy FreeType once we're finished
    // // TODO: FT_Done_Face(face);
    // // TODO: FT_Done_FreeType(ft);
    
  }
  // ********************************************** GlTextmapShaders::str_dump
  std::string str_dump () const
  {
    std::stringstream dump;
    dump << "TextMap" << std::endl;
    for( auto& gg: _glyphs) {
      dump << " " << gg.first << " : " << str_glyph( gg.second ) << std::endl;
    }

    return dump.str();
  }
  // ******************************************* GLTextmapShaders::render_dump
  void render_textmap( const glm::mat4& proj )
  {
    //   //pre_render();
    _tex_shader->use();
    // pass transformation matrices to the shader
    glUniformMatrix4fv( _proj_view_loc, 1, GL_FALSE,
                        glm::value_ptr( proj ) );
    glUniform4f( _text_color_loc, 1.f, 1.f, 1.f, 1.f );

    //glActiveTexture(GL_TEXTURE0); // not always usefull ??
    glBindTexture( GL_TEXTURE_2D, _textmap );
    glUniform1i( _tex_loc, 0);
    
    // Coordinate of the triangle to render map
    GLfloat vertices[6][4] = {
      {   -0.2f,    0.5f,     0.f, 0.f },
      {   -0.2f,    0.f,     0.f, 1.f },
      {   0.5f,    0.f,     1.f, 1.f },
      
      {   -0.2f,    0.5f,     0.f, 0.f },
      {   0.5f,    0.f,     1.f, 1.f },
      {   0.5f,    0.5f,     1.f, 0.f }
    };
      
    
    /* Set up the VAO/VBO for our vertex data */
    glBindVertexArray( _vao_tex );
    glBindBuffer( GL_ARRAY_BUFFER, _vbo_tex );
    glBufferData( GL_ARRAY_BUFFER, 
                  sizeof( vertices ), vertices,
                  GL_DYNAMIC_DRAW );
    //glBufferSubData( GL_ARRAY_BUFFER, 0 /*offset*/,  sizeof(vertices), vertices );
    // position attribute of shader
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE,
                          4 * sizeof(GLfloat), // stride
                          (void*)0);         // offset

    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    glDrawArrays( GL_TRIANGLES, 0, 6 );

    glBindVertexArray( 0 );
    glBindTexture( GL_TEXTURE_2D, 0 );

  //   // coord screen, coord texture
  //   GLfloat carre_vtx[] = {
  //     0.f, 0.f, 0.f, 0.f,
  //     0.5f, 0.f, 1.f, 0.f,
  //     0.f, 0.5f, 0.f, 1.f,
  //     0.5f, 0.5f, 1.f, 1.f
  //   };
  //   glBufferData(GL_ARRAY_BUFFER, sizeof carre_vtx, carre_vtx, GL_DYNAMIC_DRAW);
  //   glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
  }
  // // ********************************************* GLTextShaders::pre_render
  // void pre_render()
  // {
  //   glUseProgram(_tex_shader);

  //   /* Enable blending, necessary for our alpha texture */
  //   glEnable(GL_BLEND);
  //   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  // }
  // ************************************************ GLTextShaders::set_color
  void set_color( const glm::vec4& fg_color = {1,0,0,1} )
  {
    glUniform4f( _text_color_loc, fg_color.r, fg_color.g, fg_color.b, fg_color.a );
  }
  // ************************************************* GLTextShaders::set_size
  void set_size( const unsigned int font_size=FONT_SIZE )
  {
    FT_Set_Pixel_Sizes( _face, 0, font_size );
    compute_lineheight();
  };
  // ************************************************ GLTextShaders::set_scale
  void set_scale( const float scale_x, const float scale_y )
  {
    _sx = scale_x;
    _sy = scale_y;
    compute_lineheight();
  }
  // // ********************************************** GLTextShaders::render_text
  // /**
  //  * Render text using the currently loaded font and currently set font size.
  //  * Rendering starts at coordinates (x, y), z is always 0.
  //  * The pixel coordinates used by FreeType2 are scaled by (_sx, _sy).
  //  */
  // void render( const char *text, float x, float y)
  // {
  //   render_text( text, x, y, _sx, _sy );
  // }
  // void render( const char16_t *text, float x, float y)
  // {
  //   render_text( text, x, y, _sx, _sy);
  // }
  // void render( const std::string& text, float x=0, float y=0 )
  // {
  //   render_text( text.c_str(), x, y, _sx, _sy );
  // }
  // // *************************************************************************
  // void render_text(const char *text, float x, float y, float sx, float sy)
  // {
  //   const char *p;
  //   FT_GlyphSlot g = _face->glyph;

  //   glActiveTexture(GL_TEXTURE0);
  //   glGenTextures(1, &_tex);
  //   glBindTexture(GL_TEXTURE_2D, _tex);
  //   glUniform1i(_uniform_tex, 0);

  //   /* We require 1 byte alignment when uploading texture data */
  //   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  //   /* Clamping to edges is important to prevent artifacts when scaling */
  //   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  //   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  //   /* Linear filtering usually looks best for text */
  //   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  //   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  //   /* Set up the VBO for our vertex data */
  //   glEnableVertexAttribArray(_attribute_coord);
  //   glBindBuffer(GL_ARRAY_BUFFER, _vbo_tex);
  //   glVertexAttribPointer(_attribute_coord, 4, GL_FLOAT, GL_FALSE, 0, 0);

  //   /* Loop through all characters */
  //   for (p = text; *p; p++) {
  //     /* Try to load and render the character */
  //     if (FT_Load_Char(_face, *p, FT_LOAD_RENDER))
  //       continue;
      
  //     /* Upload the "bitmap", which contains an 8-bit grayscale image, as an alpha texture */
  //     glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, g->bitmap.width, g->bitmap.rows, 0, GL_ALPHA, GL_UNSIGNED_BYTE, g->bitmap.buffer);

  //     /* Calculate the vertex and texture coordinates */
  //     float x2 = x + g->bitmap_left * sx;
  //     float y2 = -y - g->bitmap_top * sy;
  //     float w = g->bitmap.width * sx;
  //     float h = g->bitmap.rows * sy;

  //     point box[4] = {
  //       {x2, -y2, 0, 0},
  //       {x2 + w, -y2, 1, 0},
  //       {x2, -y2 - h, 0, 1},
  //       {x2 + w, -y2 - h, 1, 1},
  //     };
  //     // std::cout << "A -" << *p << "-";
  //     // std::cout << "  box=" << x2 << ", " << -y2 << ", " << w << ", " << h  << std::endl;
  //     // _lineheight = g->bitmap_top * sy;
      
  //     /* Draw the character on the screen */
  //     glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);
  //     glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
      
  //     /* Advance the cursor to the start of the next character; 
  //        in 1/64th of pixels */
  //     x += (g->advance.x >> 6) * sx;
  //     y += (g->advance.y >> 6) * sy;
  //   }
  // };
  // /** This version takes UTF-8 (char16_t), ie. u"Enervé" */ 
  // void render_text(const char16_t *text, float x, float y, float sx, float sy)
  // {
  //   const char16_t *p;
  //   FT_GlyphSlot g = _face->glyph;

  //   glActiveTexture(GL_TEXTURE0);
  //   glGenTextures(1, &_tex);
  //   glBindTexture(GL_TEXTURE_2D, _tex);
  //   glUniform1i(_uniform_tex, 0);

  //   /* We require 1 byte alignment when uploading texture data */
  //   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  //   /* Clamping to edges is important to prevent artifacts when scaling */
  //   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  //   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  //   /* Linear filtering usually looks best for text */
  //   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  //   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  //   /* Set up the VBO for our vertex data */
  //   glEnableVertexAttribArray(_attribute_coord);
  //   glBindBuffer(GL_ARRAY_BUFFER, _vbo_tex);
  //   glVertexAttribPointer(_attribute_coord, 4, GL_FLOAT, GL_FALSE, 0, 0);

  //   /* Loop through all characters */
  //   for (p = text; *p; p++) {

  //     FT_UInt glyph_index = FT_Get_Char_Index( _face, *p );
  //     /* Try to load and render the character */
  //     if (FT_Load_Glyph(_face, glyph_index, FT_LOAD_RENDER))
  //       continue;
  //     /* convert to an anti-aliased bitmap */
  //     if( FT_Render_Glyph( _face->glyph, FT_RENDER_MODE_NORMAL ))
  //       continue;
      
  //     /* Upload the "bitmap", which contains an 8-bit grayscale image, as an alpha texture */
  //     glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, g->bitmap.width, g->bitmap.rows, 0, GL_ALPHA, GL_UNSIGNED_BYTE, g->bitmap.buffer);

  //     /* Calculate the vertex and texture coordinates */
  //     float x2 = x + g->bitmap_left * sx;
  //     float y2 = -y - g->bitmap_top * sy;
  //     float w = g->bitmap.width * sx;
  //     float h = g->bitmap.rows * sy;

  //     point box[4] = {
  //       {x2, -y2, 0, 0},
  //       {x2 + w, -y2, 1, 0},
  //       {x2, -y2 - h, 0, 1},
  //       {x2 + w, -y2 - h, 1, 1},
  //     };
  //     // std::cout << "U -" << *p << "-";
  //     // std::cout << "  box=" << x2 << ", " << -y2 << ", " << w << ", " << h  << std::endl;
  //     // _lineheight = g->bitmap_top * sy;
      
  //     /* Draw the character on the screen */
  //     glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);
  //     glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
      
  //     /* Advance the cursor to the start of the next character; 
  //        in 1/64th of pixels */
  //     x += (g->advance.x >> 6) * sx;
  //     y += (g->advance.y >> 6) * sy;
  //   }
  // };
  // // ********************************************** GLTExtShaders::post_render
  // void post_render()
  // {
  //   //glDisableVertexAttribArray(_attribute_coord);
  //   glDeleteTextures(1, &_tex);
  // };
  // *************************************** GLTextShaders::compute_lineheight
  /**
   * Size of the 'O'
   */
  void compute_lineheight()
  {
    if (FT_Load_Char(_face, 79, FT_LOAD_RENDER))
      _lineheight = 0;    
    else
      _lineheight =  _face->glyph->bitmap.rows * _sy;

  }
  // ************************************************ GlTextShaders::attributs
  float line_height() const { return _lineheight; }
  // *************************************************************************
  FT_Library _ft;
  FT_Face _face;
  float _sx, _sy;    // scale
  float _lineheight; // current line height
protected:
  /** Program GLSL */
  GLShader *_tex_shader;
  /** Variables for Shader */
  GLuint _proj_view_loc, _tex_loc, _text_color_loc;

  /** Vertex Buffer Object */
  GLuint _vao_tex, _vbo_tex;
  /* Create a texture that will be used to hold one "glyph" */
  GLuint _textmap, _tex;
  std::map<char16_t,GLGlyph> _glyphs;
};

#endif // GL_TEXT_SHADERS_HPP
