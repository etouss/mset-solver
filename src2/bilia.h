/**************************************************************************
 *
 *  BaTS decision procedure
 *
 *  you can redistribute it and/or modify it under the terms of the GNU
 *  Lesser General Public License as published by the Free Software
 *  Foundation, version 3.
 *
 *  It is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  See the GNU Lesser General Public License version 3.
 *  for more details (enclosed in the file LICENSE).
 *
 **************************************************************************/

/**
 * Additional definitions needed to parse BILIA files
 */

#ifndef _BILIA_H_
#define _BILIA_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include "bilia_types.h"
#include "bilia_vars.h"
#include "bilia_form.h"
#include "bilia_entl.h"
#include "bilia_option.h"

/* ====================================================================== */
/* Datatypes */
/* ====================================================================== */

/** AST built from the SMTLIB format.
 *  It is defined only for terms and formulas.
 */

/* Valid term builder in BILIA */
  typedef enum
  {
    BILIA_F_FALSE = 0,           /* boolean operators */
    //BILIA_F_TRUE,
    BILIA_F_NOT,
    BILIA_F_AND,
    BILIA_F_OR,
    BILIA_F_IMPLIES,
    // BILIA_F_EXISTS,
    // BILIA_F_FORALL,
    BILIA_F_LVAR,                /* variable, field, or predicate */
    BILIA_F_EQ,                  /* pure operators */
    BILIA_F_DISTINCT,
    BILIA_F_ITE,
    BILIA_F_LT,                  /* Integer theory */
    BILIA_F_GT,
    BILIA_F_LE,
    BILIA_F_GE,
    BILIA_F_PLUS,
    BILIA_F_MINUS,
    BILIA_F_MAX,
    BILIA_F_MIN,
    BILIA_F_INT,                 /* integer constant */
    BILIA_F_BAG,                 /* Bag_of_Int theory */
    BILIA_F_BAGN,
    BILIA_F_EMPTYBAG,
    BILIA_F_BAGUNION,
    BILIA_F_BAGMINUS,
    BILIA_F_BAGINTER,
    BILIA_F_BAGSUM,
    //BILIA_F_SUBSET,
    BILIA_F_SUBSETEQ,
    BILIA_F_NSUBSETEQ,
    BILIA_F_INN,
    BILIA_F_NIN,
    BILIA_F_IN,                  /* Share theory */
    BILIA_F_NI,
    BILIA_F_BAGMAX,
    BILIA_F_BAGMIN,
    BILIA_F_OTHER
/* NOT TO BE USED */
  } bilia_expkind_t;

  typedef struct bilia_exp_t
  {
    bilia_expkind_t discr;

    union
    {
      /* user-defined function or symbol name */
      uint_t sid;
      /* constant */
      long value;
    } p;

    struct bilia_exp_t **args;   /* array of expression args or NULL */
    uint_t size;                /* size of the array above */

  } bilia_exp_t;

/* Context used to parse smtlib2 formulas */
  typedef struct bilia_context_t
  {
    /* array storing the size of the frame of the stack
     * for location variables at each
     * quantifier level (only 3 levels are possible):
     * 0 -- size of globals,
     * 1 -- size of define-fun or exists in assert,
     * 2 -- exists in define-fun */
    // bilia_uint_array *lvar_stack;
    /* location variables environment */
    bilia_var_array *lvar_env;

    /* array storing the size of the frame of the stack
     * for set of locations variables (only 2 levels are possible):
     * 0 -- size of globals,
     * 1 -- size of exists in assert */
    /* set of locations variables environment */

    /* predicate in definition */
    char *pname;
  } bilia_context_t;

/**
 * Constants used for variables & parameters identifiers
 */
#define VID_NIL 0
#define VID_FST_PARAM 1
#define VID_SND_PARAM 2

/* ====================================================================== */
/* Globals */
/* ====================================================================== */

  extern int bilia_error_parsing;

/* ====================================================================== */
/* Constructors/destructors */
/* ====================================================================== */

/* Initialize global variables */
  void bilia_init (void);

/* Parsing context */
  bilia_context_t *bilia_mk_context (void);
  void bilia_del_context (bilia_context_t * ctx);
/* Allocator/deallocator. */

/* Parsing logic */
  int bilia_set_logic (bilia_context_t * ctx, const char *logic);

/* Functions */
  bilia_type_t *bilia_mk_fun_decl (bilia_context_t * ctx, const char *name,
                                 bilia_type_t * ty);
/* Commands */
  int bilia_assert (bilia_context_t * ctx, bilia_exp_t * term);
  int bilia_check (bilia_context_t * ctx);

/* Terms */
  void bilia_push_var (bilia_context_t * ctx, const char *name,
                      bilia_type_t * vty);
// #<{(| Declare a local variable in the context. |)}>#
//   int bilia_push_quant (bilia_context_t * ctx);
//   int bilia_pop_quant (bilia_context_t * ctx);
/* Increments/decrements the nesting of quantifiers. */

  // bilia_exp_t *bilia_mk_exists (bilia_context_t * ctx, bilia_exp_t * term);
  // bilia_exp_t *
  bilia_exp_t *bilia_mk_app (bilia_context_t * ctx, const char *name, bilia_exp_t ** args, uint_t size);
  bilia_exp_t *bilia_mk_number (bilia_context_t * ctx, const char *str);
  bilia_exp_t *bilia_mk_symbol (bilia_context_t * ctx, const char *name);
  bilia_exp_t *bilia_mk_and (bilia_context_t * ctx, bilia_exp_t ** args,
                           uint_t size);
  bilia_exp_t *bilia_mk_or (bilia_context_t * ctx, bilia_exp_t ** args,
                           uint_t size);
  bilia_exp_t *bilia_mk_not (bilia_context_t * ctx, bilia_exp_t ** args,
                           uint_t size);
  bilia_exp_t *bilia_mk_implies (bilia_context_t * ctx, bilia_exp_t ** args,
                               uint_t size);
  bilia_exp_t *bilia_mk_eq (bilia_context_t * ctx, bilia_exp_t ** args,
                          uint_t size);
  bilia_exp_t *bilia_mk_distinct (bilia_context_t * ctx, bilia_exp_t ** args,
                                uint_t size);
  bilia_exp_t *bilia_mk_ite (bilia_context_t * ctx, bilia_exp_t ** args,
                           uint_t size);
  bilia_exp_t *bilia_mk_lt (bilia_context_t * ctx, bilia_exp_t ** args,
                          uint_t size);
  bilia_exp_t *bilia_mk_gt (bilia_context_t * ctx, bilia_exp_t ** args,
                          uint_t size);
  bilia_exp_t *bilia_mk_le (bilia_context_t * ctx, bilia_exp_t ** args,
                          uint_t size);
  bilia_exp_t *bilia_mk_ge (bilia_context_t * ctx, bilia_exp_t ** args,
                          uint_t size);
  bilia_exp_t *bilia_mk_plus (bilia_context_t * ctx, bilia_exp_t ** args,
                            uint_t size);
  bilia_exp_t *bilia_mk_minus (bilia_context_t * ctx, bilia_exp_t ** args,
                             uint_t size);
  bilia_exp_t *bilia_mk_bag (bilia_context_t * ctx, bilia_exp_t ** args,
                           uint_t size);
  bilia_exp_t *bilia_mk_bagn (bilia_context_t * ctx, bilia_exp_t ** args,
                           uint_t size);
  bilia_exp_t *bilia_mk_emptybag (bilia_context_t * ctx);
  bilia_exp_t *bilia_mk_bagunion (bilia_context_t * ctx, bilia_exp_t ** args,
                                uint_t size);
  bilia_exp_t *bilia_mk_bagminus (bilia_context_t * ctx, bilia_exp_t ** args,
                                uint_t size);
  bilia_exp_t *bilia_mk_bagsum (bilia_context_t * ctx, bilia_exp_t ** args,
                                uint_t size);
  bilia_exp_t *bilia_mk_baginter (bilia_context_t * ctx, bilia_exp_t ** args,
                                uint_t size);
  bilia_exp_t *bilia_mk_subseteq (bilia_context_t * ctx, bilia_exp_t ** args,
                              uint_t size);
  bilia_exp_t *bilia_mk_nsubseteq (bilia_context_t * ctx, bilia_exp_t ** args,
                              uint_t size);
  bilia_exp_t *bilia_mk_in (bilia_context_t * ctx, bilia_exp_t ** args,
                              uint_t size);
  bilia_exp_t *bilia_mk_inn (bilia_context_t * ctx, bilia_exp_t ** args,
                              uint_t size);
  bilia_exp_t *bilia_mk_ni (bilia_context_t * ctx, bilia_exp_t ** args,
                              uint_t size);
  bilia_exp_t *bilia_mk_nin (bilia_context_t * ctx, bilia_exp_t ** args,
                              uint_t size);
  bilia_exp_t *bilia_mk_max (bilia_context_t * ctx, bilia_exp_t ** args,
                              uint_t size);
  bilia_exp_t *bilia_mk_min (bilia_context_t * ctx, bilia_exp_t ** args,
                              uint_t size);
  bilia_exp_t *bilia_mk_bagmax (bilia_context_t * ctx, bilia_exp_t ** args,
                              uint_t size);
  bilia_exp_t *bilia_mk_bagmin (bilia_context_t * ctx, bilia_exp_t ** args,
                              uint_t size);

/* ====================================================================== */
/* Typechecking */
/* ====================================================================== */

  bilia_exp_t *bilia_exp_typecheck (bilia_context_t * ctx, bilia_exp_t * f);
/* Typecheck a formula and do some simplifications if needed. */

/* ====================================================================== */
/* Translation */
/* ====================================================================== */
/* Build a formula from AST in args */
  bilia_dterm_t *bilia_exp_push_dterm (bilia_exp_t * e, bilia_var_array * lvar);
  bilia_dform_t *bilia_exp_push_dform (bilia_exp_t * e, bilia_var_array * lvar);
  // int bilia_exp_push_pure (bilia_form_t * form, bilia_pure_t * pure,
  //                         bilia_exp_t * e, bilia_var_array * lenv,
  //                         const char *msg, const char *ctx);
  void bilia_exp_push (bilia_context_t * ctx, bilia_exp_t * e);
/* Translates the expression into a formula and push in global formulas. */

/* ====================================================================== */
/* Handling errors */
/* ====================================================================== */

  void bilia_error (int level, const char *fun, const char *msg);

#ifndef NDEBUG

#define BILIA_DEBUG(...) \
	do { \
			fprintf (stdout, __VA_ARGS__); \
	} while (0)

#else                           /* #ifndef NDEBUG */

#define BILIA_DEBUG(...)         /* empty */

#endif                          /* #ifndef NDEBUG */

#ifdef __cplusplus
}
#endif

#endif                          /* _BILIA_H */
