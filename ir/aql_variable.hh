/*
AQCSL: the Affine Quantum Circuit Specification Language
Filename: aql_variable.hh
Creation date: Thu May 16 11:43:12 EDT 2019
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

#ifndef AQCL_VARIABLE
#define AQCL_VARIABLE

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include "aql_val.hh"
#include "aql_node.hh"

using namespace std;



class aqcl_variable : public aqcl_node
{
  protected:
  aqcl_val * m_val;
  aqcl_node * m_block; // in reality it will be a aqcl_block, must cast after retrieving

  public:

  aqcl_variable () : aqcl_node ()
  {
    m_node_category = AQL_NODE_VAR;
    m_is_temp = false;
    m_block = NULL;
    m_pos = 0;
    m_val = new aqcl_val (0);
  }

  aqcl_variable (string p_name, int p_vartype = 0) : aqcl_node ()
  {
    m_node_category = AQL_NODE_VAR;
    m_name = p_name;
    m_vartype = p_vartype;
    m_is_temp = false;
    m_block = NULL;
    m_pos = 0;
    m_val = new aqcl_val (0);
  }

  aqcl_variable (string p_name, int p_vartype, int p_pos) : aqcl_node ()
  {
    m_node_category = AQL_NODE_VAR;
    m_name = p_name;
    m_vartype = p_vartype;
    m_is_temp = false;
    m_block = NULL;
    m_pos = p_pos;
    m_val = new aqcl_val (0);
  }

  ~ aqcl_variable ()
  {

  }

  aqcl_node * get_block ()
  {
    return m_block;
  }

  bool has_load ()
  {
    return has_payload ();
  }

  bool has_payload ()
  {
    return m_block != NULL;
  }

  void set_block (aqcl_node * p_block)
  {
    m_block = p_block;
  }

  string get_name ()
  {
    return m_name;
  }

  void set_name (string p_name)
  {
    m_name = p_name;
  }

  virtual string get_string ()
  {
    return m_name;
  }

  int get_category ()
  {
    return m_node_category;
  }

  bool is_same_category (int p_cat)
  {
    return m_node_category == p_cat;
  }

  virtual void set_temp ()
  {
    m_is_temp = true;
  }

  virtual aqcl_node * get_result ()
  {
    return this;
  }

  virtual string get_result_name ()
  {
    return get_name ();
  }

  virtual string get_instruction_str ()
  {
    return get_name ();
  }


  virtual bool is_variable ()
  {
    return true;
  }

  virtual void print ()
  {
    if (m_block)
      m_block->print ();
    if (m_val)
      m_val->print ();
  }

  aqcl_val * get_val ()
  {
    return m_val;
  }

  void operator = (aqcl_val p_val)
  {
    if (m_val)
      delete m_val;
    m_val = new aqcl_val ();
    *m_val = p_val;
  }

  void operator = (aqcl_variable * p_var)
  {
    if (m_val)
      delete m_val;
    m_val = new aqcl_val ();
    *m_val = *(p_var->get_val ());
  }

  void operator = (int p_ival)
  {
    if (m_val)
      delete m_val;
    m_val = new aqcl_val (p_ival);
  }

  void set_val(double p_dval)
  {
    if (m_val)
      delete m_val;
    m_val = new aqcl_val (p_dval);
  }

  void operator = (float p_fval)
  {
    if (m_val)
      delete m_val;
    m_val = new aqcl_val (p_fval);
  }

  void mul (aqcl_val * left, aqcl_val * right)
  {
    if (m_val)
      delete m_val;
    m_val = new aqcl_val (left->get_val () * right->get_val ());
  }

  void load_val (aqcl_val * src_val)
  {
    if (m_val)
      delete m_val;
    m_val = new aqcl_val ();
    *m_val = *src_val;
  }

  void load_val (aqcl_variable * src_var)
  {
    if (m_val)
      delete m_val;
    m_val = new aqcl_val ();
    *m_val = *(src_var->get_val ());
  }

  void load_val_neg (aqcl_val * src_val)
  {
    if (m_val)
      delete m_val;
    m_val = new aqcl_val (- src_val->get_val ());
  }

  void load_val_neg (aqcl_variable * src_var)
  {
    if (m_val)
      delete m_val;
    m_val = new aqcl_val (- src_var->get_val ()->get_val ());
  }

  void canonicalize_dimensions (string oldname, string newname)
  {
    if (m_name == oldname)
    {
      m_name = newname;
    }
  }

  
  virtual string get_expression_plain ()
  {
    if (is_param ())
      return get_name ();
    if (not is_temp ())
      return get_name ();
    return this->get_val ()->get_string ();
  }
};

#endif
