/*
AQCSL: the Affine Quantum Circuit Specification Language
Filename: circuit-schedule.hh
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



#ifndef AXL_CIRCUIT_SCHEDULE
#define AXL_CIRCUIT_SCHEDULE

#include "utils.hh"
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

struct circuit_dimension;

#define AXL_SCALAR_DIM 0
#define AXL_LINEAR_DIM 1
#define AXL_LINCOM_DIM 2

struct circuit_dimension { 
  int level;
  isl_union_map * partial;
  isl_set * filter;
  int scalar;
  int dim_type;
  struct circuit_dimension * parent;
  struct circuit_dimension ** subtree;
  int n_children;
  int id;
  int spliced;
};

typedef vector<int> scalar_list;

typedef struct circuit_dimension circuit_schedule;
typedef vector<struct circuit_dimension*> schedule_dimension;


struct s_schedule_band {
  int nbs;
  int ndim;
  int maxdim;
  isl_basic_set *** set;
  isl_basic_map *** map;
  int * len;
};

typedef struct s_schedule_band schedule_band;

extern
void
axl_circuit_schedule_free (circuit_schedule * cs);


extern
void
axl_circuit_schedule_show (circuit_schedule * cs, const char * msg = "");


extern
circuit_schedule *
axl_circuit_schedule_from_union_map (__isl_keep isl_union_map * umap);


extern
void
axl_circuit_eliminate_redundant_dimensions (circuit_schedule * cs);


extern
__isl_give
isl_union_map *
axl_execute_schedule_dimension_removal (circuit_schedule * cs);


#endif
