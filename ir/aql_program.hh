/*
AQCSL: the Affine Quantum Circuit Specification Language
Filename: aql_program.hh
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

#ifndef AQCL_PROGRAM
#define AQCL_PROGRAM

#include <vector>
#include <map>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <stack>
//#include "aqcsl-ir.hh"
#include "aql_node.hh"
#include "aql_transformation.hh"
#include "aql_statement.hh"
#include "aql_variable.hh"
#include "aql_goperator.hh"
#include "aql_sexpr.hh"
#include "aql_symtab.hh"
#include "../utils.hh"

// ISL specific headers
#include "isl/ctx.h"
#include "isl/set.h"
#include "isl/map.h"
#include "isl/union_set.h"
#include "isl/union_map.h"
#include "isl/options.h"
#include "isl/ast.h"
#include "isl/ast_build.h"
#include "isl/flow.h"
#include "isl/schedule.h"

#include "osl/scop.h"


using namespace std;

class aqcl_program : public aqcl_node
{
  private:
  //map<string,aqcl_node*> m_symtab;
  stack<aqcl_symtab*> m_symtab; // stack table = [bottom = main, stmt1, stmt .
  aqcl_symtab * m_main_st;      // main symbol table
  aqcl_symtab * m_partab;       // parameter symbol table
  map<string,int> m_transfo_map;
  map<string,int> m_backend_map;
  
  vector<aqcl_goperator*> m_itab; // instruction table
  int m_ntempV;
  int m_ntempB;
  int m_nstmt;
  int m_ntempS;
  int m_numparam;
  int m_solver; // values defined in aql_statement.hh
  isl_ctx * m_ctx;
  string m_ctx_str;
  int m_global_gate_number;

  vector<aqcl_statement*> m_stmt;


  public:

  aqcl_program () : aqcl_node ()
  {
    m_ntempV = 0; 
    m_ntempB = 0;
    m_nstmt = 0;
    m_ntempS = 0;
    m_numparam = 0;
    m_ctx = isl_ctx_alloc ();
    push_symtab ();
    m_main_st = m_symtab.top ();
    m_partab = new aqcl_symtab ();
    m_ctx_str = "";
    init_transfo_map ();
    init_backend_map ();
    m_solver = AXL_SOLVER_PIPLIB_GMP;
    m_global_gate_number = 0;
  }

  ~ aqcl_program ()
  {
    int ii;
    isl_ctx_free (m_ctx);
    pop_symtab ();
  }

  virtual string get_string ()
  {
    return "aqcl_program::get_string ()";
  }


  /*
  * Initialize the transformation map
  *
  */
  void init_transfo_map ()
  {
    /** NOTE: every new transformation intended to be invoked from here, 
        must also modify accordingly:
        - aql_transformation::get_instruction_str  to print new instructions 
          for debugging purposes
        - aql_statement::compute_dependences if using directly some isl feature
        - declare the transformation code in aql_statement
        - aql_transformation::run to invoke the pertinent apply method
        - aql_statement::apply_XYZ the transformation method
    **/
    m_transfo_map["pluto"] = AXL_TRANSFO_PLUTO;
    m_transfo_map["plutomin"] = AXL_TRANSFO_PLUTO_MINFUSE;
    m_transfo_map["plutomax"] = AXL_TRANSFO_PLUTO_MAXFUSE;
    m_transfo_map["feautrier"] = AXL_TRANSFO_FEAUTRIER;
  }

  void init_backend_map ()
  {
    m_backend_map["qasm"] = QBE_QASM;
    m_backend_map["projectq"] = QBE_PROJQ;
    m_backend_map["rigetti"] = QBE_RIGETTI;;
  }

  const char * get_solver_name ()
  {
    switch (m_solver) {
      case AXL_SOLVER_PIPLIB_GMP: return "PIPLIB_GMP";
      case AXL_SOLVER_ISL: return "ISL";
      case AXL_SOLVER_CPLEX: return "CPLEX";
      case AXL_SOLVER_PIPLIB: return "PIPLOB";
      default: assert (0 && "ERROR: Solver undefined.");
    }
    return NULL;
  }

  int select_solver (const char * flag)
  {
    int ret = 0;
    m_solver = AXL_SOLVER_PIPLIB_GMP;
    char * ptr = (char*)flag;
    while (*ptr != '_') ptr++;
    ptr++;
    if (!strcmp (ptr, "cplex"))
    {
      ret = 1;
      m_solver = AXL_SOLVER_CPLEX;
    }
    if (!strcmp (ptr, "isl"))
    {
      ret = 1;
      m_solver = AXL_SOLVER_ISL;
    }
    if (!strcmp (ptr, "piplib"))
    {
      ret = 1;
      m_solver = AXL_SOLVER_PIPLIB;
    }
    if (!strcmp (ptr, "piplib_gmp"))
    {
      ret = 1;
      m_solver = AXL_SOLVER_PIPLIB_GMP;
    }
    return ret;
  }

  void print_transfo_list ()
  {
    map<string,int>::iterator tt;
    cout << "Available transformations: ";
    for (tt = m_transfo_map.begin (); tt != m_transfo_map.end (); tt++)
    {
      cout << tt->first << endl; 
    }
    cout << endl;
  }

  void print_backend_list ()
  {
    map<string,int>::iterator tt;
    cout << "Available backends: ";
    for (tt = m_backend_map.begin (); tt != m_backend_map.end (); tt++)
    {
      cout << tt->first << endl; 
    }
    cout << endl;
  }

  /*
  * Return the transformation code
  *
  */
  int get_transfo_code (string p_transfo_name)
  {
    map<string,int>::iterator tt;
    tt = m_transfo_map.find (p_transfo_name);
    if (tt != m_transfo_map.end ())
      return tt->second;
    return 0;
  }

  int get_backend_code (string p_backend_name)
  {
    map<string,int>::iterator tt;
    tt = m_backend_map.find (p_backend_name);
    if (tt != m_backend_map.end ())
      return tt->second;
    return -1;
  }

  bool in_stmt ()
  {
    return m_symtab.size () > 1;
  }

  void push_symtab ()
  {
    aqcl_symtab * st = new aqcl_symtab ();
    m_symtab.push (st);
  }

  void pop_symtab ()
  {
    m_symtab.pop ();
  }

  aqcl_symtab * get_current_table ()
  {
    return m_symtab.top();
  }

  aqcl_symtab * get_parameter_table ()
  {
    return m_partab;
  }

  bool exists_sym (string p_varname)
  {
    bool ret;
    ret = m_symtab.top()->get_table().find (p_varname) != m_symtab.top()->get_table().end ();

    if (not ret)
    {
      ret = m_partab->get_table ().find (p_varname) != m_partab->get_table().end ();
    }
    return ret;
  }

  aqcl_node * get_sym (string p_varname)
  {
    map<string,aqcl_node*>::iterator ret;

    // Search at the top of the symbol table stack
    ret = m_symtab.top()->get_table().find (p_varname);
    if (ret != m_symtab.top()->get_table().end ())
      return ret->second;

    // Search in main table
    ret = m_main_st->get_table ().find (p_varname);
    if (ret != m_main_st->get_table ().end ())
      return ret->second;

    // Search in the parameter symbol table
    ret = m_partab->get_table ().find (p_varname);
    if (ret != m_partab->get_table ().end ())
      return ret->second;

    cerr << "aql_program::get_sym () - Did not find symbol [" <<
      p_varname << "] " << endl;
    assert (0);

    return NULL;
  }

  aqcl_variable * get_sym_as_variable (string p_varname)
  {
    aqcl_variable * ret = (aqcl_variable*)(this->get_sym (p_varname));
    return ret;
  }

  aqcl_block * get_sym_as_block (string p_varname)
  {
    aqcl_block * ret = (aqcl_block*)(this->get_sym (p_varname));
    return ret;
  }

  aqcl_statement * get_sym_as_statement (string p_varname)
  {
    aqcl_statement * temp = (aqcl_statement*)(this->get_sym (p_varname));
    return temp;
  }

  aqcl_node * new_temp (int p_vartype = 0)
  {
    aqcl_node * ret;
    char tempname[20];
    if (p_vartype == SEMTYPE_CIRC)
    {
      sprintf (tempname, "__B%d__", m_ntempB);
      m_ntempB++;
      ret = new aqcl_variable (string(tempname), p_vartype);
    }
    else
    {
      sprintf (tempname, "__T%d__", m_ntempV);
      m_ntempV++;
      ret = new aqcl_variable (string(tempname), p_vartype);
    }
    ret->set_temp ();
    m_symtab.top()->get_table()[string(tempname)] = ret;
    return ret;
  }

  aqcl_variable * new_temp_as_variable (int p_vartype = 0)
  {
    aqcl_variable * ret;
    char tempname[20];
    sprintf (tempname, "__T%d__", m_ntempV);
    m_ntempV++;
    ret = new aqcl_variable (string(tempname), p_vartype);
    ret->set_temp ();
    m_symtab.top()->get_table()[string(tempname)] = ret;
    return ret;
  }

  aqcl_statement * new_temp_as_statement ()
  {
    aqcl_statement * ret;
    char tempname[20];
    sprintf (tempname, "__tS%d__", m_ntempS);
    m_ntempS++;
    bool res = add_stmt (tempname); //new aqcl_variable (string(tempname), SEMTYPE_STMT);
    if (not res)
      return NULL;
    ret = get_sym_as_statement (tempname);
    ret->set_temp ();
    //m_symtab.top()->get_table()[string(tempname)] = ret;
    return ret;
  }

  aqcl_block * new_temp_as_block ()
  {
    aqcl_block * ret;
    char tempname[20];
    sprintf (tempname, "__B%d__", m_ntempB);
    m_ntempB++;
    ret = new aqcl_block (string(tempname));
    ret->set_temp ();
    m_symtab.top()->get_table()[string(tempname)] = ret;
    return ret;
  }


  bool add_variable (aqcl_variable * p_newvar)
  {
    string varname = p_newvar->get_name ();
    bool var_exists = exists_sym (varname);
    if (not var_exists)
    {
      m_symtab.top()->get_table()[varname] = p_newvar;
    }
    else
    {
      //assert (0 && "aqcl_program::add_variable ==> variable already declared.");
    }
    return not var_exists;
  }


  bool add_variable (string varname, int semtype)
  {
    bool var_exists = exists_sym (varname);
    if (not var_exists)
    {
      m_symtab.top()->get_table()[varname] = new aqcl_variable (varname, semtype);
    }
    else
    {
      //assert (0 && "aqcl_program::add_variable ==> variable already declared.");
    }
    return not var_exists;
  }

  bool add_variable_with_pos (string varname, int semtype, int p_pos)
  {
    bool var_exists = exists_sym (varname);
    if (not var_exists)
    {
      m_symtab.top()->get_table()[varname] = new aqcl_variable (varname, semtype, p_pos);
    }
    else
    {
      //assert (0 && "aqcl_program::add_variable ==> variable already declared.");
    }
    return not var_exists;
  }

  bool add_param (string varname)
  {
    bool var_exists = exists_sym (varname);
    if (not var_exists)
    {
      //m_symtab.top()->get_table()[varname] = new aqcl_variable (varname, SEMTYPE_PARAM);
      m_partab->get_table()[varname] = new aqcl_variable (varname, SEMTYPE_PARAM, m_numparam);
      m_numparam ++;
    }
    else
    {
      //assert (0 && "aqcl_program::add_variable ==> variable already declared.");
    }
    return not var_exists;
  }

  bool add_stmt (string varname)
  {
    bool var_exists = exists_sym (varname);
    if (not var_exists)
    {
      m_symtab.top()->get_table()[varname] = new aqcl_statement (varname, m_nstmt, m_partab, m_ctx);
      m_nstmt ++;
    }
    return not var_exists;
  }

  bool add_symbol (string varname, int semtype)
  {
    switch (semtype)
    {
      case SEMTYPE_CIRC:
        return this->add_block (varname);
      case SEMTYPE_INT:
        return this->add_variable (varname, semtype);
      case SEMTYPE_PARAM:
        return this->add_param (varname);
      case SEMTYPE_STMT:
        return this->add_stmt (varname);
      default:
        return false;
    }
    return false;
  }


  bool add_block (string blockname)
  {
    bool var_exists = exists_sym (blockname);
    if (not var_exists)
    {
      m_symtab.top()->get_table()[blockname] = new aqcl_block (blockname);
    }
    return not var_exists;
  }

  void print_symtab ()
  {
    map<string,aqcl_node*>::iterator ii;
    int idx = 0;
    cout << endl << "Symbol Table: " << endl;
    for (ii = m_symtab.top()->get_table().begin (); ii != m_symtab.top()->get_table().end (); ii++, idx++)
    {
      if (ii->second->is_block ())
      {
        aqcl_block * var = (aqcl_block*)(ii->second);
        cout << "SYM " << idx << " : " << "<" << 
          var->get_semtype_str () << "> " << var->get_name () << 
          " [is_temp=" << var->get_temp () << 
          ", is_leaf=" << var->is_leaf () << 
          "] " << endl; 
      }
      else
      {
        aqcl_variable * var = (aqcl_variable*)(ii->second);
        cout << "SYM " << idx << " : " << "<" << 
          var->get_semtype_str () << "> " << var->get_name () << 
          " [is_temp=" << var->get_temp () << 
          ", is_leaf=" << var->is_leaf () << 
          "] " << endl; 
      }
    }
    for (ii = m_partab->get_table().begin (); ii != m_partab->get_table().end (); ii++, idx++)
    {
      aqcl_variable * var = (aqcl_variable*)(ii->second);
      cout << "SYM " << idx << " : " << "<" << 
        var->get_semtype_str () << "> " << var->get_name () << 
        " [is_temp=" << var->get_temp () << 
        ", is_leaf=" << var->is_leaf () << 
        "] " << endl; 

    }
  }


  void add_instr (aqcl_goperator * newinstr)
  {
    assert (newinstr != NULL);
    if (newinstr->is_gate ())
    {
      aqcl_gate * gg = (aqcl_gate*)(newinstr);
      gg->set_global_gate_number (this->m_global_gate_number++);
    }
    if (this->in_stmt ())
    {
      newinstr->disable_circuit_checkings ();
    }
    m_itab.push_back (newinstr);
  }

  void print_itab ()
  {
    vector<aqcl_goperator*>::iterator ii;
    int idx = 0;
    int n_entries = m_itab.size ();
    cout << endl << "Instruction Table (" << n_entries << "): " << endl;
    for (ii = m_itab.begin (); ii != m_itab.end (); ii++, idx++)
    {
      cout << "INSTR " << idx << " : " ;
      if (*ii)
      {
          cout << (*ii)->get_instruction_str () << endl;
      }
      else
        cout << "NULL" << endl;
    }
  }

  void print_domain_statements ()
  {
    int idx = 0;
    map<string,aqcl_node*>::iterator ii;
    cout << endl << "Statement Table " << endl;
    for (ii = m_symtab.top()->get_table().begin (); ii != m_symtab.top()->get_table().end (); ii++, idx++)
    {
      if (ii->second->is_statement ())
      {
        aqcl_statement * stmt = (aqcl_statement*)(ii->second);
        //cout << stmt->get_name () << " : ";
        cout << stmt->get_string () << "; ";
        //stmt->print ();
        cout << endl;
      }
    }
  }


  virtual string get_instruction_str ()
  {
    return "";
  }
  

  int get_category ()
  {
    return m_node_category;
  }

  bool is_same_category (int p_cat)
  {
    return m_node_category == p_cat;
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
    return "<aqcsl_program>";
  }


  int get_semtype (aqcl_node * p_left, aqcl_node * p_right)
  {
    int lst;
    int rst; 
    if (!p_right)
      return p_left->get_result()->get_semtype ();
    if (!p_left)
      return p_right->get_result()->get_semtype ();
    lst = p_left->get_result()->get_semtype ();
    rst = p_right ->get_result ()->get_semtype ();
    if (lst == rst)
      return lst;
    if (lst == SEMTYPE_STMT || rst == SEMTYPE_STMT)
      return SEMTYPE_STMT;
    if (lst == SEMTYPE_CIRC || rst == SEMTYPE_CIRC)
      return SEMTYPE_CIRC;
    if (lst == SEMTYPE_DOUBLE || rst  == SEMTYPE_DOUBLE)
      return SEMTYPE_DOUBLE;
    if (lst == SEMTYPE_FLOAT || rst  == SEMTYPE_FLOAT)
      return SEMTYPE_FLOAT;
    if (lst == SEMTYPE_CIRC && rst == SEMTYPE_INT)
      return SEMTYPE_CIRC;
    if (rst == SEMTYPE_CIRC && lst == SEMTYPE_INT)
      return SEMTYPE_CIRC;
    if (rst == SEMTYPE_PARAM && lst == SEMTYPE_INT)
      return SEMTYPE_INT;
    if (lst == SEMTYPE_PARAM && rst == SEMTYPE_INT)
      return SEMTYPE_INT;
    return SEMTYPE_ERROR;
  }

  int get_semtype (int p_opcode, aqcl_node * p_left, aqcl_node * p_right)
  {
    if (p_opcode == OP_DIV)
      return SEMTYPE_FLOAT;
    return get_semtype (p_left, p_right);
  }

  // one must be an int, the other circ
  int is_scaling_ok (aqcl_node * p_left, aqcl_node * p_right)
  {
    if (!p_left)
      return 0;
    if (!p_right)
      return 0;
    int st1 = p_left->get_semtype ();
    int st2 = p_right->get_semtype ();
    if (st1 == SEMTYPE_INT and st2 == SEMTYPE_CIRC)
      return 1;
    if (st2 == SEMTYPE_INT and st1 == SEMTYPE_CIRC)
      return 1;
    return 0;
  }


  string get_isl_param_str ()
  {
    string ret = "";
    ret = "[ " + m_partab->get_symbol_list () + " ]";
    return ret;
  }

  string get_isl_ctx_param_str ()
  {
    string ret = "";
    map<string,aqcl_node*>::iterator ss;
    bool first = true;
    for (ss = m_partab->get_table().begin (); ss != m_partab->get_table().end (); ss++)
      if (ss->second->is_param ())
    {
      if (not first)
      {
        ret += " and ";
      }
      first = false;
      // FIXME: must allow to generate 3 cases: 
      // a) fixed default value
      // b) parametric 
      // c) user-provided value
      ret += ss->second->get_name () + " = 12 ";
    }
    return ret;
  }

  __isl_give isl_set * create_isl_set (aqcl_statement * stmt)
  {
    string param_str = this->get_isl_param_str ();
    string islstr;
    islstr = param_str + " -> " + stmt->get_string ();
    assert (m_ctx && "aql_program.hh: isl_ctx not yet instantiated.");
    return isl_set_read_from_str (m_ctx, islstr.c_str());
  }


  __isl_give isl_map * create_identity_schedule_map (__isl_keep isl_set * set)
  {
    isl_map * ret;
    ret = isl_set_identity (isl_set_copy (set));
    ret = isl_map_set_tuple_name (ret, isl_dim_out, "");
    return ret;
  }

  __isl_give isl_map * build_schedule_map_test (__isl_keep isl_set * set)
  {
    isl_map * ret;
    ret = isl_set_identity (isl_set_copy (set));
    ret = isl_map_set_tuple_name (ret, isl_dim_out, "");
    return ret;
  }


  void generate_prologue (int p_backend)
  {
    if (p_backend == QBE_QASM)
    {

    }
    if (p_backend == QBE_PROJQ)
    {

    }
    if (p_backend == QBE_RIGETTI)
    {


    }
  }


  void generate_body (int p_backend)
  {
    if (p_backend == QBE_QASM)
    {

    }
    if (p_backend == QBE_PROJQ)
    {

    }
    if (p_backend == QBE_RIGETTI)
    {


    }
  }

  void generate_epilogue (int p_backend)
  {
    if (p_backend == QBE_QASM)
    {

    }
    if (p_backend == QBE_PROJQ)
    {

    }
    if (p_backend == QBE_RIGETTI)
    {


    }
  }

  void call_backend (int p_backend)
  {

  }

  void lower_to_backend (int p_backend)
  {
    generate_prologue (p_backend);
    generate_body (p_backend);
    generate_epilogue (p_backend);
    call_backend (p_backend);
  }


  /*
   * Collect all the statement from the current instruction code, backwards
   * until reaching the immediately last GENSTART
   * Can collect either the "original" statements (rhs of SLOAD op, arg a2) or the 
   * "result" statement from its lhs.
   */
  void collect_codegen_statements (int instr_index, 
    vector<aqcl_statement*> & stmts, bool use_original = false)
  {
    int ii = instr_index;
    int icode;
    int stmt_idx = 0;
    do {
      ii--;
      aqcl_goperator * op = m_itab[ii];
      icode = op->get_opcode ();
      if (icode == OP_SLOAD)
      {
        aqcl_sexpr * sexpr = (aqcl_sexpr*)(op);
        // Use the below statement to collect statement in their "temp" / "result" form
        if (use_original)
        {
          stringstream ss;
          ss << "R" << stmt_idx++;
          ((aqcl_statement*)(sexpr->get_a2 ()))->set_name (ss.str ());
          stmts.push_back ((aqcl_statement*)(sexpr->get_a2 ()));
        }
        else
        {
          stringstream ss;
          ss << "R" << stmt_idx++;
          ((aqcl_statement*)(sexpr->get_result_stmt ()))->set_name (ss.str ());
          stmts.push_back ((aqcl_statement*)(sexpr->get_result_stmt ()));
        }
      }
    } while (icode != OP_GENSTART);
  }


  virtual int run ()
  {
    int stmt_id_offset = 0;
    cout << endl << "Output: " << endl;
    int res;
    bool active = true;
    int ni = m_itab.size ();
    int ii = 0;
    for (ii = 0; ii < ni; ii++)
    {
      aqcl_goperator * op = m_itab[ii];
      cout << "i" << ii<< ": " << op->get_instruction_str () << endl;
      int icode = op->get_opcode ();
      if (active)
        op->activate ();
      else
        op->deactivate ();
      switch (icode)
      {
        case OP_ADD: 
          // op_str = "+";
          res = op->run (); 
          break;
        case OP_SUB: 
          // op_str = "-"; 
          res = op->run ();
          break;
        case OP_MUL: 
          //op_str = "*"; 
          res = op->run ();
          break;
        case OP_DIV: 
          // op_str = "/"; 
          res = op->run ();
          break;
        case OP_COMP_S: 
          //op_str = "[+]"; 
          res = op->run ();
          break;
        case OP_COMP_T: 
          //op_str = "(+)"; 
          res = op->run ();
          break;
        case OP_ITER_S: 
          //op_str = "[*]"; 
          res = op->run ();
          break;
        case OP_ITER_T: 
          //op_str = "(*)"; 
          res = op->run ();
          break;
        case OP_UMIN: 
          //op_str = "UMIN"; 
          res = op->run ();
          break;
        case OP_MOV: 
          assert (0);
          break;
        case OP_LT: 
          res = 0;
          //op_str = "<"; 
          break;
        case OP_LEQ: 
          res = 0;
          //op_str = "<="; 
          break;
        case OP_GT: 
          res = 0;
          //op_str = ">"; 
          break;
        case OP_GEQ: 
          res = 0;
          //op_str = ">="; 
          break;
        case OP_ATCH_DOM_B: 
          //op_str = "%"; 
          //op->set_stmt_id_offset (stmt_id_offset);
          res = op->run ();
          //stmt_id_offset = op->get_stmt_id_offset ();
          break;
        case OP_ATCH_DOM_T: 
          //op_str = "(%)"; 
          //op->set_stmt_id_offset (stmt_id_offset);
          res = op->run ();
          //stmt_id_offset = op->get_stmt_id_offset ();
          break;
        case OP_ATCH_DOM_S: 
          //op_str = "[%]"; 
          //op->set_stmt_id_offset (stmt_id_offset);
          res = op->run ();
          //stmt_id_offset = op->get_stmt_id_offset ();
          break;
        case OP_ALOAD:
        case OP_CLOAD:
        case OP_SLOAD:
          res = op->run ();    
          break;
        case OP_ASTORE:
        case OP_CSTORE:
          res = op->run ();    
          break;
        case OP_PIPE_PRE:
          res = op->run ();
          break;
        case OP_PIPE_SUF:
          res = op->run ();
          break;
        case OP_PRINT:
          //op->run ();      
          // Implemented in aql_print.hh
          op->print ();
          res = 0;
          cout << endl;
          break;
        case OP_G2C:
          res = op->run ();          
          break;
        case OP_PAUSE_INT:
          active = false;
          res = 0;
          break;
        case OP_START_INT:
          active = true;
          res = 0;
          break;
        case OP_GENSTART:
          res = 0;
          break;
        case OP_CODEGEN:
          {
            aqcl_print * prt = (aqcl_print*)(op);
            aqcl_statement * stmt = prt->get_result_statement ();
            res = stmt->codegen ();
            stmt->aquma_format_dump ();
          }
          break;
        case OP_APPLY:
          {
            aqcl_transformation * transfo = (aqcl_transformation*)(op);
            vector<aqcl_statement*> stmts;
            if (transfo->get_transfo_code () != AXL_TRANSFO_REVERSE_TIME)
            {
              this->collect_codegen_statements (ii, stmts);
              transfo->set_statement_vector (&stmts);
            }
            transfo->set_solver (m_solver);
            res = transfo->run ();
          }
          break;
        case OP_EXPORT_OSL:
          {
          }
          break;
        case OP_EXPORT_SCOP:
          {
          }
          break;
        default: 
          res = op->run ();
          break;
      }
      cout << "-------------------------------------------------------" << endl;
      if (res)
        return 1;
    }
    return 0; // 0 means "OK"
  }

  virtual string get_expression_plain ()
  {
    return "axl_program";
  }

  void export_scop ()
  {
    cout << "Trying to export scop " << endl;
    osl_scop_p osl_scop = osl_scop_malloc ();
    osl_scop_dump (stderr, osl_scop); 
    osl_scop_free (osl_scop);
  }

  void parse_command_line_arguments (int narg, char * argv[])
  {
    int aa;
    // Set the solver
    for (aa = 1; aa < narg && argv[aa]; aa++)
    {
      char * needle = strstr (argv[aa], "--solver");
      int ret = select_solver (argv[aa]);
      if (!ret)
      {
        printf ("WARNING: Solver not recognized. Using default solver (%s)\n",
          get_solver_name ());
      }
      else
      {
        printf ("AXL [INFO]: Using solver %s\n", get_solver_name ());
      }
    }
  }
};

#endif
