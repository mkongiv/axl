/*
AQCSL: the Affine Quantum Circuit Specification Language
Filename: aql_aoperator.hh
Creation date: Mon May 27 19:55:24 EDT 2019
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

#ifndef AQCL_AOPERATOR
#define AQCL_AOPERATOR

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include "aql_node.hh"
#include "aql_goperator.hh"
#include "aql_variable.hh"

using namespace std;

class aqcl_aoperator : public aqcl_goperator
{
  protected:
  aqcl_node * m_res;
  aqcl_node * m_a1;
  aqcl_node * m_a2;

  public:

  aqcl_aoperator (int p_opcode) : aqcl_goperator (p_opcode)
  {
    m_node_category =  AQL_NODE_ARIT ;
    m_vartype = SEMTYPE_INT;
  }

  ~ aqcl_aoperator ()
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

  virtual string get_string () 
  {
    string ret = "";
    string op_str = get_operator_string ();
    if (m_res && m_a2)
    {
      ret += op_str;
      ret += " ";
      ret += m_a2->get_string ();
      return ret;
    }
    if (m_res && m_a1 && m_a2)
    {
      ret += m_a1->get_string ();
      ret += " ";
      ret += op_str;
      ret += " ";
      ret += m_a2->get_string ();
      return ret;
    }
    if (m_res && !m_a1 && !m_a2)
    {
      //if (m_block)
      // what opcodes do we expect here?
      {
        ret += m_res->get_name () + "_[#] ";
        return ret;
      }
      assert (0);
    }
    return "aqcl_aoperator::get_string () -  Unxepcted case";
  }

  virtual aqcl_node * get_result ()
  {
    return m_res;
  }

  virtual string get_result_name ()
  {
    return m_res->get_name ();
  }

  virtual string get_name ()
  {
    if (m_res)
      return m_res->get_name ();
    else
      return "No Name found for result of " + get_operator_string ();
  }

  virtual bool is_variable ()
  {
    return false;
  }

  virtual string get_instruction_str ()
  {
    string op_str;
    op_str = get_operator_string ();

    if (m_res && m_a1 && m_a2)
    {

      string ret = "";
      ret += op_str + " , ";
      ret += m_res->get_name ();
      ret += ", ";

      if (m_a1->is_leaf ())
        ret += m_a1->get_name ();
      else
        ret += m_a1->get_result_name ();
      ret += ", ";

      if (m_a2->is_leaf ())
        ret += m_a2->get_name ();
      else
        ret += m_a2->get_result_name ();

      ret += " // aql_aoperator";

      return ret;
    }

    if (m_res && !m_a1 && m_a2)
    {
      //cout << "At aexpr::get_string () " << endl;

      string ret = "";
      ret += op_str + " , ";
      ret += m_res->get_name () + ", ";

      ret += " N/A, ";

      if (m_a2->is_leaf ())
        ret += m_a2->get_name ();
      else
        ret += m_a2->get_result_name ();

      ret += " // aql_aoperator";

      return ret;
    }

    if (m_res && !m_a1 && !m_a2)
    {
      string ret = "";
      ret += op_str + " , ";
      ret += m_res->get_name ();
      if (m_res->is_variable ())
      {
        aqcl_variable * var = (aqcl_variable *)(m_res);
        if (var->has_payload ())
        {
          #if 0
          ret += ", N/A, ";
          aqcl_block * a_block = (aqcl_block*)(var->get_block ());
          ret += a_block->get_result_name ();//var->get_block()->get_name ();
          #endif
        }
        else
          ret += ", N/A, N/A";
      }
      return ret;
    }


    assert (0 && "aqcl_aoperator::get_instruction_str: unexpected case");
  }


  virtual string get_expression_plain ()
  {
    string op_str;
    op_str = get_operator_string ();
    string ret = "";
    string str1;
    string str2;
    if (m_a1)
    {
      if (m_a1->is_variable () or m_a1->is_param ())
      {
        aqcl_variable * var = (aqcl_variable *)(m_a1);
        str1 = var->get_name ();
      }
      if (m_a1->is_val ())
      {
        aqcl_val* val = (aqcl_val*)(m_a1);
        str1 = val->get_string ();
      }
      assert (0 && "aql_aoperator::get_expression_plain (): Could not parse left argument.");
    }
    if (m_a2)
    {
      if (m_a2->is_variable () or m_a2->is_param ())
      {
        aqcl_variable * var = (aqcl_variable *)(m_a2);
        str2 = var->get_name ();
      }
      if (m_a2->is_val ())
      {
        aqcl_val* val = (aqcl_val*)(m_a2);
        str2 = val->get_string ();
      }
      assert (0 && "aql_aoperator::get_expression_plain (): Could not parse left argument.");
    }
    if (m_a1 && m_a2)
      return str1 + " " + op_str + " " + str2;
    if (!m_a1 && m_a2)
      return str2;
    if (m_a1 && !m_a2)
      return str1;
    assert (0);
    return "ERROR";
  }

  virtual void print ()
  {
    cout << "aql_aoperator::print ()" << endl;
  }

  void canonicalize_dimensions (string oldname, string newname)
  {
    if (m_res && m_res->is_variable () && m_res->get_name () == oldname)
    {
      aqcl_variable * oldvar = (aqcl_variable *)(m_res);
      aqcl_variable * newvar = new aqcl_variable (newname);
      m_res = newvar;
      //->canonicalize_dimensions (oldname, newname);
    }
    if (m_a1 && m_a1->is_variable () && m_a1->get_name () == oldname)
    {
      aqcl_variable * oldvar = (aqcl_variable *)(m_a1);
      aqcl_variable * newvar = new aqcl_variable (newname);
      m_a1 = newvar;
      //var->canonicalize_dimensions (oldname, newname);
    }
    if (m_a2 && m_a2->is_variable () && m_a2->get_name () == oldname)
    {
      aqcl_variable * oldvar = (aqcl_variable *)(m_a2);
      aqcl_variable * newvar = new aqcl_variable (newname);
      //var->canonicalize_dimensions (oldname, newname);
      m_a2 = newvar;
    }
  }
};

#endif
