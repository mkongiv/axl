/*
AQCSL: the Affine Quantum Circuit Specification Language
Filename: aql_val.hh
Creation date: Thu May 16 09:47:07 EDT 2019
Copyright (C) 2019, Martin Kong

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef AQCL_VAL
#define AQCL_VAL

#include <vector>
#include <map>
#include <string>
#include <iostream>
//#include "aqcsl-ir.hh"
#include "aql_node.hh"

using namespace std;

const int AS_ERROR = 0;
const int AS_INT = 1;
const int AS_DOUBLE = 2;
const int AS_FLOAT = 3;

class aqcl_val : public aqcl_node
{
  private:

  int m_ival;
  double m_dval;
  float m_fval;
  int m_as;

  public:

  aqcl_val () : aqcl_node ()
  {
    m_node_category = AQL_NODE_IVAL;
    m_as = AS_ERROR;
  }

  aqcl_val (int p_ival) : aqcl_node ()
  {
    m_node_category = AQL_NODE_IVAL;
    m_ival = p_ival;
    m_as = AS_INT;
    m_vartype = SEMTYPE_INT;
  }

  aqcl_val (double p_dval) : aqcl_node ()
  {
    m_node_category = AQL_NODE_DVAL;
    m_dval = p_dval;
    m_as = AS_DOUBLE;
    m_vartype = SEMTYPE_DOUBLE;
  }

  aqcl_val (float p_fval) : aqcl_node ()
  {
    m_node_category = AQL_NODE_FVAL;
    m_fval = p_fval;
    m_as = AS_FLOAT;
    m_vartype = SEMTYPE_FLOAT;
  }

  ~ aqcl_val ()
  {

  }

  inline bool is_int ()
  {
    return m_as == AS_INT;
  }

  inline bool is_double ()
  {
    return m_as == AS_DOUBLE;
  }

  inline bool is_float ()
  {
    return m_as == AS_FLOAT;
  }

  virtual string get_string ()
  {
    if (m_as == AS_INT)
      return std::to_string(m_ival);
    if (m_as == AS_DOUBLE)
      return std::to_string(m_dval);
    if (m_as == AS_FLOAT)
      return std::to_string(m_fval);
    assert (0 && "aql_val.hh: Unknown literal stored.");
  }

  inline int get_ival ()
  {
    return m_ival;
  }

  inline double get_dval ()
  {
    return m_dval;
  }

  inline float get_fval ()
  {
    return m_fval;
  }

  inline double get_val ()
  {
    if (is_int ())
      return get_ival ();
    if (is_float ())
      return get_fval ();
    if (is_double ())
      return get_dval ();
    assert (0);
  }


  int get_as ()
  {
    return m_as;
  }

  int get_category ()
  {
    return m_node_category;
  }

  bool is_same_category (int p_cat)
  {
    return m_node_category == p_cat;
  }

  virtual aqcl_node * get_result ()
  {
    return this;
  }

  virtual string get_result_name ()
  {
    return get_string ();
  }

  virtual string get_name ()
  {
    return get_result_name ();
  }

  virtual string get_instruction_str ()
  {
    return get_name ();
  }

  void operator = (aqcl_val * p_val)
  {
    m_ival = p_val->get_ival ();
    m_dval = p_val->get_dval ();
    m_fval = p_val->get_fval ();
    m_as = p_val->get_as ();
  }

  void operator = (aqcl_val & p_val)
  {
    m_ival = p_val.get_ival ();
    m_dval = p_val.get_dval ();
    m_fval = p_val.get_fval ();
    m_as = p_val.get_as ();
  }

  virtual bool is_val ()
  {
    return true;
  }

  double operator * (aqcl_val & p_val)
  {
    return this->get_val () * p_val.get_val ();
  }

  virtual string get_expression_plain ()
  {
    return get_string ();
  }


  virtual void print ()
  {
    cout << get_string ();
  }

/*
  void operator << ()
  {
    cout << this->get_val ();
  }
  */
};

#endif
