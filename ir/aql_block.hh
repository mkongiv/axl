/*
AQCSL: the Affine Quantum Circuit Specification Language
Filename: aql_block.hh
Creation date: Sun May 26 09:08:14 EDT 2019
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

#ifndef AQCL_BLOCK
#define AQCL_BLOCK

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <utility>
//#include "aqcsl-ir.hh"
#include "aql_node.hh"
#include "aql_goperator.hh"
#include "aql_gate.hh"
#include "aql_variable.hh"

using namespace std;

class aqcl_block : public aqcl_goperator
{
  protected:
  int m_offset; // space offset, if 0, then start is 0
  int m_padding;// 
  int m_smul;   // space multiplier used in scaling.
  int m_tmul;   // time multiplier used in scaling.
  aqcl_block * m_a1; // input arg #1
  aqcl_block * m_a2; // input arg #2
  aqcl_block * m_tl; // top-left
  aqcl_block * m_tr; // top-right
  aqcl_block * m_bl; // bottom-left
  aqcl_block * m_br; // bottom-right

  aqcl_block * m_res;

  string m_name;
  /*
  0: m_tl | m_tr

  */

  aqcl_gate  * m_body;

  public:

  aqcl_block () : aqcl_goperator()
  {
    block_setup ();
  }

  aqcl_block (string p_name) : aqcl_goperator()
  {
    block_setup ();
    m_name = p_name;
  }

  // used when promoting a gate to a circuit
  aqcl_block (int p_opcode, aqcl_block * p_res, aqcl_gate * p_gate) : 
    aqcl_goperator (p_opcode)
  {
    block_setup ();
    m_body = p_gate;
    m_res = p_res;
    assert (m_res && "aqcl_block::aqcl_block - Result block is null.");
    m_name = m_res->get_name ();
  }

  /*
  * Used for LOAD and STORE.
  *
  */
  aqcl_block (int p_opcode, aqcl_block * p_res) :
    aqcl_goperator (p_opcode)
  {
    block_setup ();
    m_body = NULL;
    m_res = p_res;
    assert (m_res && "aqcl_block::aqcl_block - Result block is null.");
    m_name = m_res->get_name ();
  }

  // Used when moving/copying circuits. CHECK!
  aqcl_block (int p_opcode, aqcl_block * p_res, aqcl_block * p_src) :
    aqcl_goperator (p_opcode)
  {
    block_setup ();
    m_body = NULL;
    m_a2 = p_src;
    assert (m_a2);
    m_res = p_res;
    assert (m_res && "aqcl_block::aqcl_block - Result block is null.");
    m_name = m_res->get_name ();
  }

  /*
  * Used with circuit operations that have two arguments:
  * c1 := c2 [+] c3;
  */
  aqcl_block (
    int p_opcode, aqcl_block * p_res, 
    aqcl_block * p_first, aqcl_block * p_second):
    aqcl_goperator (p_opcode)
  {
    block_setup ();
    m_a1 = p_first;
    m_a2 = p_second;
    m_res = p_res;
    assert (m_res && "aqcl_block::aqcl_block - Result block is null.");
    m_name = m_res->get_name ();
    //assert (m_tl);
    //assert (m_tr);
    //m_tl->set_parent (this);
    //m_tr->set_parent (this);
  }

  void block_setup ()
  {
    m_node_category = AQL_NODE_CIRC;
    m_a1 = NULL;
    m_a2 = NULL;
    m_tl = NULL;
    m_tr = NULL;
    m_bl = NULL;
    m_br = NULL;
    m_body = NULL;
    m_offset = 0;
    m_padding = 0;
    m_smul = 1;
    m_tmul = 1;
    m_vartype = SEMTYPE_CIRC;
  }


  ~ aqcl_block ()
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
    //assert (0 && "Method generated from aql_block.hh. Must implement in new class.");
    return m_name;
  }

  virtual aqcl_node * get_result ()
  {
    return this;
  }

  virtual string get_result_name ()
  {
    //return m_res->get_name ();
    return get_name ();
  }

  virtual string get_name ()
  {
    return m_name;
  }

  virtual string get_instruction_str ()
  {
    string op_str;

    return "aqcl_block::get_instruction_str () - Undefined value (2)";
  }

   
  virtual bool is_block ()
  {
    return true;
  }

  virtual bool is_circ ()
  {
    return true;
  }

  virtual void print ()
  {
    cout << "(";
    if (m_offset > 0)
      cout << m_offset << "|";
    if (m_body)
    {
      m_body->print ();
    }
    else
    {
      if (m_tl && m_tr)
      {
        m_tl->print();
        cout << ",";
        m_tr->print();
        //cout << endl;
      }
      else if (m_tl && m_bl)
      {
        m_tl->print ();
        cout << ";";
        m_bl->print ();
        //cout << endl;
      }
      else if (m_tl && !m_tr)
      {
        int tt;
        int ss;
        for (tt = 0; tt < m_tmul; tt++)
        {
          if (m_tmul > 1 && m_smul > 1)
            cout << "(";
          if (tt > 0)
            cout << ",";
          for (ss = 0; ss < m_smul; ss++)
          {
            if (ss > 0)
              cout << ";";
            m_tl->print ();
          }
          if (m_tmul > 1 && m_smul > 1)
            cout << ")";
        }
        //cout << endl;
      }
      else if (!m_tl && m_tr)
      {
        assert (0 && "aql_block::print () - This shouldn't happen.");
      }
      else if (m_res)
      {
        m_res->print ();
      }
    }
    if (m_padding > 0)
      cout << "|" << m_padding;
    cout << ")";
  }

  void check_subcircuits ()
  {
    cout << "Checking sub-circuits of " << get_name () << " = {" << endl;
    if (!m_tl)
      cout << "m_tl is not NULL" << endl;
    if (!m_tr)
      cout << "m_tr is not NULL" << endl;
    if (!m_bl)
      cout << "m_bl is not NULL" << endl;
    if (!m_br)
      cout << "m_br is not NULL" << endl;
    cout << endl << "}" << endl;
  }

  virtual void set_temp ()
  {
    m_is_temp = true;
  }

  virtual bool is_param ()
  {
    return false;
  }

  aqcl_gate * get_body ()
  {
    return m_body;
  }

  #if 0
  string get_read_access_expression_str ()
  {
    if (m_body)
      return m_body->get_read_expression_str ();
    return "";
  }
  #endif
  vector<string> get_read_access_expression_str ()
  {
    vector<string> ret;
    if (m_body)
      ret = m_body->get_read_expression_str ();
    return ret;
  }

  #if 0
  string get_write_access_expression_str ()
  {
    if (m_body)
      return m_body->get_write_expression_str ();
    return "";
  }
  #endif
  vector<string> get_write_access_expression_str ()
  {
    vector<string> ret;
    if (m_body)
      ret = m_body->get_write_expression_str ();
    return ret;
  }

  string get_gate_call_str ()
  {
    if (m_body)
      return m_body->get_gate_arguments_str (true);
    return "";
  }

  bool has_body ()
  {
    return m_body != NULL;
  }

  void set_body (aqcl_gate * gate_body)
  {
    assert (!m_body);
    m_body = gate_body;
  }

  void set_result (aqcl_block * blk)
  {
    m_res = blk;
  }

  aqcl_block * get_result_block ()
  {
    return m_res;
  }

  aqcl_block * get_block_a2()
  {
    return m_a2;
  }

  void set_tl (aqcl_block * blk)
  {
    assert (!m_tl);
    m_tl = blk;
  }

  virtual void shallow_copy (aqcl_block & p_block)
  {
    cout << "In aqcl_block::shallow_copy ()" << endl;
    bool has_subc = false;
    if (p_block.get_tl ())
    {
      assert (!m_tl);
      //m_tl = new aqcl_block ();
      m_tl = (p_block.get_tl ());
      has_subc = true;
    }
    if (p_block.get_tr ())
    {
      assert (!m_tr);
      //m_tr = new aqcl_block ();
      m_tr = (p_block.get_tr ());
      has_subc = true;
    }
    if (p_block.get_bl ())
    {
      assert (!m_bl);
      //m_bl = new aqcl_block ();
      m_bl = (p_block.get_bl ());
      has_subc = true;
    }
    if (p_block.get_br ())
    {
      assert (!m_br);
      //m_br = new aqcl_block ();
      m_br = (p_block.get_br ());
      has_subc = true;
    }
    // A circuit cannot have sub-circuits and body pointing to a gate
    // at the same time!
    if (not has_subc and p_block.get_body () != NULL)
    {
      if (!m_body)
      {
        delete m_body;
      }
      m_body = new aqcl_gate ();
      *m_body = *(p_block.get_body ());
    }
    //*m_res = *(p_block.get_res ());
    m_offset = p_block.get_offset ();
    m_padding = p_block.get_padding ();
    m_tmul = p_block.get_tmul ();
    m_smul = p_block.get_smul ();
    //cout << "copied from " << p_block.get_name () << " to " << m_name << endl;
  }

  virtual inline int get_offset ()
  {
    return m_offset;
  }

  virtual void set_offset (int p_offset)
  {
    m_offset = p_offset;
  }

  virtual inline int get_padding ()
  {
    return m_padding;
  }

  virtual void set_padding (int p_padding)
  {
    m_padding = p_padding;
  }

  virtual inline int get_smul ()
  {
    return m_smul;
  }

  virtual void set_smul (int p_smul)
  {
    m_smul = p_smul;
  }

  virtual inline int get_tmul ()
  {
    return m_tmul;
  }

  virtual void set_tmul (int p_tmul)
  {
    m_tmul = p_tmul;
  }


  aqcl_block * get_a2 ()
  {
    return m_a2;
  }

  aqcl_block * get_a1 ()
  {
    return m_a1;
  }

  aqcl_block * get_tl ()
  {
    return m_tl;
  }

  aqcl_block * get_tr ()
  {
    return m_tr;
  }

  aqcl_block * get_bl ()
  {
    return m_bl;
  }

  aqcl_block * get_br ()
  {
    return m_br;
  }

  aqcl_block * get_res ()
  {
    return m_res;
  }

  virtual bool is_pure_block ()
  {
    return true;
  }

  virtual bool is_cfact ()
  {
    return false;
  }

  virtual bool is_cterm ()
  {
    return false;
  }

  virtual bool is_cexpr ()
  {
    return false;
  }


  void print_body ()
  {
    if (m_body)
      cout << m_body->get_string () << endl;
    else
      cout << "<NBF>" << endl;
  }


  virtual string get_expression_plain ()
  {
    if (m_a2)
      return m_a2->get_expression_plain ();
    if (m_body)
      return m_body->get_expression_plain ();
    return get_name ();
  }

  void comp_time (aqcl_block * left, aqcl_block * right)
  {
    assert (!m_tl);
    assert (!m_tr);
    // size compatibility assertions should go here.
    m_tl = left;
    m_tr = right;
  }

  void comp_space (aqcl_block * up, aqcl_block * down)
  {
    assert (!m_tl);
    assert (!m_bl);
    // size compatibility assertions should go here.
    m_tl = up;
    m_bl = down;
    m_padding = up->get_padding () + down->get_padding ();
    m_offset = up->get_offset () + down->get_offset ();
  }

  virtual void time_scaling (aqcl_block * arg, int factor)
  {
    m_tl = arg;
    m_tmul = factor;
  }

  virtual void space_scaling (aqcl_block * arg, int factor)
  {
    m_tl = arg;
    m_smul = factor;
  }

  virtual void time_space_scaling (aqcl_block * arg, int t_factor, int s_factor)
  {
    m_tl = arg;
    m_tmul = t_factor;
    m_smul = s_factor;
  }

  virtual int get_dim ()
  {
    int ret = 0;
    if (m_offset > 0)
      ret++;
    if (m_padding > 0)
      ret++;
    if (m_tl)
      ret += m_tl->get_dim ();
    if (m_bl)
      ret += m_bl->get_dim ();
    if (m_body)
      ret += m_body->get_dim ();
    return ret * m_smul;
  }

  virtual int get_width ()
  {
    int ret = 0;
    if (m_body)
      return m_body->get_width ();
    if (m_res)
      return m_res->get_width ();
    if (m_tl)
      ret += m_tl->get_width ();
    if (m_tr)
      ret += m_tr->get_width ();
    //if (m_bl)
    //  ret += m_bl->get_width ();
    return ret * m_tmul;
  }


  void get_locations (string sname, int d_shift, int t_shift,
    map<pair<int,int>,aqcl_block*> & gm)
  {
    if (m_body)
    {
      gm[make_pair(m_offset + d_shift,t_shift)] = this;
      //print ();
    }
    int dd;
    int tt;
    int c_space = get_dim () / m_smul;
    int c_time  = get_width () / m_tmul;
    for (dd = 0; dd < m_smul; dd++)
    {
      for (tt = 0; tt < m_tmul; tt++)
      {
        if (m_tl)
        {
          //cout << "storing m_tl\n";
          m_tl->get_result_block ()->get_locations (
            sname, 
            dd * c_space + d_shift + m_offset, 
            tt * c_time + t_shift, 
            gm);
        }
        if (m_tr)
        {
          //cout << "storing m_tr\n";
          m_tr->get_result_block ()->get_locations (sname, 
            dd * c_space + d_shift + m_offset,
            tt * c_time + t_shift + m_tl->get_width (), 
            gm);
        }
        if (m_bl)
        {
          //cout << "storing m_bl\n";
          m_bl->get_result_block ()->get_locations (sname, 
            dd * c_space + d_shift + m_offset + m_tl->get_dim (), 
            tt * c_time + t_shift,
            gm);
        }
      }
    }
  }


  virtual void print_info ()
  {
    cout << "Nothing " << endl;
  }

  virtual string get_class_name ()
  {
    return "aql_block";
  }


  virtual void print_to_stream (stringstream & strmbuf)
  {
    strmbuf << "(";
    if (m_offset > 0)
      strmbuf << m_offset << "|";
    if (m_body)
    {
      m_body->print_to_stream (strmbuf);
    }
    else
    {
      if (m_tl && m_tr)
      {
        m_tl->print_to_stream(strmbuf);
        strmbuf << ",";
        m_tr->print_to_stream(strmbuf);
      }
      else if (m_tl && m_bl)
      {
        m_tl->print_to_stream (strmbuf);
        strmbuf << ";";
        m_bl->print_to_stream (strmbuf);
      }
      else if (m_tl && !m_tr)
      {
        int tt;
        int ss;
        for (tt = 0; tt < m_tmul; tt++)
        {
          if (m_tmul > 1 && m_smul > 1)
            strmbuf << "(";
          if (tt > 0)
            strmbuf << ",";
          for (ss = 0; ss < m_smul; ss++)
          {
            if (ss > 0)
              strmbuf << ";";
            m_tl->print_to_stream (strmbuf);
          }
          if (m_tmul > 1 && m_smul > 1)
            strmbuf << ")";
        }
      }
      else if (!m_tl && m_tr)
      {
        assert (0 && "aql_block::print_to_stream () - This shouldn't happen.");
      }
      else if (m_res)
      {
        m_res->print_to_stream (strmbuf);
      }
    }
    if (m_padding > 0)
      strmbuf << "|" << m_padding;
    strmbuf << ")";
  }
};

#endif
