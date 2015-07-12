/* -*- coding: utf-8 -*- */

#ifndef GL_ARROW_HPP
#define GL_ARROW_HPP

/** 
 * Viewer d'une arrow,
 * change de taille en fonction de l'index d'animation
 * TODO traînée de même couleur, fading...
 */

#include <gl_engine.hpp>
#include <gl_sprite.hpp>

// ******************************************************************** GLOBAL
#define ANIM_LENGTH 30
#define IDX_CURSOR 0
#define IDX_ARROW  1
#define IDX_CROSS  2
#define NB_SPRITE_ROW (IDX_CROSS+1)
#define NB_SPRITE_COL 4

// ***************************************************************************
// ******************************************************************* GLArrow
// ***************************************************************************
class GLArrow : public GLSprite
{
public:
  // ******************************************************* GLArrow::creation
  GLArrow( GLEngine& engine ) :
    GLSprite( engine.gl_texture(), "../Images/tex_arrow.png",
	      NB_SPRITE_ROW, NB_SPRITE_COL)
  {
  };
  // ***************************************************** GLArrow::destruction
  virtual ~GLArrow()
  {
  };
  // ************************************************** GLArrow::render_cursor
  /** 'anim_idx' indique l'index de l'animation 
   * on s'en sert pour scaler la flèche.
   */
  void render_cursor( glm::mat4& proj, const Vec2& pos, 
		      unsigned int color_idx, unsigned int anim_idx )
  {

    // Suivant anim, on scale entre 0.4 et 0.6    
    double anim_scale = 0.4;
    if( anim_idx < ANIM_LENGTH/2 ) 
      anim_scale += anim_idx * 0.01;
    else 
      anim_scale += (ANIM_LENGTH - anim_idx) * 0.01;

    GLSprite::render( proj, {pos.x+0.5, pos.y-0.5}, anim_scale, 
		      IDX_CURSOR * NB_SPRITE_COL + color_idx);
  };
  // ************************************************** GLArrow::render_arrow
  void render_arrow( glm::mat4& proj, const Vec2& pos, const Direction& dir, 
		     unsigned int color_idx )
  {
    render( proj, {pos.x+0.5, pos.y+0.5}, 
	    -M_PI/2 * dir.index, 0.5,
	    IDX_ARROW*NB_SPRITE_COL + color_idx );
  };
  // ************************************************** GLArrow::render_cross
  void render_cross( glm::mat4& proj, const Vec2& pos )  
  {
    render( proj, {pos.x+0.5, pos.y+0.5}, 0.5, 
	    IDX_CROSS*NB_SPRITE_COL // 1 seul cursor
	    );
  };
  // ******************************************************** GLArrow::attributs
private:
};

#endif // GL_ARROW_HPP
