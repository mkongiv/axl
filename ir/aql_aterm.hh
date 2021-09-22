/*
AQCSL: the Affine Quantum Circuit Specification Language
Filename: aql_aterm.hh
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

#ifndef AQCL_ATERM
#define AQCL_ATERM

#include <vector>
#include <map>
#include <string>
#include <iostream>
//#include "aqcsl-ir.hh"
#include "aql_aoperator.hh"
#include "aql_node.hh"
#include "aql_variable.hh"

using namespace std;

class aqcl_aterm : public aqcl_aoperator
{

  public:

  aqcl_aterm (int p_opcode, 
    aqcl_node * p_res, aqcl_node * p_a1, aqcl_node * p_a2):
    aqcl_aoperator (p_opcode)
  {
    m_node_category = AQL_NODE_ARIT;
    m_res = p_res;
    m_a1 = p_a1;
    m_a2 = p_a2;
  }


  ~ aqcl_aterm ()
  {

  }

/*
  virtual string get_string ()
  {
    string op_str;
    assert (m_left);
    assert (m_right);

    switch (m_opcode)
    {
      default: op_str = "ERROR"; break;
    }

    return m_left->get_string () + " " + op_str + " " + m_right->get_string (); 
  }
  */

  int get_category ()
  {
    return m_node_category;
  }

  bool is_same_category (int p_cat)
  {
    return m_node_category == p_cat;
  }

  virtual int run ()
  {
    aqcl_variable * left  = (aqcl_variable*)(((aqcl_aterm*)m_a1)->get_result ());
    aqcl_variable * right = (aqcl_variable*)(((aqcl_aterm*)m_a2)->get_result ());
    aqcl_variable * res = (aqcl_variable*)(((aqcl_aterm*)m_res)->get_result ());
    switch (m_opcode)
    {
      case OP_MUL:
        //cout << this->get_instruction_str () << endl;
        //cout << res->get_val ()->get_val () << endl;
        res->set_val  (left->get_val ()->get_val () * right->get_val ()->get_val ());
        break;
      case OP_DIV:
        res->set_val  (left->get_val ()->get_val () / right->get_val ()->get_val ());
        break;
    }
    return 0;
  }

  virtual string get_expression_plain ()
  {
    return m_a1->get_expression_plain () + " " +
      get_operator_string () + " " + 
      m_a2->get_expression_plain ();
  }
};

#endif
