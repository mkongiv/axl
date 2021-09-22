%language "c++"
%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.0.2"
%verbose
%locations
%defines
%define api.token.constructor
%define api.value.type variant
%define parse.assert true
%define parser_class_name {aqcsl_parser}
%define parse.trace true
%define parse.error verbose
%parse-param {aqcl_program * program}
/*
%parse-param {aqcsl_root & root}
*/

%code requires {

#include "ir/aqcsl-class-list.hh"

#ifndef YY_NULLPTR
	#define YY_NULLPTR 0
#endif

/*
#define _AXL_DEBUG_
*/
#ifdef _AXL_DEBUG_
#define _D_(x) cout << x << endl
#else
#define _D_(x) 
#endif


}



%code
{


#define _USE_MATH_DEFINES

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <cmath>
#include <string.h>
#include <cstddef>
#include <string>
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include "aqcsl.h"
#include "ir/aqcsl-ir.hh"
#include "ir/aqcsl-class-list.hh"

bool m_skip_st = false;
static int axl_gate_num = 0;
static int axl_op_num = 0;

extern yy::location loc;



using namespace std;
extern yy::aqcsl_parser::symbol_type yylex(); 

extern char * yytext;




/*
%type <aqcsl_region_expr*>										region_fact;
%type <aqcsl_expr*>														domain_range;
%type <std::map<string,aqcsl_tuple*> >									prop_order;
*/
}






%token  T_EOF 0
%token	T_COMP_S
%token	T_COMP_T
%token  T_PIPE
%token	T_ITER_S
%token	T_ITER_T
%token  T_ASSIGN
%token  T_EMPTY_QG

%token  T_GATE_U
%token  T_GATE_H
%token  T_GATE_X
%token  T_GATE_Y
%token  T_GATE_Z
%token  T_GATE_RTNOT
%token  T_GATE_R
%token  T_GATE_SWAP
%token  T_GATE_SQRT_SWAP 
%token  T_GATE_CX
%token  T_GATE_CY
%token  T_GATE_CZ
%token  T_GATE_CCX
%token  T_GATE_CSWAP
%token  T_GATE_XX
%token  T_GATE_YY
%token  T_GATE_ZZ
%token  T_GATE_QFT
%token  T_GATE_MEAS

%token  T_ADD
%token  T_SUB
%token  T_MUL
%token  T_DIV

%token  T_LT
%token  T_GT
%token  T_LEQ
%token  T_GEQ
%token  T_EQ

%token  T_ATCH_DOM_B
%token  T_ATCH_DOM_T
%token  T_ATCH_DOM_S

%token  T_LPAR
%token  T_RPAR
%token  T_LCUR
%token  T_RCUR
%token  T_LBRA
%token  T_RBRA
%token  T_COMMA
%token  T_COLON
%token  T_SEP

%token  T_DT_INT
%token  T_DT_CIRC
%token  T_DT_PARAM
%token  T_DT_STMT

%token  T_PI
%token  T_PRINT
%token  T_CODEGEN
%token  T_REVERSE

%token  T_EQUAL
%token  T_WITH
%token  T_APPLY
%token  T_BACKEND
%token  T_EXPORT_SCOP
%token  T_CLASSIC
%token  T_OSL

%token <string>		T_ID ;
%token <string>		T_INTEGER ;
%token <string>		T_DOUBLE;


%type <std::vector< aqcl_node*> >  program;
%type <std::vector< aqcl_node*> >  stmt_list;
%type <aqcl_node*>             statement;
%type <aqcl_node*>             operation;
%type <aqcl_node*>             circuit;
%type <aqcl_node*>             print;
%type <aqcl_node*>             declaration;

%type <aqcl_goperator*> op_assign;
%type <int>             semtype;

%type <aqcl_goperator*> a_expr;
%type <aqcl_goperator*> a_term;
%type <aqcl_node*>      a_fact;
%type <aqcl_goperator*> c_gate_arg;

%type <aqcl_block*>   c_expr;
%type <aqcl_block*>   c_term;
%type <aqcl_block*>   c_fact;
%type <aqcl_gate*>    c_gate;
%type <e_gate> q1_gate;
%type <e_gate> q2_gate;
%type <e_gate> q3_gate;

%type <int>     c_expr_op;
%type <int>     c_term_op;

%type <int> oprel;
%type <int> dom_atch_op;
%type <aqcl_constraint*>  constraint;
%type <vector<aqcl_constraint*> > constraint_list;
%type <vector<string> > id_list;
%type <aqcl_domain*> domain;

%type <aqcl_sexpr*> stmt_expr;
%type <aqcl_sexpr*> stmt_term;
%type <int>        stmt_op;

%type <aqcl_symtab*> context_param_values ;
%type <vector<pair<string,string>>> param_list_values ;
%type <pair<string,string>> param_value;

%type <vector<int>> apply;
%type <vector<int>> transfo_list;
%type <int>         transfo;
%type <int>         backend;
%type <int>         scop_export_mode;

%type <aqcl_statement*> lhs_stmt;



%%

program: stmt_list
        { 
          
        }

stmt_list : stmt_list statement { $1.push_back ($2); $$ = $1; _D_(1.1); }
          | statement{ $$.push_back ($1); _D_(1.2); }
          ;

statement : declaration { $$ = $1; }
          | operation   { $$ = $1; }
          | print       { $$ = $1; }
          ;

print : T_PRINT c_expr T_SEP
        {
          
          
          

          aqcl_block * block;
          aqcl_print * prt; 
          if ($2->is_block ())
          {
            
            
            
            
            prt = new aqcl_print ($2->get_result ());
          }
          else
          {
            
            
            
            aqcl_variable * var = program->get_sym_as_variable ($2->get_result_name ());
            prt = new aqcl_print (var);
          }
          program->add_instr (prt);
          $$ = prt;
        }
      | T_EXPORT_SCOP 
        {
          aqcl_print * prt = new aqcl_print (OP_GENSTART);
          program->add_instr (prt);
        }
        T_LCUR stmt_expr T_COMMA scop_export_mode T_RCUR T_SEP
        {
          aqcl_print * prt; 
          aqcl_sexpr * sexpr = (aqcl_sexpr*)($4);
          aqcl_statement * stmt_res = sexpr->get_result_stmt ();
          prt = new aqcl_print (stmt_res, $6);
          program->add_instr (prt);
          $$ = prt;
        }
      | T_CODEGEN 
        {
          aqcl_print * prt = new aqcl_print (OP_GENSTART);
          program->add_instr (prt);
        }
        T_LCUR 
        stmt_expr
        T_RCUR 
        context_param_values
        apply
        backend
        T_SEP
        {
          aqcl_sexpr * sexpr = (aqcl_sexpr*)($4);
          /*
          if (sexpr->get_result_stmt ()) sexpr->get_result_stmt ()->print_info ();
          if (sexpr->get_a1 ()) sexpr->get_a1()->print_info ();
          if (sexpr->get_a2 ()) sexpr->get_a2()->print_info ();
          */
          if (not sexpr->get_result_stmt ()->is_stmt ())
          {
            cerr << "ERROR: expecting expression of type <statement>. Aborting ...";
            cerr << endl;
            program->print_symtab ();
            program->print_itab ();
            YYABORT;
          }
          aqcl_statement * stmt_res = sexpr->get_result_stmt ();
          stmt_res->set_parameter_table ($6);
          vector<int>::iterator tt;
          for (tt = $7.begin (); tt != $7.end (); tt++)
          {
            aqcl_transformation * transfo = new aqcl_transformation (stmt_res, *tt);
            program->add_instr (transfo);
          }
          stmt_res->set_backend ($8);
          aqcl_print * prt = new aqcl_print (stmt_res);
          program->add_instr (prt);
        }
      ;

backend : T_BACKEND T_EQUAL T_ID 
          { 
            int code = program->get_backend_code ($3); 
            if (code < 0)
            {
              cerr << "AXL error: selected unknown back-end." << endl;
              YYABORT;
            }
            $$ = code;
          }
      | { $$ = program->get_backend_code ("projectq");}
      ;

apply : T_APPLY T_LCUR transfo_list T_RCUR { $$ = $3; }
      | {}
      ;

scop_export_mode  : T_CLASSIC { $$ = OP_EXPORT_SCOP; }
                  | T_OSL { $$ = OP_EXPORT_OSL; }
                  ;

transfo_list : transfo_list T_COMMA transfo { $1.push_back ($3); $$ = $1; }
        | transfo { $$.push_back ($1); }
        ;


transfo : T_ID 
          {
            int code = program->get_transfo_code ($1);
            if (!code)
            {
              cerr << "Unknown transformation given to AXL.";
              program->print_transfo_list  ();
              YYABORT;
            }
            $$ = code;
          }
        ;

context_param_values 
        : T_WITH 
          T_LCUR
          param_list_values
          T_RCUR
          {
            aqcl_symtab * st = program->get_parameter_table ()->shallow_copy ();
            vector<pair<string,string>>::iterator ii;
            for (ii = $3.begin (); ii != $3.end (); ii++)
            {
              st->set_parameter_value (ii->first, ii->second);
              
            }
            $$ = st;
          }
        | { $$ = NULL; }
        ;

param_list_values : param_value T_COMMA param_list_values 
          { 
            $3.push_back ($1); 
            $$ = $3;
          }
        | param_value 
          { $$.push_back ($1); }
        ;

param_value : T_ID T_EQUAL T_INTEGER 
              { $$ = std::pair<string,string>($1,$3); }
            ;

stmt_expr : stmt_expr stmt_op stmt_term
            {
              aqcl_sexpr * expr;
              aqcl_statement * src1 = $1->get_result_stmt ();
              aqcl_statement * src2 = $3->get_result_stmt ();
              aqcl_statement * dst = program->new_temp_as_statement ();
              expr = new aqcl_sexpr ($2, dst, src1, src2, axl_op_num++);
              expr->unset_leaf ();
              program->add_instr (expr);
              $$ = expr;
            }
          | stmt_term
            { $$ = $1; }
          ;

stmt_term : T_ID
            {
              aqcl_sexpr * expr;
              aqcl_statement * src = program->get_sym_as_statement ($1);
              aqcl_statement * dst = program->new_temp_as_statement ();
              expr = new aqcl_sexpr (OP_SLOAD, dst, NULL, src, axl_op_num++);
              program->add_instr (expr);
              $$ = expr;
            }
          | T_LT stmt_expr T_GT
            {
              $$ = $2;
            }
          ;

stmt_op : T_COMP_T { $$ = OP_COMP_T; }
        | T_COMP_S { $$ = OP_COMP_S; }
        ;
          

declaration : semtype T_ID T_SEP
        {
          
          assert (program->add_symbol ($2, $1));
          $$ = NULL;
        }
        ;

operation : circuit T_SEP 
            { 
              $$ = $1;
            }
      ;

circuit : op_assign { $$ = $1; }
        | c_expr { $$ = $1; }
        ;

id_list : id_list T_COMMA T_ID 
          { 
            $1.push_back ($3); 
            $$ = $1; 
          }
        | T_ID { $$.push_back ($1); }
        ;



op_assign : T_ID T_ASSIGN c_expr 
            {
              
              aqcl_node * lhs = program->get_sym ($1);
              
              
              if (lhs->is_circ ())
              {
                aqcl_cfact * ass;
                aqcl_block * blhs = program->get_sym_as_block ($1);
                
                aqcl_block * rhs = program->get_sym_as_block ($3->get_name ());
                
                
                ass = new aqcl_cfact (OP_CSTORE, blhs, rhs);
                program->add_instr (ass);
                $$ = ass;
                ass->unset_leaf ();
              }
              else if (lhs->is_stmt ())
              {
                
                
                aqcl_statement * tgt_stmt = program->get_sym_as_statement ($1);
                aqcl_statement * src_stmt = program->get_sym_as_statement ($3->get_name ());
                assert (src_stmt->is_stmt ());
                aqcl_sexpr * expr = new aqcl_sexpr (OP_SLOAD, tgt_stmt, NULL, src_stmt, axl_op_num++);
                program->add_instr (expr);
                $$ = expr;
              }
              else
              {
                aqcl_afact * ass;
                
                ass = new aqcl_afact (OP_ASTORE, lhs, $3);
                program->add_instr (ass);
                $$ = ass;
                ass->unset_leaf ();
              }
              
            }
          | lhs_stmt
            T_REVERSE 
            T_LPAR 
            T_ID 
            T_RPAR
            {
              aqcl_statement * tgt_stmt = $1;
              aqcl_statement * src_stmt = program->get_sym_as_statement ($4);

              aqcl_sexpr * expr;
              expr = new aqcl_sexpr (OP_SLOAD, tgt_stmt, NULL, src_stmt, axl_op_num++);
              program->add_instr (expr);

              aqcl_transformation * transfo;
              transfo = new aqcl_transformation (tgt_stmt, AXL_TRANSFO_REVERSE_TIME);
              program->add_instr (transfo);
              
            }
          | lhs_stmt
            { 
              
              aqcl_status * pause_int;
              pause_int = new aqcl_status (OP_PAUSE_INT);
              program->add_instr (pause_int);
            }
            T_LCUR 
            domain
            {
              aqcl_status * restart_int;
              restart_int = new aqcl_status (OP_START_INT);
              program->add_instr (restart_int);
            }
            circuit
            T_RCUR
            {
              
              program->pop_symtab (); 
              aqcl_statement * stmt = $1;
              stmt->set_domain ($4);

              
              stmt->set_body ((aqcl_block*)($6));

              stmt->set_operator ();

              program->add_instr (stmt);
              
              

              $$ = stmt;
            }
      ;

lhs_stmt :  T_ID T_ASSIGN 
            {
              aqcl_statement * stmt;
              stmt = program->get_sym_as_statement ($1);
              $$ = stmt;
            }
          ;

domain :  id_list 
          T_COLON     
          { 
            m_skip_st = false; 
            program->push_symtab (); 
            vector<string>::iterator ss;
            int idx = 0;
            for (ss = $1.begin (); ss != $1.end (); ss++, idx++)
            {
              program->add_variable_with_pos (*ss, SEMTYPE_INT, idx);
            }
          }
          constraint_list 
          { 
            m_skip_st = false; 
          }
          dom_atch_op
          {
            
            aqcl_domain * dom;
            aqcl_variable * res;
            
            aqcl_symtab * local_table = program->get_current_table ();
            dom = new aqcl_domain ($6, local_table, $4);
            
            
            
            $$ = dom;
          }
        ;

constraint_list : constraint_list T_COMMA constraint 
                  {
                    
                    $1.push_back ($3); $$ = $1; 
                  }
                | constraint { $$.push_back ($1); }
                ;

constraint : a_expr oprel a_expr
             {
               
               aqcl_constraint * cst;
               cst = new aqcl_constraint ($2, (aqcl_aoperator*)$1, (aqcl_aoperator*)$3);
               $$ = cst;
               
               
             }
          ;

a_expr  : a_expr T_ADD a_term 
          { 
            
            aqcl_variable * res;
            int semtype = program->get_semtype (OP_ADD, $1, $3);
            if (semtype != SEMTYPE_INT && semtype != SEMTYPE_FLOAT && semtype != SEMTYPE_DOUBLE)
            {
              cerr << "Error: Expected <int> or <float> type in arithmetic expression. Aborting ..." << endl;
              YYABORT;
            }
            
            
            
            res = program->new_temp_as_variable (semtype);
            aqcl_aexpr * newop;
            newop = new aqcl_aexpr (OP_ADD, res, $1, $3);
            $$ = newop;
            newop->unset_leaf ();
            program->add_instr (newop);
          }
        | a_expr T_SUB a_term 
          { 
            
            aqcl_variable * res;
            int semtype = program->get_semtype (OP_SUB, $1, $3);
            if (semtype != SEMTYPE_INT && semtype != SEMTYPE_FLOAT && semtype != SEMTYPE_DOUBLE)
            {
              cerr << "Error: Expected <int> or <float> type in arithmetic expression. Aborting ..." << endl;
              YYABORT;
            }
            res = program->new_temp_as_variable (semtype);
            aqcl_aexpr * newop;
            newop = new aqcl_aexpr (OP_SUB, res, $1, $3);
            $$ = newop;
            newop->unset_leaf ();
            program->add_instr (newop);
          }
        | a_term 
          { 
            
            $$ = $1; 
            
          }
        ;

a_term  : a_term T_MUL a_fact 
          { 
            
            aqcl_variable * res;
            int semtype = program->get_semtype (OP_MUL, $1, $3);
            res = program->new_temp_as_variable (semtype);
            aqcl_aterm * newop;
            newop = new aqcl_aterm (OP_MUL, res, $1, (aqcl_goperator*)$3);
            $$ = newop;
            newop->unset_leaf ();
            program->add_instr (newop);
            
          }
        | a_term T_DIV a_fact 
          { 
            
            aqcl_variable * res;
            assert ($1 && "a_term is null");
            assert ($3 && "a_fact is null");
            int semtype = program->get_semtype (OP_DIV, $1, $3);
            res = program->new_temp_as_variable (semtype);
            aqcl_aterm * newop;
            newop = new aqcl_aterm (OP_DIV, res, $1, (aqcl_goperator*)$3);
            $$ = newop;
            newop->unset_leaf ();
            program->add_instr (newop);
          }
        | a_fact 
          { 
            
            
            if ($1->get_result () && $1->get_result ()->is_stmt ())
            {
              $$ = (aqcl_block*)($1);
            }
            else
            if ($1->is_block ())
            {
              
              $$ = (aqcl_block*)($1);
            }
            else
            {
              $$ = (aqcl_aoperator*)$1;
            }
          }
        ;

a_fact  : T_ID arraydim
          { 
            
            if (not m_skip_st)
            {
              aqcl_node * asym = program->get_sym ($1);
              
              $$ = asym;
              if (asym->is_stmt ())
              {
                
                aqcl_statement * src = program->get_sym_as_statement (asym->get_name ());
                aqcl_statement * dst = program->new_temp_as_statement ();
                aqcl_cfact * circ = new aqcl_cfact (OP_SLOAD, dst, src);
                program->add_instr (circ);
                $$ = circ;
              }
              else if (asym->is_circ ())
              {
                
                
                
                aqcl_block * src = program->get_sym_as_block (asym->get_name ());
                aqcl_block * dst = program->new_temp_as_block ();
                
                
                aqcl_cfact * circ = new aqcl_cfact (OP_CLOAD, dst, src);
                program->add_instr (circ);
                $$ = circ;
                
                
                
                
                
                
                
                
                
                
              }
              else
              {
                
                if (asym->is_variable ())
                {
                  $$ = asym;
                }
                else
                  assert (0);
              }
              
              
            }
            else
            {
              
              assert (0);
              aqcl_variable * var = program->new_temp_as_variable (SEMTYPE_INT);
              $$ = var;
            }
          }
        | T_DOUBLE
          {
            
            aqcl_variable * res;
            res = program->new_temp_as_variable (SEMTYPE_DOUBLE);
            aqcl_val * val;
            val = new aqcl_val (std::stod($1));
            aqcl_afact * op = new aqcl_afact (OP_ALOAD, res, val);
            program->add_instr (op);
            $$ = op;
          }
        | T_INTEGER 
          {  
            
            aqcl_variable * res;
            res = program->new_temp_as_variable (SEMTYPE_INT);
            aqcl_val * val;
            val = new aqcl_val (std::atoi($1.c_str ()));
            aqcl_afact * op = new aqcl_afact (OP_ALOAD, res, val);
            program->add_instr (op);
            $$ = op;
            
          }
        | T_PI 
          {
            
            aqcl_val * val;
            val = new aqcl_val (M_PI);
            
            aqcl_variable * res;
            res = program->new_temp_as_variable (SEMTYPE_DOUBLE);
            aqcl_afact * op = new aqcl_afact (OP_ALOAD, res, val);
            program->add_instr (op);
            $$ = op;
          }
        | T_LPAR a_expr T_RPAR 
          {
            
            $$ = $2; 
          }
        | T_SUB a_fact 
          { 
            _D_(14.9);   
            aqcl_afact * newop;
            aqcl_variable * res;
            res = program->new_temp_as_variable ($2->get_result()->get_semtype ());
            newop = new aqcl_afact (OP_UMIN, res, $2->get_result ());
            newop->unset_leaf ();
            $$ = newop;
            program->add_instr (newop);
          }
        ;

arraydim  : T_LBRA a_expr T_RBRA
          | 
            {

            }
          ;

c_expr  : c_expr c_expr_op c_term 
          { 
            _D_(16.0);   
            
            
            
            if ($1->get_result_block ()->is_circ ())
            {
              aqcl_cexpr * expr;
              aqcl_block * res;
              res = program->new_temp_as_block ();
              expr = new aqcl_cexpr ($2, res, $1, $3);
              program->add_instr (expr);
              expr->unset_leaf ();
              $$ = expr;
            }
            else
            {
              cerr << "Unexpected argument types. Aborting ..." << endl;
              YYABORT;
            }
            
          }
        | c_expr T_PIPE c_term
          {
            aqcl_cexpr * circ;
            aqcl_block * res;
            res = program->new_temp_as_block ();
            if ($1->is_int () and $3->is_block ())
            {
              circ = new aqcl_cexpr (OP_PIPE_PRE, res, $3, $1);
            }
            else if ($1->is_block () and $3->is_int ())
            {
              circ = new aqcl_cexpr (OP_PIPE_SUF, res, $1, $3);
            }
            else 
            {
              cerr << "PIPE operation requires operands of type <block> and <int>. Aborting ...";
              cerr << endl;
              YYABORT;
            }
            program->add_instr (circ);
            circ->unset_leaf ();
            $$ = circ;
          }
        | c_term 
          { 
            
            $$ = $1;
            
            
          }
        ;

c_term  : c_term c_term_op c_fact 
          { 
            aqcl_cterm * circ;
            aqcl_block * res;
            res = program->new_temp_as_block ();
            if ($1->is_int () and $3->is_block ())
            {
              circ = new aqcl_cterm ($2, res, $3, $1);
            }
            else if ($1->is_block () and $3->is_int ())
            {
              circ = new aqcl_cterm ($2, res, $1, $3);
              
            }
            else 
            {
              cerr << "Scaling operation requires operands of type <block> and <int>. Aborting ...";
              cerr << endl;
              YYABORT;
            }
            program->add_instr (circ);
            circ->unset_leaf ();
            $$ = circ;
          }
        | c_fact 
          {
            
            
            $$ = $1;
          }
        ;

c_fact  : T_LT c_expr T_GT
          {
            $$ = $2;
          }
        | a_expr
          {
            
            
            
            
            if ($1->is_circ ())
            {
              
              $$ = (aqcl_block*)($1);

              
              
            }
            else
            {
              $$ = (aqcl_block*)($1);
            }
            
            
          }
        | c_gate 
          {
            
            assert ($1->is_gate ());
            aqcl_block * res;
            aqcl_cfact * circ;
            res = program->new_temp_as_block ();
            circ = new aqcl_cfact (OP_G2C, res, $1);
            program->add_instr (circ);
            
            $$ = circ;
          }
        ;

c_gate  : q1_gate
          { 
            
            aqcl_gate * gate;
            gate = new aqcl_gate ($1, axl_gate_num);
            program->add_instr (gate);
            
            $$ = gate;
          }
        | q1_gate 
          T_LPAR
          a_expr
          c_gate_arg
          T_RPAR
          { 
            
            aqcl_gate * gate;
            if (!$4)
            {
              aqcl_aoperator * arg1 = (aqcl_aoperator*)($3);
              gate = new aqcl_gate ($1, arg1, axl_gate_num);
              program->add_instr (gate);
              
              $$ = gate;
            }
            else
            {
              cerr << "Pending implementation of single qubit quantum gates with angles." << endl;
              YYABORT;
            }
          }
        | q2_gate 
          T_LPAR
          a_expr
          T_COMMA
          a_expr
          T_RPAR
          { 
            
            aqcl_gate * gate;
            aqcl_aoperator * arg1 = (aqcl_aoperator*)($3);
            aqcl_aoperator * arg2 = (aqcl_aoperator*)($5);
            gate = new aqcl_gate ($1, arg1, arg2, axl_gate_num);
            program->add_instr (gate);
            
            $$ = gate;
          }
        | q3_gate 
          T_LPAR
          a_expr
          T_COMMA
          a_expr
          T_COMMA
          a_expr
          T_RPAR
          { 
            
            aqcl_gate * gate;
            aqcl_aoperator * arg1 = (aqcl_aoperator*)($3);
            aqcl_aoperator * arg2 = (aqcl_aoperator*)($5);
            aqcl_aoperator * arg3 = (aqcl_aoperator*)($7);
            gate = new aqcl_gate ($1, arg1, arg2, arg3, axl_gate_num);
            program->add_instr (gate);
            
            $$ = gate;
          }
        ;




q1_gate : T_GATE_H { $$ = GATE_H; }
        | T_GATE_X { $$ = GATE_X; }
        | T_GATE_Y { $$ = GATE_Y; }
        | T_GATE_Z { $$ = GATE_Z; }
        | T_GATE_RTNOT { $$ = GATE_RTNOT; }
        | T_GATE_R { $$ = GATE_R; }
        | T_GATE_QFT { $$ = GATE_QFT; }
        ;

q2_gate : T_GATE_SWAP { $$ = GATE_SWAP; }
        | T_GATE_SQRT_SWAP { $$ = GATE_SQRT_SWAP; }
        | T_GATE_XX { $$ = GATE_XX; }
        | T_GATE_U { $$ = GATE_U; }
        | T_GATE_YY { $$ = GATE_YY; }
        | T_GATE_ZZ { $$ = GATE_ZZ; }
        | T_GATE_CX { $$ = GATE_CX; }
        | T_GATE_CY { $$ = GATE_CY; }
        | T_GATE_CZ { $$ = GATE_CZ; }
        ;

q3_gate : T_GATE_CCX { $$ = GATE_CCX; }
        | T_GATE_CSWAP { $$ = GATE_CSWAP; }
        ;

c_gate_arg  : T_COMMA a_expr { $$ = $2; }
            | { $$ = NULL; } 
            ;

oprel : T_LT  { $$ = OP_LT; }
      | T_LEQ { $$ = OP_LEQ; }
      | T_GT  { $$ = OP_GT; }
      | T_GEQ { $$ = OP_GEQ; }
      | T_EQUAL { $$ = OP_EQ; }
      ;

semtype : T_DT_INT { $$ = SEMTYPE_INT; }
        | T_DT_CIRC { $$ = SEMTYPE_CIRC; }
        | T_DT_PARAM { $$ = SEMTYPE_PARAM; }
        | T_DT_STMT { $$ = SEMTYPE_STMT; }
        ;

c_expr_op : T_COMP_T { $$ = OP_COMP_T; }
          | T_COMP_S { $$ = OP_COMP_S; }
          ;

c_term_op : T_ITER_S { $$ = OP_ITER_S; }
          | T_ITER_T { $$ = OP_ITER_T; }
          ;

dom_atch_op : T_ATCH_DOM_T { $$ = OP_ATCH_DOM_T; }
            | T_ATCH_DOM_S { $$ = OP_ATCH_DOM_S; }
            ;

%%



void yy::aqcsl_parser::error (const yy::location & l, const std::string & s) {
	std::cerr << "AXL parsing error at " << l << " : " << s << std::endl;
}



int main (int nargs, char * argv[])
{
  int res;
  int export_scop = 0;
  if (nargs == 2) {
    
    
    if (strcmp (argv[1], "--export-scop")==0)
      export_scop = 1;
  }
  aqcl_program * program;
  program = new aqcl_program ();
  program->parse_command_line_arguments (nargs, argv);
	yy::aqcsl_parser parser (program); 
  res = parser.parse();
	if (res) 
    fprintf (stderr, "AXL syntax error\n");
  else
  {
    fprintf (stderr, "AXL program is valid\n");
    program->print_symtab ();
    program->print_itab ();
    program->print_domain_statements ();
    
    res = program->run ();
    if (export_scop)
      program->export_scop ();
    if (res)
    {
      cerr << "Program aborted with error." << endl;
    }
  }
	return 0;
}
