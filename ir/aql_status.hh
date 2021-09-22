/*
AQCSL: the Affine Quantum Circuit Specification Language
Filename: aql_status.hh
Creation date: Sat Jun  8 13:11:43 EDT 2019
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

#ifndef AQCL_status
#define AQCL_status

#include <vector>
#include <map>
#include <string>
#include <iostream>
//#include "aqcsl-ir.hh"
#include "aql_goperator.hh"

using namespace std;

class aqcl_status : public aqcl_goperator
{

  public:

  aqcl_status (int p_opcode) : aqcl_goperator (p_opcode)
  {
    m_node_category = AQL_NODE_OPERATOR;
  }

  ~ aqcl_status ()
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
    return "";
  }

  virtual aqcl_node * get_result ()
  {
    return NULL;
  }

  virtual string get_result_name ()
  {
    return "";
  }

  virtual string get_name ()
  {
    return "";
  }

  virtual void print ()
  {
    switch (m_opcode)
    {
      case OP_START_INT:
        cout << "starting ..." << endl;
        break;
      case OP_PAUSE_INT:
        cout << "pausing ..." << endl;
        break;
    }
  }

  virtual string get_instruction_str ()
  {
    switch (m_opcode)
    {
      case OP_START_INT:
        return "START, N/A, N/A, N/A";
      case OP_PAUSE_INT:
        return "PAUSE, N/A, N/A, N/A";
    }
    assert (0);
  }
    
    

};

#endif
