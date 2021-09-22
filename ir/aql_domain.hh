/*
AQCSL: the Affine Quantum Circuit Specification Language
Filename: aql_domain.hh
Creation date: Wed May 22 14:10:50 EDT 2019
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

#ifndef AQCL_DOMAIN
#define AQCL_DOMAIN

#include <vector>
#include <map>
#include <string>
#include <iostream>
//#include "aqcsl-ir.hh"
#include "aql_constraint.hh"
#include "aql_variable.hh"
#include "aql_goperator.hh"
#include "aql_node.hh"
#include "aql_symtab.hh"

using namespace std;

class aqcl_domain : public aqcl_goperator 
{
  protected:
  //vector<string> m_dims;
  aqcl_symtab * m_symtab;
  vector<aqcl_constraint*> m_constraint;

  public:

  aqcl_domain () : aqcl_goperator ()
  {
    m_node_category =  SEMTYPE_DOMAIN ;
  }

  aqcl_domain (int p_domtype /* opcode */, 
    //aqcl_variable * p_res,
    //vector<string> p_dims, 
    aqcl_symtab * p_symtab,
    vector<aqcl_constraint*> & p_constraint) : 
    aqcl_goperator(p_domtype)
  {
    m_node_category =  SEMTYPE_DOMAIN ;
    //m_dims = p_dims;
    m_symtab = p_symtab;
    m_constraint = p_constraint;
  }

  ~ aqcl_domain ()
  {

  }

  int get_domain_type ()
  {
    return get_opcode ();
  }

  int get_category ()
  {
    return m_node_category;
  }

  bool is_same_category (int p_cat)
  {
    return m_node_category == p_cat;
  }

  int canonicalize_dimensions (int dim_off)
  {
    int ns = m_symtab->size ();
    int ii;
    //for (ss = m_dims.begin (); ss != m_dims.end (); ss++)
    for (ii = 0; ii < ns; ii++)
    {
      string newname = "i" + std::to_string (dim_off + ii);
      (*m_symtab)[ii]->set_name (newname);
    }
    return dim_off + ns;
  }

  string get_declared_tuple_string ()
  {
    string ret = "";
    vector<string>::iterator ss;
    m_symtab->initialize_sortab ();
    return m_symtab->get_symbol_list ();
  }

  string get_tuple_string ()
  {
    string ret = "[ ";
    ret += get_declared_tuple_string ();
    ret += " ]";
    return ret;
  }

  string get_constraint_string ()
  {
    string ret = "";
    vector<aqcl_constraint*>::iterator cc;
    for (cc = m_constraint.begin (); cc != m_constraint.end (); cc++)
    {
      if (cc != m_constraint.begin ())
        ret += " and ";
      ret += (*cc)->get_string (); 
    }
    return ret;
  }

  void show_individual_constraints ()
  {
    vector<aqcl_constraint*>::iterator cc;
    int idx = 0;
    for (cc = m_constraint.begin (); cc != m_constraint.end (); cc++, idx++)
    {
      cout << "Constraint " << idx << ": ";
      cout << (*cc)->get_expression_plain () << endl; 
    }
  }

  virtual string get_string () 
  {
    string ret = "{";
    ret += this->get_tuple_string ();
    ret += " : ";
    ret += this->get_constraint_string ();
    ret += " }";
    return ret;
  }

  virtual string get_instruction_str ()
  {
    string ret = "Domain : " + get_string (); //"domain_GIS";
    return ret;
  }

  virtual aqcl_node * get_result ()
  {
    //return m_res;
    return NULL;
  }

  virtual string get_result_name ()
  {
    //return m_res->get_name ();
    return "";
  }

  virtual string get_name ()
  {
    return get_result_name ();
  }

  virtual void print ()
  {
    cout << get_constraint_string () << endl;
  }

};

#endif
