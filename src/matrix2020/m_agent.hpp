/* -*- coding: utf-8 -*- */

#ifndef M_AGENT_HPP
#define M_AGENT_HPP

/** 
 * Move : advance or turn right if obstacle
 * TODO : move_apply
 */
#include <iostream>
#include <matrix2020/m_def.hpp>
#include <matrix2020/m_environment.hpp>

namespace matrix2020 {
// ***************************************************************************
// ********************************************************************* Agent
// ***************************************************************************
class Agent : public GameObject
{
public:
  // ****************************************************** Agent::constructor
  Agent( const Environment& env, const std::string& name ) :
    GameObject(name), _env(env)
  {
  }

  // ****************************************************** Agent::move_intent
  /** move_intent : advance if no obstaclen otherwise turn right */
  virtual void take_decision( bool debug=false )
  {
    if (debug) {
      std::cout << "__AGENT " << _name << std::endl;
      std::cout << "  intent=" << (pos()+orient()) << std::endl;
    }
    if (_env.is_obstacle( pos() + orient() )) {
      if (debug) std::cout << "  obstacle" << std::endl;
      _action = { pos(), turn_right( orient() )};
    }
    else {
      _action = { pos() + orient(), orient() };
    }
    if (debug) {
      std::cout << "  move=" << _action.pos << " / " << _action.dir << std::endl;
    }
  }
  // ******************************************************** Agent::attributs
  const Environment& _env;
};
// *************************************************************** Agent - END
};  // matrix2020

#endif // M_AGENT_HPP
