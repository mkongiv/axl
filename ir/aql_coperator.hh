/*
AQCSL: the Affine Quantum Circuit Specification Language
Filename: aql_coperator.hh
Creation date: Mon May 27 19:55:34 EDT 2019
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

#ifndef AQCL_COPERATOR
#define AQCL_COPERATOR

#include <vector>
#include <map>
#include <string>
#include <iostream>
//#include "aqcsl-ir.hh"
#include "aql_node.hh"
#include "aql_goperator.hh"

using namespace std;

class aqcl_coperator : public aqcl_goperator
{
  protected:
  aqcl_block * m_res;

  public:

  aqcl_coperator (int p_opcode) : aqcl_goperator (p_opcode)
  {
    //m_node_category =  DEFINE_ME;
  }

  ~ aqcl_coperator ()
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
    assert (0 && "Method generated from aql_coperator.hh. Must implement in new class.");
  }

  virtual aqcl_node * get_result ()
  {
    assert (0 && "Method generated from aql_coperator.hh. Must implement in new class.");
  }

  virtual string get_result_name ()
  {
    assert (0 && "Method generated from aql_coperator.hh. Must implement in new class.");
  }

  virtual string get_name ()
  {
    assert (0 && "Method generated from aql_coperator.hh. Must implement in new class.");
  }

};

#endif
