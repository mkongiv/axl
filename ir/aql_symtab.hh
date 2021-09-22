/*
AQCSL: the Affine Quantum Circuit Specification Language
Filename: aql_symtab.hh
Creation date: Wed Jun 12 11:43:25 EDT 2019
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

#ifndef AQCL_symtab
#define AQCL_symtab

#include <vector>
#include <map>
#include <string>
#include <iostream>
//#include "aqcsl-ir.hh"
#include "aql_node.hh"
#include "aql_variable.hh"

using namespace std;

class aqcl_symtab : public aqcl_node
{
  map<string,aqcl_node*> m_symtab;
  map<int,aqcl_variable*> m_sortab;
  map<string,string> m_ctxtab; // default values for parameters


  public:

  aqcl_symtab () : aqcl_node ()
  {
    m_node_category =  AQL_NODE_SYMTAB ;
    m_vartype = SEMTYPE_SYMTAB;
  }

  ~ aqcl_symtab ()
  {

  }

  map<string,aqcl_node*> & get_table ()
  {
    return m_symtab;
  }

  void initialize_sortab ()
  {
    map<string,aqcl_node*>::iterator ss;
    m_sortab.clear ();
    for (ss = m_symtab.begin (); ss != m_symtab.end (); ss++)
    {
      m_sortab[ss->second->get_pos ()] = (aqcl_variable*)(ss->second);
    }
  }

  bool exists_sym (string p_varname)
  {
    bool ret;
    ret = m_symtab.find (p_varname) != m_symtab.end ();
    return ret;
  }
  
  bool add_variable (string varname)
  {
    bool var_exists = exists_sym (varname);
    if (not var_exists)
    {
      m_symtab[varname] = new aqcl_variable (varname, SEMTYPE_INT);
    }
    else
    {
      //assert (0 && "aqcl_program::add_variable ==> variable already declared.");
    }
    return not var_exists;
  }

  bool add_variable (aqcl_variable * p_var)
  {
    string varname = p_var->get_name ();
    bool var_exists = exists_sym (varname);
    if (not var_exists)
    {
      m_symtab[varname] = p_var;
    }
    else
    {
      //assert (0 && "aqcl_program::add_variable ==> variable already declared.");
    }
    return not var_exists;
  }

  int size ()
  {
    return m_sortab.size ();
  }

  aqcl_variable * operator [] (int idx)
  {
    assert (idx < m_sortab.size ());
    return m_sortab[idx];
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
    assert (0 && "Method generated from aql_symtab.hh. Must implement in new class.");
  }

  virtual aqcl_node * get_result ()
  {
    assert (0 && "Method generated from aql_symtab.hh. Must implement in new class.");
  }

  virtual string get_result_name ()
  {
    assert (0 && "Method generated from aql_symtab.hh. Must implement in new class.");
  }

  virtual string get_name ()
  {
    assert (0 && "Method generated from aql_symtab.hh. Must implement in new class.");
  }

  virtual string get_instruction_str ()
  {
    assert (0 && "Pending implementation");
    return "";
  }

  virtual string get_expression_plain ()
  {
    assert (0 && "Pending implementation");
    return "";
  }

  string get_symbol_list ()
  {
    if (m_sortab.empty ())
      initialize_sortab ();
    map<int,aqcl_variable*>::iterator pp;
    stringstream ret;
    //ret << "[";
    for (pp = m_sortab.begin (); pp != m_sortab.end (); pp++)
    {
      aqcl_variable * sym = pp->second;
      if (pp != m_sortab.begin ())
        ret << ", ";
      ret << sym->get_name ();
    }
    //ret << "]";
    return ret.str ();
  }

  string get_symbol_sum_expr_str ()
  {
    if (m_sortab.empty ())
      initialize_sortab ();
    map<int,aqcl_variable*>::iterator pp;
    stringstream ret;
    //ret << "[";
    for (pp = m_sortab.begin (); pp != m_sortab.end (); pp++)
    {
      aqcl_variable * sym = pp->second;
      if (pp != m_sortab.begin ())
        ret << " + ";
      ret << sym->get_name ();
    }
    //ret << "]";
    return ret.str ();
  }

  void set_parameter_value (string p_param, string p_value)
  {
    m_ctxtab[p_param] = p_value;
  }

  string get_parameter_string_values ()
  {
    stringstream ret;
    map<int,aqcl_variable*>::iterator ii;
    int idx = 0;
    for (ii = m_sortab.begin (); ii != m_sortab.end (); ii++, idx++)
    {
      if (idx > 0)
        ret << " and ";
      string varname = ii->second->get_name ();
      if (m_ctxtab.find (varname) != m_ctxtab.end ())
        ret << varname << " = " << m_ctxtab[varname];
      else
        ret << varname << " = 12 ";;;;
    }
    return ret.str ();
  }

  aqcl_symtab * shallow_copy ()
  {
    aqcl_symtab * ret;
    ret = new aqcl_symtab ();
    map<string,aqcl_node*>::iterator ii;
    for (ii = m_symtab.begin (); ii != m_symtab.end (); ii++)
    {
      aqcl_variable * var = (aqcl_variable*)(ii->second);
      ret->add_variable (var);
    }
    ret->initialize_sortab ();
    return ret;
  }

  void export_parameters_to_vector (std::vector<string> & params)
  {
    map<int,aqcl_variable*>::iterator ii;
    for (ii = m_sortab.begin (); ii != m_sortab.end (); ii++)
    {
      string varname = ii->second->get_name ();
      params.push_back (varname);
    }
  }
};

#endif
