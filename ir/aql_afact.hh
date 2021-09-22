/*
AQCSL: the Affine Quantum Circuit Specification Language
Filename: aql_afact.hh
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

#ifndef AQCL_AFACT
#define AQCL_AFACT

#include <vector>
#include <map>
#include <string>
#include <iostream>
//#include "aqcsl-ir.hh"
#include "aql_node.hh"
#include "aql_aoperator.hh"
#include "aql_variable.hh"

using namespace std;

class aqcl_afact : public aqcl_aoperator
{

  public:

  aqcl_afact (int p_opcode, 
    aqcl_node * p_res, aqcl_node * p_a2):
    aqcl_aoperator (p_opcode)
  {
    m_node_category = AQL_NODE_ARIT;
    m_res = p_res;
    m_a1 = NULL;
    m_a2 = p_a2;
  }


  aqcl_afact (int p_opcode, aqcl_node * p_res) :
    aqcl_aoperator (p_opcode)
  {
    m_node_category = AQL_NODE_ARIT;
    m_res = p_res;
    m_a1 = NULL;
    m_a2 = NULL;
  }


  ~ aqcl_afact ()
  {

  }

/*
  virtual string get_string ()
  {

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

  virtual string get_expression_plain ()
  {
    if (m_opcode == OP_ALOAD)
    {
      if (m_a2->is_val ())
      {
        aqcl_val * val = (aqcl_val *)(m_a2);
        return val->get_string ();
      }
      if (m_a2->is_param () or m_a2->is_variable ())
      {
        aqcl_variable * var = (aqcl_variable*)(m_a2);
        return var->get_name ();
      }
    }
    if (m_opcode == OP_UMIN)
    {
      if (m_a2->is_val ())
      {
        aqcl_val * val = (aqcl_val *)(m_a2);
        return "-" + val->get_string ();
      }
      if (m_a2->is_variable ())
      {
        aqcl_variable * var = (aqcl_variable*)(m_a2);
        if (var->is_param ())
          return "-" + var->get_name ();
        if (not var->is_temp ())
          return "-" + var->get_name ();
        return "-" + var->get_val ()->get_string ();
      }
      return "-" + m_a2->get_expression_plain ();
    }
    return "aql_afact::get_expression_plain ()";
  }




  virtual int run ()
  {
    aqcl_variable * dst = (aqcl_variable*)m_res;
    switch (m_opcode)
    {
      case OP_ASTORE:
      case OP_ALOAD:
        if (m_a2->is_val ())
        {
          aqcl_val * src = (aqcl_val*)m_a2;
          assert (src);
          assert (dst);
          dst->load_val (src);
        }
        else if (m_a2->is_variable ())
        {
          aqcl_variable * src = (aqcl_variable*)(m_a2);
          dst->load_val (src);
        }
        else 
        {
          assert (0);
        }
        break;
      case OP_UMIN:
        if (m_a2->is_variable ())
        {
          aqcl_variable * src = (aqcl_variable*)(m_a2);
          dst->load_val_neg (src);
        }
        else
        {
          assert (0);
        }
    }
    return 0;
  }
};

#endif
