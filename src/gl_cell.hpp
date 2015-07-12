/* -*- coding: utf-8 -*- */

#ifndef GL_CELL_HPP
#define GL_CELL_HPP

/** 
 * Viewer OpenGL des differentes Cell : texture.
 */

#include <gl_engine.hpp>
#include <gl_sprite.hpp>

#include <cell.hpp>

// ******************************************************************** GLOBAL
#define ROCKET_ANIM_LENGTH 10
// ***************************************************************************
// ****************************************************************** GLRocket
// ***************************************************************************
class GLRocket : public GLSprite
{
public:
  // ********************************************************** GLRocket::type
  typedef struct {
    RocketPtr rocket;
    unsigned int idx_anim;
  } SRocket;
  typedef std::list<SRocket>  CSRocket;

public:
  // ****************************************************************** creation
  GLRocket( GLEngine& engine ) :
    GLSprite( engine.gl_texture(), "../Images/tex_rocket.png",
	      1, 4)
  {
  };
  // *************************************************************** destruction
  virtual ~GLRocket()
  {
  };
  // ********************************************************** GLRocket::init
  void init( CRocketPtr liste_rocket )
  {
    _l_rocket.clear();
    for( auto& rocket_ptr: liste_rocket) {
      _l_rocket.push_back( {rocket_ptr, ROCKET_ANIM_LENGTH} );
    }    
  }
  // *********************************************************************render
  void render( glm::mat4& proj, double simu_time )
  {    
    
    for( auto& anim: _l_rocket) {
      
      // L'idx_anim dépend de la durée depuis _last_time des rocket
      if( (simu_time - anim.rocket->time_last() ) < 0.001 ) {
	anim.idx_anim = 0;
      }
      else if( anim.idx_anim < ROCKET_ANIM_LENGTH) {
	anim.idx_anim += 1;
      }
      // Le reste du temps, on dessine avec idx_anim = ANIM_LENGTH
      
      // scale entre 0.4 et 0.6
      double anim_scale = 0.4;
      if( anim.idx_anim < ROCKET_ANIM_LENGTH/2 )
	anim_scale += anim.idx_anim * 0.01;
      else 
	anim_scale += (ROCKET_ANIM_LENGTH - anim.idx_anim) * 0.01;
      
      GLSprite::render( proj, 
			{anim.rocket->pos().x+0.5,anim.rocket->pos().y+0.5},
			anim_scale, 
			0 /*id*/ );      
    }
  };
  // ***************************************************************** attributs
private:
  /** Liste des Rocket et Animation */
  CSRocket _l_rocket;
};

#endif // GL_CELL_HPP
