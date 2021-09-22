/*
AQCSL: the Affine Quantum Circuit Specification Language
Filename: converter.hh
Creation date: July 20, 2020.
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

#ifndef _AXL_CONVERTER_HH_
#define _AXL_CONVERTER_HH_

#include "utils.hh"
#include "osl/relation.h"
#include "osl/scop.h"
#include "osl/statement.h"
#include "osl/macros.h"

#include "isl/mat.h"
#include "isl/map.h"
#include "isl/set.h"
#include "isl/mat.h"
#include "isl/union_map.h"
#include "isl/union_set.h"
#include "isl/val.h"

#include "candl/candl.h"
#include "scoplib/scop.h"

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>


#define IS_EQU 0
#define IS_INEQ 1

#define AXL_READ 0
#define AXL_WRITE 1
#define AXL_SCHEDULE 2

#define AXL_PREC OSL_PRECISION_DP

typedef CandlStatement candl_statement;
typedef candl_statement * candl_statement_p;
typedef CandlMatrix candl_matrix;
typedef candl_matrix * candl_matrix_p;
typedef CandlOptions candl_options;
typedef candl_options * candl_options_p;

struct s_converter_info;
typedef struct s_converter_info convinfo;

#define AXL_ARRAY_ID_PLACEHOLDER 1

typedef map<isl_union_set*,string> s2b_map;
typedef map<isl_union_set*,string>::iterator s2b_map_it;

struct s_converter_info {
  isl_set * domain;
  isl_map * schedule;
  isl_map * accread;
  isl_map * accwrite;
  isl_union_map * schedule_all;
  isl_union_map * accread_all;
  isl_union_map * accwrite_all;
  isl_union_map * calls_all;
  isl_union_set * domain_all;
  //osl_scop_p scop;
  void * scop;
  osl_statement_p stmt; // stmt will point to the last statement to be created in the osl scop
  candl_statement_p candl_stmt;
  scoplib_statement_p scoplib_stmt;
  int map_mode;
  int idx;
  int offset;
  s2b_map * stm2body;
  int max_dim; // maximum dimensionality
};

typedef struct s_converter_info axl_proto_scop;


extern
osl_scop_p
axl_converter_axl_scop_to_openscop (
  __isl_keep isl_union_set * udom,
  __isl_keep isl_union_map * uaccread,
  __isl_keep isl_union_map * uaccwrite,
  __isl_keep isl_union_map * usched);

extern
candl_program_p
axl_converter_axl_scop_to_candl (
  __isl_keep isl_union_set * udom,
  __isl_keep isl_union_map * uaccread,
  __isl_keep isl_union_map * uaccwrite,
  __isl_keep isl_union_map * usched,
  int n_param);

extern
scoplib_scop_p
axl_converter_axl_scop_to_scoplib (
  t_mini_scop * mscop,
//  __isl_keep isl_union_set * udom,
//  __isl_keep isl_union_map * uaccread,
//  __isl_keep isl_union_map * uaccwrite,
//  __isl_keep isl_union_map * usched,
//  __isl_keep isl_union_map * ucalls,
  std::vector<string> & parameters,
  s2b_map & stm2body);

extern
candl_options_p axl_init_candl_options ();

extern
__isl_give
isl_union_map *
axl_convert_scoplib_schedule_matrix_to_union_map (
  isl_ctx * ctx, scoplib_scop_p scop);

#endif
