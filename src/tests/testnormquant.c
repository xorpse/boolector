/*  Boolector: Satisfiablity Modulo Theories (SMT) solver.
 *
 *  Copyright (C) 2016 Mathias Preiner.
 *
 *  All rights reserved.
 *
 *  This file is part of Boolector.
 *  See COPYING for more information on using this software.
 */

#include "btorcore.h"
#include "normalizer/btornormquant.h"
#include "testrunner.h"

#ifdef NDEBUG
#undef NDEBUG
#endif

#include <assert.h>
#include <stdio.h>

#define RUN_TEST(TEST)        \
  {                           \
    init_normquant_test ();   \
    BTOR_RUN_TEST (TEST);     \
    finish_normquant_test (); \
  }

static Btor *g_btor = NULL;

static void
init_normquant_test (void)
{
  g_btor = btor_new_btor ();
}

static void
finish_normquant_test (void)
{
  btor_delete_btor (g_btor);
}

void
init_normquant_tests (void)
{
}

void
finish_normquant_tests (void)
{
}

/* -------------------------------------------------------------------------- */

/*
 * exp: \not (\exists x,y . x = y)
 * res: \forall x,y . x != y
 *
 */
static void
test_normquant_inv_exists (void)
{
  BtorNode *exists, *eqx, *eqy, *x[2], *y[2], *expected, *result;
  BtorSortId sort;

  sort   = btor_bitvec_sort (g_btor, 32);
  x[0]   = btor_param_exp (g_btor, sort, "x0");
  x[1]   = btor_param_exp (g_btor, sort, "x1");
  eqx    = btor_eq_exp (g_btor, x[0], x[1]);
  exists = btor_exists_n_exp (g_btor, x, 2, eqx);

  y[0]     = btor_param_exp (g_btor, sort, "y0");
  y[1]     = btor_param_exp (g_btor, sort, "y1");
  eqy      = btor_eq_exp (g_btor, y[0], y[1]);
  expected = btor_forall_n_exp (g_btor, y, 2, BTOR_INVERT_NODE (eqy));

  result = btor_normalize_quantifiers_node (g_btor, BTOR_INVERT_NODE (exists));
  assert (result == expected);

  btor_release_exp (g_btor, x[0]);
  btor_release_exp (g_btor, x[1]);
  btor_release_exp (g_btor, eqx);
  btor_release_exp (g_btor, y[0]);
  btor_release_exp (g_btor, y[1]);
  btor_release_exp (g_btor, eqy);
  btor_release_exp (g_btor, exists);
  btor_release_exp (g_btor, expected);
  btor_release_exp (g_btor, result);
  btor_release_sort (g_btor, sort);
}

/*
 * exp: \not (\forall x,y . x = y)
 * res: \exists x,y . x != y
 *
 */
static void
test_normquant_inv_forall (void)
{
  BtorNode *forall, *eqx, *eqy, *x[2], *y[2], *expected, *result;
  BtorSortId sort;

  sort   = btor_bitvec_sort (g_btor, 32);
  x[0]   = btor_param_exp (g_btor, sort, 0);
  x[1]   = btor_param_exp (g_btor, sort, 0);
  eqx    = btor_eq_exp (g_btor, x[0], x[1]);
  forall = BTOR_INVERT_NODE (btor_forall_n_exp (g_btor, x, 2, eqx));

  y[0]     = btor_param_exp (g_btor, sort, 0);
  y[1]     = btor_param_exp (g_btor, sort, 0);
  eqy      = btor_eq_exp (g_btor, y[0], y[1]);
  expected = btor_exists_n_exp (g_btor, y, 2, BTOR_INVERT_NODE (eqy));

  result = btor_normalize_quantifiers_node (g_btor, forall);
  assert (result == expected);

  btor_release_exp (g_btor, x[0]);
  btor_release_exp (g_btor, x[1]);
  btor_release_exp (g_btor, eqx);
  btor_release_exp (g_btor, y[0]);
  btor_release_exp (g_btor, y[1]);
  btor_release_exp (g_btor, eqy);
  btor_release_exp (g_btor, forall);
  btor_release_exp (g_btor, expected);
  btor_release_exp (g_btor, result);
  btor_release_sort (g_btor, sort);
}

/*
 * exp: \forall x . \not (\exists y . x = y)
 * res: \forall x,y . x != y
 *
 */
static void
test_normquant_inv_exists_nested (void)
{
  BtorNode *forall, *exists, *eqx, *x[2];
  BtorNode *expected, *eqy, *y[2];
  BtorNode *result;
  BtorSortId sort;

  sort   = btor_bitvec_sort (g_btor, 32);
  x[0]   = btor_param_exp (g_btor, sort, 0);
  x[1]   = btor_param_exp (g_btor, sort, 0);
  eqx    = btor_eq_exp (g_btor, x[0], x[1]);
  exists = btor_exists_exp (g_btor, x[0], eqx);
  forall = btor_forall_exp (g_btor, x[1], BTOR_INVERT_NODE (exists));

  y[0]     = btor_param_exp (g_btor, sort, 0);
  y[1]     = btor_param_exp (g_btor, sort, 0);
  eqy      = btor_eq_exp (g_btor, y[0], y[1]);
  expected = btor_forall_n_exp (g_btor, y, 2, BTOR_INVERT_NODE (eqy));

  result = btor_normalize_quantifiers_node (g_btor, forall);
  assert (result == expected);

  btor_release_exp (g_btor, x[0]);
  btor_release_exp (g_btor, x[1]);
  btor_release_exp (g_btor, eqx);
  btor_release_exp (g_btor, exists);
  btor_release_exp (g_btor, forall);
  btor_release_exp (g_btor, y[0]);
  btor_release_exp (g_btor, y[1]);
  btor_release_exp (g_btor, eqy);
  btor_release_exp (g_btor, expected);
  btor_release_exp (g_btor, result);
  btor_release_sort (g_btor, sort);
}

/*
 * exp: \not (\forall x . \not (\exists y . x = y))
 * res: \exists x, y . x = y
 *
 */
static void
test_normquant_inv_exists_nested2 (void)
{
  BtorNode *forall, *exists, *eqx, *x[2];
  BtorNode *expected, *eqy, *y[2];
  BtorNode *result;
  BtorSortId sort;

  sort   = btor_bitvec_sort (g_btor, 32);
  x[0]   = btor_param_exp (g_btor, sort, 0);
  x[1]   = btor_param_exp (g_btor, sort, 0);
  eqx    = btor_eq_exp (g_btor, x[0], x[1]);
  exists = btor_exists_exp (g_btor, x[0], eqx);
  forall = BTOR_INVERT_NODE (
      btor_forall_exp (g_btor, x[1], BTOR_INVERT_NODE (exists)));

  y[0]     = btor_param_exp (g_btor, sort, 0);
  y[1]     = btor_param_exp (g_btor, sort, 0);
  eqy      = btor_eq_exp (g_btor, y[0], y[1]);
  expected = btor_exists_n_exp (g_btor, y, 2, eqy);

  result = btor_normalize_quantifiers_node (g_btor, forall);
  assert (result == expected);

  btor_release_exp (g_btor, x[0]);
  btor_release_exp (g_btor, x[1]);
  btor_release_exp (g_btor, eqx);
  btor_release_exp (g_btor, exists);
  btor_release_exp (g_btor, forall);
  btor_release_exp (g_btor, y[0]);
  btor_release_exp (g_btor, y[1]);
  btor_release_exp (g_btor, eqy);
  btor_release_exp (g_btor, expected);
  btor_release_exp (g_btor, result);
  btor_release_sort (g_btor, sort);
}

/*
 * exp: \not (\forall x . \exists y . \forall z . x /\ y /\ z)
 * res: \exists x . \forall y . \exists z . \not (x /\ y /\ z)
 *
 */
static void
test_normquant_inv_prefix (void)
{
  BtorNode *forall0, *forall1, *exists, *x[3], *and;
  BtorNode *exists0, *expected, *forall, *y[3], *andy;
  BtorNode *result;
  BtorSortId sort;

  sort    = btor_bool_sort (g_btor);
  x[0]    = btor_param_exp (g_btor, sort, 0);
  x[1]    = btor_param_exp (g_btor, sort, 0);
  x[2]    = btor_param_exp (g_btor, sort, 0);
  and     = btor_and_n_exp (g_btor, x, 3);
  forall0 = btor_forall_exp (g_btor, x[0], and);
  exists  = btor_exists_exp (g_btor, x[1], forall0);
  forall1 = BTOR_INVERT_NODE (btor_forall_exp (g_btor, x[2], exists));

  y[0]     = btor_param_exp (g_btor, sort, 0);
  y[1]     = btor_param_exp (g_btor, sort, 0);
  y[2]     = btor_param_exp (g_btor, sort, 0);
  andy     = btor_and_n_exp (g_btor, y, 3);
  exists0  = btor_exists_exp (g_btor, y[0], BTOR_INVERT_NODE (andy));
  forall   = btor_forall_exp (g_btor, y[1], exists0);
  expected = btor_exists_exp (g_btor, y[2], forall);

  result = btor_normalize_quantifiers_node (g_btor, forall1);
  assert (result == expected);

  btor_release_exp (g_btor, x[0]);
  btor_release_exp (g_btor, x[1]);
  btor_release_exp (g_btor, x[2]);
  btor_release_exp (g_btor, and);
  btor_release_exp (g_btor, forall0);
  btor_release_exp (g_btor, exists);
  btor_release_exp (g_btor, forall1);

  btor_release_exp (g_btor, y[0]);
  btor_release_exp (g_btor, y[1]);
  btor_release_exp (g_btor, y[2]);
  btor_release_exp (g_btor, andy);
  btor_release_exp (g_btor, exists0);
  btor_release_exp (g_btor, forall);
  btor_release_exp (g_btor, expected);
  btor_release_exp (g_btor, result);
  btor_release_sort (g_btor, sort);
}

/*
 * NOTE: since we don't have NNF we need to fix the polarities of the
 * quantifiers (quantifier is flipped if uneven number of not)
 *
 * exp: \forall x . \not ((\exists y . x > y) /\ (\exists z . x < z))
 *
 * after fixing polarities:
 *
 * res: \forall x . \not ((\forall y . x > y) /\ (\forall z . x < z))
 *
 */
static void
test_normquant_inv_and_exists (void)
{
  BtorNode *forall, *exists0, *exists1, *and, *x, *y[2], *ult, *ugt;
  BtorNode *expected, *forall0, *forall1, * or, *X, *Y[2], *ugte, *ulte;
  BtorNode *result;
  BtorSortId sort;

  sort    = btor_bitvec_sort (g_btor, 32);
  x       = btor_param_exp (g_btor, sort, 0);
  y[0]    = btor_param_exp (g_btor, sort, 0);
  y[1]    = btor_param_exp (g_btor, sort, 0);
  ugt     = btor_ugt_exp (g_btor, x, y[0]);
  exists0 = btor_exists_exp (g_btor, y[0], ugt);
  ult     = btor_ult_exp (g_btor, x, y[1]);
  exists1 = btor_exists_exp (g_btor, y[1], ult);
  and     = btor_and_exp (g_btor, exists0, exists1);
  forall  = btor_forall_exp (g_btor, x, BTOR_INVERT_NODE (and));

  X        = btor_param_exp (g_btor, sort, 0);
  Y[0]     = btor_param_exp (g_btor, sort, 0);
  Y[1]     = btor_param_exp (g_btor, sort, 0);
  ulte     = btor_ugt_exp (g_btor, X, Y[0]);
  forall0  = btor_forall_exp (g_btor, Y[0], ulte);
  ugte     = btor_ult_exp (g_btor, X, Y[1]);
  forall1  = btor_forall_exp (g_btor, Y[1], ugte);
  or       = btor_and_exp (g_btor, forall0, forall1);
  expected = btor_forall_exp (g_btor, X, BTOR_INVERT_NODE (or));

  result = btor_normalize_quantifiers_node (g_btor, forall);
  assert (result == expected);

  btor_release_exp (g_btor, x);
  btor_release_exp (g_btor, y[0]);
  btor_release_exp (g_btor, y[1]);
  btor_release_exp (g_btor, ugt);
  btor_release_exp (g_btor, exists0);
  btor_release_exp (g_btor, ult);
  btor_release_exp (g_btor, exists1);
  btor_release_exp (g_btor, and);
  btor_release_exp (g_btor, forall);
  btor_release_exp (g_btor, X);
  btor_release_exp (g_btor, Y[0]);
  btor_release_exp (g_btor, Y[1]);
  btor_release_exp (g_btor, ulte);
  btor_release_exp (g_btor, forall0);
  btor_release_exp (g_btor, ugte);
  btor_release_exp (g_btor, forall1);
  btor_release_exp (g_btor, or);
  btor_release_exp (g_btor, expected);
  btor_release_exp (g_btor, result);
  btor_release_sort (g_btor, sort);
}

/*
 * NOTE: since we don't have NNF we need to fix the polarities of the
 * quantifiers (quantifier is flipped if uneven number of not)
 *
 * exp: \forall x . (\not ((\not \exists y . y > x) /\ x > 0))
 *
 * after eliminating negated quantifiers:
 *
 * res: \forall x . (\not ((\forall y . y <= x) /\ x > 0))
 *
 * after fixing ploarities:
 *
 * res: \forall x . (\not ((\exists y . y <= x) /\ x > 0))
 *
 */
static void
test_normquant_normalize_negated_quant (void)
{
  BtorNode *x, *y;
  BtorNode *forallx, *existsy, *yugtx, *xugt0, *zero, *and;
  BtorNode *result;
  BtorSortId sort;

  sort    = btor_bitvec_sort (g_btor, 32);
  x       = btor_param_exp (g_btor, sort, "x");
  y       = btor_param_exp (g_btor, sort, "y");
  zero    = btor_zero_exp (g_btor, sort);
  xugt0   = btor_ugt_exp (g_btor, x, zero);
  yugtx   = btor_ugt_exp (g_btor, y, x);
  existsy = btor_exists_exp (g_btor, y, yugtx);
  and     = btor_and_exp (g_btor, BTOR_INVERT_NODE (existsy), xugt0);
  forallx = btor_forall_exp (g_btor, x, BTOR_INVERT_NODE (and));

  result = btor_normalize_quantifiers_node (g_btor, forallx);
  assert (result == forallx);

  btor_release_exp (g_btor, x);
  btor_release_exp (g_btor, y);
  btor_release_exp (g_btor, forallx);
  btor_release_exp (g_btor, existsy);
  btor_release_exp (g_btor, yugtx);
  btor_release_exp (g_btor, xugt0);
  btor_release_exp (g_btor, zero);
  btor_release_exp (g_btor, and);
  btor_release_exp (g_btor, result);
  btor_release_sort (g_btor, sort);
}

static void
test_normquant_expand_quant (void)
{
  BtorNode *forall, *x, *and, *redandx, *result, *expected;
  BtorNode *exists, *X, *redandX;
  BtorSortId sort;

  btor_set_opt (g_btor, BTOR_OPT_REWRITE_LEVEL, 0);

  sort    = btor_bitvec_sort (g_btor, 32);
  x       = btor_param_exp (g_btor, sort, "x");
  redandx = btor_redand_exp (g_btor, x);
  forall  = btor_forall_exp (g_btor, x, redandx);
  and     = btor_and_exp (g_btor, BTOR_INVERT_NODE (forall), forall);
  result  = btor_normalize_quantifiers_node (g_btor, and);

  X        = btor_param_exp (g_btor, sort, "X");
  redandX  = btor_redand_exp (g_btor, X);
  exists   = btor_exists_exp (g_btor, X, BTOR_INVERT_NODE (redandX));
  expected = btor_and_exp (g_btor, forall, exists);

  assert (result == expected);

  btor_release_exp (g_btor, x);
  btor_release_exp (g_btor, redandx);
  btor_release_exp (g_btor, forall);
  btor_release_exp (g_btor, and);
  btor_release_exp (g_btor, result);
  btor_release_exp (g_btor, X);
  btor_release_exp (g_btor, redandX);
  btor_release_exp (g_btor, exists);
  btor_release_exp (g_btor, expected);
  btor_release_sort (g_btor, sort);
  btor_set_opt (g_btor, BTOR_OPT_REWRITE_LEVEL, 3);
}

/*
 * NOTE: since we don't have NNF we need to fix the polarities of the
 * quantifiers (quantifier is flipped if uneven number of not)
 *
 * exp: v = \exists x, y . x < y ? v0 : v1
 *
 * after eliminating ITE:
 *
 * res_ite: v = v_ite
 *	    /\ \not ((\exists x, y . x < y) /\ v_ite != v0)
 *	    /\ \not (\not (\exists x, y . x < y) /\ v_ite != v1)
 *
 * after fixing polarities:
 *
 * res: v = v_ite
 *	/\ \not ((\forall x, y . x < y) /\ v_ite != v0)
 *	/\ \not (\not (\exists x, y . x < y) /\ v_ite != v1)
 *
 */
static void
test_normquant_elim_ite1 (void)
{
  BtorNode *forall, *exists, *xy[2], *v0, *v1, *ult, *ite;
  BtorNode *expected, *XY[2], *V[4], *eq, *eq0, *eq1, *eq2, *v;
  BtorNode *and0, *and1, *and2, *and3, *ultXY;
  BtorNode *result;
  BtorSortId sort;

  sort   = btor_bitvec_sort (g_btor, 32);
  v      = btor_var_exp (g_btor, sort, "v");
  v0     = btor_var_exp (g_btor, sort, "v0");
  v1     = btor_var_exp (g_btor, sort, "v1");
  xy[0]  = btor_param_exp (g_btor, sort, "x");
  xy[1]  = btor_param_exp (g_btor, sort, "y");
  ult    = btor_ult_exp (g_btor, xy[0], xy[1]);
  exists = btor_exists_n_exp (g_btor, xy, 2, ult);
  ite    = btor_cond_exp (g_btor, exists, v0, v1);
  eq     = btor_eq_exp (g_btor, v, ite);

  result = btor_normalize_quantifiers_node (g_btor, eq);

  V[0]   = btor_param_exp (g_btor, sort, "V0");
  V[1]   = btor_param_exp (g_btor, sort, "V_ite");
  V[2]   = btor_param_exp (g_btor, sort, "V1");
  V[3]   = btor_param_exp (g_btor, sort, "V");
  XY[0]  = btor_param_exp (g_btor, sort, "x0");
  XY[1]  = btor_param_exp (g_btor, sort, "y0");
  ultXY  = btor_ult_exp (g_btor, XY[0], XY[1]);
  forall = btor_forall_n_exp (g_btor, XY, 2, ultXY);
  eq0    = btor_eq_exp (g_btor, V[1], V[0]);
  eq1    = btor_eq_exp (g_btor, V[1], V[2]);
  eq2    = btor_eq_exp (g_btor, V[3], V[1]);
  and0   = btor_and_exp (g_btor, forall, BTOR_INVERT_NODE (eq0));
  and1 =
      btor_and_exp (g_btor, BTOR_INVERT_NODE (exists), BTOR_INVERT_NODE (eq1));
  and2 =
      btor_and_exp (g_btor, BTOR_INVERT_NODE (and0), BTOR_INVERT_NODE (and1));
  and3     = btor_and_exp (g_btor, eq2, and2);
  expected = btor_exists_n_exp (g_btor, V, 4, and3);

  assert (result == expected);

  btor_release_exp (g_btor, v);
  btor_release_exp (g_btor, v0);
  btor_release_exp (g_btor, v1);
  btor_release_exp (g_btor, xy[0]);
  btor_release_exp (g_btor, xy[1]);
  btor_release_exp (g_btor, ult);
  btor_release_exp (g_btor, exists);
  btor_release_exp (g_btor, ite);
  btor_release_exp (g_btor, eq);
  btor_release_exp (g_btor, result);
  btor_release_exp (g_btor, forall);
  btor_release_exp (g_btor, XY[0]);
  btor_release_exp (g_btor, XY[1]);
  btor_release_exp (g_btor, ultXY);
  btor_release_exp (g_btor, and0);
  btor_release_exp (g_btor, and1);
  btor_release_exp (g_btor, and2);
  btor_release_exp (g_btor, and3);
  btor_release_exp (g_btor, eq0);
  btor_release_exp (g_btor, eq1);
  btor_release_exp (g_btor, eq2);
  btor_release_exp (g_btor, V[0]);
  btor_release_exp (g_btor, V[1]);
  btor_release_exp (g_btor, V[2]);
  btor_release_exp (g_btor, V[3]);
  btor_release_exp (g_btor, expected);
  btor_release_sort (g_btor, sort);
}

/*
 * NOTE: since we don't have NNF we need to fix the polarities of the
 * quantifiers (quantifier is flipped if uneven number of not)
 *
 * exp: \forall x . x = (\exists y . x < y) ? v0 : v1
 *
 * after eliminating ITE:
 *
 * res_ite: \exists v0,v1 . \forall x. x = v_ite(x)
 *                     /\ \not ((\exists y . x < y) /\ v_ite(x) != v0)
 *                     /\ \not (\not (\exists y . x < y) /\ v_ite(x) != v1)
 *
 * after fixing polarities:
 *
 * res: \exists v0,v1 . \forall x . x = v_ite(x)
 *		    /\ \not ((\forall y . x < y) /\ v_ite(x) != v0)
 *		    /\ \not (\not (\exists y . x < y) /\ v_ite(x) != v1)
 *
 */
static void
test_normquant_elim_ite2 (void)
{
  BtorNode *forall, *exists, *x, *y, *v0, *v1, *ult, *ite, *eqx;
  BtorNode *expected, *existsY, *forallY, *X, *Y[2], *Z, *ultY1, *ultY;
  BtorNode *eqZX, *eqZv1, *eqZv0, *imp_if, *imp_else, *and0, *and1;
  BtorNode *result, *uf, *V[2], *forallX;
  BtorSortId sort;

  sort   = btor_bitvec_sort (g_btor, 32);
  v0     = btor_var_exp (g_btor, sort, "v0");
  v1     = btor_var_exp (g_btor, sort, "v1");
  x      = btor_param_exp (g_btor, sort, "x");
  y      = btor_param_exp (g_btor, sort, "y");
  ult    = btor_ult_exp (g_btor, x, y);
  exists = btor_exists_exp (g_btor, y, ult);
  ite    = btor_cond_exp (g_btor, exists, v0, v1);
  eqx    = btor_eq_exp (g_btor, x, ite);
  forall = btor_forall_exp (g_btor, x, eqx);

  result = btor_normalize_quantifiers_node (g_btor, forall);
  /* new UF introduced for ITE */
  assert (g_btor->ufs->count == 1);
  uf = g_btor->ufs->first->key;

  X    = btor_param_exp (g_btor, sort, 0);
  Y[0] = btor_param_exp (g_btor, sort, 0);
  Y[1] = btor_param_exp (g_btor, sort, 0);
  Z    = btor_apply_exps (g_btor, &X, 1, uf);

  V[0]     = btor_param_exp (g_btor, sort, "V0");
  V[1]     = btor_param_exp (g_btor, sort, "V1");
  eqZX     = btor_eq_exp (g_btor, X, Z);
  eqZv0    = btor_eq_exp (g_btor, Z, V[0]);
  eqZv1    = btor_eq_exp (g_btor, Z, V[1]);
  ultY     = btor_ult_exp (g_btor, X, Y[0]);
  ultY1    = btor_ult_exp (g_btor, X, Y[1]);
  forallY  = btor_forall_exp (g_btor, Y[0], ultY);
  existsY  = btor_exists_exp (g_btor, Y[1], ultY1);
  imp_if   = btor_and_exp (g_btor, forallY, BTOR_INVERT_NODE (eqZv0));
  imp_else = btor_and_exp (
      g_btor, BTOR_INVERT_NODE (existsY), BTOR_INVERT_NODE (eqZv1));
  and0 = btor_and_exp (
      g_btor, BTOR_INVERT_NODE (imp_if), BTOR_INVERT_NODE (imp_else));
  and1     = btor_and_exp (g_btor, eqZX, and0);
  forallX  = btor_forall_exp (g_btor, X, and1);
  expected = btor_exists_n_exp (g_btor, V, 2, forallX);

  assert (result == expected);

  btor_release_exp (g_btor, v0);
  btor_release_exp (g_btor, v1);
  btor_release_exp (g_btor, V[0]);
  btor_release_exp (g_btor, V[1]);
  btor_release_exp (g_btor, x);
  btor_release_exp (g_btor, y);
  btor_release_exp (g_btor, ult);
  btor_release_exp (g_btor, exists);
  btor_release_exp (g_btor, ite);
  btor_release_exp (g_btor, eqx);
  btor_release_exp (g_btor, forall);
  btor_release_exp (g_btor, result);
  btor_release_exp (g_btor, X);
  btor_release_exp (g_btor, Y[0]);
  btor_release_exp (g_btor, Y[1]);
  btor_release_exp (g_btor, Z);
  btor_release_exp (g_btor, eqZX);
  btor_release_exp (g_btor, eqZv0);
  btor_release_exp (g_btor, eqZv1);
  btor_release_exp (g_btor, ultY);
  btor_release_exp (g_btor, ultY1);
  btor_release_exp (g_btor, existsY);
  btor_release_exp (g_btor, forallY);
  btor_release_exp (g_btor, imp_if);
  btor_release_exp (g_btor, imp_else);
  btor_release_exp (g_btor, and0);
  btor_release_exp (g_btor, and1);
  btor_release_exp (g_btor, forallX);
  btor_release_exp (g_btor, expected);
  btor_release_sort (g_btor, sort);
}

/*
 * NOTE: since we don't have NNF we need to fix the polarities of the
 * quantifiers (quantifier is flipped if uneven number of not)
 *
 * exp: v2 = (\exists y . v2 < y) ? v0 : v1
 *
 * after eliminating ITE:
 *
 * res: \exists v0,v1,v2,v_ite .
 *	   v2 = v_ite
 *         /\ \not ((\exists y . v2 < y) /\ v_ite != v0)
 *         /\ \not ((\forall y . v2 >= y) /\ v_ite != v!)
 *
 * after fixing polarities:
 *
 * res: \exists v0,v1,v2,v_ite .
 *	   v2 = v_ite
 *	   /\ \not ((\forall y . v2 < y) /\ v_ite != v0)
 *	   /\ \not ((\exists y . v2 >= y) /\ v_ite != v1)
 */
#if 0
static void
test_normquant_elim_top_ite (void)
{
  BtorNode *exists, *y, *v0, *v1, *v2, *ult, *ite, *eqv;
  BtorNode *expected, *existsY, *forallY, *Y[2], *ugte, *ultY;
  BtorNode *eqZv2, *eqZv1, *eqZv0, *imp_if, *imp_else, *and0, *and1;
  BtorNode *result, *V[4];

  v0 = btor_var_exp (g_btor, 32, "v0");
  v1 = btor_var_exp (g_btor, 32, "v1");
  v2 = btor_var_exp (g_btor, 32, "v2");
  y = btor_param_exp (g_btor, 32, "y");
  ult = btor_ult_exp (g_btor, v2, y); 
  exists = btor_exists_exp (g_btor, y, ult);
  ite = btor_cond_exp (g_btor, exists, v0, v1); 
  eqv = btor_eq_exp (g_btor, v2, ite);

  result = btor_normalize_quantifiers_node (g_btor, eqv);

  Y[0] = btor_param_exp (g_btor, 32, "Y0");
  Y[1] = btor_param_exp (g_btor, 32, "Y1");

  V[0] = btor_param_exp (g_btor, 32, "V0");
  V[1] = btor_param_exp (g_btor, 32, "V1");
  V[2] = btor_param_exp (g_btor, 32, "V2");
  V[3] = btor_param_exp (g_btor, 32, "V_ite");
  eqZv2 = btor_eq_exp (g_btor, V[2], V[3]);
  eqZv0 = btor_eq_exp (g_btor, V[3], V[0]); 
  eqZv1 = btor_eq_exp (g_btor, V[3], V[1]);
  ultY = btor_ult_exp (g_btor, V[2], Y[0]);
  ugte = btor_ugte_exp (g_btor, V[2], Y[1]);
  forallY = btor_forall_exp (g_btor, Y[0], ultY);
  existsY = btor_exists_exp (g_btor, Y[1], ugte);
  imp_if = btor_and_exp (g_btor, forallY, BTOR_INVERT_NODE (eqZv0));
  imp_else = btor_and_exp (g_btor, existsY, BTOR_INVERT_NODE (eqZv1));
  and0 = btor_and_exp (g_btor, BTOR_INVERT_NODE (imp_if),
		       BTOR_INVERT_NODE (imp_else));
  and1 = btor_and_exp (g_btor, eqZv2, and0); 
  expected = btor_exists_n_exp (g_btor, V, 4, and1);

  printf ("\n"); btor_dump_smt2_node (g_btor, stdout, result, -1);
  printf ("\n"); btor_dump_smt2_node (g_btor, stdout, expected, -1);
  assert (result == expected);

  btor_release_exp (g_btor, v0);
  btor_release_exp (g_btor, v1);
  btor_release_exp (g_btor, v2);
  btor_release_exp (g_btor, V[0]);
  btor_release_exp (g_btor, V[1]);
  btor_release_exp (g_btor, V[2]);
  btor_release_exp (g_btor, V[3]);
  btor_release_exp (g_btor, y);
  btor_release_exp (g_btor, ult);
  btor_release_exp (g_btor, exists);
  btor_release_exp (g_btor, ite);
  btor_release_exp (g_btor, eqv);
  btor_release_exp (g_btor, result);
  btor_release_exp (g_btor, Y[0]);
  btor_release_exp (g_btor, Y[1]);
  btor_release_exp (g_btor, eqZv2);
  btor_release_exp (g_btor, eqZv0);
  btor_release_exp (g_btor, eqZv1);
  btor_release_exp (g_btor, ultY);
  btor_release_exp (g_btor, ugte);
  btor_release_exp (g_btor, existsY);
  btor_release_exp (g_btor, forallY);
  btor_release_exp (g_btor, imp_if);
  btor_release_exp (g_btor, imp_else);
  btor_release_exp (g_btor, and0);
  btor_release_exp (g_btor, and1);
  btor_release_exp (g_btor, expected);
}
#endif

/*
 * test quantifier hashing
 *
 * Ex0-Ex1.x0=x1 == Ey0-Ey1.y0=y1
 * Ex0x1.x0=x1 == Ey0y1.y1==y0
 *
 *
 *
 *
 */

void
run_normquant_tests (int argc, char **argv)
{
  RUN_TEST (normquant_inv_exists);
  RUN_TEST (normquant_inv_forall);
  RUN_TEST (normquant_inv_exists_nested);
  RUN_TEST (normquant_inv_exists_nested2);
  RUN_TEST (normquant_inv_prefix);
  RUN_TEST (normquant_inv_and_exists);
  RUN_TEST (normquant_normalize_negated_quant);
  RUN_TEST (normquant_expand_quant);
  RUN_TEST (normquant_elim_ite1);
  RUN_TEST (normquant_elim_ite2);
  // FIXME (ma): this one is disabled for now since hashing of quantifier
  // expressions does not recognize result and expected to be the same
  //  RUN_TEST (normquant_elim_top_ite);
}
