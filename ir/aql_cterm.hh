/*
AQCSL: the Affine Quantum Circuit Specification Language
Filename: aql_cterm.hh
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

#ifndef AQCL_CTERM
#define AQCL_CTERM

#include <vector>
#include <map>
#include <string>
#include <iostream>
//#include "aqcsl-ir.hh"
#include "aql_node.hh"
//#include "aql_gbinop.hh"
#include "aql_variable.hh"
#include "aql_block.hh"

using namespace std;

class aqcl_cterm : public aqcl_block 
{

  public:

  aqcl_cterm (int p_opcode, 
    aqcl_block * p_res, aqcl_block * p_left, aqcl_block * p_right) :
    aqcl_block (p_opcode, p_res, p_left, p_right)
  {
    m_node_category = AQL_NODE_CIRC;
  }

  ~ aqcl_cterm ()
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
      case OP_ITER_S: op_str = "[*]"; break;
      case OP_ITER_T: op_str = "(*)"; break;
      default: op_str = "ERROR"; assert (0 && "aql_cterm.hh: invalid operator"); break;
    }

    return m_left->get_string () + " " + op_str + " " + m_right->get_string (); 

  }
  */

  virtual string get_instruction_str ()
  {
    string op_str;
    if (m_a1 && m_a2)
    {
      switch (m_opcode)
      {
        case OP_ITER_S: op_str = "[*]"; break;
        case OP_ITER_T: op_str = "(*)"; break;
        default: op_str = "ERROR"; break;
      }

      string ret = "";
      ret += op_str + ", ";
      ret += m_res->get_name () + ", ";

      if (m_a1->is_leaf ())
        ret += m_a1->get_name ();
      else
        ret += m_a1->get_result_name ();
      ret += ", ";

      if (m_a2->is_leaf ())
        ret += m_a2->get_name ();
      else
        ret += m_a2->get_result_name ();
      ret += " // aql_cterm";
      return ret;
    }
    else
    {
      assert (0);
    }
  }

  int get_category ()
  {
    return m_node_category;
  }

  virtual bool is_pure_block ()
  {
    return false;
  }

  virtual bool is_cterm ()
  {
    return true;
  }

  bool is_same_category (int p_cat)
  {
    return m_node_category == p_cat;
  }

  virtual int get_dim ()
  {
    return m_res->get_dim ();
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

  virtual int run ()
  {
    if (not m_active)
      return 0;
    switch (m_opcode)
    {
      case OP_ITER_T:
      case OP_ITER_S:
        aqcl_variable * scalar = (aqcl_variable *)(m_a2->get_result ());
        int factor = scalar->get_val ()->get_val ();
        //res->set_val  (left->get_val ()->get_val () - right->get_val ()->get_val ());
        //m_res->comp_space (m_a1,m_a2);
        if (m_opcode == OP_ITER_T)
          m_res->time_scaling (m_a1, factor);
        if (m_opcode == OP_ITER_S)
          m_res->space_scaling (m_a1, factor);
        break;
    }
    //m_a1->print ();
    m_res->print ();
    cout << endl;
    return 0;
  }

};

#endif
