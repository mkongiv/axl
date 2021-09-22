/*
AQCSL: the Affine Quantum Circuit Specification Language
Filename: aql_cexpr.hh
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

#ifndef AQCL_CEXPR
#define AQCL_CEXPR

#include <vector>
#include <map>
#include <string>
#include <iostream>
//#include "aqcsl-ir.hh"
#include "aql_node.hh"
#include "aql_block.hh"
#include "aql_statement.hh"

using namespace std;

class aqcl_cexpr    : public aqcl_block
{

  public:

  aqcl_cexpr (int p_opcode, 
    aqcl_block * p_res, aqcl_block * p_a1, aqcl_block * p_a2) :
    aqcl_block (p_opcode, p_res, p_a1, p_a2)
  {
    m_node_category = AQL_NODE_CIRC;
  }

  ~ aqcl_cexpr ()
  {

  }

  virtual string get_instruction_str ()
  {
    string op_str;
    if (m_a1 && m_a2)
    {
      switch (m_opcode)
      {
        case OP_COMP_S: op_str = "[+]"; break;
        case OP_COMP_T: op_str = "(+)"; break;
        case OP_PIPE_PRE: op_str = "|pre"; break;
        case OP_PIPE_SUF: op_str = "|suf"; break;
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
      ret += " // aql_cexpr";
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

  virtual bool is_cexpr ()
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
    /*
    aqcl_variable * left  = (aqcl_variable*)(((aqcl_aexpr*)m_a1)->get_result ());
    aqcl_variable * right = (aqcl_variable*)(((aqcl_aexpr*)m_a2)->get_result ());
    aqcl_variable * res = (aqcl_variable*)(((aqcl_aexpr*)m_res)->get_result ());
    */
    if (not m_active)
      return 0;
    switch (m_opcode)
    {
      case OP_COMP_T:
        if (m_a1->get_result_block ()->is_block () and
            m_a2->get_result_block ()->is_block ())
        {
          int a1_dim = m_a1->get_dim ();
          int a2_dim = m_a2->get_dim ();
          if (a1_dim != a2_dim and m_check)
          {
            cout << "ERROR: Time composition of circuits requires them to have the same dimensionality." << endl;
            string lhs_expr = m_a1->get_expression_plain ();
            string rhs_expr = m_a2->get_expression_plain (); 
            cout << "Offending circuit-expression: " << this->get_expression_plain () << endl;
            cout << lhs_expr << " dimensionality: " << a1_dim << endl;
            cout << rhs_expr << " dimensionality: " << a2_dim << endl;
            return 1;
          }
          m_res->comp_time (m_a1,m_a2);
          //m_res->print ();
        }
        else 
        {
          assert (0 && "aqcl_cexpr.hh: Unexpected operand types.");
        }
        break;
      case OP_COMP_S:
        if (m_a1->get_result_block ()->is_block () and
            m_a2->get_result_block ()->is_block ())
        {
          m_res->comp_space (m_a1,m_a2);
        }
        else
        {
          assert (0 && "aqcl_cexpr.hh: Unexpected operand types.");
        }
        break;
      case OP_PIPE_PRE:
      case OP_PIPE_SUF:
        assert (m_a1->is_block ());
        assert (m_a2->is_int ());
        aqcl_variable * scalar = (aqcl_variable *)(m_a2->get_result ());
        int delta = scalar->get_val ()->get_val ();
        m_res->shallow_copy (*m_a1);
        if (m_opcode == OP_PIPE_PRE && delta > 0)
          m_res->set_offset (delta);
        if (m_opcode == OP_PIPE_SUF && delta > 0)
          m_res->set_padding (delta);
        break;
    }
    m_a1->print ();
    m_res->print ();
    return 0;
  }

};

#endif
