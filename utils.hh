/*
AQCSL: the Affine Quantum Circuit Specification Language
Filename: utils.hh
Creation date: July 20, 2020.
Copyright (C) 2020, Martin Kong

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

#ifndef AXL_UTILS
#define AXL_UTILS

#include "isl/set.h"
#include "isl/map.h"
#include "isl/union_map.h"
#include "isl/union_set.h"
#include "isl/ast.h"
#include "isl/ast_build.h"
#include "isl/printer.h"
#include "isl/options.h"
#include "isl/ast_type.h"
#include "isl/id.h"
#include "isl/schedule_node.h"
#include "isl/schedule_type.h"
#include "isl/aff.h"

#include <assert.h>
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include <utility>

using namespace std;

#define AXL_NAN 999999

typedef map<string,int> t_program_order;

struct s_iss_data_t {
  int dist;
  int dim;
  string domain_name;
  isl_set * set;
};

typedef struct s_iss_data_t t_iss_data;


struct s_foreach_data_t {
  isl_union_map * umap;
  isl_union_set * uset;
  isl_map * map;
  set<string> * affected;
  set<string> * keep;
  vector<t_iss_data*> * iss;
  isl_ast_expr_list * ast_exl;
  isl_ast_build * build;
  int scal;
  int pos;
  string str_ret;
  char * name;
};
typedef struct s_foreach_data_t t_foreach;

typedef vector<isl_basic_set*> vector_basic_set;
typedef vector<isl_set*> vector_set;
typedef vector<isl_basic_map*> vector_basic_map;
typedef vector<isl_map*> vector_map;


void print_isl_basic_map_debug (__isl_keep isl_basic_map * basic_map, const char * msg);
void print_isl_map_debug (__isl_keep isl_map * map, const char * msg);
void print_isl_union_map_debug (__isl_keep isl_union_map * union_map, const char * msg);
void print_isl_basic_set_debug (__isl_keep isl_basic_set * basic_set, const char * msg);
void print_isl_set_debug (__isl_keep isl_set * set, const char * msg);
void print_isl_union_set_debug (__isl_keep isl_union_set * union_set, const char * msg);

extern
__isl_give
isl_union_map *
axl_clean_union_map (__isl_take isl_union_map * umap);

extern
isl_stat insert_scalar_dimensions (__isl_take isl_map * map, void * usr);

extern
int get_max_dim (__isl_keep isl_union_map * umap);

extern
__isl_give isl_union_map *
axl_union_map_pad (__isl_take isl_union_map * umap);

extern
__isl_give isl_union_map *
axl_build_2dp1_identity_schedule_from_domain (__isl_keep isl_union_set * uset);

typedef map<string,int> t_spo; // statement program order

struct s_mini_scop_t {
  isl_union_set * domain;
  isl_union_map * read;
  isl_union_map * write;
  isl_union_map * schedule;
  isl_union_map * dependence;
  isl_union_map * filter;
  isl_union_map * call;
  int backend;
  t_spo prog_order;
};

typedef struct s_mini_scop_t t_mini_scop;

struct s_user_call_t {
  t_mini_scop * scop;
  isl_printer * printer;
};

typedef struct s_user_call_t t_user_call;


struct s_isl_plot {
  ofstream * strm;
  int index;
};
typedef struct s_isl_plot t_isl_plot;

struct s_axl_dot {
  ofstream * strm;
  map<string,int> node_map;
  int index;
};
typedef struct s_axl_dot t_axl_dot;

extern
int
axl_compute_iss_distance (__isl_keep isl_union_map * deps, vector<t_iss_data*> & iss);


extern
string axl_build_isl_iss_filter_str (
  __isl_keep isl_set * set, int split_dim, int dep_dist, int res_class);


extern
void show_collected_iss_data (vector<t_iss_data*> & iss);


extern
int 
axl_isl_basic_map_is_dim_single_valued (
  __isl_keep isl_basic_map * map, isl_dim_type dt, int pos);

extern
int 
axl_isl_map_is_dim_single_valued (
  __isl_keep isl_map * map, isl_dim_type dt, int pos);


extern
int
axl_isl_set_get_si (__isl_keep isl_set * set, isl_dim_type dt, int pos);

extern
int
axl_isl_basic_map_get_si (__isl_keep isl_basic_map * map, isl_dim_type dt, int pos);

extern
int
axl_isl_map_get_si (__isl_keep isl_map * map, isl_dim_type dt, int pos);


extern
int
import_isl_sets_for_iss (
  __isl_keep isl_union_set * uset, vector<t_iss_data*> * iss);


extern
t_mini_scop *
axl_iss_stair_pattern (t_mini_scop * scop_in);


extern
t_mini_scop * axl_mini_scop_alloc ();


extern
void axl_mini_scop_free (t_mini_scop * scop);


extern
void axl_mini_scop_append_unaffected_objects (
  t_mini_scop * src, t_mini_scop * dst, __isl_take isl_union_set * affected_doms);


extern
void axl_mini_scop_show (t_mini_scop * scop, const char * scop_name);


extern
__isl_give 
isl_printer * axl_stmt_print_user(
  __isl_take isl_printer * prt, 
  __isl_take isl_ast_print_options *options,
  __isl_keep isl_ast_node * node,
  void * usr);


extern
__isl_give 
isl_ast_node * axl_at_each_domain (
  __isl_take isl_ast_node * node,
  __isl_keep isl_ast_build * build,
  void * usr);


extern
__isl_give 
isl_schedule_node *
axl_schedule_node_band_unroll_loop (
  __isl_take isl_schedule_node * node, void * usr);


extern
isl_bool
axl_schedule_node_band_unroll_loop_bool (
  __isl_take isl_schedule_node * node, void * usr);


extern
__isl_give
isl_union_map * 
axl_create_unroll_union_map (__isl_keep isl_union_map * schedule_map);

extern
__isl_give
isl_union_map *
axl_build_schedule_map_from_iteration_domain (__isl_keep isl_union_set * udom);

extern
__isl_give
isl_union_map *
axl_reverse_schedule_map (__isl_keep isl_union_map * umap);

extern
__isl_give
isl_set *
axl_rename_set (__isl_take isl_set * set, const char * newname);

extern
__isl_give
isl_map *
axl_rename_map (__isl_take isl_map * map, const char * newname);

extern
__isl_give
isl_union_set *
axl_rename_union_set (__isl_take isl_union_set * uset, const char * newname);

extern
__isl_give
isl_union_map *
axl_rename_union_map (__isl_take isl_union_map * umap, const char * newname);

extern
__isl_give
isl_union_map *
axl_copy_names_to_output_tuples (__isl_keep isl_union_map * umap);

extern
string
axl_union_map_apply_parametric_shift (
  __isl_keep isl_union_map * umap, const char * bigsum);


extern
void
axl_union_set_to_vector_set (__isl_keep isl_union_set * uset, vector_set * vs);

extern
void
axl_union_map_to_vector_map (__isl_keep isl_union_map * umap, vector_map * vm);



extern
void axl_plot_isl_domains (__isl_keep isl_union_set * udom, ofstream & strm);

extern
void axl_plot_dependence_graph (__isl_keep isl_union_map * umap);


extern
__isl_give
isl_basic_map * isl_map_to_basic_map (__isl_take isl_map * map);

#define AXL_CPU_TIME (getrusage(RUSAGE_SELF,&ruse), ruse.ru_utime.tv_sec + \
  ruse.ru_stime.tv_sec + 1e-6 * \
  (ruse.ru_utime.tv_usec + ruse.ru_stime.tv_usec))

extern
double axl_time ();


extern
__isl_give
isl_union_map * 
axl_union_map_drop_redundant_dimensions_from_output_tuple (
  __isl_keep isl_union_map * umap, int keep_name, bool);


extern
__isl_give
isl_union_map * 
axl_union_set_redundance_removal_union_map (__isl_keep isl_union_set * uset, bool consider_id);


extern
__isl_give  isl_union_map * 
axl_union_map_drop_all_scalar_dimensions_from_output_tuple (
  __isl_take isl_union_map * umap, int keep_name, int start_dim, bool);


extern
__isl_give isl_union_map *
axl_convert_to_2dp1_insert_scalar_dimensions (
  __isl_keep isl_union_set * uset, __isl_take isl_union_map * umap, bool);


extern
__isl_give isl_union_map *
axl_union_map_coalesce_scalar_prefix_dimensions (__isl_take isl_union_map * umap, bool);

extern
void axl_show_mini_scop (t_mini_scop * mscop);

extern void 
axl_sort_statements_by_program_order (vector_set & vs, t_program_order & po);

extern void vector_set_free (vector_set & vs);

extern void vector_map_free (vector_map & vm);

extern void vector_basic_map_free (vector_basic_map & vbm);

extern
__isl_give isl_union_map *
vector_map_apply_range (__isl_take isl_union_map * umap1, 
  __isl_take isl_union_map * umap2);

extern
void
axl_map_deunionize (__isl_keep isl_map * map, vector_basic_map * vbm);

extern
__isl_give
isl_union_map * 
axl_isolate_domains (__isl_keep isl_union_map * umap);

extern
__isl_give
isl_union_map *
axl_vector_map_to_union_map (vector_map * vm);

extern
__isl_give
isl_union_set *
axl_vector_set_to_union_set (vector_set * vm);

extern
__isl_give
isl_union_map *
axl_union_set_deunionize (__isl_keep isl_union_set * uset);

extern
__isl_give
isl_union_map *
axl_union_map_deunionize (__isl_keep isl_union_map* umap);

extern
int
axl_union_map_is_deunionized (__isl_keep isl_union_map * umap);

extern
void 
assert_single_basic_map_in_map (__isl_keep isl_map * map);

extern
__isl_give
isl_union_map *
axl_coalesce_schedule_suffix (__isl_take isl_union_map * umap);

extern
void
axl_union_map_separate_basic_maps (
  __isl_keep isl_union_map* umap, vector_basic_map & vbm);

extern
void
axl_vector_basic_map_show (vector_basic_map & vbm);

extern
__isl_give
isl_union_set * 
axl_union_set_drop_all_parameters (__isl_take isl_union_set * uset);

extern
__isl_give
isl_union_map * 
axl_union_map_drop_all_parameters (__isl_take isl_union_map * umap);


#endif
