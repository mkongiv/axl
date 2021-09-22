/*
AQCSL: the Affine Quantum Circuit Specification Language
Filename: aql_sexpr.hh
Creation date: Fri Jun 14 08:51:11 EDT 2019
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

#ifndef AQCL_sexpr
#define AQCL_sexpr

#include <vector>
#include <map>
#include <string>
#include <iostream>
//#include "aqcsl-ir.hh"
#include "aql_node.hh"
#include "aql_goperator.hh"
#include "aql_statement.hh"
#include "aql_block.hh"

using namespace std;

class aqcl_sexpr : public aqcl_goperator
{
  aqcl_statement * m_a1;
  aqcl_statement * m_a2;
  aqcl_statement * m_res;
  int m_op_num;

  public:

  aqcl_sexpr (int p_opcode, aqcl_statement * p_res,
    aqcl_statement * p_a1, aqcl_statement * p_a2, int p_op_num) : aqcl_goperator (p_opcode)
  {
    m_opcode = p_opcode;
    m_res = p_res;
    m_a1 = p_a1;
    m_a2 = p_a2;
    m_op_num = p_op_num;
    m_vartype = SEMTYPE_STMT;
  }

  ~ aqcl_sexpr ()
  {

  }

  aqcl_statement * get_result_stmt ()
  {
    return m_res;
  }

  aqcl_statement * get_a1 ()
  {
    return  m_a1;
  }

  aqcl_statement * get_a2 ()
  {
    return m_a2;
  }

  int get_category ()
  {
    return m_node_category;
  }

  bool is_same_category (int p_cat)
  {
    return m_node_category == p_cat;
  }

  virtual string get_string () 
  {
    return get_expression_plain ();
  }

  virtual aqcl_node * get_result ()
  {
    return m_res;
  }

  virtual string get_result_name ()
  {
    if (m_res)
      return m_res->get_name ();
    return "result_is_null";
  }

  virtual string get_name ()
  {
    return m_name;
  }

  virtual string get_instruction_str ()
  {
    string op_str = get_operator_string ();

    string ret = "";
    ret += op_str + ", ";
    ret += m_res->get_name () + ", ";

    if (!m_a1)
      ret += "N/A";
    else if (m_a1->is_leaf ())
      ret += m_a1->get_name ();
    else
      ret += m_a1->get_result_name ();
    ret += ", ";

    if (m_a2->is_leaf ())
      ret += m_a2->get_name ();
    else
      ret += m_a2->get_result_name ();
    ret += " // aql_sexpr";
    return ret;
  }

  virtual string get_expression_plain ()
  {
    string op_str;
    if (m_a1 && m_a2)
    {
      op_str = get_operator_string ();
      string ret = "";
      ret += m_a1->get_expression_plain ();
      ret += " ";
      ret += op_str;
      ret += " ";
      ret += m_a2->get_expression_plain ();
      return ret;
    }
    if (!m_a1 && m_a2)
    {
      op_str = get_operator_string ();
      string ret = "";
      //ret += m_a1->get_expression_plain ();
      //ret += " ";
      //ret += op_str;
      //ret += " ";
      ret += m_a2->get_expression_plain ();
      return ret;
    }
    assert (0 && "aql_sexpr.hh: Unexpected expression structure.");
  }

  virtual void print ()
  {
    if (m_res)
      m_res->print ();
    if (m_a1)
      m_a1->print ();
    if (m_a2)
      m_a2->print ();
  }

  virtual int run ()
  {
    if (not m_active)
      return 0;
    switch (m_opcode)
    {
      case OP_SLOAD:
        //aqcl_statement * src = (aqcl_statement*)(m_a2);
        //aqcl_statement * dst = (aqcl_statement*)(m_res);
        m_res->set_result (m_a2->get_result_block ());
        m_res->set_domain (m_a2->get_domain ());
        m_res->copy_isl_objects (m_a2);
        m_res->init_scop_stmts ();
        break;
      case OP_COMP_T:
      case OP_COMP_S:
        {
          /*
           * Note: we check the type of expression (circuit/block or statement)
           * with get_result_block () because depending on the underlying node,
           * we may have a source gate or a source block.
           */
          assert (m_res && "aql_sexpr.hh: Expected non-null statement for result");
          assert (m_a1 && "aql_sexpr.hh: Expected non-null statement for operand-1");
          assert (m_a2 && "aql_sexpr.hh: Expected non-null statement for operand-2");

          m_res->schedule_composition (m_a1, m_a2, m_op_num);
        }
        break;
      default:
        assert (0 && "aql_sexpr.hh: Unexpected operation for expressions of type <statement>");
        break;
    }
    return 0;
  }

  virtual string get_class_name ()
  {
    return "aql_sexpr";
  }

  virtual bool is_sexpr ()
  {
    return true;
  }

};

#endif
