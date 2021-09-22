/*
AQCSL: the Affine Quantum Circuit Specification Language
Filename: aql_print.hh
Creation date: Mon May 27 17:56:34 EDT 2019
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

#ifndef AQCL_PRINT
#define AQCL_PRINT

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include "aqcsl-ir.hh"
#include "aql_node.hh"
#include "aql_goperator.hh"
#include "aql_variable.hh"
#include "aql_block.hh"
#include "aql_statement.hh"

using namespace std;

class aqcl_print : public aqcl_goperator
{
  protected:
  aqcl_node * m_arg;

  public:

  aqcl_print (aqcl_node * p_arg) : aqcl_goperator (OP_PRINT)
  {
    m_arg = p_arg;
  }

  aqcl_print (aqcl_variable * p_arg) : aqcl_goperator (OP_PRINT)
  {
    m_arg = p_arg;
  }

  aqcl_print (aqcl_block * p_arg) : aqcl_goperator (OP_PRINT)
  {
    m_arg = p_arg;
  }

  aqcl_print (aqcl_statement * p_arg) : aqcl_goperator (OP_CODEGEN)
  {
    m_arg = p_arg;
  }

  aqcl_print (aqcl_statement * p_arg, int custom_op) : aqcl_goperator (custom_op)
  {
    m_arg = p_arg;
  }

  aqcl_print (int pcode) : aqcl_goperator (pcode)
  {
    m_arg = NULL;
  }

  ~ aqcl_print ()
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

  aqcl_statement * get_result_statement ()
  {
    return (aqcl_statement*)(m_arg); 
  }

  virtual string get_string () 
  {
    assert (0 && "Method generated from aql_print.hh. Must implement in new class.");
  }

  virtual aqcl_node * get_result ()
  {
    assert (0 && "Method generated from aql_print.hh. Must implement in new class.");
  }

  virtual string get_result_name ()
  {
    assert (0 && "Method generated from aql_print.hh. Must implement in new class.");
  }

  virtual string get_name ()
  {
    assert (0 && "Method generated from aql_print.hh. Must implement in new class.");
  }


  virtual string get_instruction_str ()
  {
    string ret = "";
    ret += get_operator_string ();
    ret += ", ";
    if (m_arg)
      ret += m_arg->get_name ();
    ret += ", N/A, N/A";
    return ret;
  }

  virtual int run ()
  {
    print ();    
    return 0;
  }

  virtual void print ()
  {
    m_arg->print ();
    cout << endl;
  }
};

#endif
