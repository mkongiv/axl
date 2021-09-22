/*
AQCSL: the Affine Quantum Circuit Specification Language
Filename: aql_statement.hh
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

#ifndef AQCL_STATEMENT
#define AQCL_STATEMENT

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <utility>
#include "utils.hh"
#include "circuit-schedule.hh"
#include "converter.hh"

#include "aql_domain.hh"
#include "aql_variable.hh"
#include "aql_block.hh"

#include "isl/ctx.h"
#include "isl/set.h"
#include "isl/map.h"
#include "isl/union_map.h"
#include "isl/union_set.h"
#include "isl/options.h"
#include "isl/ast.h"
#include "isl/ast_build.h"
#include "isl/flow.h"
#include "isl/schedule.h"

#include "osl/scop.h"
#include "irconverter/osl2scoplib.h"
#include "ponos/ponos.h"


using namespace std;

#define AXL_TRANSFO_PLUTO 1
#define AXL_TRANSFO_FEAUTRIER 2
#define AXL_TRANSFO_ISS_STAIRS 3
#define AXL_TRANSFO_REVERSE_TIME 4
#define AXL_TRANSFO_QUBIT_PERM 5
#define AXL_TRANSFO_PLUTO_MINFUSE 6
#define AXL_TRANSFO_PLUTO_MAXFUSE 7
#define AXL_AQUMA_FORMAT 10

#define AXL_SOLVER_PIPLIB_GMP 0
#define AXL_SOLVER_ISL 1
#define AXL_SOLVER_CPLEX 2
#define AXL_SOLVER_PIPLIB 3

#define QBE_QASM 1
#define QBE_RIGETTI 2
#define QBE_PROJQ 3

class aqcl_statement;

typedef vector<aqcl_statement*> vector_statement;
typedef vector<aqcl_statement*>::iterator statement_iterator;

class aqcl_statement : public aqcl_block
{
  protected:
  vector<aqcl_domain*> m_domain;
  int m_id;
  isl_set * m_idom;
  isl_map * m_sched;
  aqcl_symtab * m_partab;
  isl_ctx * m_ctx;
  isl_union_map * m_usched;
  isl_union_set * m_uidom;
  isl_union_map * m_accread;
  isl_union_map * m_accwrite;
  isl_union_map * m_call; // call to quantum gate
  string m_ctx_str;
  vector<aqcl_statement*> m_scop_stmts; // used to store statements involved in scop assembly operations (codegen, apply, export_scop)
  double m_codegen_time;
  double m_depanal_time;
  double m_sched_time;
  int m_depanal_calls;
  int m_backend;
  int m_solver;
  int m_prog_deps;

  public:

  aqcl_statement (int p_id, aqcl_symtab * p_partab, isl_ctx * p_ctx) : aqcl_block ()
  {
    m_node_category = AQL_NODE_STMT;
    m_vartype = SEMTYPE_STMT;
    m_id = p_id;
    m_name = "S" + std::to_string(m_id);
    //m_domain = NULL;
    m_res = NULL;  // statement body, a qircuit
    m_idom = NULL;
    m_sched = NULL;
    m_partab = p_partab;
    m_ctx = p_ctx;
    m_usched = NULL;
    m_uidom = NULL;
    m_accread = NULL;
    m_accwrite = NULL;
    m_call = NULL;
    m_ctx_str = "";
    m_backend = QBE_QASM;
    m_codegen_time = 0.0;
    m_depanal_time = 0.0;
    m_sched_time = 0.0;
    m_depanal_calls = 0;
  }

  aqcl_statement (string p_name, int p_id, aqcl_symtab * p_partab, isl_ctx * p_ctx) : aqcl_block ()
  {
    m_node_category = AQL_NODE_STMT;
    m_vartype = SEMTYPE_STMT;
    m_id = p_id;
    m_name = p_name;
    //m_domain = NULL;
    m_res = NULL; // statement body, a qircuit
    m_idom = NULL;
    m_sched = NULL;
    m_accread = NULL;
    m_accwrite = NULL;
    m_usched = NULL;
    m_uidom = NULL;
    m_partab = p_partab;
    m_ctx = p_ctx;
    m_ctx_str = "";
    m_backend = QBE_QASM;
    m_codegen_time = 0.0;
    m_depanal_time = 0.0;
    m_sched_time = 0.0;
    m_depanal_calls = 0;
  }

  /*
  aqcl_statement (string p_name, int p_id, aqcl_symtab * p_partab) : aqcl_block ()
  {
  }
  */

  ~ aqcl_statement ()
  {

  }

  int get_solver ()
  {
    return m_solver;
  }

  void set_solver (int p_solver)
  {
    m_solver = p_solver;
  }

  void set_backend (int p_backend)
  {
    m_backend = p_backend;
  }

  void set_name (string p_name)
  {
    m_name = p_name;
  }

  void set_run_name (int op_num)
  {
    stringstream st;
    st << "R" << op_num;
    m_name = st.str ();
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
      // b) parameteric 
      // c) user-provided value
      ret += ss->second->get_name () + " = 12 ";
    }
    return ret;
  }


  __isl_give isl_set * create_context_set ()
  {
    isl_set * ret;
    string ctx_str = "";
    string param_str = this->get_isl_param_str ();
    ctx_str += param_str;
    ctx_str += " -> { : ";
    ctx_str += m_ctx_str;//this->get_isl_ctx_param_str ();
    ctx_str += " }";
    cout << "Showing ctx string: " << ctx_str << endl;
    ret = isl_set_read_from_str (m_ctx, ctx_str.c_str());
    return ret;
  }

  __isl_give isl_map * get_isl_schedule_map ()
  {
    return isl_map_copy (m_sched);
  }

  __isl_give isl_union_map * get_isl_union_schedule_map ()
  {
    assert (m_usched && "aql_statement.hh: get_isl_union_schedule_map ()");
    return isl_union_map_copy (m_usched);
  }

  __isl_give isl_set * get_isl_iteration_domain ()
  {
    return isl_set_copy (m_idom);
  }


  __isl_give isl_union_set * get_isl_union_iteration_domain ()
  {
    assert (m_uidom && "aql_statement.hh: get_isl_union_iteration_domain ()");
    return isl_union_set_copy (m_uidom);
  }

  __isl_give isl_union_map * get_isl_read_access_relations ()
  {
    string err_msg = "aql_statement.hh: Read access relation not defined for statement " + m_name;
    if (!m_accread)
    {
      cout << err_msg << endl;
      print ();
      exit (1);
    }
    return isl_union_map_copy (m_accread);
  }

  __isl_give isl_union_map * get_isl_write_access_relations ()
  {
    string err_msg = "aql_statement.hh: Write access relation not defined for statement " + m_name;
    assert (m_accwrite && err_msg.c_str ());
    return isl_union_map_copy (m_accwrite);
  }

  __isl_give isl_union_map * get_isl_call_access_relations ()
  {
    string err_msg = "aql_statement.hh: quantum gate not defined for statement" + m_name;
    assert (m_call && err_msg.c_str ());
    return isl_union_map_copy (m_call);
  }

  __isl_give isl_union_map * get_isl_all_access_relations ()
  {
    string err_msg = "aql_statement.hh: Read/Write access relation not defined for statement " + m_name;
    assert (m_accwrite && m_accread && err_msg.c_str ());
    return isl_union_map_union (
      get_isl_write_access_relations (),
      get_isl_read_access_relations ());
  }




  void canonicalize_dimensions ()
  {
    int nd = m_domain.size ();
    int ii;
    int dim_off = 0;
    for (ii = 0; ii < nd; ii++)
    {
      cout << "Canonicalizing domain " << ii << endl;
      dim_off += m_domain[ii]->canonicalize_dimensions (dim_off);
    }
  }

  string get_declared_tuple_string ()
  {
    string ret = "";
    if (m_domain.size () == 1)
    {
      ret += m_domain[0]->get_declared_tuple_string ();
    }
    else
    {
      int nd = m_domain.size ();
      int ii;
      string tuple_str = "";
      for (ii = 0; ii < nd; ii++)
      {
        if (ii > 0)
        {
          tuple_str += ", ";
        }
        tuple_str += m_domain[ii]->get_declared_tuple_string ();  //get_tuple_string ();
      }
      ret += tuple_str;
    }
    return ret;
  }

  string get_domain_string ()
  {
    string ret = "";
    if (m_domain.size () == 1)
    {
      ret += m_domain[0]->get_tuple_string ();
      ret += " : ";
      ret += m_domain[0]->get_constraint_string ();
    }
    else
    {
      int nd = m_domain.size ();
      int ii;
      string tuple_str = "";
      string const_str = "";
      for (ii = 0; ii < nd; ii++)
      {
        if (ii > 0)
        {
          tuple_str += ", ";
          const_str += " and ";
        }
        tuple_str += m_domain[ii]->get_declared_tuple_string ();  //get_tuple_string ();
        const_str += m_domain[ii]->get_constraint_string ();
      }
      ret += "[" + tuple_str + " ] : " + const_str;
    }
    return ret;
  }



  virtual string get_string ()
  {
    string stmt_id = this->get_name ();
    string ret = "{ ";
    ret += stmt_id;
    ret += get_domain_string ();
    ret += " }";
    return ret;
  }


  int get_category ()
  {
    return m_node_category;
  }

  bool is_same_category (int p_cat)
  {
    return m_node_category == p_cat;
  }

  virtual string get_instruction_str ()
  {
    string ret = "";
    ret += get_operator_string ();
    ret += ", ";
    ret += get_name ();
    ret += ",";
    ret += m_res->get_name ();
    ret += ", ";
    //ret += m_domain->get_string ();
    ret += get_string ();
    return ret;
  }

  void set_operator ()
  {
    assert (m_domain.size () > 0);
    int nd = m_domain.size ();
    m_opcode = m_domain[nd-1]->get_opcode ();
  }

  virtual /*aqcl_node*/ aqcl_block * get_result ()
  {
    return m_res;
  }

  virtual string get_result_name ()
  {
    return get_name ();
  }

  virtual string get_name ()
  {
    return m_name;
  }

  virtual bool is_statement ()
  {
    return true;
  }

  virtual bool is_stmt ()
  {
    return is_statement ();
  }

  virtual bool is_circ ()
  {
    return false;
  }


  virtual bool is_block ()
  {
    return false;
  }

  virtual void print_info ()
  {
    cout << "Statement : " << m_name << " : " <<
      is_stmt () << ", " << get_semtype () << endl;
  }

  void set_domain (aqcl_domain * p_domain)
  {
    // The statement class inherits from block, which inherits from
    // the goperator class.
    // The proposed syntax in grammar.y defines the operator type as
    // a property of the domain, and not of the statement.
    // Hence, once we attach the domain to the statement, we also
    // invoke set_operator method, which retrieves the opcode from the
    // domain.
    m_domain.push_back (p_domain);
    set_operator ();
  }

  void set_domain (vector<aqcl_domain*> & p_domain)
  {
    // The statement class inherits from block, which inherits from
    // the goperator class.
    // The proposed syntax in grammar.y defines the operator type as
    // a property of the domain, and not of the statement.
    // Hence, once we attach the domain to the statement, we also
    // invoke set_operator method, which retrieves the opcode from the
    // domain.
    vector<aqcl_domain*>::iterator ii;
    for (ii = p_domain.begin (); ii != p_domain.end (); ii++)
    {
      m_domain.push_back (*ii); 
    }
    set_operator ();
  }

  aqcl_domain * get_domain (int idx)
  {
    assert (idx < m_domain.size ());
    return m_domain[idx];
  }


  vector<aqcl_domain*> & get_domain ()
  {
    return m_domain;
  }

  void set_body (aqcl_block * p_body)
  {
    cout << endl;
    m_res = p_body;
  }

  aqcl_block * get_body ()
  {
    return m_res;
  }



  /*
   * This method attaches the domain and body of a statement to a particular
   * IR instance.
   *
   */
  virtual int run ()
  {
    cout << "In aqcl_statement::run ()" << endl;
    cout << "Will instantiate statement: " <<
      get_string () << endl;
    if (m_res->get_result_block ()->is_statement ())
    {
      aqcl_statement * dst_stmt = (aqcl_statement*)(m_res->get_result_block ());
      set_domain (dst_stmt->get_domain ());
      set_body (dst_stmt->get_body ());
      canonicalize_dimensions ();
    }
    instantiate_isl_objects ();
    print ();
    return 0;
  }

  void print_domain ()
  {
    cout << get_domain_string () << endl;
  }

  virtual void print ()
  {
    cout << "Statement " << m_name << endl;
    //cout << "Is the body really a statement ? " << m_res->is_statement () << " (" << m_res->get_name () << ")" << endl;
    cout << get_string () << endl;
    if (m_res)
      cout << "Dimensionality : " << m_res->get_dim () << endl;
    else
      cout << "result is null" << endl;
    cout << "Operator type  : " << get_operator_string () << endl;

    if (m_res)
    {
      if (m_res->is_statement ())
      {
        cout << "Body of statement is a statement" << endl;
      }
      else if (m_res->is_block ())
      {
        cout << "Body of statement is a circuit " << endl;
      }
      else {
        assert (0 && "Unexpected case in aqcl_statement::get_domain ()");
      }
    }


    if (m_tl || m_tr || m_bl) {
      cout << "Body : { " << endl;
      if (m_tl)
        m_tl->print ();
      if (m_tr)
        m_tr->print ();
      if (m_bl)
        m_bl->print ();
      cout << endl << "}" << endl;
    }
    else if (m_res)
    {
      //m_res->print ();
      //cout << m_res->get_expression_plain ();
      m_res->aqcl_block::print ();
      cout << endl;
    }
    else
      cout << "--> Circuit body was not found!!!" << endl;

    if (m_domain.size () > 0)
    {
      cout << "Constraints : ";
      //m_domain->print ();
      print_domain ();
      cout << endl;
      //m_domain->show_individual_constraints ();
    }
    else
    {
      cout << "--> Circuit domain not found!!!!" << endl;
    }

    cout << "Iteration domain: " << build_iteration_domain_string () << endl;
    cout << "Read access relation: " << get_read_access_relation () << endl;;
    cout << "Write access relation: " << get_write_access_relation () << endl;;
    cout << "Schedule : " << build_schedule_string () << endl;
    cout << endl;
    cout << "ISL objects: " << endl;

    print_isl_union_set_debug (m_uidom, "ISL Iteration domain");
    print_isl_union_map_debug (m_usched, "ISL schedule map");
    print_isl_union_map_debug (m_accread, "ISL read access relations");
    print_isl_union_map_debug (m_accwrite, "ISL write access relations");
    print_isl_union_map_debug (m_call, "ISL call access relations");

  }

  void show_subcircuit_as_grid (map<pair<int,int>,aqcl_block*> & gm)
  {
    int dim = m_res->get_dim ();
    int width = m_res->get_width ();
    aqcl_block * grid[dim][width];
    int ss;
    int tt;
    for (ss = 0; ss < dim; ss++)
      for (tt = 0; tt < width; tt++)
        grid[ss][tt] = NULL;

    map<pair<int,int>,aqcl_block*>::iterator ii;
    for (ii = gm.begin (); ii != gm.end (); ii++)
    {
      int row = ii->first.first;
      int col = ii->first.second;
      grid[row][col] = ii->second;
    }

    for (ss = 0; ss < dim; ss++)
    {
      for (tt = 0; tt < width; tt++)
        if (grid[ss][tt])
      {
        cout << grid[ss][tt]->get_body ()->get_string () << " ";
      }
      cout << "; ";
      cout << endl;
    }
  }

  string get_read_access_relation (string sname = "")
  {
    return get_access_relation (true, sname);
  }

  string get_write_access_relation (string sname = "")
  {
    // 02/20/2020 (MK): 
    // Ugly hack to produce both write references for a SWAP operation.
    // We query the opcode, and if it's a swap, then we call the
    // read version.
    
    // FIXME: must add other gate types right here.
    if (m_res && m_res->get_body () && m_res->get_body ()->get_gt () &&
        m_res->get_body ()->get_gt () == GATE_SWAP)
      return get_access_relation (true, sname);
    return get_access_relation (false, sname);
  }

  virtual string get_access_relation (bool use_read, string sname = "")
  {
    int dim = m_res->get_dim ();
    int width = m_res->get_width ();
    map<pair<int,int>,aqcl_block*> gm;
    map<pair<int,int>,aqcl_block*>::iterator ii;
    m_res->get_result_block ()->get_locations (sname, 0, 0, gm);
    for (ii = gm.begin (); ii != gm.end (); ii++)
    {
    }

    cout << endl;
    show_subcircuit_as_grid (gm);

    // Build the domain constraints to attach to the access relations

    int nd = m_domain.size ();
    int dd;
    string domcons = "";
    for (dd = 0; dd < nd; dd++)
    {
      if (dd > 0)
        domcons += " and ";
      domcons += m_domain[dd]->get_constraint_string ();
    }

    string tuple_str = get_declared_tuple_string ();
    stringstream ret;
    ret << "{ ";
    if (gm.size () > 0)
    {
      for (ii = gm.begin (); ii != gm.end (); ii++)
      {
        vector<string> daf; // dimension access function
        if (use_read)
          daf = ii->second->get_read_access_expression_str ();
        else
          daf = ii->second->get_write_access_expression_str ();
        vector<string>::iterator ss;
        if (daf.size () > 0)
        {
          int stmt_idx = 0;
          for (ss = daf.begin (); ss != daf.end (); ss++)
          {
            ret << this->get_name () + "_" + ii->second->get_body ()->get_name_as_space ();
            cout << "Found " << ret.str () << endl;
            ret << "[ ";
            ret << tuple_str;
            ret << ", ";
            ret << ii->first.first;
            ret << ", ";
            ret << ii->first.second;
            ret << " ] -> q[ ";
            ret << ii->first.first;
            ret << *ss;
            ret << " ] : ";
            ret << domcons;
            ret << "; ";
          }
        }
        else
        {
            ret << this->get_name () + "_" + ii->second->get_body ()->get_name_as_space ();
            ret << "[ ";
            ret << tuple_str;
            ret << ", ";
            ret << ii->first.first;
            ret << ", ";
            ret << ii->first.second;
            ret << " ] -> q[ ";
            ret << ii->first.first;
            //ret << *ss;
            ret << " ] : ";
            ret << domcons;
            ret << "; ";

        }
      }
    }
    else
    {
      assert (0);
    }
    ret << " }";
    cout << "Access relations (" << use_read << ") ::: " << ret.str () << endl;
    return ret.str ();
  }


  virtual string get_gate_call (string sname = "")
  {
    int dim = m_res->get_dim ();
    int width = m_res->get_width ();
    map<pair<int,int>,aqcl_block*> gm;
    map<pair<int,int>,aqcl_block*>::iterator ii;
    m_res->get_result_block ()->get_locations (sname, 0, 0, gm);

    cout << endl;
    show_subcircuit_as_grid (gm);

    // Build the domain constraints to attach to the access relations

    int nd = m_domain.size ();
    int dd;
    string domcons = "";
    for (dd = 0; dd < nd; dd++)
    {
      if (dd > 0)
        domcons += " and ";
      domcons += m_domain[dd]->get_constraint_string ();
    }

    string tuple_str = get_declared_tuple_string ();
    stringstream ret;
    ret << "{ ";
    for (ii = gm.begin (); ii != gm.end (); ii++)
    {
      ret << get_name () + "_" + ii->second->get_body ()->get_name_as_space ();
      ret << "[ ";
      ret << tuple_str;
      ret << ", ";
      ret << ii->first.first;
      ret << ", ";
      ret << ii->first.second;
      ret << " ] -> ";
      string op_name = ii->second->get_body ()->get_name ();
      const char * op_str = op_name.c_str ();
      char * ptr;
      ptr  = (char*)(op_str);
      ptr++;
      ret << ptr;
      ret << "[ ";
      string space_dim = ii->second->get_gate_call_str ();
      if (space_dim == "")
        ret << ii->first.first;
      else
        ret << space_dim;
      //ret << ii->first.first;
      ret << " ] : ";
      ret << domcons;
      ret << "; ";
    }
    ret << " }";

    return ret.str ();
  }

  void set_parameter_table (aqcl_symtab * p_partab)
  {
    if (p_partab)
      m_partab = p_partab;
  }

  aqcl_symtab * get_parameter_table ()
  {
    return m_partab;
  }

  string build_schedule_string ()
  {
    stringstream ret;
    int nd = m_domain.size ();
    int dd;
    ret << "[";
    ret << m_partab->get_symbol_list ();
    ret << "]";
    ret << " -> ";
    ret << "{";
    for (dd = 0; dd < nd; dd++)
    {
      string tuple_str = m_domain[dd]->get_declared_tuple_string ();
      ret << " ";
      ret << get_name ();
      ret << "[";
      ret << tuple_str;
      ret << " ]";
      ret << " -> ";
      ret << "[";
      ret << tuple_str;
      ret << " ]";
      ret << "; ";
    }
    ret << " }";
    return ret.str ();
  }

  string build_iteration_domain_string ()
  {
    stringstream ret;
    int nd = m_domain.size ();
    int dd;
    ret << "[";
    ret << m_partab->get_symbol_list ();
    ret << "]";
    ret << " -> ";
    ret << "{";
    for (dd = 0; dd < nd; dd++)
    {
      string tuple_str = m_domain[dd]->get_declared_tuple_string ();
      ret << " ";
      ret << get_name ();
      ret << "[";
      ret << tuple_str;
      ret << " ]";
      ret << " : ";
      ret << m_domain[dd]->get_constraint_string ();
      ret << "; ";
    }
    ret << " }";
    return ret.str ();
  }

  string build_access_relation_string (bool use_read)
  {
    stringstream ret;
    ret << "[";
    ret << m_partab->get_symbol_list ();
    ret << "]";
    ret << " -> ";
    // 02/20/2020 (MK): Ugly hack to fix missing writes of SWAP
    // Check whether the gate is a SWAP, if so, return the same READ relation.
    // Otherwise, do the usual stuff.
    if (m_res && m_res->get_body () && m_res->get_body ()->get_gt () &&
        m_res->get_body ()->get_gt () == GATE_SWAP)
      ret << get_access_relation (true); 
    else
      ret << get_access_relation (use_read); 
    return ret.str (); 
  }

  string build_gate_call_string ()
  {
    stringstream ret;
    ret << "[";
    ret << m_partab->get_symbol_list ();
    ret << "]";
    ret << " -> ";
    ret << this->get_gate_call (); 
    return ret.str (); 
  }

  __isl_give isl_set *
  build_iteration_domain_from_access_relation (__isl_keep isl_union_map * accrel)
  {
    isl_union_set * dom;
    assert (accrel);
    dom = isl_union_map_domain (isl_union_map_copy (accrel));
    return isl_set_from_union_set (dom);
  }

  __isl_give isl_map *
  build_schedule_map_from_iteration_domain (__isl_keep isl_set * idom)
  {
    assert (idom);
    isl_map * smap;
    smap = isl_set_identity (isl_set_copy (idom));
    smap = isl_map_set_tuple_name (smap, isl_dim_out, "");
    return smap;
  }

  /*
  * Instantiate all the ISL objects of the current statement (scop).
  *
  */
  void instantiate_isl_objects ()
  {
    if (!m_accread)
    {
      m_accread = isl_union_map_read_from_str (m_ctx, build_access_relation_string (true).c_str ());
      //m_accread = isl_union_map_coalesce (m_accread);
    }
    if (!m_accwrite)
    {
      m_accwrite = isl_union_map_read_from_str (m_ctx, build_access_relation_string (false).c_str ());
      //m_accwrite = isl_union_map_coalesce (m_accwrite);
    }
    if (!m_call)
    {
      string call_str = this->build_gate_call_string ();
      m_call = isl_union_map_read_from_str (m_ctx, call_str.c_str ());
      //m_call = isl_union_map_coalesce (m_call);
    }    
    if (!m_uidom)
    {
      //m_idom = isl_set_read_from_str (m_ctx, build_iteration_domain_string ().c_str ());
      isl_union_map * access;

      access = isl_union_map_union (
        isl_union_map_copy (m_accread), isl_union_map_copy (m_accwrite));

      m_uidom = isl_union_map_domain (access); 

    }
    if (!m_usched)
    {
      m_usched = axl_build_schedule_map_from_iteration_domain (m_uidom);
    }
    if (m_scop_stmts.empty ())
    {
      m_scop_stmts.push_back (this);
    }
  }

  void remove_redundant_dimensions (t_mini_scop * mscop)
  {
  }

  void schedule_insert_fixed_dimension (int posdim, int si)
  {
    instantiate_isl_objects ();
    isl_val * val = isl_val_int_from_si (m_ctx, si);
    m_sched = isl_map_insert_dims (m_sched, isl_dim_out, posdim, 1);
    m_sched = isl_map_fix_val (m_sched, isl_dim_out, posdim, val);
  }

  /*
   * Perform the [+] and (+) operation. Method invoked from aqcl_sexpr::run ().
   *
   */
  void schedule_composition (aqcl_statement * s1, aqcl_statement * s2, int p_op_num)
  {
    //instantiate_isl_objects ();
    assert (s1);
    assert (s2);
    isl_union_map * udom1 = s1->get_isl_union_schedule_map ();
    isl_union_map * udom2 = s2->get_isl_union_schedule_map ();
    t_foreach data;
    data.umap = NULL;
    data.pos = 0;
    isl_stat res;
    data.scal = 0;
    res = isl_union_map_foreach_map (
      udom1, &insert_scalar_dimensions, (void*)(&data));
    data.scal = 1;
    res = isl_union_map_foreach_map (
      udom2, &insert_scalar_dimensions, (void*)(&data));

    m_usched = data.umap;
    m_uidom = isl_union_set_union (
      s1->get_isl_union_iteration_domain (),
      s2->get_isl_union_iteration_domain ());
    m_accread = isl_union_map_union (
      s1->get_isl_read_access_relations (),
      s2->get_isl_read_access_relations ());
    m_accwrite = isl_union_map_union (
      s1->get_isl_write_access_relations (),
      s2->get_isl_write_access_relations ());
    m_call = isl_union_map_union (
      s1->get_isl_call_access_relations (),
      s2->get_isl_call_access_relations ());

    s1->set_run_name (2 * p_op_num);
    s2->set_run_name (2 * p_op_num + 1);
    this->append_statements_from (s1);
    this->append_statements_from (s2);

    isl_union_map_free (udom1);
    isl_union_map_free (udom2);
  }


  __isl_give
  isl_union_map * 
  build_isl_iss_filter (__isl_keep isl_set * set, int split_dim, int dep_dist)
  {
    string ret = "";
    int dd;
    string param_list = "";
    param_list += "[";
    param_list += m_partab->get_symbol_list ();
    param_list += "] -> ";
    isl_union_map * filters = NULL;
    // Oh yes, reminder: we have to construct @dep_dist maps.
    // Each one represents one partition of the ISS with residual class dd (below)
    int real_dd = 0;
    int abs_dep_dist = (dep_dist > 0 ? dep_dist : - dep_dist);
    for (dd = 0, real_dd = 0; dd < abs_dep_dist; dd++)
    {
      string filter_str = param_list;
      filter_str += axl_build_isl_iss_filter_str (set, split_dim, abs_dep_dist, dd);
      isl_map * filter_map = isl_map_read_from_str (m_ctx, filter_str.c_str ());
      isl_union_map * fltr_umap = isl_union_map_from_map (filter_map);
      if (filters)
        filters = isl_union_map_union (filters, fltr_umap);
      else
        filters = fltr_umap;
      if (dep_dist > 0)
        real_dd++;
      else 
        real_dd--;
    }
    // Do something with the <filters> union_map
    return filters;
  }

  t_mini_scop * get_mini_scop ()
  {
    t_mini_scop * ret = axl_mini_scop_alloc ();
    ret->domain = get_isl_union_iteration_domain ();
    ret->schedule = get_isl_union_schedule_map ();
    ret->read = get_isl_read_access_relations ();
    ret->write = get_isl_write_access_relations ();
    ret->call = get_isl_call_access_relations ();
    return ret;
  }

  vector<aqcl_statement*> & get_scop_stmts ()
  {
    return m_scop_stmts;
  }

  void show_scop_stmts ()
  {
    cout << "Showing statements in scop:" << endl;
    vector<aqcl_statement*>::iterator ss;
    for ( ss = m_scop_stmts.begin (); ss != m_scop_stmts.end (); ss++)
    {
      cout << "Statement in scop: " << (*ss)->get_statement_body_str () << endl;
    }
  }

  void append_statements_from (aqcl_statement * src)
  {
    vector<aqcl_statement*>::iterator ss;
    for ( ss = src->get_scop_stmts ().begin ();
          ss != src->get_scop_stmts ().end ();
          ss++)
    {
      m_scop_stmts.push_back (*ss);
    }
  }

  void init_scop_stmts ()
  {
    m_scop_stmts.push_back (this);
  }

  t_mini_scop * apply_stair_iss (vector<t_iss_data*> & iss)
  {
    vector<t_iss_data*>::iterator ii;
    isl_union_map * iss_filter = NULL;
    isl_union_set * affected_doms = NULL;
    int nn = import_isl_sets_for_iss (m_uidom, &iss);
    // Build the union map containing all the ISS residual class filters.
    // In the end we should have \sum @data->dist maps in the union.
    for (ii = iss.begin (); ii != iss.end (); ii++)
    {
      t_iss_data* data = *ii;
      isl_union_map * filter;
      filter = build_isl_iss_filter (data->set, data->dim, data->dist);
      if (iss_filter)
        iss_filter = isl_union_map_union (iss_filter, filter);
      else
        iss_filter = filter;
      if (affected_doms)
        affected_doms = isl_union_set_union (affected_doms,
          isl_union_set_from_set (data->set));
      else
        affected_doms = isl_union_set_from_set (data->set);
    }
    // Note: all the data->set field of iss will be non-usable from here on.
    t_mini_scop * old_scop = get_mini_scop ();
    //axl_mini_scop_show (old_scop, "Original scop");
    iss_filter = isl_union_map_intersect_domain (iss_filter,
      isl_union_set_copy (affected_doms));
    old_scop->filter = iss_filter;
    t_mini_scop * new_scop = axl_iss_stair_pattern (old_scop);
    axl_mini_scop_show (new_scop, "ISS-ed scop");
    axl_mini_scop_append_unaffected_objects (old_scop, new_scop, affected_doms);
    axl_mini_scop_free (old_scop); 
    // Note: filters will be freed inside axl_mini_scop_free.
    return new_scop;
  }
  

  virtual string get_class_name ()
  {
    return "aql_statement";
  }

  void copy_isl_objects (aqcl_statement * src)
  {
    m_idom = src->get_isl_iteration_domain ();
    m_sched = src->get_isl_schedule_map ();
    m_uidom = src->get_isl_union_iteration_domain ();
    m_usched = src->get_isl_union_schedule_map ();
    m_accread = src->get_isl_read_access_relations ();
    m_accwrite = src->get_isl_write_access_relations ();
    m_call = src->get_isl_call_access_relations ();
  }

  /*
  * Codegen and transformations. Relocated from aql_program.hh on June 21, 2019.
  *
  */

  void codegen_c (__isl_keep isl_union_set * doms, 
    __isl_keep isl_union_map * smap, string fileout = "test.axl.c")
  {
    FILE * fout = fopen (fileout.c_str (), "w");
    isl_options_set_ast_build_allow_else (m_ctx, 0);
    isl_options_set_ast_build_allow_or (m_ctx, 0);
    isl_options_set_ast_always_print_block (m_ctx, 1);
    isl_set * context = this->create_context_set ();

    isl_ast_build * build = isl_ast_build_from_context (context);

    isl_printer * printer = isl_printer_to_file (m_ctx, fout);
    printer = isl_printer_set_output_format (printer, ISL_FORMAT_C);
    isl_ast_print_options * options = isl_ast_print_options_alloc (m_ctx);

    isl_union_map * schedule_map = isl_union_map_copy (smap);
    //schedule_map = create_identity_schedule_map ();

    // FIXME: use this to print each specific gate
    options = isl_ast_print_options_set_print_user (options, &axl_stmt_print_user, (void*)(this));
    t_mini_scop * scop = this->get_mini_scop ();
    scop->backend = this->m_backend;
    build = isl_ast_build_set_at_each_domain (build, &axl_at_each_domain, (void*)(scop));

    print_isl_union_map_debug (schedule_map, "Schedule map used in isl_ast_build_ast_from_schedule ()");
    isl_ast_node * root = isl_ast_build_ast_from_schedule (build, schedule_map);

    printer = isl_ast_node_print (root, printer, options);
    printer = isl_printer_flush (printer);

    isl_ast_build_free (build);
    isl_printer_free (printer);
    axl_mini_scop_free (scop);

    fclose (fout);
  }


  __isl_give isl_union_flow * compute_dependences (
    __isl_keep isl_union_map * smap,
    __isl_keep isl_union_map * writes,
    __isl_keep isl_union_map * reads)
  {
    double t_start = axl_time ();
    isl_union_map * all;
    all = isl_union_map_union (
      isl_union_map_copy (writes), isl_union_map_copy (reads));
    isl_union_access_info * accinfo;
    accinfo = isl_union_access_info_from_sink (
      isl_union_map_copy (all));
    accinfo = isl_union_access_info_set_must_source (accinfo, all);

    accinfo = isl_union_access_info_set_schedule_map (accinfo, 
      isl_union_map_copy (smap));
    isl_union_flow * ret = isl_union_access_info_compute_flow (accinfo);
    double t_stop = axl_time ();
    m_depanal_time += t_stop - t_start;
    m_depanal_calls++;
    return ret;
  }

  __isl_give isl_schedule * compute_transformation (
    __isl_keep isl_union_set * doms,
    __isl_keep isl_union_map * smap, // actually not used here!
    __isl_keep isl_union_flow * deps,
    __isl_keep isl_set * context,
    int p_sched_algo = AXL_TRANSFO_PLUTO)
  {
    switch (p_sched_algo)
    {
      case AXL_TRANSFO_PLUTO:
        assert (isl_options_set_schedule_algorithm (m_ctx, ISL_SCHEDULE_ALGORITHM_ISL) == isl_stat_ok);
        break;
      case AXL_TRANSFO_PLUTO_MINFUSE:
        assert (isl_options_set_schedule_algorithm (m_ctx, ISL_SCHEDULE_ALGORITHM_ISL) == isl_stat_ok);
        isl_options_set_schedule_serialize_sccs (m_ctx, 1);
        break;
      case AXL_TRANSFO_PLUTO_MAXFUSE:
        assert (isl_options_set_schedule_algorithm (m_ctx, ISL_SCHEDULE_ALGORITHM_ISL) == isl_stat_ok);
        isl_options_set_schedule_whole_component (m_ctx, 1);
        isl_options_set_schedule_maximize_band_depth (m_ctx, 1);
        break;
      case AXL_TRANSFO_FEAUTRIER:
        assert (isl_options_set_schedule_algorithm (m_ctx, ISL_SCHEDULE_ALGORITHM_FEAUTRIER) == isl_stat_ok);
        //isl_options_set_schedule_split_scaled (m_ctx, 1);
        break;
    }

    // Collect the union maps representing all the types of dependences
    isl_union_map * must = isl_union_flow_get_must_dependence (deps);
    isl_union_map * may = isl_union_flow_get_may_dependence (deps);
    isl_union_map * fullmust = isl_union_flow_get_full_must_dependence (deps);
    isl_union_map * fullmay = isl_union_flow_get_full_may_dependence (deps);
    isl_union_map * mustnosource = isl_union_flow_get_must_no_source (deps);
    // Build the constraints
    isl_schedule_constraints * cons;

    cons = isl_schedule_constraints_on_domain (isl_union_set_copy (doms));
    cons = isl_schedule_constraints_set_context (cons, isl_set_copy (context));
    cons = isl_schedule_constraints_set_validity (cons, 
      isl_union_map_copy (must));
    isl_union_map * must_and_may;
    must_and_may = isl_union_map_union (
      isl_union_map_copy (must),
      isl_union_map_copy (may)
    );

    m_prog_deps = isl_union_map_n_map (must);

    isl_union_map_free (must);
    isl_union_map_free (fullmust);
    isl_union_map_free (fullmay);
    isl_union_map_free (mustnosource);
    isl_schedule * sched;
    sched = isl_schedule_constraints_compute_schedule (cons);
    return sched;
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

  void set_ponos_solver (s_ponos_options_t * popts)
  {
    switch (m_solver) 
    {
      case AXL_SOLVER_PIPLIB:
        popts->pipsolve_gmp = 0;
        break;
      case AXL_SOLVER_PIPLIB_GMP:
        popts->pipsolve_gmp = 1;
        break;
      case AXL_SOLVER_CPLEX:
        popts->solver = PONOS_SOLVER_CPLEX;
        popts->solver_cplex = 1;
        break;
      case AXL_SOLVER_ISL:
        assert (0 && "AXL: Pending to implement ISL solver for ponos");
        break;
    }
  }

  void
  compute_transformation (t_mini_scop * mscop, 
    vector<aqcl_statement*> * stmts, __isl_keep isl_set * context, int p_algo)
  {
  }


  int codegen (bool use_decomp)
  {
    vector<aqcl_statement*> scop;
    // NOTE: m_ctx_str will be used in codegen_c
    m_ctx_str = this->get_parameter_table ()->get_parameter_string_values ();

    isl_union_set * domains = this->get_isl_union_iteration_domain ();
    isl_union_map * schedule_map = this->get_isl_union_schedule_map ();

    schedule_map = axl_union_map_pad (schedule_map);
    schedule_map = isl_union_map_intersect_domain (
      schedule_map, isl_union_set_copy (domains));


    double t_start = axl_time ();
    this->codegen_c (domains, schedule_map, "axl.trans.c");
    double t_stop = axl_time ();
    m_codegen_time = t_stop - t_start;


    print_stats ();

    isl_union_set_free (domains);
    isl_union_map_free (schedule_map);

    //this->isl_plot ();
    
    // Build the dependence graph and print a dot file (axl.dot)
    //print_dependence_graph_as_dot ();
    return 0;
  }

  int aquma_format_dump ()
  {
    FILE * ff = fopen ("aquma.scop", "w");
    isl_union_set * domains = this->get_isl_union_iteration_domain ();
    isl_union_map * schedule_map = this->get_isl_union_schedule_map ();

    isl_set * context = this->create_context_set ();

    domains = isl_union_set_gist_params (domains, isl_set_copy (context));
    //domains = isl_union_set_gist (domains, isl_union_set_from_set (context));

    schedule_map = axl_union_map_pad (schedule_map);
    schedule_map = isl_union_map_intersect_domain (
      schedule_map, isl_union_set_copy (domains));
    schedule_map = isl_union_map_gist_params (schedule_map, isl_set_copy (context));

    isl_union_map * reads = this->get_isl_read_access_relations ();
    isl_union_map * writes = this->get_isl_write_access_relations ();

    reads = isl_union_map_intersect_domain (reads, isl_union_set_copy (domains));
    writes = isl_union_map_intersect_domain (writes, isl_union_set_copy (domains));

    reads = isl_union_map_gist_params (reads, isl_set_copy (context));
    writes = isl_union_map_gist_params (writes, isl_set_copy (context));

    char * s_doms = isl_union_set_to_str (domains);
    char * s_sch  = isl_union_map_to_str (schedule_map);
    char * s_reads = isl_union_map_to_str (reads);
    char * s_writes = isl_union_map_to_str (writes);

    fprintf (ff, "%s\n", s_doms);
    fprintf (ff, "%s\n", s_reads);
    fprintf (ff, "%s\n", s_writes);
    fprintf (ff, "%s\n", s_sch);

    isl_union_set_free (domains);
    isl_union_map_free (schedule_map);
    isl_union_map_free (reads);
    isl_union_map_free (writes);
    isl_set_free (context);

    free (s_doms);
    free (s_reads);
    free (s_writes);
    free (s_sch);
    

    fclose (ff);
  }

  int codegen ()
  {
    // NOTE: m_ctx_str will be used in codegen_c
    m_ctx_str = this->get_parameter_table ()->get_parameter_string_values ();

    isl_union_set * domains = this->get_isl_union_iteration_domain ();
    isl_union_map * schedule_map = this->get_isl_union_schedule_map ();

    print_isl_union_set_debug (domains, "Iteration Domain used for codegen");

    schedule_map = axl_union_map_pad (schedule_map);
    schedule_map = isl_union_map_intersect_domain (
      schedule_map, isl_union_set_copy (domains));

    print_isl_union_map_debug (schedule_map, "Schedule map used for codegen");
    print_isl_union_map_debug (m_call, "Call map used for codegen"); 

    double t_start = axl_time ();
    this->codegen_c (domains, schedule_map, "axl.trans.c");
    double t_stop = axl_time ();
    m_codegen_time = t_stop - t_start;


    print_stats ();

    isl_union_set_free (domains);
    isl_union_map_free (schedule_map);

    //this->isl_plot ();
    
    // Build the dependence graph and print a dot file (axl.dot)
    //print_dependence_graph_as_dot ();
    return 0;
  }

  int apply_generic (int p_algo, vector<aqcl_statement*> * stmts = NULL)
  {
    m_ctx_str = this->get_parameter_table ()->get_parameter_string_values ();
    isl_union_set * domains = this->get_isl_union_iteration_domain ();
    isl_union_map * schedule_map = this->get_isl_union_schedule_map ();
    schedule_map = axl_union_map_pad (schedule_map);
    schedule_map = isl_union_map_intersect_domain (
      schedule_map, isl_union_set_copy (domains));
    isl_union_map * reads = this->get_isl_read_access_relations ();
    isl_union_map * writes = this->get_isl_write_access_relations ();

    isl_set * context = this->create_context_set ();
    if (this->uses_legal_space (p_algo))
    {
      assert (0);
    }
    else
    {
      isl_union_flow * deps = compute_dependences (schedule_map, writes, reads);

      double t_start = axl_time ();
      isl_schedule * new_sched = compute_transformation (domains, schedule_map, deps, context, p_algo);
      double t_stop = axl_time ();
      m_sched_time += t_stop - t_start;

      isl_schedule_node * root = isl_schedule_get_root (new_sched);

      isl_union_map * new_schedule_map = isl_schedule_get_map (new_sched);

      if (!new_sched || !new_schedule_map || 
            isl_union_map_is_empty (new_schedule_map) == isl_bool_true)
      {
        cout << "WARNING: Scheduling failed." << endl;
      }
      else
      {
        //new_schedule_map = isl_union_map_intersect_domain (
        //  new_schedule_map, isl_union_set_copy (domains));
        isl_schedule_free (new_sched);
        new_sched = isl_schedule_node_get_schedule (root);
        isl_union_map_free (m_usched);
        m_usched = isl_schedule_get_map (new_sched);
        m_usched = isl_union_map_intersect_domain (m_usched, isl_union_set_copy (domains));
      }
      isl_union_set_free (domains);
      isl_union_map_free (schedule_map);
      isl_union_map_free (reads);
      isl_union_map_free (writes);
    }
    isl_set_free (context);
    return 0;
  }

  int print_dependence_graph_as_dot ()
  {
    m_ctx_str = this->get_parameter_table ()->get_parameter_string_values ();
    isl_union_set * domains = this->get_isl_union_iteration_domain ();
    isl_union_map * schedule_map = this->get_isl_union_schedule_map ();
    schedule_map = axl_union_map_pad (schedule_map);
    schedule_map = isl_union_map_intersect_domain (
      schedule_map, isl_union_set_copy (domains));
    isl_union_map * reads = this->get_isl_read_access_relations ();
    isl_union_map * writes = this->get_isl_write_access_relations ();
    isl_union_flow * deps = compute_dependences (schedule_map, writes, reads);

    isl_union_map * must = isl_union_flow_get_must_dependence (deps);

    axl_plot_dependence_graph (must);
    print_stats ();

    isl_union_set_free (domains);
    isl_union_map_free (schedule_map);
    isl_union_map_free (reads);
    isl_union_map_free (writes);
    isl_union_map_free (must);
    isl_union_flow_free (deps);

    return 0;
  }

  int apply_feautrier ()
  {
    //m_ctx_str = this->get_parameter_table ()->get_parameter_string_values ();
    return this->apply_generic (AXL_TRANSFO_FEAUTRIER);
  }

  int apply_pluto ()
  {
    //m_ctx_str = this->get_parameter_table ()->get_parameter_string_values ();
    return this->apply_generic (AXL_TRANSFO_PLUTO);
  }

  int apply_pluto_minfuse ()
  {
    //m_ctx_str = this->get_parameter_table ()->get_parameter_string_values ();
    return this->apply_generic (AXL_TRANSFO_PLUTO_MINFUSE);
  }

  int apply_pluto_maxfuse ()
  {
    //m_ctx_str = this->get_parameter_table ()->get_parameter_string_values ();
    return this->apply_generic (AXL_TRANSFO_PLUTO_MAXFUSE);
  }

  int apply_aquma_format ()
  {
    return 0;
  }

  int apply_iss_stairs ()
  {
    double t_start = axl_time ();
    m_ctx_str = this->get_parameter_table ()->get_parameter_string_values ();
    vector<t_iss_data*> iss;
    isl_union_set * domains = this->get_isl_union_iteration_domain ();
    isl_union_map * schedule_map = this->get_isl_union_schedule_map ();
    schedule_map = axl_union_map_pad (schedule_map);
    schedule_map = isl_union_map_intersect_domain (
      schedule_map, isl_union_set_copy (domains));
    isl_union_map * reads = this->get_isl_read_access_relations ();
    isl_union_map * writes = this->get_isl_write_access_relations ();
    isl_union_flow * deps = compute_dependences (schedule_map, writes, reads);
    isl_union_map * must_dep = isl_union_flow_get_must_dependence(deps);
    int num_iss = axl_compute_iss_distance (must_dep, iss);
    if (num_iss > 0)
    {
      show_collected_iss_data (iss);
      t_mini_scop * new_scop = this->apply_stair_iss (iss);
      if (m_uidom)
        isl_union_set_free (m_uidom);
      m_uidom = isl_union_set_copy (new_scop->domain);
      if (m_accread)
        isl_union_map_free (m_accread);
      m_accread = isl_union_map_copy (new_scop->read);
      if (m_accwrite)
        isl_union_map_free (m_accwrite);
      m_accwrite = isl_union_map_copy (new_scop->write);
      if (m_call)
        isl_union_map_free (m_call);
      m_call = isl_union_map_copy (new_scop->call);
      if (m_usched)
        isl_union_map_free (m_usched);
      m_usched = isl_union_map_copy (new_scop->schedule);
      axl_mini_scop_free (new_scop);
    }
    isl_union_set_free (domains);
    isl_union_map_free (schedule_map);
    if (reads)
      isl_union_map_free (reads);
    if (writes)
      isl_union_map_free (writes);
    isl_union_map_free (must_dep);
    isl_union_flow_free (deps);
    double t_stop = axl_time ();
    m_sched_time += t_stop - t_start;
    return 0;
  }

  int apply_reverse_time ()
  {
    assert (m_uidom);
    assert (m_usched);
    assert (isl_union_set_n_set (m_uidom) == 1);
    isl_set * set = isl_set_from_union_set (isl_union_set_copy (m_uidom));
    const char * setname = isl_set_get_tuple_name (set);
    char * newname = (char*)malloc (sizeof(char) * strlen((char*)(setname)) * 2);
    sprintf (newname, "%s_rev", setname);
    m_uidom = axl_rename_union_set (m_uidom, newname);
    m_usched = axl_rename_union_map (m_usched, newname);
    m_accread = axl_rename_union_map (m_accread, newname);
    m_accwrite = axl_rename_union_map (m_accwrite, newname);
    m_call = axl_rename_union_map (m_call, newname);
    free (newname);

    m_usched = axl_reverse_schedule_map (m_usched);
    apply_parametric_shift_to_schedule ();
    if (!m_usched)
      return 1;
    return 0;
  }

  int apply_codelet (vector<aqcl_statement*> * stmts)
  {
    return 0;
  }

  int apply_hlt4qra (vector<aqcl_statement*> * stmts)
  {
    return 0;
  }


  void rename_domains_of_isl_objects ()
  {
    const char * localname = m_name.c_str ();
    m_idom = axl_rename_set (m_idom, localname);
    m_sched = axl_rename_map (m_sched, localname);
    m_uidom = axl_rename_union_set (m_uidom, localname);
    m_usched = axl_rename_union_map (m_usched, localname);
    m_accread = axl_rename_union_map (m_accread, localname);
    m_accwrite = axl_rename_union_map (m_accwrite, localname);
    m_call = axl_rename_union_map (m_call, localname);
  }

  void apply_parametric_shift_to_schedule ()
  {
    string bigsum = m_partab->get_symbol_sum_expr_str ();
    string str_shift = axl_union_map_apply_parametric_shift (m_usched, bigsum.c_str());
    string umap_str = this->get_isl_param_str ();
    umap_str += " -> {";
    umap_str += str_shift;
    umap_str += "}";
    isl_union_map * schedmap;
    schedmap = isl_union_map_read_from_str (m_ctx, umap_str.c_str ());
    string original = string(isl_union_map_to_str (m_usched));
    // Dirty hack to avoid multiple maps hidden, weird negative coefficient in output
    // product of the reversal
    isl_union_map_free (m_usched);
    m_usched = isl_union_map_read_from_str (m_ctx, original.c_str ());
    m_usched = isl_union_map_apply_range (m_usched, schedmap);
  }

  int apply_dlt_qubit_perm ()
  {
  }

  /*
   * Try to plot the domains using islpy. 
   * Domains need to be 2D sets.
   * Need to instantiate parameters.
   *
   */
  void isl_plot ()
  {
    ofstream pltstrm;
    pltstrm.open ("axl-islplot.py");
    pltstrm << "from islpy import *" << endl;
    pltstrm << "from islplot.plotter import *" << endl;
    pltstrm << "import matplotlib as mpl" << endl;
    pltstrm << "import matplotlib.pyplot as plt" << endl;
    pltstrm << "fig = plt.figure ()" << endl;

    axl_plot_isl_domains (m_uidom, pltstrm);

    pltstrm << "fig.savefig('axl-islplot.pdf', dpi=fig.dpi)" << endl;

    pltstrm.close ();
  }

  void print_stats (__isl_keep isl_union_map * deps = NULL)
  {
    ofstream sf;
    sf.open ("axl.stats.txt");
    sf << "Dependence analysis: " << (m_depanal_time/m_depanal_calls) << endl;
    sf << "Scheduling: " << m_sched_time << endl;
    sf << "Codegen: " << m_codegen_time << endl;
    {
      sf << "No. dependences: " << m_prog_deps << endl;
    }
    sf.close ();
  }

  int export_scop_classic (vector<aqcl_statement*> & stmts)
  {

    return 0;
  }

  string get_statement_body_str ()
  {
    stringstream strm;
    this->print_to_stream (strm);  
    return strm.str ();
  }

  /*
   * Create an std::map<string,string> with the string representation of
   * an isl_set and its corresponding statement body.
   *
   */
  void build_isl_set_to_body_std_map (
    vector<aqcl_statement*> & stmts, s2b_map & stmbody, t_mini_scop * mscop = NULL)
  {
    isl_union_map * filter = mscop->filter;
    vector<aqcl_statement*>::iterator ss;
    //for (ss = stmts.begin (); ss != stmts.end (); ss++)
    int pos = 0;
    for (ss = m_scop_stmts.begin (); ss != m_scop_stmts.end (); ss++, pos++)
    {
      (*ss)->instantiate_isl_objects ();
      isl_union_set * udom = (*ss)->get_isl_union_iteration_domain ();
      if (filter)
      {
        udom = isl_union_map_range (
          isl_union_map_intersect_domain (isl_union_map_copy (filter), udom));
      }
      print_isl_union_set_debug (udom, "Storing");
      stmbody[udom] = (*ss)->get_statement_body_str ();

    }
  }

  int export_scop_osl (vector<aqcl_statement*> & stmts)
  {
    return 0;
  }

  bool uses_legal_space (int p_algo)
  {
  }
};


#endif
