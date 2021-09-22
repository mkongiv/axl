/*
AQCSL: the Affine Quantum Circuit Specification Language
Filename: utils.cc
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

#include "utils.hh"


__isl_give
isl_union_map *
axl_clean_union_map (__isl_take isl_union_map * umap)
{
  isl_union_map * ret;
  isl_ctx * ctx = isl_union_map_get_ctx (umap);
  char * str = isl_union_map_to_str (umap);
  isl_union_map_free (umap);
  ret = isl_union_map_read_from_str (ctx, str);
  free (str);
  return ret;
}

void print_isl_basic_map_debug (__isl_keep isl_basic_map * basic_map, const char * msg)
{
  #ifdef _AXL_DEBUG_
  if (!basic_map)
  {
    printf ("%s : NULL\n", msg);
    return;
  }
  char * basic_mapstr = isl_basic_map_to_str (basic_map);
  printf ("%s : %s\n", msg, basic_mapstr);
  free (basic_mapstr);
  #endif
}

void print_isl_map_debug (__isl_keep isl_map * map, const char * msg)
{
  #ifdef _AXL_DEBUG_
  if (!map)
  {
    printf ("%s : NULL\n", msg);
    return;
  }
  char * mapstr = isl_map_to_str (map);
  printf ("%s : %s\n", msg, mapstr);
  free (mapstr);
  #endif
}

void print_isl_union_map_debug (__isl_keep isl_union_map * union_map, const char * msg)
{
  #ifdef _AXL_DEBUG_
  if (!union_map)
  {
    printf ("%s : NULL\n", msg);
    return;
  }
  char * union_mapstr = isl_union_map_to_str (union_map);
  printf ("%s : %s\n", msg, union_mapstr);
  free (union_mapstr);
  #endif
}

void print_isl_basic_set_debug (__isl_keep isl_basic_set * basic_set, const char * msg)
{
  #ifdef _AXL_DEBUG_
  if (!basic_set)
  {
    printf ("%s : NULL\n", msg);
    return;
  }
  char * basic_setstr = isl_basic_set_to_str (basic_set);
  printf ("%s : %s\n", msg, basic_setstr);
  free (basic_setstr);
  #endif
}

void print_isl_set_debug (__isl_keep isl_set * set, const char * msg)
{
  #ifdef _AXL_DEBUG_
  if (!set)
  {
    printf ("%s : NULL\n", msg);
    return;
  }
  char * setstr = isl_set_to_str (set);
  printf ("%s : %s\n", msg, setstr);
  free (setstr);
  #endif
}

void print_isl_union_set_debug (__isl_keep isl_union_set * union_set, const char * msg)
{
  #ifdef _AXL_DEBUG_
  if (!union_set)
  {
    printf ("%s : NULL\n", msg);
    return;
  }
  char * union_setstr = isl_union_set_to_str (union_set);
  printf ("%s : %s\n", msg, union_setstr);
  free (union_setstr);
  #endif
}

int 
axl_isl_basic_map_is_dim_single_valued (
  __isl_keep isl_basic_map * map, isl_dim_type dt, int pos)
{
  isl_basic_map * temp = isl_basic_map_copy (map);
  int nout = isl_basic_map_dim (temp, dt);
  if (pos < nout - 1)
    temp = isl_basic_map_remove_dims (temp, dt, pos + 1, nout - pos - 1);
  if (pos - 1 >= 0)
    temp = isl_basic_map_remove_dims (temp, dt, 0, pos);
  isl_set * set = isl_set_from_basic_set (isl_basic_map_range (temp));
  int ret = isl_set_is_singleton (set) == isl_bool_true;
  isl_set_free (set);
  return ret;
}

int 
axl_isl_map_is_dim_single_valued (
  __isl_keep isl_map * map, isl_dim_type dt, int pos)
{
  isl_map * temp = isl_map_copy (map);
  int nout = isl_map_dim (temp, dt);
  if (pos < nout - 1)
    temp = isl_map_remove_dims (temp, dt, pos + 1, nout - pos - 1);
  if (pos - 1 >= 0)
    temp = isl_map_remove_dims (temp, dt, 0, pos);

  // If after reducing the map to a single dimension it still involves
  // constraints using parameters, then we can't consider it as a fixed-single 
  // valued dimension.
  int ret;
  isl_bool uses_param;
  int nparam = isl_map_dim (temp, isl_dim_param);
  uses_param = isl_map_involves_dims (temp, isl_dim_param, 0, nparam);
  ret = uses_param == isl_bool_true;
  if (ret)
  {
    isl_map_free (temp);
    return 0;
  }

  // Last check: if the set is a singleton after everything, then we
  // definitely consider it as single-valued.
  isl_set * set = isl_map_range (temp);
  ret = isl_set_is_singleton (set) == isl_bool_true;
  isl_set_free (set);
  return ret;
}

int
axl_isl_set_get_si (__isl_keep isl_set * set, isl_dim_type dt, int pos)
{
  isl_val * val;
  int ret;
  val = isl_set_plain_get_val_if_fixed (set, dt, pos);
  if (isl_val_is_nan (val) == isl_bool_true)
    ret = AXL_NAN;
  else
    ret = isl_val_get_num_si (val);
  isl_val_free (val);
  return ret;
}

int
axl_isl_basic_map_get_si (__isl_keep isl_basic_map * map, isl_dim_type dt, int pos)
{
  isl_val * val;
  int ret;
  val = isl_basic_map_plain_get_val_if_fixed (map, dt, pos);
  if (isl_val_is_nan (val) == isl_bool_true)
    ret = AXL_NAN;
  else
    ret = isl_val_get_num_si (val);
  isl_val_free (val);
  return ret;
}

int
axl_isl_map_get_si (__isl_keep isl_map * map, isl_dim_type dt, int pos)
{
  isl_val * val;
  int ret;
  val = isl_map_plain_get_val_if_fixed (map, dt, pos);
  if (isl_val_is_nan (val) == isl_bool_true)
    ret = AXL_NAN;
  else
    ret = isl_val_get_num_si (val);
  isl_val_free (val);
  return ret;
}

/*
* Insert a scalar dimension with value usr->scal and at position usr->pos
*
*/
isl_stat insert_scalar_dimensions (__isl_take isl_map * map, void * usr)
{
  t_foreach * data = (t_foreach*)(usr);
  map = isl_map_insert_dims (map, isl_dim_out, data->pos, 1);
  map = isl_map_fix_si (map, isl_dim_out, data->pos, data->scal);
  if (data->umap)
    data->umap = isl_union_map_union (data->umap,
      isl_union_map_from_map (map));
  else
    data->umap = isl_union_map_from_map (map);
  return isl_stat_ok;
}

/*
 * Create an identity schedule in 2d+1 format from an isl_set.
 *
 */
isl_stat build_identity_2dp1_schedule_map (__isl_take isl_set * set, void * usr)
{
  t_foreach * data = (t_foreach*)(usr);
  int ndim = isl_set_n_dim (set);
  isl_map * map = isl_set_identity (isl_set_copy (set));
  map = isl_map_set_tuple_name (map, isl_dim_out, "");
  int dd;
  // Insert d+1 scalar dimensions, set new scalar dimensions to zero.
  for (dd = 0; dd <= ndim; dd++)
  {
    map = isl_map_insert_dims (map, isl_dim_out, ndim - dd, 1);
    map = isl_map_fix_si (map, isl_dim_out, ndim - dd, 0);
  }
  // Copy all fixed scalar dimensions dd, to schedule dimension (2*dd+1)
  for (dd = 0; dd < ndim; dd++)
  {
    isl_val * val = isl_set_plain_get_val_if_fixed (set, isl_dim_set, dd);
    if (isl_val_is_nan (val) == isl_bool_false)
    {
      int si = isl_val_get_num_si (val);
      map = isl_map_fix_si (map, isl_dim_out, 2*dd + 1, si);
    }
    isl_val_free (val);
  }
  if (data->umap)
    data->umap = isl_union_map_union (data->umap,
      isl_union_map_from_map (map));
  else
    data->umap = isl_union_map_from_map (map);
  isl_set_free (set);
  return isl_stat_ok;
}

__isl_give
isl_basic_map * isl_map_to_basic_map (__isl_take isl_map * map)
{
  char * ptr = isl_map_to_str (map);
  isl_basic_map * ret;
  isl_ctx * ctx = isl_map_get_ctx (map);
  ret = isl_basic_map_read_from_str (ctx, ptr);
  free (ptr);
  isl_map_free (map);
  return ret;
}

/*
* Compute the output dimenionality of the map and store it in
* data->scal if it's greater than the current value.
*/
isl_stat get_map_dim (__isl_take isl_map * map, void * usr)
{
  t_foreach * data = (t_foreach*)(usr);
  int ndim = isl_map_dim (map, isl_dim_out);
  if (ndim > data->scal)
    data->scal = ndim;
  isl_map_free (map);
  return isl_stat_ok;
}

/*
* Return the maximum output dimensionality among all the maps in the
* union map.
*/
int get_max_dim (__isl_keep isl_union_map * umap)
{
  t_foreach data;
  data.scal = 0;
  isl_stat res;
  res = isl_union_map_foreach_map (umap, &get_map_dim, (void*)(&data));
  assert (res == isl_stat_ok);
  return data.scal;
}

/*
* If the number of output dimensions is less than usr->scal, then
* append new dimensions to the map, set to zero, and aggregate
* the map to data->umap.
*/
isl_stat pad_map (__isl_take isl_map * map, void * usr)
{
  t_foreach * data = (t_foreach*)(usr);
  int ndim = isl_map_dim (map, isl_dim_out);
  if (ndim < data->scal)
  {
    map = isl_map_insert_dims (map, isl_dim_out, ndim, data->scal - ndim);
    int ii;
    for (ii = ndim; ii < data->scal; ii++)
      map = isl_map_fix_si (map, isl_dim_out, ii, 0);
  }
  if (data->umap)
    data->umap = isl_union_map_union (data->umap,
      isl_union_map_from_map (map));
  else
    data->umap = isl_union_map_from_map (map);
  return isl_stat_ok;
}

/*
* Pad all the maps in the union to make them be max-dimensional.
*
*/
__isl_give isl_union_map *
axl_union_map_pad (__isl_take isl_union_map * umap)
{
  int md = get_max_dim (umap);
  t_foreach data;
  data.umap = NULL;
  data.scal = md;
  isl_stat res;
  res = isl_union_map_foreach_map (umap, &pad_map, (void*)(&data));
  isl_union_map_free (umap);
  assert (res == isl_stat_ok);
  return data.umap;
}

__isl_give isl_union_map *
axl_build_2dp1_identity_schedule_from_domain (__isl_keep isl_union_set * uset)
{
  t_foreach data;
  data.umap = NULL;
  isl_stat res;
  res = isl_union_set_foreach_set (uset, &build_identity_2dp1_schedule_map, (void*)(&data));
  assert (res == isl_stat_ok);
  return data.umap;
}



/*
* Determine if the current basic map fulfills the stair-pattern.
* If so, add it to @usr->umap and add the tuple name to @usr->affected.
* A candidate dependence must meet the following conditions:
* a)  it must be a self dependence, i.e., the input and output name tuple
*     must be the same
* b)  we seek only one depence of this type for each statement,
*     so we check that the current tuple name is not in @usr->affected
* c)  the dependence distance vector must have all but one of its components
*     equal to zero, and the non-zero component must have positive value 
*     greater than 1
*/
isl_stat
check_delta_map (__isl_take isl_basic_map * bmap, void * usr)
{
  t_foreach * data = (t_foreach*)(usr);
  const char * src_name = isl_basic_map_get_tuple_name (bmap, isl_dim_in);
  const char * dst_name = isl_basic_map_get_tuple_name (bmap, isl_dim_out);
  // skip bmap if it's not a self dependence
  if (strcmp (src_name, dst_name))
  {
    isl_basic_map_free (bmap);
    return isl_stat_ok;
  }
  if (data->affected->find (string(src_name)) != data->affected->end ())
  {
    isl_basic_map_free (bmap);
    return isl_stat_ok;
  }
  isl_basic_map * original_map = isl_basic_map_copy (bmap);
  isl_set * deltas = isl_set_from_basic_set (isl_basic_map_deltas (bmap));
  isl_ctx * ctx = isl_set_get_ctx (deltas);
  int ii;
  int nd = isl_set_dim (deltas, isl_dim_set);
  int nzeros = 0;
  int has_nz = 0;
  int dist[nd];
  isl_val * neg_one = isl_val_int_from_si (ctx, -1);
  for (ii = 0; ii < nd; ii++)
  {
    isl_val * val = isl_set_plain_get_val_if_fixed (deltas, isl_dim_set, ii);
    if (isl_val_is_zero (val) == isl_bool_true)
      nzeros ++;
    else 
    {
      // Must be an integer
      // Delta step must be > 1 for ascending schedules
      // and < - 1 for descending schedules
      if (isl_val_is_int (val) == isl_bool_true &&
          (isl_val_gt_si (val, 1) == isl_bool_true || 
           isl_val_lt (val, neg_one) == isl_bool_true))
        has_nz++;
    }
    dist[ii] = isl_val_get_num_si (val);
    isl_val_free (val);
  }
  isl_val_free (neg_one);
  if (nzeros == nd - 1 && has_nz == 1)
  {
    data->scal = 0;
    for (ii = 0; ii < nd; ii++)
      if (dist[ii] > 1 || dist[ii] < -1)
      {
        t_iss_data * info = new t_iss_data;
        info->dim = ii;
        info->dist = dist[ii];
        info->domain_name = string(src_name);
        data->iss->push_back (info);
        data->affected->insert (string(src_name));
        break;
      }
  }
  else
  {
  }
  isl_basic_map_free (original_map);
  isl_set_free (deltas);
  return isl_stat_ok;
}

void show_collected_iss_data (vector<t_iss_data*> & iss)
{
  vector<t_iss_data*>::iterator ii;
  int idx = 0;
  printf ("\n\nShowing collected ISS candidates:\n");
  for (ii = iss.begin (); ii != iss.end (); ii++, idx++)
  {
    printf ("ISS target #%d: domain=%s, dist=%d, dim=%d\n",
      idx, (*ii)->domain_name.c_str (), (*ii)->dist, (*ii)->dim);
  }
  printf ("\n\n");
}

isl_stat
check_iss_condition (__isl_take isl_map * map, void * usr)
{
  t_foreach * data = (t_foreach*)(usr);
  isl_stat res;
  res = isl_map_foreach_basic_map (map, &check_delta_map, usr);
  assert (res == isl_stat_ok);
  return res;
}

int
axl_compute_iss_distance (__isl_keep isl_union_map * deps, vector<t_iss_data*> & iss)
{
  t_foreach data;
  set<string> affected;
  data.scal = 0;
  data.iss = &iss;
  data.affected = &affected;
  isl_stat res;
  res = isl_union_map_foreach_map (deps, &check_iss_condition, (void*)(&data));
  iss = *data.iss;
  return iss.size ();
}

/*
* Build a generic string for a map of the shape:
*   S[i1,i2,...in] -> S_<res_class>[...,split_dim,...] : 
*     split_dim = i_k % dep_dist and split_dim = res_class
*/
string axl_build_isl_iss_filter_str (
  __isl_keep isl_set * set, int split_dim, int dep_dist, int res_class)
{
  stringstream ret;
  string name = isl_set_get_tuple_name (set);
  int nd = isl_set_dim (set, isl_dim_set);
  ret << "{";
  ret << name;
  ret << "[";
  int ii;
  for (ii = 0; ii < nd; ii++)
  {
    if (ii > 0)
      ret << ", ";
    ret << "i";
    ret << ii;
  }
  ret << "] -> ";
  ret << name;
  ret << "_";
  ret << res_class;
  ret << "[";
  for (ii = 0; ii < nd; ii++)
  {
    if (ii > 0)
      ret << ", ";
    if (ii != split_dim)
    {
      ret << "i";
      ret << ii;
    }
    else
    {
      ret << "sd";
    }
  }
  ret << "] :";
  ret << "sd = i" << split_dim;
  ret << " and ";
  ret << "i" << split_dim << " % " << dep_dist << " = " << res_class;
  ret << "}";
  return ret.str ();
}

/*
* Perform: @usr->uset += @set if name(set) \in @usr->affected
*
*/
isl_stat
extract_set (__isl_take isl_set * set, void * usr)
{
  t_foreach * data = (t_foreach*)(usr);
  const char * name = isl_set_get_tuple_name (set);
  if (data->scal)
  {
    vector<t_iss_data*>::iterator ii;
    for (ii = data->iss->begin (); ii != data->iss->end (); ii++)
      if ((*ii)->domain_name == string(name))
      {
        (*ii)->set = isl_set_copy (set);
        break;
      }
  }
  else
  {
    if (data->affected->find (string(name)) != data->affected->end ())
    {
      if (data->uset)
        data->uset = isl_union_set_union (data->uset, 
          isl_union_set_from_set (isl_set_copy (set)));
      else
        data->uset = isl_union_set_from_set (isl_set_copy (set));
    }
  }
  isl_set_free (set);
  return isl_stat_ok;
}

/*
* Construct and return an isl_union_set that only contains the names
* of sets that appear in @affected.
*
*/
__isl_give
isl_union_set *
filter_union_set_by_names (
  __isl_keep isl_union_set * uset, set<string> & affected)
{
  t_foreach data;
  data.uset = NULL;
  data.affected = &affected;
  isl_stat res;
  res = isl_union_set_foreach_set (uset, &extract_set, (void*)(&data));
  assert (res == isl_stat_ok);
  return data.uset;
}

int
import_isl_sets_for_iss (
  __isl_keep isl_union_set * uset, vector<t_iss_data*> * iss)
{
  t_foreach data;
  data.uset = NULL;
  data.iss = iss;
  data.scal = 1;
  isl_stat res;
  res = isl_union_set_foreach_set (uset, &extract_set, (void*)(&data));
  assert (res == isl_stat_ok);
  return iss->size ();
}

/*
* Simply add the input tuple name of @map to @usr->affected
*
*/
isl_stat
collect_name (__isl_take isl_map * map, void * usr)
{
  t_foreach * data = (t_foreach*)(usr);
  data->affected->insert (string (isl_map_get_tuple_name (map, isl_dim_in))); 
  return isl_stat_ok;
}

/*
* Extract the names of the iteration domains to be affected/processed in some
* future transformation or analysis.
* In particular, the @deps argument could represent the dependences
* selected for some ISS transformation, such as identifying the stair pattern.
*/
void axl_collect_statement_name_from_dependence (
  __isl_keep isl_union_map * deps, set<string> & affected)
{
  t_foreach data;
  data.affected = &affected;
  isl_stat res;
  res = isl_union_map_foreach_map (deps, &collect_name, (void*)(&data));
  assert (res == isl_stat_ok);
}


t_mini_scop * 
axl_iss_stair_pattern (t_mini_scop * scop_in)
{
  t_mini_scop * scop_out = axl_mini_scop_alloc ();
  // Create new domains
  scop_out->domain = isl_union_map_range (
    isl_union_map_intersect_domain (
      isl_union_map_copy (scop_in->filter),
      isl_union_set_copy (scop_in->domain)));
  // Copy the filter map and reverse it.
  isl_union_map * invfilt;
  invfilt = isl_union_map_reverse (isl_union_map_copy (scop_in->filter));
  // Modify the schedules
  scop_out->schedule = isl_union_map_copy (scop_in->schedule);
  scop_out->schedule = isl_union_map_apply_range (
    isl_union_map_copy (invfilt),
    scop_out->schedule);
  // Modify the reads
  scop_out->read = isl_union_map_copy (scop_in->read);
  scop_out->read = isl_union_map_apply_range (
    isl_union_map_copy (invfilt), scop_out->read);
  // Modify the writes
  scop_out->write = isl_union_map_copy (scop_in->write);
  scop_out->write = isl_union_map_apply_range (
    isl_union_map_copy (invfilt), scop_out->write);
  // Modify call union map
  scop_out->call = isl_union_map_copy (scop_in->call);
  scop_out->call = isl_union_map_apply_range (
    isl_union_map_copy (invfilt), scop_out->call);
  // Free the temporary filter map
  isl_union_map_free (invfilt);
  return scop_out;
}

t_mini_scop * axl_mini_scop_alloc ()
{
  t_mini_scop * ret;
  ret = new t_mini_scop;
  ret->domain = NULL;
  ret->schedule = NULL;
  ret->read = NULL;
  ret->write = NULL;
  ret->dependence = NULL;
  ret->filter = NULL;
  ret->call = NULL;
  return ret;
}

#define axl_free_isl(field,atype) if (scop->field) isl_ ## atype ## _free(scop->field)
void axl_mini_scop_free (t_mini_scop * scop)
{
  axl_free_isl(domain,union_set);
  axl_free_isl(schedule,union_map);
  axl_free_isl(read,union_map);
  axl_free_isl(write,union_map);
  axl_free_isl(dependence,union_map);
  axl_free_isl(filter,union_map);
  axl_free_isl(call,union_map);
}

#define axl_add_non_affected(field,atype) \
  {\
    dst->field = \
      isl_ ## atype ## _union (dst->field, \
        isl_ ## atype ## _intersect_domain ( \
          isl_ ## atype ## _copy (src->field), isl_union_set_copy (unaffected_doms))); \
  }
void axl_mini_scop_append_unaffected_objects (
  t_mini_scop * src, t_mini_scop * dst, __isl_take isl_union_set * affected_doms)
{
  assert (affected_doms);
  isl_union_set * unaffected_doms;
  unaffected_doms = isl_union_set_subtract (
    isl_union_set_copy (src->domain), affected_doms);
  if (isl_union_set_is_empty (unaffected_doms) == isl_bool_false)
  {
    axl_add_non_affected(schedule,union_map);
    axl_add_non_affected(read,union_map);
    axl_add_non_affected(write,union_map);
    axl_add_non_affected(call,union_map);
    dst->domain = isl_union_set_union (dst->domain, unaffected_doms);
    return;
  }
  isl_union_set_free (unaffected_doms);
}
#undef axl_add_non_affected
  

void axl_mini_scop_show (t_mini_scop * scop, const char * scop_name)
{
  printf ("Showing SCOP %s summary: \n", scop_name);
  printf ("Showing domain of scop  : %s\n", isl_union_set_to_str(scop->domain));
  printf ("Showing schedule of scop: %s\n", isl_union_map_to_str(scop->schedule));
  printf ("Showing read of scop    : %s\n", isl_union_map_to_str(scop->read));
  printf ("Showing write of scop   : %s\n", isl_union_map_to_str(scop->write));
}

isl_stat axl_print_gate_call (__isl_take isl_map * call_accrel, void * usr)
{
  t_user_call * data = (t_user_call*)(usr); 
  const char * gate_name = isl_map_get_tuple_name (call_accrel, isl_dim_out);
  data->printer = isl_printer_print_str (data->printer, gate_name);
  data->printer = isl_printer_print_str (data->printer," ");
  int nd = isl_map_dim (call_accrel, isl_dim_out);
  int ii;
  int dim_printer = 0;
  for (ii = 0; ii < nd; ii++)
  {
    isl_val * val;
    val = isl_map_plain_get_val_if_fixed (call_accrel, isl_dim_out, ii);
    if (ii > 0)
      data->printer = isl_printer_print_str (data->printer, ", ");
    if (isl_val_is_nan (val) == isl_bool_false)
    {
      data->printer = isl_printer_print_val (data->printer, val);
    }
    else
    {
        /*
      isl_ast_expr * accexpr; 
      accexpr = isl_ast_build_access_from_pw_multi_aff(
      __isl_keep isl_ast_build *build, __isl_take isl_pw_multi_aff *pma);
      */

    }
  }
  data->printer = isl_printer_print_str (data->printer,";");
  isl_map_free (call_accrel);
  return isl_stat_ok;
}

isl_stat
print_single_gate_operation (__isl_take isl_ast_expr * ae, void * usr)
{
  isl_printer * prt = (isl_printer*)(usr);
  prt = isl_printer_start_line (prt);
  prt = isl_printer_print_ast_expr (prt, ae);
  prt = isl_printer_end_line (prt);
  isl_ast_expr_free (ae);
  return isl_stat_ok;
}

__isl_give 
isl_printer * axl_stmt_print_user(
  __isl_take isl_printer * prt, 
  __isl_take isl_ast_print_options *options,
  __isl_keep isl_ast_node * node,
  void * usr)
{
  t_mini_scop * scop = (t_mini_scop*)(usr);
  isl_id * id = isl_ast_node_get_annotation (node);
  isl_ast_expr_list * gate_call_expr_list = (isl_ast_expr_list*)(isl_id_get_user (id));


  isl_stat res;
  res = isl_ast_expr_list_foreach (gate_call_expr_list, &print_single_gate_operation, (void*)(prt));
  assert (res == isl_stat_ok && "utils.cc: error when generating gate call");

  isl_ast_expr_list_free (gate_call_expr_list);
  return prt;
}

isl_stat
build_single_access_expression (__isl_take isl_map * acc, void * usr)
{
  t_foreach * data = (t_foreach*)(usr);
  isl_pw_multi_aff * pma;

  print_isl_map_debug (acc, "Access map used in pw_multi_aff");  

  pma = isl_pw_multi_aff_from_map (acc);

  isl_ast_expr * aexp;
  aexp = isl_ast_build_access_from_pw_multi_aff (data->build, pma);

  data->ast_exl = isl_ast_expr_list_add (data->ast_exl, aexp);

  return isl_stat_ok;
}

__isl_give
isl_ast_expr_list *
axl_build_access_at_domain (
  __isl_keep isl_ast_build * build, 
  __isl_keep isl_union_map * sched, 
  __isl_keep isl_union_map * call)
{
  isl_union_map * sched_to_call;
  sched_to_call = isl_union_map_apply_range (
    isl_union_map_reverse (isl_union_map_copy (sched)),
    isl_union_map_copy (call));

  t_foreach data;
  isl_ctx * ctx = isl_union_map_get_ctx (sched);
  data.ast_exl = isl_ast_expr_list_alloc (ctx, 0);
  data.build = build;

  isl_stat res;
  res = isl_union_map_foreach_map (
    sched_to_call, &build_single_access_expression, (void*)(&data));
  assert (res == isl_stat_ok);
  isl_union_map_free (sched_to_call);
  return data.ast_exl;
}

__isl_give 
isl_ast_node * axl_at_each_domain (
  __isl_take isl_ast_node * node,
  __isl_keep isl_ast_build * build,
  void * usr)
{
  t_mini_scop * scop = (t_mini_scop*)(usr);
  isl_union_map * sched = isl_ast_build_get_schedule (build);
  isl_map * map = isl_map_from_union_map (isl_union_map_copy (sched));
  const char * dom_name = isl_map_get_tuple_name (map, isl_dim_in);

  isl_ctx * ctx = isl_ast_node_get_ctx (node);
  isl_union_map * gate_call = isl_union_map_intersect_domain (
    isl_union_map_copy (scop->call), 
    isl_union_map_domain (isl_union_map_copy (sched)));


  isl_ast_expr_list * acc_expr_list;
  acc_expr_list = axl_build_access_at_domain (build, sched, gate_call);

  isl_id * id;
  id = isl_id_alloc (ctx, dom_name, acc_expr_list);
  node = isl_ast_node_set_annotation(node, id);
  isl_union_map_free (sched);

  isl_ast_expr * expr = isl_ast_node_user_get_expr(node);
  isl_union_map_free (gate_call);
  return node;
}

/*
* This function must be passed to isl_schedule_map_schedule_node_bottom_up
* or to isl_schedule_node_map_descendant_bottom_up.
* It checks whether the node is of type isl_schedule_node_band; if so,
* then it traverses all the band members setting their type to isl_ast_loop_unroll.
*/
__isl_give 
isl_schedule_node *
axl_schedule_node_band_unroll_loop (
  __isl_take isl_schedule_node * node, void * usr)
{
  if (isl_schedule_node_get_type (node) == isl_schedule_node_band)
  {
    int nn = isl_schedule_node_band_n_member (node);
    if (nn < 0)
    {
      isl_schedule_node * ret;
      ret = isl_schedule_node_copy (node);
      isl_schedule_node_free (node);
      return ret;
    }
    int ii;
    {
      isl_ctx * ctx = isl_schedule_node_get_ctx (node);
      node = isl_schedule_node_band_set_ast_build_options (node, 
        isl_union_set_read_from_str (ctx, "{ unroll[0] }"));
    }
    return node;
  }
  isl_schedule_node * ret;
  ret = isl_schedule_node_copy (node);
  isl_schedule_node_free (node);
  return ret;
}


isl_bool
axl_schedule_node_band_unroll_loop_bool (
  __isl_take isl_schedule_node * node, void * usr)
{
  if (isl_schedule_node_get_type (node) == isl_schedule_node_band)
  {
    int nn = isl_schedule_node_band_n_member (node);
    if (nn < 0)
    {
      isl_schedule_node * ret;
      ret = isl_schedule_node_copy (node);
      isl_schedule_node_free (node);
      return isl_bool_true;
    }
    int ii;
    {
      isl_ctx * ctx = isl_schedule_node_get_ctx (node);
      node = isl_schedule_node_band_set_ast_build_options (node, 
        isl_union_set_read_from_str (ctx, "{ unroll[0] }"));
    }
    return isl_bool_true;
  }
  isl_schedule_node * ret;
  ret = isl_schedule_node_copy (node);
  isl_schedule_node_free (node);
  return isl_bool_true;
}
/*

#include <isl/schedule.h>
        __isl_give isl_schedule *
        isl_schedule_map_schedule_node_bottom_up(
                __isl_take isl_schedule *schedule,
                __isl_give isl_schedule_node *(*fn)(
                        __isl_take isl_schedule_node *node,
                        void *user), void *user);
        #include <isl/schedule_node.h>
        __isl_give isl_schedule_node *
        isl_schedule_node_map_descendant_bottom_up(
                __isl_take isl_schedule_node *node,
                __isl_give isl_schedule_node *(*fn)(
                        __isl_take isl_schedule_node *node,
                        void *user), void *user);

__isl_give isl_schedule_node *
        isl_schedule_node_band_member_set_ast_loop_type(
                __isl_take isl_schedule_node *node, int pos,
                enum isl_ast_loop_type type);

where type is : isl_ast_loop_unroll

  __isl_give isl_union_set *
        isl_schedule_node_domain_get_domain(
                __isl_keep isl_schedule_node *node);
*/

static
int
is_basic_map_dimension_single_valued (__isl_keep isl_basic_map * map, int adim)
{
  int nd = isl_basic_map_dim (map, isl_dim_out);
  stringstream strm;
  int ii;
  strm << "[M,T] -> {[";
  for (ii = 0; ii < nd; ii++)
  {
    if (ii > 0)
      strm << ",";
    strm << "i" << ii;
  }
  strm << "] -> [";
  strm << "i" << adim;
  strm << "] }";
  isl_ctx * ctx = isl_basic_map_get_ctx (map);
  isl_basic_map * temp = isl_basic_map_read_from_str (ctx, strm.str().c_str());
  isl_basic_set * sr = isl_basic_map_range (isl_basic_map_copy (map));
  int ret = isl_basic_map_is_single_valued (temp) == isl_bool_true ? 1 : 0;
  isl_basic_map_free (temp);
  return ret;
}

static
isl_stat
create_unroll_basic_map (__isl_take isl_basic_map * bmap, void * usr)
{
  t_foreach * data = (t_foreach*)(usr);
  int nd = isl_basic_map_dim (bmap, isl_dim_out);
  int ii,jj; 
  stringstream itup;  
  stringstream otup;
  itup << "[M,T] -> { [";
  otup << "unroll[";
  for (ii = 0, jj = 0; ii < nd; ii++)
  {
    if (ii > 0)
      itup << ",";
    isl_val * val = isl_basic_map_plain_get_val_if_fixed (bmap, isl_dim_out, ii);
    if (isl_val_is_nan (val) == isl_bool_true)
    {
      itup << "i" << ii;
      if (jj > 0)
        otup << ",";
      otup << ii;
      jj++;
    }
    else if (!is_basic_map_dimension_single_valued (bmap, ii))
    {
      itup << "i" << ii;
      if (jj > 0)
        otup << ",";
      otup << ii;
      jj++;
    }
    else
    {
      itup << isl_val_to_str (val);
    }
    isl_val_free (val);
  }
  itup << "] -> ";
  otup << "] }";
  string res = itup.str () + otup.str ();
  isl_ctx * ctx = isl_basic_map_get_ctx (bmap);
  isl_basic_map_free (bmap);
  bmap = isl_basic_map_read_from_str (ctx, res.c_str ());
  if (data->map)
    data->map = isl_map_union (data->map,
      isl_map_from_basic_map (bmap));
  else
    data->map = isl_map_from_basic_map (bmap);
  return isl_stat_ok;
}

static
isl_stat
create_unroll_map (__isl_take isl_map * map, void * usr)
{
  t_foreach * data = (t_foreach*)(usr);
  t_foreach info;
  info.map = NULL;
  isl_stat res = isl_map_foreach_basic_map (map, &create_unroll_basic_map, (void*)(&info));
  if (data->umap)
    data->umap = isl_union_map_union (data->umap,
      isl_union_map_from_map (info.map));
  else
    data->umap = isl_union_map_from_map (info.map);
  isl_map_free (map);
  return isl_stat_ok;
}

__isl_give
isl_union_map * 
axl_create_unroll_union_map (__isl_keep isl_union_map * schedule_map)
{
  int nd = get_max_dim (schedule_map);
  t_foreach data;
  data.umap = NULL;
  isl_stat res = isl_union_map_foreach_map (schedule_map, &create_unroll_map, (void*)(&data));

  return data.umap;
}

static
isl_stat
create_schedule_map_from_domain (__isl_take isl_set * set, void * usr)
{
  t_foreach * data = (t_foreach*)(usr);
  isl_map * map = isl_set_identity (set);
  map = isl_map_set_tuple_name (map, isl_dim_out, "");
  isl_union_map * umap = isl_union_map_from_map (map);
  if (data->umap)
    data->umap = isl_union_map_union (data->umap, umap);
  else
    data->umap = umap;
  return isl_stat_ok;
}

__isl_give
isl_union_map *
axl_build_schedule_map_from_iteration_domain (__isl_keep isl_union_set * udom)
{
  isl_stat res;
  t_foreach data;
  data.umap = NULL;
  res = isl_union_set_foreach_set (udom, &create_schedule_map_from_domain, (void*)(&data));
  assert (res == isl_stat_ok);
  return data.umap;
}

/*
* Return a map with the same domain as the input map, but where the
* linear dimensions in the range have opposite signs to the input map.
* Scalar dimensions should remain intact.
*/
static
isl_stat
negate_linear_dimensions (__isl_take isl_map * map, void * usr)
{
  t_foreach * data = (t_foreach*)(usr);
  map = isl_map_neg (map);
  int nbm = isl_map_n_basic_map (map);
  if (nbm > 1)
  {
    printf ("WARNING: Input map given [%s] consists of more than one basic map (%d).\n",
      isl_map_to_str (map), nbm);
  }
  int nd = isl_map_dim (map, isl_dim_out);
  int ii;
  for (ii = 0; ii < nd; ii++)
  {
    isl_val * val;
    val = isl_map_plain_get_val_if_fixed (map, isl_dim_out, ii);
    if (isl_val_is_neg (val) == isl_bool_true)
    {
      val = isl_val_neg (val);
      map = isl_map_drop_constraints_involving_dims (map, isl_dim_out, ii, 1);
      map = isl_map_fix_val (map, isl_dim_out, ii, val);
    }
    else
    {
      isl_val_free (val);
    }
  }
  isl_union_map * umap = isl_union_map_from_map (map);
  if (data->umap)
    data->umap = isl_union_map_union (data->umap, umap);
  else
    data->umap = umap;
  return isl_stat_ok;
}

/*
* Negate the output linear dimensions of each map in the given union map.
*
*/
__isl_give
isl_union_map *
axl_reverse_schedule_map (__isl_take isl_union_map * umap)
{
  isl_stat res;
  t_foreach data;
  data.umap = NULL;
  res = isl_union_map_foreach_map (umap, &negate_linear_dimensions, (void*)(&data));
  assert (res == isl_stat_ok);
  isl_union_map_free (umap);
  return data.umap;
}


__isl_give
isl_set *
axl_rename_set (__isl_take isl_set * set, const char * prefix_name)
{
  isl_set * ret;
  if (!set)
    return NULL;
  const char * oldname = isl_set_get_tuple_name (set);
  char * newname;
  newname = (char*)malloc (sizeof(char) * (strlen(prefix_name)*2 + strlen(oldname)));
  sprintf (newname,"%s_%s",prefix_name, oldname);
  ret = isl_set_set_tuple_name (set, newname); 
  return ret;
}


__isl_give
isl_map *
axl_rename_map (__isl_take isl_map * map, const char * prefix_name)
{
  isl_map * ret;
  if (!map)
    return NULL;
  const char * oldname = isl_map_get_tuple_name (map, isl_dim_in);
  char * newname;
  newname = (char*)malloc (sizeof(char) * (strlen(prefix_name)*2 + strlen(oldname)));
  sprintf (newname,"%s_%s",prefix_name, oldname);
  ret = isl_map_set_tuple_name (map, isl_dim_in, newname); 
  free (newname);
  return ret;
}

static
isl_stat rename_set (__isl_take isl_set * set, void * usr)
{
  t_foreach * data = (t_foreach*)(usr);
  isl_set * ret;
  const char * oldname = isl_set_get_tuple_name (set);
  char * newname;
  newname = (char*)malloc (sizeof(char) * (strlen(data->name)*2 + strlen(oldname)));
  sprintf (newname,"%s_%s",data->name, oldname);
  ret = isl_set_set_tuple_name (set, newname); 
  if (data->uset)
    data->uset = isl_union_set_union (data->uset, 
      isl_union_set_from_set (ret));
  else
    data->uset = isl_union_set_from_set (ret);
  return isl_stat_ok;
}

static
isl_stat rename_map (__isl_take isl_map * map, void * usr)
{
  t_foreach * data = (t_foreach*)(usr);
  isl_map * ret;
  const char * oldname = isl_map_get_tuple_name (map, isl_dim_in);
  char * newname;
  newname = (char*)malloc (sizeof(char) * (strlen(data->name)*2 + strlen(oldname)));
  sprintf (newname,"%s_%s",data->name, oldname);
  ret = isl_map_set_tuple_name (map, isl_dim_in, newname); 
  if (data->umap)
    data->umap = isl_union_map_union (data->umap, 
      isl_union_map_from_map (ret));
  else
    data->umap = isl_union_map_from_map (ret);
  return isl_stat_ok;
}

isl_stat map_copy_name_to_output_tuple (__isl_take isl_map * map, void * usr)
{
  t_foreach * data = (t_foreach*)(usr);
  isl_map * ret;
  const char * oldname = isl_map_get_tuple_name (map, isl_dim_in);
  char * newname = strdup (oldname);
  ret = isl_map_set_tuple_name (map, isl_dim_out, newname); 
  if (data->umap)
    data->umap = isl_union_map_union (data->umap, 
      isl_union_map_from_map (ret));
  else
    data->umap = isl_union_map_from_map (ret);
  return isl_stat_ok;
}

__isl_give
isl_union_set *
axl_rename_union_set (__isl_take isl_union_set * uset, const char * newname)
{
  t_foreach data;
  data.uset = NULL;
  data.name = (char*)(newname);
  isl_stat res;
  res = isl_union_set_foreach_set (uset, &rename_set, (void*)(&data));
  assert (res == isl_stat_ok);
  isl_union_set_free (uset);
  return data.uset;
}


__isl_give
isl_union_map *
axl_rename_union_map (__isl_take isl_union_map * umap, const char * newname)
{
  t_foreach data;
  data.umap = NULL;
  data.name = (char*)(newname);
  isl_stat res;
  res = isl_union_map_foreach_map (umap, &rename_map, (void*)(&data));
  assert (res == isl_stat_ok);
  isl_union_map_free (umap);
  return data.umap;
}


__isl_give
isl_union_map *
axl_copy_names_to_output_tuples (__isl_keep isl_union_map * umap)
{
  t_foreach data;
  data.umap = NULL;
  data.name = NULL;
  isl_stat res;
  res = isl_union_map_foreach_map (umap, &map_copy_name_to_output_tuple, (void*)(&data));
  assert (res == isl_stat_ok);
  return data.umap;
}


__isl_give
isl_union_map * 
axl_isolate_domains (__isl_keep isl_union_map * umap)
{
  vector_map vm;
  axl_union_map_to_vector_map (umap, &vm);
  int ii;
  for (ii = 0; ii < vm.size (); ii++)
  {
    char newname[20];
    sprintf (newname, "R%d", ii);
    vm[ii] = isl_map_set_tuple_name (vm[ii], isl_dim_in, newname);
    sprintf (newname, "Q%d", ii);
    vm[ii] = isl_map_set_tuple_name (vm[ii], isl_dim_out, newname);
  }
  isl_union_map * left;
  // The below call destroys all the maps in vm
  left = axl_vector_map_to_union_map (&vm);
  vm.clear ();
  axl_union_map_to_vector_map (umap, &vm);
  for (ii = 0; ii < vm.size (); ii++)
  {
    char newname[20];
    sprintf (newname, "Q%d", ii);
    vm[ii] = isl_map_set_tuple_name (vm[ii], isl_dim_out, newname);
  }
  isl_union_map * right;
  right = axl_vector_map_to_union_map (&vm);
  print_isl_union_map_debug (left, "\n\nLeft union map");
  print_isl_union_map_debug (right, "\n\nRight union map");

  left = isl_union_map_apply_domain (left, isl_union_map_reverse (right));
  print_isl_union_map_debug (left, "\n\nRenaming union map");

  return left;
}


isl_stat
apply_parametric_shift_to_map (__isl_take isl_map * map, void * usr)
{
  t_foreach * data = (t_foreach*)(usr);
  string ret = "[";
  int nd = isl_map_dim (map, isl_dim_out);
  int ii;
  for (ii = 0; ii < nd; ii++)
  {
    if (ii > 0)
      ret += ",";
    ret += "i" + std::to_string(ii);
  }
  ret += "] -> [";
  for (ii = 0; ii < nd; ii++)
  {
    if (ii > 0)
      ret += ",";
    ret += "o" + std::to_string(ii);
  }
  ret += "] : ";
  for (ii = 0; ii < nd; ii++)
  {
    if (ii > 0)
      ret += " and ";
    ret += "o" + std::to_string(ii) + " = i" + std::to_string(ii) + " + " + string(data->name);
  }
  ret += "; ";
  data->str_ret += ret;
  isl_map_free (map);
  return isl_stat_ok;
}

string
axl_union_map_apply_parametric_shift (
  __isl_keep isl_union_map * umap, const char * bigsum)
{
  t_foreach data;
  data.umap = NULL;
  data.name = (char*)(bigsum);
  data.str_ret = "";
  isl_stat res;
  res = isl_union_map_foreach_map (umap, &apply_parametric_shift_to_map, (void*)(&data));
  assert (res == isl_stat_ok);
  return data.str_ret;
}

static
isl_stat axl_isl_plot_define_set (__isl_take isl_set * set, void * usr)
{
  t_isl_plot * data = (t_isl_plot*)(usr);
  *(data->strm) << string("s") << data->index << string(" = Set(\"");
  *(data->strm) << string(isl_set_to_str(set)) << "\")" << endl;
  data->index++;
  isl_set_free (set);
}

static
isl_stat axl_isl_plot_draw_set (__isl_take isl_set * set, void * usr)
{
  t_isl_plot * data = (t_isl_plot*)(usr);
  *(data->strm) << string("plot_set_shapes(s") << data->index << string(", color=\"lightblue\", vertex_size=0)") << endl;
  *(data->strm) << string("plot_set_points(s") << data->index << string(", color=\"darkblue\")") << endl;
  data->index++;
  isl_set_free (set);
}


void axl_plot_isl_domains (__isl_keep isl_union_set * udom, ofstream & strm)
{
  isl_stat res;
  t_isl_plot data;
  data.index = 0;
  data.strm = &strm;
  res = isl_union_set_foreach_set (udom, &axl_isl_plot_define_set, &data);
  assert (res == isl_stat_ok);
  data.index = 0;
  res = isl_union_set_foreach_set (udom, &axl_isl_plot_draw_set, &data);
  assert (res == isl_stat_ok);
}

isl_stat
axl_process_graph_edge (__isl_take isl_map * map, void * usr)
{
  t_axl_dot * data = (t_axl_dot*)(usr);
  string srcname = string(isl_map_get_tuple_name (map, isl_dim_in));
  string dstname = string(isl_map_get_tuple_name (map, isl_dim_out));
  if (data->node_map.find (srcname) == data->node_map.end ())
    data->node_map[srcname] = data->index++;
  if (data->node_map.find (dstname) == data->node_map.end ())
    data->node_map[dstname] = data->index++;
  int srcid = data->node_map[srcname];
  int dstid = data->node_map[dstname];
  *(data->strm) << "\t" << srcid << " -> " << dstid << ";" << endl;
  isl_map_free (map);
  return isl_stat_ok;
}

void axl_plot_dependence_graph (__isl_keep isl_union_map * umap)
{
  isl_stat res; 
  ofstream axldot;
  axldot.open ("axl.graph.dot");
  axldot << "digraph {" << endl;
  t_axl_dot data;
  data.index = 0;
  data.strm = &axldot;
  res = isl_union_map_foreach_map (umap, &axl_process_graph_edge, (void*)(&data));
  assert (res == isl_stat_ok);
  axldot << "}" << endl;
  axldot.close ();
  int ret = system ("dot -Tpdf axl.graph.dot -o axl.graph.pdf");
}

double axl_time ()
{
  struct timeval tv;
  gettimeofday (&tv, NULL);
  double ret = (double)(tv.tv_usec/1000000.0) + (double)(tv.tv_sec);
  return ret;
}


isl_stat
store_isl_set_in_vector (__isl_take isl_set * set, void * usr)
{
  vector_set * vs = (vector_set*)(usr);
  vs->push_back (isl_set_copy (set));
  isl_set_free (set);
  return isl_stat_ok;
}

void
axl_union_set_to_vector_set (__isl_keep isl_union_set * uset, vector_set * vs)
{
  isl_stat res;
  print_isl_union_set_debug (uset, "Received union set as schedule for coalescing");
  res = isl_union_set_foreach_set (uset, &store_isl_set_in_vector, (void*)(vs));
  assert (res == isl_stat_ok);
}


isl_stat
store_isl_basic_map_in_vector (__isl_take isl_basic_map * bmap, void * usr)
{
  vector_basic_map * vbm = (vector_basic_map*)(usr);
  vbm->push_back (isl_basic_map_copy (bmap));
  isl_basic_map_free (bmap);
  return isl_stat_ok;
}


__isl_give
isl_union_map *
axl_vector_map_to_union_map (vector_map * vm)
{
  // All maps in @vm will be destroyed.
  isl_union_map * ret = NULL;
  int nn = vm->size ();
  int ii;
  for (ii = 0; ii < nn; ii++)
  {
    isl_union_map * umap = isl_union_map_from_map ((*vm)[ii]);
    if (ret)
      ret = isl_union_map_union (ret, umap);
    else
      ret = umap;
  }
  return ret;
}


__isl_give
isl_union_set *
axl_vector_set_to_union_set (vector_set * vm)
{
  // All sets in @vm will be destroyed.
  isl_union_set * ret = NULL;
  int nn = vm->size ();
  int ii;
  for (ii = 0; ii < nn; ii++)
  {
    isl_union_set * uset = isl_union_set_from_set ((*vm)[ii]);
    if (ret)
      ret = isl_union_set_union (ret, uset);
    else
      ret = uset;
  }
  return ret;
}


isl_stat
store_isl_map_in_vector (__isl_take isl_map * map, void * usr)
{
  vector_map * vm = (vector_map*)(usr);
  vm->push_back (isl_map_copy (map));
  isl_map_free (map);
  return isl_stat_ok;
}

void
axl_union_map_to_vector_map (__isl_keep isl_union_map * umap, vector_map * vm)
{
  isl_stat res;
  res = isl_union_map_foreach_map (umap, &store_isl_map_in_vector, (void*)(vm));
  assert (res == isl_stat_ok);
}

int axl_isl_set_is_dimension_fixed_scalar (__isl_keep isl_set * set, int pos)
{
  int ret;
  if (pos >= isl_set_n_dim (set))
    return 0;
  isl_val * val = isl_set_plain_get_val_if_fixed (set, isl_dim_set, pos);
  ret = (isl_val_is_nan (val) == isl_bool_false);
  isl_val_free (val);
  return ret;
}

int 
axl_isl_map_is_dimension_fixed_scalar (
  __isl_keep isl_map * map, int pos, bool consider_id)
{
  int ret = 0;
  // Return 0 if @pos is out of bounds
  if (pos >= isl_map_dim (map, isl_dim_out))
    return 0;
  if (pos < 0)
    return 0;
  // Return 0 if the dimension has an id attached to it, e.g. A[0,t=0] pos = 1
  if (isl_map_has_dim_id (map, isl_dim_out, pos) == isl_bool_true and consider_id)
    return 0;
  isl_val * val = isl_map_plain_get_val_if_fixed (map, isl_dim_out, pos);
  if (!val)
    return 0;
  #ifdef _AXL_DEBUG_
  assert (val && "isl_val is null in axl_isl_map_is_dimension_fixed_scalar");
  #endif
  if (isl_val_is_nan (val) == isl_bool_false)
    ret = 1;
  isl_val_free (val);
  if (ret)
    return ret;
  if (!ret)
    ret = axl_isl_map_is_dim_single_valued (map, isl_dim_out, pos);
  return ret;
}

int vector_set_get_max_dim (vector_set & vs)
{
  int ss;
  int ns = vs.size ();
  int maxdim = 0;
  for (ss = 0; ss < ns; ss++)
  {
    int ndim = isl_set_n_dim (vs[ss]);
    if (ndim > maxdim)
      maxdim = ndim;
  }
  return maxdim;
}

int vector_map_get_max_dim (vector_map & vm)
{
  int ss;
  int ns = vm.size ();
  int maxdim = 0;
  for (ss = 0; ss < ns; ss++)
  {
    int ndim = isl_map_dim (vm[ss], isl_dim_out);
    if (ndim > maxdim)
      maxdim = ndim;
  }
  return maxdim;
}


void vector_set_free (vector_set & vs)
{
  int ss;
  int ns = vs.size ();
  for (ss = 0; ss < ns; ss++)
    isl_set_free (vs[ss]);
}

void vector_map_free (vector_map & vm)
{
  int ss;
  int nm = vm.size ();
  for (ss = 0; ss < nm; ss++)
    if (vm[ss])
      isl_map_free (vm[ss]);
}

void vector_basic_map_free (vector_basic_map & vbm)
{
  int ss;
  int nm = vbm.size ();
  for (ss = 0; ss < nm; ss++)
    if (vbm[ss])
      isl_basic_map_free (vbm[ss]);
}

vector_map * vector_map_from_union_map (__isl_take isl_union_map * umap)
{
  vector_map * ret = new vector_map;
  isl_stat res;
  res = isl_union_map_foreach_map (umap, &store_isl_map_in_vector, (void*)(ret));
  isl_union_map_free (umap);
  return ret;
}

__isl_give
isl_map * axl_isl_map_apply_if_same_space (
  __isl_keep isl_map * map1, __isl_keep isl_map * map2)
{
  if (!map1)
    return NULL;
  if (!map2)
    return NULL;
  const char * name1 = isl_map_get_tuple_name (map1, isl_dim_out);
  const char * name2 = isl_map_get_tuple_name (map2, isl_dim_in);
  if (!name1)
    return NULL;
  if (!name2)
    return NULL;
  int same = strcmp ((char*)name1,(char*)name2) == 0;
  if (!same)
    return NULL;
  isl_map * ret = NULL;
  ret = isl_map_apply_range (
    isl_map_copy (map1), isl_map_copy (map2));
  return ret;
}

__isl_give
isl_union_map *
vector_map_apply_range (__isl_take isl_union_map * umap1, 
  __isl_take isl_union_map * umap2)
{
  vector_map * vm1 = vector_map_from_union_map (umap1);
  vector_map * vm2 = vector_map_from_union_map (umap2);
  int n1 = vm1->size ();
  int n2 = vm2->size ();
  int ii, jj;
  isl_union_map * ret = NULL; 
  for (ii = 0; ii < n1; ii++)
  {
    for (jj = 0; jj < n2; jj++)
      if ((*vm1)[ii] && (*vm2)[jj])
    {
      isl_map * map = axl_isl_map_apply_if_same_space ((*vm1)[ii], (*vm2)[jj]);
      if (map && isl_map_is_empty (map) == isl_bool_false)
      {
        isl_map_free ((*vm1)[ii]);
        (*vm1)[ii] = NULL;
        isl_map_free ((*vm2)[jj]);
        (*vm2)[jj] = NULL;
        if (ret)
          ret = isl_union_map_union (ret, isl_union_map_from_map (map));
        else
          ret = isl_union_map_from_map (map);
        break;
      }
    }
  }
  vector_map_free (*vm1);
  vector_map_free (*vm2);
  free (vm1);
  free (vm2);
  //assert (isl_union_map_is_equal (ret, ref) == isl_bool_true);
  //isl_union_map_free (ref);
  return ret;
}


__isl_give
isl_union_map * 
axl_union_map_drop_redundant_dimensions_from_output_tuple (
  __isl_take isl_union_map * umap, int keep_name, bool consider_id)
{
  isl_stat res;
  vector_map vm;
  res = isl_union_map_foreach_map (umap, &store_isl_map_in_vector, (void*)(&vm));
  int nmap = isl_union_map_n_map (umap);
  int ss;
  int maxdim = vector_map_get_max_dim (vm);
  int ii;
  isl_union_map * ret = NULL;

  if (nmap == 1)
  {
    // drop all scalar dimensions in map
    isl_map * domtr = isl_map_from_union_map (isl_union_map_copy (umap));

    const char * name;
    if (keep_name)
    {
      name = isl_map_get_tuple_name (domtr, isl_dim_in);
    }

    for (ii = maxdim - 1; ii >= 0; ii--)
      if (axl_isl_map_is_dimension_fixed_scalar (domtr, ii, consider_id))
        domtr = isl_map_remove_dims (domtr, isl_dim_out, ii, 1);

    if (keep_name)
    {
      domtr = isl_map_set_tuple_name (domtr, isl_dim_out, name);
    }
    ret = isl_union_map_from_map (domtr);
    return ret;
  }

  // If we have 2 or more maps, drop all scalar dimensions that have the same
  // value. Start with the last.
  for (ii = maxdim - 1; nmap > 1 && ii >= 0; ii--)
  {
    if (!axl_isl_map_is_dimension_fixed_scalar (vm[0], ii, consider_id))
      continue;
    int ref_si = axl_isl_map_get_si (vm[0], isl_dim_out, ii);

    int equal = 1;
    int mm;
    // Determine whether dimension ii is droppable.
    for (mm = 1; equal && mm < nmap; mm++)
    {

      if (!axl_isl_map_is_dimension_fixed_scalar (vm[mm], ii, consider_id))
      {
        equal = 0;

        break;
      }
      int curr_si = axl_isl_map_get_si (vm[mm], isl_dim_out, ii);
      if (curr_si != ref_si)
      {
        equal = 0;
      }
    }
    if (equal)
    {
    }
    // If it's droppable, we drop it in all maps
    for (mm = 0; equal && mm < nmap; mm++)
    {
      vm[mm] = isl_map_remove_dims (vm[mm], isl_dim_out, ii, 1);
    }
  }

  for (ii = 0; ii < nmap; ii++)
  {
    if (keep_name)
    {
      const char * name = isl_map_get_tuple_name (vm[ii], isl_dim_in);
      vm[ii] = isl_map_set_tuple_name (vm[ii], isl_dim_out, name);
    }
    if (ret)
      ret = isl_union_map_union (ret, isl_union_map_from_map (vm[ii]));
    else
      ret = isl_union_map_from_map (vm[ii]);
  }

  //vector_map_free (vm); 
  isl_union_map_free (umap);
  return ret;
}


__isl_give
isl_union_map * 
axl_union_set_redundance_removal_union_map (__isl_keep isl_union_set * uset, bool consider_id)
{
  isl_union_map * umap = isl_union_set_identity (isl_union_set_copy (uset));

  //umap = axl_union_map_drop_redundant_dimensions_from_output_tuple (umap, 1);
  umap = axl_union_map_drop_all_scalar_dimensions_from_output_tuple (umap, 1, 0, consider_id);

  return umap;
}

void axl_show_mini_scop (t_mini_scop * mscop)
{
  cout << "Showing mini scop" << endl;
  cout << "Iteration domain: " << string (isl_union_set_to_str (mscop->domain)) << endl;
  cout << "Schedule: " << string (isl_union_map_to_str (mscop->schedule)) << endl;
  if (mscop->filter)
    cout << "Filter: " << string (isl_union_map_to_str (mscop->filter)) << endl;
  else
    cout << "Filter: N/A" << endl;
  cout << endl;
}

void axl_sort_statements_by_program_order (vector_set & vs, t_program_order & po)
{
  int ns = vs.size ();
  int ii;
  int jj;

  for (ii = 0; ii < ns - 1; ii++)
    for (jj = ii + 1; jj < ns; jj++)
      if (isl_set_plain_cmp (vs[ii],vs[jj]) > 0)
  {
    isl_set * temp = vs[ii];
    vs[ii] = vs[jj];
    vs[jj] = temp;

  }

  for (ii = 0; ii < ns; ii++)
  {
    const char * name = isl_set_to_str (vs[ii]);
    po[string(name)] = ii;
  }

}


__isl_give
isl_union_map * 
axl_union_map_drop_all_scalar_dimensions_from_output_tuple (
  __isl_take isl_union_map * umap, int keep_name, int start_dim, bool consider_id)
{
  isl_stat res;
  vector_map vm;
  res = isl_union_map_foreach_map (umap, &store_isl_map_in_vector, (void*)(&vm));
  int nmap = isl_union_map_n_map (umap);
  int ss;
  int maxdim = vector_map_get_max_dim (vm);
  int ii;
  isl_union_map * ret = NULL;

  for (ii = 0; ii < nmap; ii++)
  {
    int dd;
    const char * name = isl_map_get_tuple_name (vm[ii], isl_dim_in);
    for (dd = maxdim - 1; dd >= start_dim; dd--)
      if (axl_isl_map_is_dimension_fixed_scalar (vm[ii], dd, consider_id))
    {
        vm[ii] = isl_map_remove_dims (vm[ii], isl_dim_out, dd, 1);
    }
    if (keep_name)
    {
      vm[ii] = isl_map_set_tuple_name (vm[ii], isl_dim_out, name);
      vm[ii] = isl_map_set_tuple_name (vm[ii], isl_dim_in, name);
    }
    if (ret)
      ret = isl_union_map_union (ret, isl_union_map_from_map (vm[ii]));
    else
      ret = isl_union_map_from_map (vm[ii]);
  }

  // Don't free the vector map, as the maps are 'taken' above when constructing
  // the union map to be returned.
  isl_union_map_free (umap);
  return ret;
}


/*
 * Insert scalar dimensions from the end of the maps, moving to the first dimension.
 * Produce a proper 2d+1 interleaving of linear and scalar dimensions.
 *
 */
__isl_give isl_union_map *
axl_convert_to_2dp1_insert_scalar_dimensions (
  __isl_keep isl_union_set * uset, __isl_take isl_union_map * umap, bool consider_id)
{
  assert (0);
}


/*
 * Insert scalar dimensions from the end of the maps, moving to the first dimension.
 * Produce a proper 2d+1 interleaving of linear and scalar dimensions.
 *
 */
__isl_give isl_union_map *
axl_union_map_coalesce_scalar_prefix_dimensions (
  __isl_take isl_union_map * umap, bool consider_id)
{
  isl_stat res;
  vector_map vm;
  print_isl_union_map_debug (umap, "Received union map as schedule for coalescing");
  axl_union_map_to_vector_map (umap, &vm);
  int nmap = isl_union_map_n_map (umap);
  int ii;
  isl_union_map * ret = NULL;
  int max_pref = 0;
  for (ii = 0; ii < nmap; ii++)
  {
    int first = 0;
    int last = -1;
    if (axl_isl_map_is_dimension_fixed_scalar (vm[ii], 0, consider_id))
    {
      last = 0;
    }
    int nout = isl_map_dim (vm[ii], isl_dim_out);
    int dd;
    for (dd = 1; dd < nout; dd++)
    {
      if (!axl_isl_map_is_dimension_fixed_scalar (vm[ii], dd, consider_id))
        break;
       last++;
    }
    if (last > max_pref)
      max_pref = last;
  }
  for (ii = 0; ii < nmap; ii++)
  {
    print_isl_map_debug (vm[ii], "Coalescing map");
    int first = 0;
    int last = -1;
    if (axl_isl_map_is_dimension_fixed_scalar (vm[ii], 0, consider_id))
    {
      last = 0;
    }
    int nout = isl_map_dim (vm[ii], isl_dim_out);
    int dd;
    for (dd = 1; last >= 0 && dd < nout; dd++)
    {
      if (!axl_isl_map_is_dimension_fixed_scalar (vm[ii], dd, consider_id))
        break;
       last++;
    }
    int pref = 0;
    for (dd = first; dd <= last; dd++)
    {
      int si = axl_isl_map_get_si (vm[ii], isl_dim_out, dd);
      pref += si << (max_pref - dd);
    }

    if (first <= last)
    {
      vm[ii] = isl_map_remove_dims (vm[ii], isl_dim_out, first, last + 1);
      vm[ii] = isl_map_insert_dims (vm[ii], isl_dim_out, first, 1);
      vm[ii] = isl_map_fix_si (vm[ii], isl_dim_out, first, pref);
      print_isl_map_debug (vm[ii], "Map after coalescing");
    }

    if (ret)
      ret = isl_union_map_union (ret, isl_union_map_from_map (vm[ii]));
    else
      ret = isl_union_map_from_map (vm[ii]);
  }
  isl_union_map_free (umap);
  return ret;
}


isl_stat
mapify_basic_map (__isl_take isl_basic_map * bmap, void * usr)
{
  vector_map * vm = (vector_map*)(usr);
  const char * bmap_name = isl_basic_map_get_tuple_name (bmap, isl_dim_in);
  int namelen = strlen (bmap_name);
  char newname[namelen*2];
  sprintf (newname,"O%lu_%s", vm->size (), bmap_name);
  bmap = isl_basic_map_set_tuple_name (bmap, isl_dim_out, newname);
  vm->push_back (isl_map_from_basic_map (bmap));
  return isl_stat_ok;
}


void
axl_map_deunionize (__isl_keep isl_map * map, vector_map * vm)
{
  isl_stat res;
  print_isl_map_debug (map, "Map received to de-unionize");
  res = isl_map_foreach_basic_map (map, &mapify_basic_map, (void*)(vm));
  assert (res == isl_stat_ok);
}

__isl_give
isl_union_map *
axl_union_set_deunionize (__isl_keep isl_union_set * uset)
{
  vector_map vm;
  print_isl_union_set_debug (uset, "Received union set to de-unionize");
  isl_union_map * umap = isl_union_set_identity (isl_union_set_copy (uset));
  axl_union_map_to_vector_map (umap, &vm);
  int nmap = isl_union_map_n_map (umap);
  int mm;
  vector_map duvm; // de-unionized vector map
  for (mm = 0; mm < nmap; mm++)
  {
    axl_map_deunionize (vm[mm], &duvm); 
  }
  vector_map_free (vm);
  isl_union_map * ret;
  ret = axl_vector_map_to_union_map (&duvm);
  return ret;
}


void
separate_basic_maps (__isl_keep isl_map * map, vector_basic_map & vbm)
{
  isl_stat res;
  res = isl_map_foreach_basic_map (map, &store_isl_basic_map_in_vector, (void*)(&vbm));
  assert (res == isl_stat_ok);
}

/*
 * Does not rename the basic maps!
 *
 */
void
axl_union_map_separate_basic_maps (
  __isl_keep isl_union_map* umap, vector_basic_map & vbm)
{
  vector_map vm;
  axl_union_map_to_vector_map (umap, &vm);
  int ii;
  for (ii = 0; ii < vm.size (); ii++)
  {
    separate_basic_maps (vm[ii], vbm);
  }
  vector_map_free (vm);
}

void
axl_vector_basic_map_show (vector_basic_map & vbm)
{
  #ifdef _AXL_DEBUG_
  int ii;
  for (ii = 0; ii < vbm.size (); ii++)
  {
    char msg[100];
    sprintf (msg, "Showing basic_map %d", ii);
    print_isl_basic_map_debug (vbm[ii], (const char*)(msg));
  }
  #endif
}

__isl_give
isl_union_map *
axl_union_map_deunionize (__isl_keep isl_union_map* umap)
{
  print_isl_union_map_debug (umap, "Received union map to de-unionize");
  vector_map vm;
  isl_union_map * left = isl_union_map_copy (umap);
  isl_union_map * right = isl_union_map_copy (umap);
  left = axl_clean_union_map (left);
  right = axl_clean_union_map (right);
  right = isl_union_map_reverse (right);
  left = isl_union_map_apply_range (left, right);
  axl_union_map_to_vector_map (left, &vm);
  int nmap = isl_union_map_n_map (left);
  int mm;
  vector_map duvm; // de-unionized vector map
  for (mm = 0; mm < nmap; mm++)
  {
    axl_map_deunionize (vm[mm], &duvm); 
  }
  vector_map_free (vm);
  isl_union_map_free (left);
  isl_union_map * ret;
  ret = axl_vector_map_to_union_map (&duvm);
  return ret;
}

void 
assert_single_basic_map_in_map (__isl_keep isl_map * map)
{
  #ifdef _AXL_DEBUG_
  int nbm = isl_map_n_basic_map (map);
  if (nbm > 1)
  {
    print_isl_map_debug (map, "Map contains more than one basic map");
    assert (0);
  }
  #endif
}

int
axl_union_map_is_deunionized (__isl_keep isl_union_map * umap)
{
  vector_map vm;
  axl_union_map_to_vector_map (umap, &vm); 
  int ii;
  for (ii = 0; ii < vm.size (); ii++)
  {
    int nb = isl_map_n_basic_map (vm[ii]);
    if (nb > 1)
    {
      #ifdef _AXL_DEBUG_
      print_isl_map_debug (vm[ii],"==> Offending isl_map is: ");
      assert (0);
      #endif
    }
    return 0;
  }
  vector_map_free (vm);
  return 1;
}

static
double 
compute_single_prefix_value (__isl_keep isl_map * map)
{
  double ret = 0.0;
  int nout = isl_map_dim (map, isl_dim_out);
  int ii;
  for (ii = 0; ii < nout; ii++)
  {
    int si = axl_isl_map_get_si (map, isl_dim_out, ii);
    if (si == AXL_NAN)
      break;
    ret = ret * 2.0 + si;
  }
  return ret;
}


__isl_give
isl_union_map *
axl_coalesce_schedule_suffix (__isl_take isl_union_map * umap)
{
  assert (0);
}

__isl_give
isl_union_map * 
axl_union_map_drop_all_parameters (__isl_take isl_union_map * umap)
{
  vector_map vm;
  axl_union_map_to_vector_map (umap, &vm);
  int ii;
  for (ii = 0; ii < vm.size (); ii++)
  {
    int npar = isl_map_dim (vm[ii], isl_dim_param);
    vm[ii] = isl_map_drop_constraints_involving_dims (vm[ii], isl_dim_param, 0, npar);
  }
  isl_union_map * ret;
  ret = axl_vector_map_to_union_map (&vm);
  return ret;
}

__isl_give
isl_union_set * 
axl_union_set_drop_all_parameters (__isl_take isl_union_set * uset)
{
  vector_set vm;
  axl_union_set_to_vector_set (uset, &vm);
  int ii;
  for (ii = 0; ii < vm.size (); ii++)
  {
    int npar = isl_set_dim (vm[ii], isl_dim_param);
    vm[ii] = isl_set_drop_constraints_involving_dims (vm[ii], isl_dim_param, 0, npar);
  }
  isl_union_set * ret;
  ret = axl_vector_set_to_union_set (&vm);
  return ret;
}
