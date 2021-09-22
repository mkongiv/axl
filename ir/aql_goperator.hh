/*
AQCSL: the Affine Quantum Circuit Specification Language
Filename: aql_goperator.hh
Creation date: Thu May 16 13:25:29 EDT 2019
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

#ifndef AQCL_GOPERATOR
#define AQCL_GOPERATOR

#include <vector>
#include <map>
#include <string>
#include <iostream>
//#include "aqcsl-ir.hh"
#include "aql_node.hh"
#include "aql_variable.hh"

using namespace std;


// Binary arithmetic operators
const static int OP_ADD = 1;
const static int OP_SUB = 2;
const static int OP_MUL = 3;
const static int OP_DIV = 4;

// Relational operators, produce no result
const static int OP_LT = 10;
const static int OP_LEQ = 11;
const static int OP_GT = 12;
const static int OP_GEQ = 13;
const static int OP_EQ = 14;

// Unary operators
const static int OP_MOV = 5;
const static int OP_UMIN = 6;
const static int OP_GATE = 7;

// Binary circuit operators
const static int OP_COMP_T = 32;
const static int OP_COMP_S = 33;
const static int OP_ITER_T = 34;
const static int OP_ITER_S = 35;
const static int OP_G2C    = 36;
const static int OP_PIPE_PRE = 37;
const static int OP_PIPE_SUF = 38;

// Domain attaching
const static int OP_ATCH_DOM_B = 64;
const static int OP_ATCH_DOM_T = 65;
const static int OP_ATCH_DOM_S = 66;


const static int OP_PRINT = 80;
const static int OP_CLOAD  = 81;
const static int OP_CSTORE = 82;
const static int OP_ALOAD  = 83;
const static int OP_ASTORE = 84;
const static int OP_WRAP  = 85;
const static int OP_START_INT = 86;
const static int OP_PAUSE_INT = 87;
const static int OP_CSTMT = 88;
const static int OP_GENSTART = 89;
const static int OP_CODEGEN = 90;
const static int OP_SLOAD = 91;
const static int OP_APPLY = 92;
const static int OP_REVERSE = 93;

const static int OP_EXPORT_OSL = 200;
const static int OP_EXPORT_SCOP = 201;


class aqcl_goperator : public aqcl_node
{
  protected:
  int m_opcode;
  int m_op_class;
  bool m_check;
  //aqcl_variable * m_res;
  int m_stmt_id_offset;

  public:

  aqcl_goperator () : aqcl_node ()
  {
    m_node_category =  AQL_NODE_OPERATOR;
  }

  aqcl_goperator (int p_opcode) : aqcl_node ()
  {
    m_node_category =  AQL_NODE_OPERATOR;
    m_opcode = p_opcode;
    m_op_class = AQL_OP_CLASS_OTHER;
    m_check = true;
    unset_leaf ();
  }


  ~ aqcl_goperator ()
  {

  }


  int get_category ()
  {
    return m_node_category;
  }

  void set_opcode (int p_opcode)
  {
    m_opcode = p_opcode;
  }


  int get_opcode ()
  {
    return m_opcode;
  }

  virtual string get_operator_string ()
  {
    string op_str;
    switch (m_opcode)
    {
      case OP_ADD: op_str = "+"; break;
      case OP_SUB: op_str = "-"; break;
      case OP_MUL: op_str = "*"; break;
      case OP_DIV: op_str = "/"; break;
      case OP_COMP_S: op_str = "[+]"; break;
      case OP_COMP_T: op_str = "(+)"; break;
      case OP_ITER_S: op_str = "[*]"; break;
      case OP_ITER_T: op_str = "(*)"; break;
      case OP_UMIN: op_str = "UMIN"; break;
      case OP_MOV: op_str = ":="; break;
      case OP_LT: op_str = "<"; break;
      case OP_LEQ: op_str = "<="; break;
      case OP_GT: op_str = ">"; break;
      case OP_GEQ: op_str = ">="; break;
      case OP_EQ: op_str = "="; break;
      case OP_ATCH_DOM_B: op_str = "%"; break;
      case OP_ATCH_DOM_T: op_str = "(%)"; break;
      case OP_ATCH_DOM_S: op_str = "[%]"; break;
      case OP_G2C: op_str = "#^[]"; break;
      case OP_PRINT: op_str = "PRT"; break;
      case OP_CLOAD: op_str = "C-LOAD"; break;
      case OP_ALOAD: op_str = "A-LOAD"; break;
      case OP_CSTORE: op_str = "C-STORE"; break;
      case OP_ASTORE: op_str = "A-STORE"; break;
      case OP_WRAP: op_str = "WRAP"; break;
      case OP_PIPE_PRE: op_str = "|PRE"; break;
      case OP_PIPE_SUF: op_str = "|SUF"; break;
      case OP_START_INT: op_str = "START"; break;
      case OP_PAUSE_INT: op_str = "PAUSE"; break;
      case OP_GENSTART: op_str = "GENSTART"; break;
      case OP_CODEGEN: op_str = "CODEGEN"; break;
      case OP_SLOAD: op_str = "SLOAD"; break;
      case OP_APPLY: op_str = "APPLY"; break;
      case OP_REVERSE: op_str = "REVERSE"; break;
      case OP_EXPORT_OSL: op_str = "EXPORT_OSL"; break;
      case OP_EXPORT_SCOP: op_str = "EXPORT_SCOP"; break;
      default: op_str = "ERROR"; break;
    }
    return op_str;
  }


  bool is_same_category (int p_cat)
  {
    return m_node_category == p_cat;
  }

  void set_stmt_id_offset (int p_offset)
  {
    m_stmt_id_offset = p_offset;
  }

  int get_stmt_id_offset ()
  {
    return m_stmt_id_offset;
  }


  virtual void print () = 0;
  /*
  {
    cout << endl;
    cout << "Printing from goperator" << endl;
  }
  */

  virtual int get_dim ()
  {
    return 0;
  }

  virtual string get_expression_plain ()
  {
    return "aql_goperator::get_expression_plain ()";
  }

  virtual bool is_variable ()
  {
    return false;
  }

  virtual string get_class_name ()
  {
    return "aql_goperator";
  }

  void disable_circuit_checkings ()
  {
    m_check = false;
  }

  void enable_circuit_checkings ()
  {
    m_check = true;
  }
};

#endif
