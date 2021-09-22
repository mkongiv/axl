/*
AQCSL: the Affine Quantum Circuit Specification Language
Filename: aql_node.hh
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

#ifndef AQCL_NODE
#define AQCL_NODE

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <assert.h>
//#include "aqcsl-ir.hh"

using namespace std;

#define AQL_NODE_NODE 1
#define AQL_NODE_ROOT 2 
#define AQL_NODE_STMT 3
#define AQL_NODE_CIRC 4
#define AQL_NODE_ARIT 5
#define AQL_NODE_VAR  6
#define AQL_NODE_INEQ 7
#define AQL_NODE_GATE 8
#define AQL_NODE_IVAL 9
#define AQL_NODE_DVAL 10
#define AQL_NODE_FVAL 11
#define AQL_NODE_SYMTAB 12


#define AQL_NODE_OPERATOR 16

#define AQL_OP_CLASS_UNI 1
#define AQL_OP_CLASS_BIN 2
#define AQL_OP_CLASS_TRI 3
#define AQL_OP_CLASS_MUL 4
#define AQL_OP_CLASS_OTHER 0



const int SEMTYPE_ERROR = 0;
const int SEMTYPE_INT = 1;
const int SEMTYPE_CIRC = 2;
const int SEMTYPE_CONSTRAINT = 3;
const int SEMTYPE_DOMAIN = 4;
const int SEMTYPE_STMT = 5;
const int SEMTYPE_PARAM = 6;
const int SEMTYPE_GATE = 7;
const int SEMTYPE_DOUBLE = 8;
const int SEMTYPE_FLOAT = 9;
const int SEMTYPE_SYMTAB = 10;


class aqcl_node 
{
  protected:

  int m_node_category;
  int m_vartype;
  aqcl_node * m_parent;
  bool m_is_leaf;
  string m_name;
  bool m_is_temp;
  bool m_active;
  int m_pos;

  public:

  aqcl_node ()
  {
    m_node_category = AQL_NODE_NODE;
    m_parent = NULL;
    m_is_leaf = true;
    m_active = true;
    m_pos = 0;
  }

  ~ aqcl_node ()
  {

  }

  int get_pos ()
  {
    return m_pos;
  }

  void activate ()
  {
    m_active = true;
  }

  void deactivate ()
  {
    m_active = false;
  }

  void set_parent (aqcl_node * p_parent)
  {
    m_parent = p_parent;
  }

  aqcl_node * get_parent ()
  {
    return m_parent;
  }

  bool has_parent ()
  {
    return m_parent != NULL;
  }

  int get_category ()
  {
    return m_node_category;
  }

  int get_semtype ()
  {
    return m_vartype;
  }

  string get_semtype_str ()
  {
    switch (m_vartype) {
      case SEMTYPE_INT : return "integer";
      case SEMTYPE_CIRC : return "circuit";
      case SEMTYPE_CONSTRAINT: return "constraint";
      case SEMTYPE_DOMAIN: return "domain";
      case SEMTYPE_STMT: return "statement";
      case SEMTYPE_PARAM: return "param";
      case SEMTYPE_FLOAT: return "float";
      case SEMTYPE_DOUBLE: return "double";
      default : return "<none>";
    }
  }

  bool is_same_category (int p_cat)
  {
    return m_node_category == p_cat;
  }

  void unset_leaf ()
  {
    m_is_leaf = false;
  }

  bool get_leaf ()
  {
    return m_is_leaf;
  }

  bool is_leaf ()
  {
    return m_is_leaf;
  }

  virtual string get_instruction_str () = 0;

  virtual string get_string () = 0;

  virtual aqcl_node * get_result () = 0;

  virtual string get_result_name () = 0;

  virtual string get_name () = 0;

  virtual bool is_param ()
  {
    return m_vartype == SEMTYPE_PARAM;
  }

  virtual bool is_circ ()
  {
    return m_vartype == SEMTYPE_CIRC;
  }

  virtual bool is_block ()
  {
    return false;
  }

  virtual bool is_int ()
  {
    return m_vartype == SEMTYPE_INT;
  }

  virtual bool is_val ()
  {
    return false;
  }

  virtual bool is_gate ()
  {
    return false;
  }

  virtual bool is_variable ()
  {
    return false;
  }

  virtual void set_temp ()
  {
    m_is_temp = true;
  }

  virtual bool get_temp ()
  {
    return m_is_temp;
  }

  virtual bool is_temp ()
  {
    return m_is_temp;
  }

  virtual int run ()
  {
    return 0;
  }

  virtual void print ()
  {

  }

  virtual bool is_stmt ()
  {
    return false;
  }

  virtual bool is_statement ()
  {
    return false;
  }

  virtual bool is_sexpr ()
  {
    return false;
  }
  
  virtual string get_expression_plain () = 0;

  virtual void canonicalize_dimensions (string oldname, string newname)
  {

  }

  virtual string get_class_name ()
  {
    return "aql_node";
  }
};

#endif
