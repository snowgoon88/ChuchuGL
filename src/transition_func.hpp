/* -*- coding: utf-8 -*- */

#ifndef TRANSITION_FUNC_HPP
#define TRANSITION_FUNC_HPP

/** 
 * Fonction to compute transition from a to b, in delta_t time.
 */

#include <math.h>
#include <sstream>

// ***************************************************************************
// ******************************************************** TransitionFunction
// ***************************************************************************
class TransitionFunction
{
public:
  // ******************************************** TransitionFunction::creation
  TransitionFunction( double a, double b, double delta_t, int order=1 ) :
    _a(a), _b(b), _delta_t(delta_t), _order(order)
  {
    // build coefficient matrix
    _coef.clear();
    for( unsigned int n = 0; n <= _order; ++n) {
      _coef.push_back( pascal_triangle( -_order - 1, n) *
                       pascal_triangle( 2 * _order + 1, _order - n) );
    }
  }
  // ************************************************* TransitionFunction::val
  double val( double t )
  {
    double x = clamp( t / _delta_t, 0.0, 1.0);
    double res = 0.0;
    for( unsigned int n = 0; n <= _order; ++n) {
      res += _coef[n] * pow( x, _order + n + 1 );
    }
    return _a + res * (_b - _a);
  }
  std::string str_dump () const
  {
    std::stringstream dump;
    dump << "TF[" << _order << "]={ ";
    for( auto& v: _coef) {
      dump << v << ", ";
    }
    dump << "}";

    return dump.str();
  }
private:
  // *********************************************** TransitionFunction::utils
  double clamp( double x, double lowerlimit, double upperlimit)
  {
    if (x < lowerlimit)
      x = lowerlimit;
    if (x > upperlimit)
      x = upperlimit;
    return x;
  }
  int pascal_triangle( int a, int b )
  {
    int res = 1;
    for (int i=0; i < b; i++) {
      res *= (a - i);
      res /= (i + 1);
    }
    //std::cout << " TP("<<a<<","<<b<<")="<<res << std::endl;
    return res;
  }
  // ******************************************* TransitionFunction::attributs
public:
  double delta_t() const { return _delta_t; }
private:
  double _a, _b, _delta_t;
  int _order;
  std::vector<int> _coef;
};



#endif // TRANSITION_FUNC_HPP
