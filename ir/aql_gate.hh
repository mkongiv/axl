/*
AQCSL: the Affine Quantum Circuit Specification Language
Filename: aql_gate.hh
Creation date: Sun May 26 08:14:25 EDT 2019
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

#ifndef AQCL_GATE
#define AQCL_GATE

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <sstream>
//#include "aqcsl-ir.hh"
#include "aql_node.hh"
#include "aql_variable.hh"
#include "aql_aoperator.hh"
#include "aql_goperator.hh"
#include "aql_aexpr.hh"

using namespace std;

// Gate types, essentially the same as those defined in the grammar and scannner
typedef enum { 
  GATE_UNKNOWN = 0,
  GATE_H,    //1,1
  GATE_X,    //1,1
  GATE_Y,    //1,1
  GATE_Z,    //1,1
  GATE_U,    //1,1
  GATE_QFT,  //1,1
  GATE_MEAS, //1,1
  GATE_RTNOT,//1,1
  GATE_R,    //1,1 + arg
  GATE_CX,   //2,1
  GATE_CY,   //2,1
  GATE_CZ,   //2,1
  GATE_XX,   //2,1 + arg
  GATE_YY,   //2,1 + arg
  GATE_ZZ,   //2,1 + arg
  GATE_SWAP, //2,2
  GATE_SQRT_SWAP, //2,2
  GATE_CCX,//3,1
  GATE_CSWAP,//3,2
  GATE_BLOCK
} e_gate;

class aqcl_gate : public aqcl_goperator
{
  protected:
  int m_gate_num; // global gate identifier, will be unique in the program
  int m_nin;  // number of IN qubits
  int m_nout; // number of OUT qubits
  int m_gt;   // gate type, i.e. H, X, etc
  int m_w0;
  int m_w1;
  int m_w2;
  int m_global_gate_number;
  /* some argument expression; should probably be aqcl_aexpr, 
  *   but I'll leave it like this for now; this is the fixed argument o angle
  */
  aqcl_aoperator * m_arg; 
  aqcl_aoperator * m_src; 
  aqcl_aoperator * m_tgt;

  /* Note:
  * For a two-qubit gate, m_src is used as the first operand, and m_arg 
  * as the second one, i.e., m_src and m_arg, in that order
  * Examples: 
  * #X(m_src)
  * #cnot(m_arg,m_src)
  * #ccnot(m_arg,m_src,m_tgt)
  */

  public:

  aqcl_gate () : aqcl_goperator()
  {
    m_node_category =  AQL_NODE_GATE;
  }

  aqcl_gate (aqcl_gate & p_gate)
  {
    m_nin = p_gate.get_nin ();
    m_nout = p_gate.get_nout ();
    m_w0 =  p_gate.get_w0 ();
    m_w1 = p_gate.get_w1 ();
    m_w2 = p_gate.get_w2 ();
    m_gt = p_gate.get_gt ();
    m_arg = p_gate.get_arg ();
    m_src = p_gate.get_src ();
    m_tgt = p_gate.get_tgt ();
  }

  aqcl_gate (e_gate p_gt, int p_gate_num) : aqcl_goperator(OP_GATE)
  {
    gate_setup (p_gt, NULL, NULL, NULL);
    m_gate_num = p_gate_num;
  }

  /*
  * Constructor used in gates with 1 input and 1 output qubit,
  * and where both are the same qubit.
  */
  aqcl_gate (
    e_gate p_gt, 
    int p_w0,
    aqcl_aoperator * p_arg/* = NULL*/) : aqcl_goperator(OP_GATE)
  {
    gate_setup (p_gt, NULL, p_arg, NULL);
    m_w0 = p_w0;
  }

  /*
  * Constructor used in gates with 2 input and 2 output qubit,
  * and where both are the same qubit.
  */
  aqcl_gate (
    e_gate p_gt, 
    int p_w0,
    int p_w1,
    aqcl_aoperator * p_arg/* = NULL*/) : aqcl_goperator(OP_GATE)
  {
    gate_setup (p_gt, NULL, p_arg, NULL);
    m_w0 = p_w0;
  }

  /*
  * Mmmm
  * 
  */
  aqcl_gate (
    e_gate p_gt, 
    int p_w0,
    int p_w1,
    int p_w2,
    aqcl_aoperator * p_arg/* = NULL*/) : aqcl_goperator(OP_GATE)
  {
    gate_setup (p_gt, NULL, p_arg, NULL);
    m_w0 = p_w0;
  }

  /*
  * Constructor used for 1-qubit gates, e.g. #X (src)
  *
  */
  aqcl_gate (
    e_gate p_gt, 
    aqcl_aoperator * p_arg,
    int p_gate_num) : aqcl_goperator(OP_GATE)
  {
    gate_setup (p_gt, NULL, p_arg, NULL);
    m_gate_num = p_gate_num;
    //m_w0 = p_w0;
  }

  /*
  * Constructor used for 2-qubit gates, e.g. #cnot (src,arg)
  * Note that #cnot = #cX
  */
  aqcl_gate (
    e_gate p_gt, 
    aqcl_aoperator * p_src,
    aqcl_aoperator * p_arg,
    int p_gate_num) : aqcl_goperator(OP_GATE)
  {
    gate_setup (p_gt, p_src, p_arg, NULL);
    m_gate_num = p_gate_num;
    //m_w0 = p_w0;
  }

  /*
  * Constructor used for 3-qubit gates, e.g. #ccnot (src,arg,tgt)
  * Note that #ccnot = #ccX
  */
  aqcl_gate (
    e_gate p_gt, 
    aqcl_aoperator * p_src,
    aqcl_aoperator * p_arg,
    aqcl_aoperator * p_tgt,
    int p_gate_num) : aqcl_goperator(OP_GATE)
  {
    gate_setup (p_gt, p_src, p_arg, p_tgt);
    m_gate_num = p_gate_num;
    //m_w0 = p_w0;
  }

  /* 
   * p_arg: expression argument, likely something like i*PI/4
   */
  void gate_setup (e_gate p_gt, 
    aqcl_aoperator * p_src, aqcl_aoperator * p_arg, aqcl_aoperator * p_tgt)
  {
    m_node_category =  AQL_NODE_GATE;
    m_gt = p_gt;
    m_src = p_src;
    m_arg = p_arg;
    m_tgt = p_tgt;
    m_nout = 1;
    // NOTE: Whenever adding a new gate to the enum type, check this condition!
    if (GATE_H <= m_gt and m_gt <= GATE_RTNOT)
    {
      m_nin = m_nout;
    } 
    else if (GATE_CX <= m_gt and m_gt <= GATE_ZZ)
    {
      m_nin = 2;
    }
    else if (GATE_SWAP == m_gt || GATE_SQRT_SWAP == m_gt)
    {
      m_nin = m_nout = 2;
    }
    else if (GATE_CCX == m_gt)
    {
      m_nin = 3; 
    }
    else if (GATE_CSWAP == m_gt)
    {
      m_nin = 3;
      m_nout = 2;
    }
    else
    {
      assert (0 && "aql_gate.hh: Unsupported guantum gate type. Aborting ...");
    }
  }

  ~ aqcl_gate ()
  {

  }


  int get_n_in ()
  {
    return m_nin;
  }

  int get_n_out ()
  {
    return m_nout;
  }

  int get_category ()
  {
    return m_node_category;
  }

  bool is_same_category (int p_cat)
  {
    return m_node_category == p_cat;
  }

  bool virtual is_gate ()
  {
    return true;
  }

  void set_global_gate_number (int p_ggn)
  {
    m_global_gate_number = p_ggn;
  }
  
  int get_global_gate_number ()
  {
    return m_global_gate_number;
  }

  virtual string get_string () 
  {
    string ret = "";
    switch (m_gt) {
      case GATE_H: ret = "#H"; break;
      case GATE_X: ret = "#X"; break;
      case GATE_Y: ret = "#Y"; break;
      case GATE_Z: ret = "#Z"; break;
      case GATE_U: ret = "#U"; break;
      case GATE_RTNOT: ret = "#rNOT"; break;
      case GATE_R: ret = "#R"; break;
      case GATE_SWAP: ret = "#SWAP"; break;
      case GATE_SQRT_SWAP: ret = "#SQRT_SWAP"; break;
      case GATE_CX: ret = "#CX"; break;
      case GATE_CY: ret = "#CY"; break;
      case GATE_CZ: ret = "#CZ"; break;
      case GATE_CCX: ret = "#CCX"; break;
      case GATE_CSWAP: ret = "#CSWAP"; break;
      case GATE_XX: ret = "#XX"; break;
      case GATE_YY: ret = "#YY"; break;
      case GATE_ZZ: ret = "#ZZ"; break;
      case GATE_QFT: ret = "#QFT"; break;
      case GATE_MEAS: ret = "#M"; break;
    }
    return ret;
  }

  string get_name_as_space ()
  {
    //return get_string().erase (0,1) + "_" + this->get_gate_num ();
    return get_string().erase (0,1) + "_" + this->get_global_gate_number_str ();
  }

  virtual aqcl_node * get_result ()
  {
    return this;
  }

  bool takes_one ()
  {
    switch (m_gt) {
      case GATE_H:    //1:1
      case GATE_X:    //1:1
      case GATE_Y:    //1:1
      case GATE_Z:    //1:1
      case GATE_U:    //1:1
      case GATE_QFT:  //1:1
      case GATE_MEAS: //1:1
      case GATE_RTNOT://1:1
      case GATE_R:    //1:1 + arg
        return true;
      default:
        return false;
    }
  }

  bool takes_two ()
  {
    switch (m_gt) {
      case GATE_CX:   //2:1
      case GATE_CY:   //2:1
      case GATE_CZ:   //2:1
      case GATE_XX:   //2:1 + arg
      case GATE_YY:   //2:1 + arg
      case GATE_ZZ:   //2:1 + arg
      case GATE_SWAP: //2:2
        return true;
      default:
        return false;
    }
  }

  bool takes_three ()
  {
    switch (m_gt) {
      case GATE_CCX://3:1
      case GATE_CSWAP://3:2
        return true;
      default:
        return false;
    }
  }

  virtual string get_result_name ()
  {
    return this->get_string ();
  }

  virtual string get_name ()
  {
    return get_string ();
  }

  int get_num_operands ()
  {
    int num_ops = 0;
    if (m_arg)
      num_ops++;
    if (m_src)
      num_ops++;
    if (m_tgt)
      num_ops++;
    return num_ops;
  }

  virtual string get_instruction_str ()
  {
    stringstream ret;
    ret << this->get_string ();
    ret << ", N/A";
    //ret << m_res->get_name ();
    ret << ", [ ";
//    ret << m_offset;
//    ret << " | ";
    ret << " ";
    ret << m_w0;
    ret << " , ";
    if (m_nin >= 2)
      ret << m_w1;
    else
      ret << "_";
    ret << " , ";
    if (m_nin >= 3)
      ret << m_w2;
    else
      ret << "_";
//    ret << " | ";
//    ret << m_padding;
    ret << " ]";
    if (m_src)
    {
      ret << ", ";
      ret << m_src->get_name ();
    }
    else
    {
      ret << ", N/A";
    }
    if (m_arg)
    {
      ret << ", ";
      ret << m_arg->get_name ();
    }
    else
    {
      ret << ", N/A";
    }
    if (m_tgt)
    {
      ret << ", ";
      ret << m_tgt->get_name ();
    }
    else
    {
      ret << ", N/A";
    }
    ret << " // aql_gate";
    return ret.str ();
  }

  virtual void print ()
  {
    cout << get_expression_plain (); /// << endl;
  }

  virtual void print_to_stream (stringstream & strm)
  {
    strm << get_expression_plain ();
  }


  inline int get_nin ()
  {
    return m_nin;
  }

  inline int get_nout ()
  {
    return m_nout;
  }

  inline int get_gt ()
  {
    return m_gt;
  }

  inline int get_w0 ()
  {
    return m_w0;
  }

  inline int get_w1 ()
  {
    return m_w1;
  }
  
  inline int get_w2 ()
  {
    return m_w2;
  }

  /*
  * Returns the m_arg field. Typically this is the only
  * qubit parameter in single qubit gates, and the second,
  * parameter in gates that take 2 and 3 qubits.
  */
  aqcl_aoperator * get_arg ()
  {
    return m_arg;
  }

  /*
  * Returns the m_src field. Typically this is the first
  * qubit in gates with 2 and 3 parameters.
  */
  aqcl_aoperator * get_src ()
  {
    return m_src;
  }

  /*
  * Returns the m_tgt field. Typically this is the target 
  * qubit in 3 qubit gates
  */
  aqcl_aoperator * get_tgt ()
  {
    return m_tgt;
  }

  virtual int run ()
  {
    cout << "NO_OP for aqcl_gate::run () " << endl;
    return 0;
  }

  virtual int get_dim ()
  {
    return get_nin ();
  }

  virtual string get_expression_plain ()
  {
    stringstream ret;
    ret << get_string ();
    ret << get_gate_arguments_str ();
    return ret.str ();
  }

  virtual int get_width ()
  {
    return 1;
  }

  /*
  * Return the control (read-only) expression
  */
  #if 0
  string get_read_expression_str ()
  {
    string ret = "";
    if (!m_src)
      return ret;
    ret += " + ";
    aqcl_aexpr * aa = (aqcl_aexpr*)(m_src);
    if (aa->is_leaf ())
      ret += aa->get_result ()->get_name ();
    else
      ret += aa->get_expression_plain ();
    return ret;
  }
  #endif
  vector<string> get_read_expression_str ()
  {
    vector<string> ret;
    string str_rel;
    if (m_nin >= 1 && m_src)
    {
      str_rel = "";
      str_rel += " + ";
      aqcl_aexpr * aa = (aqcl_aexpr*)(m_src);
      if (aa->is_leaf ())
        str_rel += aa->get_result ()->get_name ();
      else
        str_rel += aa->get_expression_plain ();
      ret.push_back(str_rel);
    }
    if (m_nin >= 1 && m_arg)
    {
      str_rel = "";
      str_rel += " + ";
      aqcl_aexpr * aa = (aqcl_aexpr*)(m_arg);
      if (aa->is_leaf ())
        str_rel += aa->get_result ()->get_name ();
      else
        str_rel += aa->get_expression_plain ();
      ret.push_back(str_rel);
    }
    if (m_nout >= 1 && m_tgt)
    {
      str_rel = "";
      str_rel += " + ";
      aqcl_aexpr * aa = (aqcl_aexpr*)(m_tgt);
      if (aa->is_leaf ())
        str_rel += aa->get_result ()->get_name ();
      else
        str_rel += aa->get_expression_plain ();
      ret.push_back(str_rel);
    }
    return ret;
  }

  /*
  * Return the control (write-only) expression
  */
  #if 0
  string get_write_expression_str ()
  {
    string ret = "";
    if (!m_arg)
      return ret;
    ret += " + ";
    aqcl_aexpr * aa = (aqcl_aexpr*)(m_arg);
    if (aa->is_leaf ())
      ret += aa->get_result ()->get_name ();
    else
      ret += aa->get_expression_plain ();
    return ret;
  }
  #endif
  vector<string> get_write_expression_str ()
  {
    vector<string> ret;
    string str_rel;
    if (m_nout >= 1 && m_arg)
    {
      str_rel = "";
      str_rel += " + ";
      aqcl_aexpr * aa = (aqcl_aexpr*)(m_arg);
      if (aa->is_leaf ())
        str_rel += aa->get_result ()->get_name ();
      else
        str_rel += aa->get_expression_plain ();
      ret.push_back(str_rel);
    }
    if (m_nout >= 2 && m_tgt)
    {
      str_rel = "";
      str_rel += " + ";
      aqcl_aexpr * aa = (aqcl_aexpr*)(m_tgt);
      if (aa->is_leaf ())
        str_rel += aa->get_result ()->get_name ();
      else
        str_rel += aa->get_expression_plain ();
      ret.push_back(str_rel);
    }
    return ret;
  }

  string get_gate_call_arguments_str ()
  {
    return get_gate_arguments_str (true);
  }

  string get_gate_num ()
  {
    stringstream ret;
    ret << m_gate_num;
    return ret.str ();
  }

  string get_global_gate_number_str ()
  {
    stringstream ret;
    ret << this->m_global_gate_number;
    return ret.str ();
  }

  string get_gate_arguments_str (bool as_isl = false)
  {
    stringstream ret;
    string left_par = "(";
    string right_par = ")";
    if  (as_isl)
    {
      left_par = "";
      right_par = "";
    }

    if (m_src && m_arg && !m_tgt)
    {
      aqcl_aexpr * a1 = (aqcl_aexpr*)(m_src);
      aqcl_aexpr * a2 = (aqcl_aexpr*)(m_arg);
      ret << left_par;
      if (a1->is_leaf ())
        ret << a1->get_result ()->get_name ();
      else
        ret << a1->get_expression_plain ();
      ret << ", ";
      if (a2->is_leaf ())
        ret << a2->get_result ()->get_name ();
      else
        ret << a2->get_expression_plain ();
      ret << right_par;
    }

    if (!m_src && m_arg && !m_tgt)
    {
      aqcl_variable * ang = (aqcl_variable *)(m_arg->get_result ());
      // FIXME: add angle arguments. (JUL 08 2019)
      // Use this (or part of this)
      //ret << ang->get_val ()->get_val ();
      aqcl_aexpr * a2 = (aqcl_aexpr*)(m_arg);
      ret << left_par;
      if (a2->is_leaf ())
        ret << a2->get_result ()->get_name ();
      else
        ret << a2->get_expression_plain ();
      ret << right_par << endl;
    }

    if (m_src && m_arg && m_tgt)
    {
      aqcl_aexpr * a1 = (aqcl_aexpr*)(m_src);
      aqcl_aexpr * a2 = (aqcl_aexpr*)(m_arg);
      aqcl_aexpr * a3 = (aqcl_aexpr*)(m_tgt);
      ret << left_par;
      if (a1->is_leaf ())
        ret << a1->get_result ()->get_name ();
      else
        ret << a1->get_expression_plain ();
      ret << ", ";
      if (a2->is_leaf ())
        ret << a2->get_result ()->get_name ();
      else
        ret << a2->get_expression_plain ();
      ret << ", ";
      if (a3->is_leaf ())
        ret << a3->get_result ()->get_name ();
      else
        ret << a3->get_expression_plain ();
      ret << right_par;
    }

    return ret.str ();
  }
};

#endif
