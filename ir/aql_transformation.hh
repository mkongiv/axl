/*
AQCSL: the Affine Quantum Circuit Specification Language
Filename: aql_transformation.hh
Creation date: Fri Jun 21 14:18:33 EDT 2019
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

#ifndef AQCL_transformation
#define AQCL_transformation

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include "aql_node.hh"
#include <vector>

#include "aql_node.hh"
#include "aql_goperator.hh"
#include "aql_variable.hh"
#include "aql_block.hh"
#include "aql_statement.hh"

using namespace std;

class aqcl_transformation : public aqcl_goperator
{
  aqcl_statement * m_arg;
  int m_transfo_code;
  vector<aqcl_statement*> * m_stmts;
  int m_solver;

  public:

  aqcl_transformation (aqcl_statement * p_arg, int p_transfo_code) : aqcl_goperator (OP_APPLY)
  {
    m_arg = p_arg;
    m_stmts = NULL;
    m_transfo_code = p_transfo_code;
  }

  ~ aqcl_transformation ()
  {

  }

  void set_solver (int p_solver)
  {
    m_solver = p_solver;
  }

  int get_solver ()
  {
    return m_solver;
  }

  void set_statement_vector (vector<aqcl_statement*> * p_stmts)
  {
    m_stmts = p_stmts;
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
    assert (0 && "Method generated from aql_transformation.hh. Must implement in new class.");
  }

  virtual aqcl_node * get_result ()
  {
    assert (0 && "aql_transformation.hh: do not use this method.");
  }

  aqcl_statement * get_result_stmt ()
  {
    return m_arg;
  }

  virtual string get_result_name ()
  {
    assert (m_arg && "aql_transformation.hh: Argument is null.");
    return m_arg->get_name ();
  }

  virtual string get_name ()
  {
    assert (0 && "Method generated from aql_transformation.hh. Must implement in new class.");
  }

  int get_transfo_code ()
  {
    return m_transfo_code;
  }

  virtual string get_instruction_str ()
  {
    string ret = get_operator_string ();
    ret += ", ";
    ret += m_arg->get_name ();
    ret += ", N/A, ";
    switch (m_transfo_code)
    {
      case AXL_TRANSFO_PLUTO:
        ret += "PLUTO";
        break;
      case AXL_TRANSFO_FEAUTRIER:
        ret += "FEAUTRIER";
        break;
      case AXL_TRANSFO_ISS_STAIRS:
        ret += "ISS_STAIRS";
        break;
      case AXL_TRANSFO_REVERSE_TIME:
        ret += "REVERSE_TIME";
        break;
      case AXL_TRANSFO_PLUTO_MINFUSE:
        ret += "PLUTO-MINFUSE";
        break;
      case AXL_TRANSFO_PLUTO_MAXFUSE:
        ret += "PLUTO-MAXFUSE";
        break;
      default:
        assert (0 && "aql_transformation.hh: Unexpected transformation.");
        break;
    }
    return ret;
  }

  virtual string get_expression_plain ()
  {
    assert (0 && "Pending implementation");
    return "";
  }

  virtual void print ()
  {
    cout << "Calling aql_transformation::print ()" << endl;
  }

  virtual int run ()
  {
    int ret = 1;
    switch (m_transfo_code)
    {
      case AXL_TRANSFO_PLUTO:
        ret = m_arg->apply_pluto ();
        break;
      case AXL_TRANSFO_PLUTO_MINFUSE:
        ret = m_arg->apply_pluto_minfuse ();
        break;
      case AXL_TRANSFO_PLUTO_MAXFUSE:
        ret = m_arg->apply_pluto_maxfuse ();
        break;
      case AXL_TRANSFO_FEAUTRIER:
        ret = m_arg->apply_feautrier ();
        break;
      case AXL_TRANSFO_ISS_STAIRS:
        ret = m_arg->apply_iss_stairs ();
        break;
      case AXL_TRANSFO_REVERSE_TIME:
        ret = m_arg->apply_reverse_time ();
        break;
      default:
        assert (0 && "aql_transformation.hh: Unexpected transformation.");
        break;
    }
    return ret;
  }
};

#endif
