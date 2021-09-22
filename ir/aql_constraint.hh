/*
AQCSL: the Affine Quantum Circuit Specification Language
Filename: aql_constraint.hh
Creation date: Wed May 22 14:11:00 EDT 2019
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

#ifndef AQCL_CONSTRAINT
#define AQCL_CONSTRAINT

#include <vector>
#include <map>
#include <string>
#include <iostream>
//#include "aqcsl-ir.hh"
#include "aql_goperator.hh"
#include "aql_node.hh"

using namespace std;

class aqcl_constraint : public aqcl_goperator
{
  int m_opcode;
  aqcl_aoperator * m_a1;
  aqcl_aoperator * m_a2;

  public:

  aqcl_constraint (int p_opcode, aqcl_aoperator * p_left,
    aqcl_aoperator * p_right) : aqcl_goperator (p_opcode)
  {
    m_node_category = AQL_NODE_INEQ; //  SEMTYPE_CONSTRAINT;
    m_a1 = p_left;
    m_a2 = p_right;
    //m_opcode = p_opcode;
  }


  ~ aqcl_constraint ()
  {

  }

  int get_category ()
  {
    return m_node_category;
  }

  bool is_same_category (int p_cat)
  {
    return m_node_category == p_cat;
  }

/*
  virtual string get_operator_string ()
  {
    string op_str;
    switch (m_opcode)
    {
      case OP_LT: op_str = "<"; break;
      case OP_LEQ: op_str = "<="; break;
      case OP_GT: op_str = ">"; break;
      case OP_GEQ: op_str = ">="; break;
      default: op_str = "ERROR"; break;
    }
    return op_str;
  }
  */

  virtual string get_string () 
  {
    string op_str;
    assert (m_a1);
    assert (m_a2);

    op_str = get_operator_string ();
    return get_expression_plain ();
  }

  virtual string get_instruction_str ()
  {
    string op_str;
    assert (m_a2 && "aql_constraint::get_instruction_str () - right operand is null");
    assert (m_a1 && "aql_constraint::get_instruction_str () - left operand is null");

    //cout << "At aexpr::get_string () " << endl;

    op_str = get_operator_string ();

    string ret = "";
    ret += op_str + ", ";

    if (m_a1->is_leaf ())
      ret += m_a1->get_name ();
    else
      ret += m_a1->get_result_name ();
    ret += ", ";

    if (m_a2->is_leaf ())
      ret += m_a2->get_name ();
    else
      ret += m_a2->get_result_name ();

    return ret;
  }

  virtual string get_expression_plain ()
  {
    string op_str;
    assert (m_a1);
    assert (m_a2);
    op_str = get_operator_string ();
    string ret = "";
    ret += m_a1->get_expression_plain ();
    ret += " ";
    ret += op_str;
    ret += " ";
    ret += m_a2->get_expression_plain ();
    return ret;
  }


  void canonicalize_dimensions (string oldname, string newname)
  {
    if (m_a1)
      m_a1->canonicalize_dimensions (oldname, newname);
    if (m_a2)
      m_a2->canonicalize_dimensions (oldname, newname);
  }

  virtual aqcl_node * get_result ()
  {
    return NULL;
  }

  virtual string get_result_name ()
  {
    return "aql_constrain.hh";
  }

  virtual string get_name ()
  {
    return "aql_constrain.hh";
  }

  virtual void print ()
  {
    cout << "aql_constraint::print ()" << endl;
  }
};

#endif
