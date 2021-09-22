/*
AQCSL: the Affine Quantum Circuit Specification Language
Filename: aql_cfact.hh
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

#ifndef AQCL_CFACT
#define AQCL_CFACT

#include <vector>
#include <map>
#include <string>
#include <iostream>
//#include "aqcsl-ir.hh"
#include "aql_gate.hh"
#include "aql_variable.hh"
#include "aql_block.hh"
#include "aql_statement.hh"

using namespace std;

class aqcl_cfact : public aqcl_block
{
  protected:
  aqcl_gate     * m_src_gate;

  public:

  aqcl_cfact () : aqcl_block ()
  {
    m_node_category = AQL_NODE_CIRC;
    assert (0);
  }

  aqcl_cfact (int p_opcode, aqcl_block * p_res, aqcl_gate * p_gate) :
    aqcl_block (p_opcode, p_res, p_gate) 
  {
    // set m_body inherited from  class block
    m_src_gate  = NULL;
  }

  // Used for CSTORE and CLOAD
  aqcl_cfact (int p_opcode, aqcl_block * p_res, aqcl_block * p_block) :
    aqcl_block (p_opcode, p_res, p_block)
  {
    m_src_gate = NULL;
    assert (m_res);
    //m_res->check_subcircuits ();
  }

  aqcl_cfact (int p_opcode, aqcl_statement * p_dst, aqcl_statement * p_src) :
    aqcl_block (p_opcode, p_dst, p_src)
  {
    m_src_gate = NULL;
    assert (m_res);
    //m_res->check_subcircuits ();
  }


  ~ aqcl_cfact ()
  {

  }

  virtual string get_string ()
  {
    if (m_src_gate)
      return m_src_gate->get_string ();
    if (m_a2)
      return m_a2->get_string ();
    return "aqcl_cfact::get_string ()";
  }

  int get_category ()
  {
    return m_node_category;
  }

  bool is_same_category (int p_cat)
  {
    return m_node_category == p_cat;
  }

  virtual bool is_pure_block ()
  {
    return false;
  }

  virtual bool is_cfact ()
  {
    return true;
  }

  virtual string get_instruction_str ()
  {
    string ret = "";
    ret += get_operator_string ();
    ret += ", ";
    if (m_res)
      ret += m_res->get_name ();
    ret += ", N/A, ";
    if (m_body)
      ret += m_body->get_name ();
    if (m_a1)
      ret += m_a1->get_name ();
    if (m_a2)
      ret += m_a2->get_name ();
    ret += " // aql_cfact";
    return ret;
  }


  virtual void print ()
  {
    if (m_offset > 0)
      cout << m_offset << "|";
    
    if (m_a2)
    {
      //cout << "acl_cfact::run () - m_a2" << endl;
      m_a2->print ();
    }
    if (m_src_gate)
    {
      //cout << "acl_cfact::run () - src_gate " << endl;
      m_src_gate->print ();
    }
    if (m_body)
    {
      m_body->print ();
    }
    if (m_padding > 0)
      cout << "|" << m_padding;
  }

  virtual int get_dim ()
  {
    return m_res->get_dim ();
  }


  virtual string get_expression_plain ()
  {
    string op_str;
    op_str = get_operator_string ();
    string ret = "";
    if (m_opcode == OP_G2C)
    {
      if (m_src_gate)
        return m_src_gate->get_expression_plain ();
      if (m_a2)
        return m_a2->get_expression_plain ();
      if (m_body)
        return m_body->get_expression_plain ();
      assert (0);
    }
    if (m_opcode == OP_CLOAD)
    {
      if (m_a2)
        return m_a2->get_expression_plain ();
      if (m_body)
        return m_body->get_expression_plain ();
      assert (0);
    }
    if (m_opcode == OP_CSTORE)
    {
      ret += m_res->get_expression_plain ();
      ret += " ";
      ret += " := ";
      ret += " ";
      ret += m_a2->get_expression_plain ();
      return ret;
    }
    assert (0);
  }

  virtual int run ()
  {
    if (not m_active)
      return 0;
    switch (m_opcode)
    {
      case OP_SLOAD:
        {
          assert (m_res);
          assert (m_a2);
          if (m_a2->is_stmt ())
          {
            aqcl_statement * src = (aqcl_statement*)(m_a2);
            aqcl_statement * dst = (aqcl_statement*)(m_res);
            m_res->set_result (m_a2->get_result_block ());
            //cout << "Domain from the source: "; src->print_domain ();
            dst->set_domain (src->get_domain ());
            dst->copy_isl_objects (src);
            dst->rename_domains_of_isl_objects ();
            //cout << "Dst is STATEMENT: " << dst->get_name () << " of type " << dst->get_semtype () << endl;
          }
        }
        break;
      case OP_CLOAD:
        if (m_src_gate)
        {
          assert (!m_body);
          m_body = new aqcl_gate ();
          *m_body = *m_src_gate;
          //cout << "--> " << m_body->get_string () << endl;
        }
        if (m_a2)
        {
          assert (m_res);
          assert (m_a2);
          {
            m_res->shallow_copy (*m_a2);
          }

        }
        break;
      case OP_CSTORE:
        if (m_a2)
        {
          assert (m_res);
          assert (m_a2);
          assert (m_res->is_circ ());
          m_res->shallow_copy (*m_a2);
        }

        break;
      case OP_G2C:
        assert (m_res);
        assert (m_body);
        m_res->set_body (m_body); // copy pointer from this to block m_res->m_body
        cout << m_res->get_name () << " -> " << m_res->get_body()->get_name () << endl;
        break;
      default:

        assert (0 && "Unexpected operation in aql_cfact::run ()");
    }
    m_res->print ();
    cout << endl;
    return 0;
  }
};

#endif
