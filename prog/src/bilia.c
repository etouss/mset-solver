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

/** Additional definitions needed to parse BILIA files
 */

#include "bilia.h"
#include "bilia_option.h"

/* ====================================================================== */
/* Globals */
/* ====================================================================== */

int bilia_error_parsing = 0;
void breakpoint(){};

/*
 * ======================================================================
 * Messages
 * ======================================================================
 */

void
bilia_error (int level, const char *fun, const char *msg)
{
  fprintf (stderr, "Error of level %d in %s: %s.\n", level, fun, msg);
  if (level == 0)
    //terminate
    exit (0);
  else
    bilia_error_parsing = level;
}

void
bilia_error_args (int level, const char *fun, uint_t size, const char *expect)
{
  fprintf (stderr,
           "Error of level %d in %s: bad number (%d) of arguments, expected (%s).\n",
           level, fun, size, expect);
  if (level == 0)
    //terminate
    exit (0);
  else
    bilia_error_parsing = level;
}

void
bilia_error_id (int level, char *fun, const char *name)
{
  fprintf (stderr,
           "Error of level %d in %s: identifier '%s' is not declared.\n",
           level, fun, name);
  if (level == 0)
    //terminate
    exit (0);
  else
    bilia_error_parsing = level;
}

/*
 * ======================================================================
 * Globals
 * ======================================================================
 */

void
bilia_init ()
{
  /* bilia_record_init (); */
  //TODO:remove bilia_vars_init();
}

/*
 * ======================================================================
 * Context
 * ======================================================================
 */

bilia_context_t *
bilia_mk_context (void)
{
  bilia_context_t *r =
    (bilia_context_t *) malloc (sizeof (struct bilia_context_t));

  /* initialize the global tables for the analysis */
  bilia_init ();

#ifndef NDEBUG
  printf ("bilia_mk_context reset qstack\n");
#endif
  /* initialize the stack of location variables to store
   * one global variable (nil) */
  /* r->lvar_stack = bilia_uint_array_new (); */
  /* bilia_uint_array_push (r->lvar_stack, 1); */

  /* initialize the set of location variables to store
   * nil */
  r->lvar_env = bilia_var_array_new ();
  r->pname = NULL;

  return r;
}

/**
 * Destroy context data at the end of parsing.
 */
void
bilia_del_context (bilia_context_t * ctx)
{
  //ctx->l_env is in general in use
  /* bilia_uint_array_delete (ctx->lvar_stack); */
  bilia_var_array_delete (ctx->lvar_env);
  free (ctx->pname);
  //not in use, usually
  free (ctx);
}

/**
 * Reinitialize the context to globals.
 * A new array shall be created for the @p ctx->*vars.
 */
void
bilia_context_restore_global (bilia_context_t * ctx)
{
  assert (ctx != NULL);
  assert (ctx->lvar_env != NULL);
  /* assert (ctx->lvar_stack != NULL); */
  /* assert (ctx->svar_env != NULL); */
  /* assert (ctx->svar_stack != NULL); */

  bilia_var_array *arr = ctx->lvar_env;
  //this array is in the formulae
  uint_t size = bilia_vector_size(ctx->lvar_env);
  ctx->lvar_env = bilia_var_array_new ();
  if (size > 0)
    bilia_var_array_reserve (ctx->lvar_env, size);
  for (uint_t i = 0; i < size; i++)
  {
    breakpoint();
    bilia_var_array_push (ctx->lvar_env,
                          bilia_var_copy (bilia_vector_at (arr, i)));
  }

  /// SetLoc vars
  //this array is in the formulae

  /* #ifndef NDEBUG */
  /*   fprintf (stderr, "bilia_context_restore_global: (end) %d lvars, %d svars\n", */
  /*            bilia_vector_size (ctx->lvar_env)); */
  /* #endif */
  return;
}

void
bilia_context_fprint (FILE * f, bilia_context_t * ctx)
{
  if (ctx == NULL)
  {
    fprintf (f, "ctx = NULL\n");
    return;
  }
  fprintf (f, "ctx = [pname => %s,\n", ctx->pname);

  /* /// Ref vars */
  /* #<{(| fprintf (f, "\tlvar_stack => ["); |)}># */
  /* #<{(| if (ctx->lvar_stack == NULL) |)}># */
  /* #<{(|   fprintf (f, "NULL"); |)}># */
  /* else */
  /*   { */
  /*     for (uint_t i = 0; i < bilia_vector_size (ctx->lvar_stack); i++) */
  /*       fprintf (stdout, "%d,", bilia_vector_at (ctx->lvar_stack, i)); */
  /*   } */
  /* fprintf (stdout, "\n\t]\n"); */

  fprintf (f, "\tlvar_env => ");
  if (ctx->lvar_env == NULL)
    fprintf (f, "NULL");
  else
    fprintf (f, "%d", bilia_vector_size (ctx->lvar_env));

  /// SetLoc vars
  fprintf (stdout, "\n\tsvar_stack=[");
  fprintf (stdout, "\n\t]\n");

  fprintf (f, "\tsvar_env => ");
  fprintf (stdout, "\n]\n");
}

/*
 * ======================================================================
 * Logic
 * ======================================================================
 */

/** Checks that the logic is supported.
 * @return 1 if the logic is correct, 0 otherwise
 */
int
bilia_set_logic (bilia_context_t * ctx, const char *logic)
{
  if (&ctx != &ctx)
  {
    assert (0);
  }

  if (0 == strcmp (logic, "QF_BILIA"))
    return bilia_form_logic = BILIA_LOGIC_BILIA;

  if (0 == strcmp (logic, "QF_SLRD"))
    return bilia_form_logic = BILIA_LOGIC_SLRD;

  if (0 == strcmp (logic, "QF_S"))
    return bilia_form_logic = BILIA_LOGIC_SLL;

  if (0 == strcmp (logic, "QF_SLRDI"))
    return bilia_form_logic = BILIA_LOGIC_SLRDI;

  bilia_error (0, "set-logic", "unknown logic");
  return bilia_form_logic = BILIA_LOGIC_OTHER;
}

/*
 * ======================================================================
 * Commands
 * ======================================================================
 */

/**
 * Declare a variable or a field.
 * @pre: The @p name is not yet used or predefined.
 * @param ctx    context of the declaration, only globals
 * @param name   identifier declared
 * @param rty    (optionnal) record type
 * @return       @p rty if correct declaration, NULL otherwise
 */
bilia_type_t *
bilia_mk_fun_decl (bilia_context_t * ctx, const char *name, bilia_type_t * rty)
{
  switch (rty->kind)
  {
  case BILIA_TYP_INT:
  case BILIA_TYP_BAGINT:
  {
    /* global variable declaration
     * register it in the array of variables
     */
    bilia_var_register (ctx->lvar_env, name, rty, BILIA_SCOPE_GLOBAL,false);
    /* if (rty != NULL) */
    /*   bilia_vector_at (ctx->lvar_stack, 0) += 1; */
    return rty;
  }
  default:
    //error, return NULL below
    break;
  }
  return NULL;
}

bilia_exp_t *
bilia_mk_app (bilia_context_t * ctx, const char *name,
              bilia_exp_t ** args, uint_t size)
{
  if (size == 0)
  {
    //if (strcmp (name, "emptybag") == 0)
      //return bilia_mk_emptybag (ctx);
    return bilia_mk_symbol (ctx, name);
  }
  /// is a data field application
  return NULL;
}

int
bilia_assert (bilia_context_t * ctx, bilia_exp_t * term)
{
  //check that the formula is not null
  if (!term)
  {
    bilia_error (1, "bilia_assert", "null formula");
    return 0;
  }
  bilia_exp_t *form = bilia_exp_typecheck (ctx, term);
  if (form == NULL)
  {
    bilia_error (1, "bilia_assert", "typechecking error");
    return 0;
  }
  bilia_exp_push (ctx, form);
  bilia_context_restore_global (ctx);
  return 1;
}

/**
 * Sets the command.
 * @return if only one formula then 0, 1, -1 for sat, unsat, unknown
 *         if entailment (two formulas) then 0, 1, -1 for valid, invalid, unknown
 */
int
bilia_check (bilia_context_t * ctx)
{
  if (&ctx != &ctx)
  {
    assert (0);               // to avoid "unused parameter" warning
  }
  if (bilia_error_parsing > 0)
  {
    assert (bilia_prob->smt_fname != NULL);
    bilia_error (0, "bilia_check", "stop because of parsing error");
    return 0;
  }

  bilia_entl_set_cmd (BILIA_FORM_SAT);
  return bilia_entl_solve ();
}

/*
 * ======================================================================
 * Terms
 * ======================================================================
 */

/** Adds the variable to the topmost local array in the context,
 * depending of this type.
 */
void
bilia_push_var (bilia_context_t * ctx, const char *name, bilia_type_t * vty)
{
  if (!ctx)
    return;
  //uid_t vid = UNDEFINED_ID;
  if ((vty->kind == BILIA_TYP_INT) || (vty->kind == BILIA_TYP_BAGINT))
  {
    assert (ctx->lvar_env != NULL);
    bilia_var_t *v = bilia_var_new (name, vty, BILIA_SCOPE_LOCAL);
    bilia_var_array_push (ctx->lvar_env, v);
    v->vid = bilia_vector_size (ctx->lvar_env) - 1;
    //update the number of elements in the top of the stack
    /* uint_t top_stack = bilia_vector_size (ctx->lvar_stack) - 1; */
    /* bilia_vector_at (ctx->lvar_stack, top_stack) += 1; */
  }
  else
    assert (0);
}

bilia_exp_t *
bilia_mk_op (bilia_expkind_t f, bilia_exp_t ** args, uint_t size)
{
  bilia_exp_t *res = (bilia_exp_t *) malloc (sizeof (struct bilia_exp_t));
  res->discr = f;
  res->size = size;
  res->args = NULL;
  if (size != 0)
  {
    res->args = (bilia_exp_t **) malloc (size * sizeof (bilia_exp_t *));
    for (uint_t i = 0; i < size; i++)
      res->args[i] = args[i];
  }
  return res;
}

/** @brief Build a term including the integer given by @p str.
 */
bilia_exp_t *
bilia_mk_number (bilia_context_t * ctx, const char *str)
{
  if (&ctx != &ctx)
  {
    assert (0);
  }

  bilia_exp_t *res = bilia_mk_op (BILIA_F_INT, NULL, 0);
  char *endstr;
  res->p.value = strtol (str, &endstr, 10);
  if (endstr == NULL)
  {
    /// bad translation to integer
    bilia_error_id (1, "bilia_mk_number", str);
    return NULL;
  }
  return res;
}

/** @brief Build a term from this variable or field.
 */
bilia_exp_t *
bilia_mk_symbol (bilia_context_t * ctx, const char *name)
{
  assert (ctx && name);
  bilia_exp_t *ret = NULL;
  uint_t sid = UNDEFINED_ID;
  bilia_type_t *typ = NULL;
#ifndef NDEBUG
  fprintf (stdout, "mk_symbol: start %s\n", name);
  fflush (stdout);
#endif
  /* special case of 'emptybag' */
  // if (strcmp (name, "emptybag") == 0)
  // {
  //   ret = bilia_mk_op (BILIA_F_EMPTYBAG, NULL, 0);
  //   return ret;
  // }
  //search the variable environment
  // -search in the location env
  assert (ctx->lvar_env != NULL);
  sid = bilia_var_array_find_local (ctx->lvar_env, name);
  if (sid != UNDEFINED_ID)
    typ = (bilia_vector_at (ctx->lvar_env, sid))->vty;
  if (typ != NULL)
  {
    if ((typ->kind == BILIA_TYP_INT) || (typ->kind == BILIA_TYP_BAGINT))
    {
      ret = bilia_mk_op (BILIA_F_LVAR, NULL, 0);
      ret->p.sid = sid;
    }
#ifndef NDEBUG
    fprintf (stdout, "mk_symbol: local %s (id %d)\n", name, ret->p.sid);
#endif
    return ret;
  }
  /* else, it shall be a field */
  if (name[0] == '?')
  {
    //fields cannot start with ?
    bilia_error_id (1, "bilia_mk_symbol", name);
    return NULL;
  }
  /* else error */
  bilia_error_id (1, "bilia_mk_symbol", name);
  return NULL;
}

bilia_exp_t *
bilia_mk_and (bilia_context_t * ctx, bilia_exp_t ** args, uint_t size)
{

  if (size == 1)
    return args[0];
  return bilia_mk_op (BILIA_F_AND, args, size);
}

bilia_exp_t *
bilia_mk_or (bilia_context_t * ctx, bilia_exp_t ** args, uint_t size)
{
  if (size == 1)
    return args[0];
  return bilia_mk_op (BILIA_F_OR, args, size);
}

bilia_exp_t *
bilia_mk_not (bilia_context_t * ctx, bilia_exp_t ** args, uint_t size)
{
  if (&ctx != &ctx)
  {
    assert (0);
  }
  if (size != 1)
    bilia_error_args (1, "bilia_mk_not", size, "= 1");
  return bilia_mk_op (BILIA_F_NOT, args, size);
}

bilia_exp_t *
bilia_mk_implies (bilia_context_t * ctx, bilia_exp_t ** args, uint_t size)
{
  if (&ctx != &ctx)
  {
    assert (0);
  }

  if (size != 2)
    bilia_error_args (1, "bilia_mk_impl", size, "= 2");
  return bilia_mk_op (BILIA_F_IMPLIES, args, size);
}

bilia_exp_t *
bilia_mk_eq (bilia_context_t * ctx, bilia_exp_t ** args, uint_t size)
{
  if (&ctx != &ctx)
  {
    assert (0);
  }

  if (size != 2)
    bilia_error_args (1, "bilia_mk_eq", size, "= 2");
  return bilia_mk_op (BILIA_F_EQ, args, size);
}

bilia_exp_t *
bilia_mk_distinct (bilia_context_t * ctx, bilia_exp_t ** args, uint_t size)
{
  if (&ctx != &ctx)
  {
    assert (0);
  }

  if (size != 2)
    bilia_error_args (1, "bilia_mk_distinct", size, "= 2");
  return bilia_mk_op (BILIA_F_DISTINCT, args, size);
}

bilia_exp_t *
bilia_mk_ite (bilia_context_t * ctx, bilia_exp_t ** args, uint_t size)
{
  if (&ctx != &ctx)
  {
    assert (0);
  }

  if (size != 3)
    bilia_error_args (1, "bilia_mk_ite", size, "= 3");
  return bilia_mk_op (BILIA_F_ITE, args, size);
}

bilia_exp_t *
bilia_mk_lt (bilia_context_t * ctx, bilia_exp_t ** args, uint_t size)
{
  if (&ctx != &ctx)
  {
    assert (0);
  }

  if (size != 2)
    bilia_error_args (1, "bilia_mk_lt", size, "= 2");
  return bilia_mk_op (BILIA_F_LT, args, size);
}

bilia_exp_t *
bilia_mk_gt (bilia_context_t * ctx, bilia_exp_t ** args, uint_t size)
{
  if (&ctx != &ctx)
  {
    assert (0);
  }

  if (size != 2)
    bilia_error_args (1, "bilia_mk_gt", size, "= 2");
  return bilia_mk_op (BILIA_F_GT, args, size);
}

bilia_exp_t *
bilia_mk_le (bilia_context_t * ctx, bilia_exp_t ** args, uint_t size)
{
  if (&ctx != &ctx)
  {
    assert (0);
  }

  if (size != 2)
    bilia_error_args (1, "bilia_mk_le", size, "= 2");
  return bilia_mk_op (BILIA_F_LE, args, size);
}

bilia_exp_t *
bilia_mk_ge (bilia_context_t * ctx, bilia_exp_t ** args, uint_t size)
{
  if (&ctx != &ctx)
  {
    assert (0);
  }

  if (size != 2)
    bilia_error_args (1, "bilia_mk_ge", size, "= 2");
  return bilia_mk_op (BILIA_F_GE, args, size);
}

bilia_exp_t *
bilia_mk_plus (bilia_context_t * ctx, bilia_exp_t ** args, uint_t size)
{
  if (&ctx != &ctx)
  {
    assert (0);
  }

  if (size < 2)
    bilia_error_args (1, "bilia_mk_plus", size, "< 2");
  return bilia_mk_op (BILIA_F_PLUS, args, size);
}

bilia_exp_t *
bilia_mk_minus (bilia_context_t * ctx, bilia_exp_t ** args, uint_t size)
{
  if (&ctx != &ctx)
  {
    assert (0);
  }

  if (size != 2)
    bilia_error_args (1, "bilia_mk_minus", size, "= 2");
  return bilia_mk_op (BILIA_F_MINUS, args, size);
}

bilia_exp_t *
bilia_mk_bag (bilia_context_t * ctx, bilia_exp_t ** args, uint_t size)
{
  if (&ctx != &ctx)
  {
    assert (0);
  }

  if (size != 1)
    bilia_error_args (1, "bilia_mk_bag", size, "= 1");
  return bilia_mk_op (BILIA_F_BAG, args, size);
}

bilia_exp_t *
bilia_mk_bagn (bilia_context_t * ctx, bilia_exp_t ** args, uint_t size)
{
  if (&ctx != &ctx)
  {
    assert (0);
  }

  if (size != 2)
    bilia_error_args (1, "bilia_mk_bagn", size, "= 2");
  return bilia_mk_op (BILIA_F_BAGN, args, size);
}


bilia_exp_t *
bilia_mk_emptybag (bilia_context_t * ctx)
{
  if (&ctx != &ctx)
  {
    assert (0);
  }

  bilia_exp_t *res = (bilia_exp_t *) malloc (sizeof (struct bilia_exp_t));
  res->discr = BILIA_F_EMPTYBAG;
  res->args = NULL;
  res->size = 0;
  return res;
}

bilia_exp_t *
bilia_mk_bagunion (bilia_context_t * ctx, bilia_exp_t ** args, uint_t size)
{
  if (&ctx != &ctx)
  {
    assert (0);
  }

  if (size < 2)
    bilia_error_args (1, "bilia_mk_bagunion", size, ">= 2");
  return bilia_mk_op (BILIA_F_BAGUNION, args, size);
}

bilia_exp_t *
bilia_mk_bagminus (bilia_context_t * ctx, bilia_exp_t ** args, uint_t size)
{
  if (&ctx != &ctx)
  {
    assert (0);
  }

  if (size != 2)
    bilia_error_args (1, "bilia_mk_bagminus", size, "= 2");
  return bilia_mk_op (BILIA_F_BAGMINUS, args, size);
}

bilia_exp_t *
bilia_mk_bagsum (bilia_context_t * ctx, bilia_exp_t ** args, uint_t size)
{
  if (&ctx != &ctx)
  {
    assert (0);
  }

  if (size != 2)
    bilia_error_args (1, "bilia_mk_bagsum", size, "= 2");
  return bilia_mk_op (BILIA_F_BAGSUM, args, size);
}

bilia_exp_t *
bilia_mk_baginter (bilia_context_t * ctx, bilia_exp_t ** args, uint_t size)
{
  if (&ctx != &ctx)
  {
    assert (0);
  }

  if (size != 2)
    bilia_error_args (1, "bilia_mk_baginter", size, "= 2");
  return bilia_mk_op (BILIA_F_BAGINTER, args, size);
}


bilia_exp_t *
bilia_mk_subseteq (bilia_context_t * ctx, bilia_exp_t ** args, uint_t size)
{
  if (&ctx != &ctx)
  {
    assert (0);
  }

  if (size != 2)
    bilia_error_args (1, "bilia_mk_subseteq", size, "= 2");
  return bilia_mk_op (BILIA_F_SUBSETEQ, args, size);
}

bilia_exp_t *
bilia_mk_nsubseteq (bilia_context_t * ctx, bilia_exp_t ** args, uint_t size)
{
  if (&ctx != &ctx)
  {
    assert (0);
  }

  if (size != 2)
    bilia_error_args (1, "bilia_mk_subseteq", size, "= 2");
  return bilia_mk_op (BILIA_F_NSUBSETEQ, args, size);
}


bilia_exp_t *
bilia_mk_in (bilia_context_t * ctx, bilia_exp_t ** args, uint_t size)
{
  if (&ctx != &ctx)
  {
    assert (0);
  }

  if (size != 2)
    bilia_error_args (1, "bilia_mk_in", size, "= 2");
  return bilia_mk_op (BILIA_F_IN, args, size);
}

bilia_exp_t *
bilia_mk_ni (bilia_context_t * ctx, bilia_exp_t ** args, uint_t size)
{
  if (&ctx != &ctx)
  {
    assert (0);
  }

  if (size != 2)
    bilia_error_args (1, "bilia_mk_in", size, "= 2");
  return bilia_mk_op (BILIA_F_NI, args, size);
}

bilia_exp_t *
bilia_mk_inn (bilia_context_t * ctx, bilia_exp_t ** args, uint_t size)
{
  if (&ctx != &ctx)
  {
    assert (0);
  }

  if (size != 3)
    bilia_error_args (1, "bilia_mk_inn", size, "= 3");
  return bilia_mk_op (BILIA_F_INN, args, size);
}

bilia_exp_t *
bilia_mk_nin (bilia_context_t * ctx, bilia_exp_t ** args, uint_t size)
{
  if (&ctx != &ctx)
  {
    assert (0);
  }

  if (size != 3)
    bilia_error_args (1, "bilia_mk_inn", size, "= 3");
  return bilia_mk_op (BILIA_F_NIN, args, size);
}


bilia_exp_t *
bilia_mk_max (bilia_context_t * ctx, bilia_exp_t ** args, uint_t size)
{
  if (&ctx != &ctx)
  {
    assert (0);
  }

  if (size != 1)
    bilia_error_args (1, "bilia_mk_max", size, "= 1");
  return bilia_mk_op (BILIA_F_MAX, args, size);
}

bilia_exp_t *
bilia_mk_min (bilia_context_t * ctx, bilia_exp_t ** args, uint_t size)
{
  if (&ctx != &ctx)
  {
    assert (0);
  }

  if (size != 1)
    bilia_error_args (1, "bilia_mk_min", size, "= 1");
  return bilia_mk_op (BILIA_F_MIN, args, size);
}

bilia_exp_t *
bilia_mk_bagmax (bilia_context_t * ctx, bilia_exp_t ** args, uint_t size)
{
  if (&ctx != &ctx)
  {
    assert (0);
  }

  if (size != 1)
    bilia_error_args (1, "bilia_mk_bagmax", size, "= 1");
  return bilia_mk_op (BILIA_F_BAGMAX, args, size);
}
bilia_exp_t *
bilia_mk_bagmin (bilia_context_t * ctx, bilia_exp_t ** args, uint_t size)
{
  if (&ctx != &ctx)
  {
    assert (0);
  }

  if (size != 1)
    bilia_error_args (1, "bilia_mk_bagmin", size, "= 1");
  return bilia_mk_op (BILIA_F_BAGMIN, args, size);
}



/*
 * ======================================================================
 * Printing
 * ======================================================================
 */

void
bilia_exp_printf (FILE * f, bilia_context_t * ctx, bilia_exp_t * e)
{
  assert (f);
  if (!e)
  {
    fprintf (f, "null\n");
    return;
  }
  switch (e->discr)
  {
  case BILIA_F_FALSE:
  {
    fprintf (f, " false ");
    return;
  }
  case BILIA_F_INT:
  {
    fprintf (f, " %ld ", e->p.value);
    return;
  }
  case BILIA_F_NOT:
  {
    fprintf (f, " (not \n");
    break;
  }
  case BILIA_F_AND:
  {
    fprintf (f, " (and \n\t");
    break;
  }
  case BILIA_F_IMPLIES:
  {
    fprintf (f, " (=> \n\t");
    break;
  }
  case BILIA_F_EQ:
  {
    fprintf (f, " (= ");
    break;
  }
  case BILIA_F_DISTINCT:
  {
    fprintf (f, " (distinct ");
    break;
  }
  case BILIA_F_ITE:
  {
    fprintf (f, " (ite ");
    break;
  }
  case BILIA_F_LT:
  {
    fprintf (f, " (< ");
    break;
  }
  case BILIA_F_GT:
  {
    fprintf (f, " (> ");
    break;
  }
  case BILIA_F_LE:
  {
    fprintf (f, " (<= ");
    break;
  }
  case BILIA_F_GE:
  {
    fprintf (f, " (>= ");
    break;
  }
  case BILIA_F_BAG:
  {
    fprintf (f, " (bag ");
    break;
  }
  case BILIA_F_BAGN:
  {
    fprintf (f, " (bagn ");
    break;
  }
  case BILIA_F_EMPTYBAG:
  {
    fprintf (f, " emptybag ");
    break;
  }
  case BILIA_F_BAGUNION:
  {
    fprintf (f, " (bagunion ");
    break;
  }
  case BILIA_F_BAGMINUS:
  {
    fprintf (f, " (bagminus ");
    break;
  }
  case BILIA_F_BAGINTER:
  {
    fprintf (f, " (baginter ");
    break;
  }
  case BILIA_F_BAGSUM:
  {
    fprintf (f, " (bagsum ");
    break;
  }
  case BILIA_F_SUBSETEQ:
  {
    fprintf (f, " (subseteq ");
    break;
  }
  case BILIA_F_NSUBSETEQ:
  {
    fprintf (f, " (nsubseteq ");
    break;
  }
  case BILIA_F_IN:
  {
    fprintf (f, " (in ");
    break;
  }
  case BILIA_F_INN:
  {
    fprintf (f, " (inn ");
    break;
  }
  case BILIA_F_NIN:
  {
    fprintf (f, " (nin ");
    break;
  }
  case BILIA_F_MAX:
  {
    fprintf (f, " (max ");
    break;
  }
  case BILIA_F_MIN:
  {
    fprintf (f, " (min ");
    break;
  }
  case BILIA_F_BAGMAX:
  {
    fprintf (f, " (bagmax ");
    break;
  }
  case BILIA_F_BAGMIN:
  {
    fprintf (f, " (bagmin ");
    break;
  }
  default:
  {
    fprintf (f, " (unknown \n\t");
    break;
  }
  }
  if (e->args)
  {
    uint_t i;
    for (i = 0; i < e->size; i++)
    {
      bilia_exp_printf (f, ctx, e->args[i]);
      fprintf (f, "\n\t");
    }
  }
  fprintf (f, " )\n");
}

/*
 * ======================================================================
 * Typechecking
 * ======================================================================
 */

bilia_exp_t *
bilia_exp_typecheck (bilia_context_t * ctx, bilia_exp_t * e)
{
    if (&ctx != &ctx)
    {
      assert (0);
    }
    if (!e)
      return e;

    switch(e->discr){
      case BILIA_F_LT:
      case BILIA_F_GT:
      case BILIA_F_LE:
      case BILIA_F_GE:
      case BILIA_F_EQ:
      case BILIA_F_DISTINCT:
      case BILIA_F_SUBSETEQ:
      case BILIA_F_NSUBSETEQ:
      case BILIA_F_IN:
      case BILIA_F_NI:
      case BILIA_F_INN:
      case BILIA_F_NIN:
        return e;
      case BILIA_F_AND:
      case BILIA_F_OR:
      case BILIA_F_IMPLIES:
      case BILIA_F_NOT:
        for (size_t i = 0;  i < e->size; i++)
          if(bilia_exp_typecheck(ctx,e->args[i]) == NULL)
          return NULL;
        return e;
      default:
        bilia_error (0, "bilia_exp_typecheck", "syntax error in formula");
        return NULL;
    }
}

/*
 * ======================================================================
 * Translation to formula
 * ======================================================================
 */

/**
 * @brief Translate the SMTLIB AST into the internal data AST.
 * Because the translation is direct, no need to push in a formula.
 */
bilia_dterm_t *
bilia_exp_push_dterm (bilia_exp_t * e, bilia_var_array * lenv)
{
  bilia_dterm_t *dt = bilia_dterm_new ();
  switch (e->discr)
  {
  case BILIA_F_INT:
  {
    dt->kind = BILIA_DATA_INT;
    dt->typ = BILIA_TYP_INT;
    dt->p.value = e->p.value;
    return dt;
    break;
  }
  case BILIA_F_LVAR:
  {
    dt->kind = BILIA_DATA_VAR;
    bilia_var_t *v = bilia_vector_at (lenv, e->p.sid);
    dt->typ = v->vty->kind;
    dt->p.sid = e->p.sid;
    return dt;
    break;
  }
  case BILIA_F_EMPTYBAG:
  {
    dt->kind = BILIA_DATA_EMPTYBAG;
    dt->typ = BILIA_TYP_BAGINT;
    return dt;
    break;
  }
  case BILIA_F_ITE:
  {
     dt->kind = BILIA_DATA_ITE;
     //dt->typ = BILIA_TYP_INT; /// NEW: or BagInt?
     break;
  }
  case BILIA_F_PLUS:
  {
    dt->kind = BILIA_DATA_PLUS;
    dt->typ = BILIA_TYP_INT;
    break;
  }
  case BILIA_F_MINUS:
  {
    dt->kind = BILIA_DATA_MINUS;
    dt->typ = BILIA_TYP_INT;
    break;
  }
  case BILIA_F_MAX:
  {
    dt->kind = BILIA_DATA_MAX;
    dt->typ = BILIA_TYP_INT;
    break;
  }
  case BILIA_F_MIN:
  {
    dt->kind = BILIA_DATA_MIN;
    dt->typ = BILIA_TYP_INT;
    break;
  }
  case BILIA_F_BAGMAX:
  {
    dt->kind = BILIA_DATA_BAGMAX;
    dt->typ = BILIA_TYP_INT;
    break;
  }
  case BILIA_F_BAGMIN:
  {
    dt->kind = BILIA_DATA_BAGMIN;
    dt->typ = BILIA_TYP_INT;
    break;
  }

  case BILIA_F_BAG:
  {
    dt->kind = BILIA_DATA_BAG;
    dt->typ = BILIA_TYP_BAGINT;
    break;
  }
  case BILIA_F_BAGN:
  {
    dt->kind = BILIA_DATA_BAGN;
    dt->typ = BILIA_TYP_BAGINT;
    break;
  }
  case BILIA_F_BAGINTER:
  {
    /*
    Here we modify exp in order to ensure INTER is a binary operation
    */
    dt->kind = BILIA_DATA_BAGINTER;
    dt->typ = BILIA_TYP_BAGINT;
    if(bilia_option_get_nr())break;
    dt->args = bilia_dterm_array_new ();
    /* bilia_dterm_array_reserve (dt->args, 3); */
    bilia_dterm_array * en_cours = dt->args;
    for (uint_t i = 0; i < e->size; i++)
    {
      bilia_dterm_t *ti = bilia_exp_push_dterm (e->args[i], lenv);
      if (ti == NULL)
      {
        bilia_error (1, "Building data term ", "(bad sub-term)");
        bilia_dterm_free (dt);
        return NULL;
      }
      bilia_dterm_array_push (en_cours, ti);
      if((i+2)<e->size){
        bilia_dterm_t *tmpi = bilia_dterm_new();
        tmpi->kind = BILIA_DATA_BAGINTER;
        tmpi->typ = BILIA_TYP_BAGINT;
        tmpi->args = bilia_dterm_array_new();
        /* bilia_dterm_array_reserve (tmpi->args, 2); */
        bilia_dterm_array_push (en_cours, tmpi);
        en_cours = tmpi->args;
      }
    }
    return dt;
  }
  case BILIA_F_BAGMINUS:
  {
    dt->kind = BILIA_DATA_BAGMINUS;
    dt->typ = BILIA_TYP_BAGINT;
    break;
  }
  case BILIA_F_BAGSUM:
  {
    /*
    Here we modify exp in order to ensure SUM is a binary operation
    */
    dt->kind = BILIA_DATA_BAGSUM;
    dt->typ = BILIA_TYP_BAGINT;
    if(bilia_option_get_nr())break;
    dt->args = bilia_dterm_array_new ();
    /* bilia_dterm_array_reserve (dt->args, 3); */
    bilia_dterm_array * en_cours = dt->args;
    for (uint_t i = 0; i < e->size; i++)
    {
      bilia_dterm_t *ti = bilia_exp_push_dterm (e->args[i], lenv);
      if (ti == NULL)
      {
        bilia_error (1, "Building data term ", "(bad sub-term)");
        bilia_dterm_free (dt);
        return NULL;
      }
      bilia_dterm_array_push (en_cours, ti);
      if((i+2)<e->size){
        bilia_dterm_t *tmpi = bilia_dterm_new();
        tmpi->kind = BILIA_DATA_BAGSUM;
        tmpi->typ = BILIA_TYP_BAGINT;
        tmpi->args = bilia_dterm_array_new();
        /* bilia_dterm_array_reserve (tmpi->args, 2); */
        bilia_dterm_array_push (en_cours, tmpi);
        en_cours = tmpi->args;
      }
    }
    return dt;
  }
  case BILIA_F_BAGUNION:
  {
    /*
    Here we modify exp in order to ensure UNION is a binary operation
    */
    dt->kind = BILIA_DATA_BAGUNION;
    dt->typ = BILIA_TYP_BAGINT;
    if(bilia_option_get_nr())break;
    dt->args = bilia_dterm_array_new ();
    /* bilia_dterm_array_reserve (dt->args, 3); */
    bilia_dterm_array * en_cours = dt->args;
    for (uint_t i = 0; i < e->size; i++)
    {
      bilia_dterm_t *ti = bilia_exp_push_dterm (e->args[i], lenv);
      if (ti == NULL)
      {
        bilia_error (1, "Building data term ", "(bad sub-term)");
        bilia_dterm_free (dt);
        return NULL;
      }
      bilia_dterm_array_push (en_cours, ti);
      if((i+2)<e->size){
        bilia_dterm_t *tmpi = bilia_dterm_new();
        tmpi->kind = BILIA_DATA_BAGUNION;
        tmpi->typ = BILIA_TYP_BAGINT;
        tmpi->args = bilia_dterm_array_new();
        /* bilia_dterm_array_reserve (tmpi->args, 2); */
        bilia_dterm_array_push (en_cours, tmpi);
        en_cours = tmpi->args;
      }
    }
    return dt;

  }
  /*a remplir*/
  default:
  {
    bilia_error (1, "Building data term ", "(bad operator)");
    bilia_dterm_free (dt);
    return NULL;
  }
  }
  if (e->size > 0)
  {
    dt->args = bilia_dterm_array_new ();
    bilia_dterm_array_reserve (dt->args, e->size);
  }
  uint_t i = 0;
  if (dt->kind == BILIA_DATA_ITE)
  {
    bilia_dform_t *cond = bilia_exp_push_dform (e->args[i], lenv);
    if (cond == NULL)
    {
      bilia_error (1, "Building data term ",
                   "(bad bollean condition in ite)");
      bilia_dterm_free (dt);
      return NULL;
    }
    dt->p.cond = cond;
    i++;
  }
  for (; i < e->size; i++)
  {
    bilia_dterm_t *ti = bilia_exp_push_dterm (e->args[i], lenv);
    if (ti == NULL)
    {
      bilia_error (1, "Building data term ", "(bad sub-term)");
      bilia_dterm_free (dt);
      return NULL;
    }
    if ((dt->kind == BILIA_DATA_ITE) && (i == 1))
       dt->typ = ti->typ;
       /*TYPE CHECK*/
    if(((dt->kind == BILIA_DATA_PLUS || dt->kind == BILIA_DATA_MINUS || dt->kind == BILIA_DATA_MIN || dt->kind == BILIA_DATA_MAX) && ti->typ != BILIA_TYP_INT)
      ||((dt->kind == BILIA_DATA_BAGUNION || dt->kind == BILIA_DATA_BAGINTER || dt->kind == BILIA_DATA_BAGSUM || dt->kind == BILIA_DATA_BAGMINUS || dt->kind == BILIA_DATA_BAGMAX || dt->kind == BILIA_DATA_BAGMIN)&&(ti->typ != BILIA_TYP_BAGINT)))
    {
      bilia_error (1, "Building data term ", "(bad type)");
      bilia_dterm_free (dt);
      return NULL;
    }
    bilia_dterm_array_push (dt->args, ti);
  }
  return dt;
}

/**
 * @brief Translate the SMTLIB AST into the internal data AST.
 * Because the translation is direct, no need to push in a formula.
 */
bilia_dform_t *
bilia_exp_push_dform (bilia_exp_t * e, bilia_var_array * lenv)
{
  assert (NULL != e);

  bilia_dform_t *df = bilia_dform_new ();
  switch (e->discr)
  {
  case BILIA_F_EQ:
  {
    df->kind = BILIA_DATA_EQ;
    break;
  }
  case BILIA_F_DISTINCT:
  {
    df->kind = BILIA_DATA_NEQ;
    break;
  }
  case BILIA_F_LT:
  {
    df->kind = BILIA_DATA_LT;
    break;
  }
  case BILIA_F_GT:
  {
    df->kind = BILIA_DATA_GT;
    break;
  }
  case BILIA_F_LE:
  {
    df->kind = BILIA_DATA_LE;
    break;
  }
  case BILIA_F_GE:
  {
    df->kind = BILIA_DATA_GE;
    break;
  }
  case BILIA_F_IN:
  {
    df->kind = BILIA_DATA_IN;
    break;
  }
  case BILIA_F_NI:
  {
    df->kind = BILIA_DATA_NI;
    break;
  }
  case BILIA_F_INN:
  {
    df->kind = BILIA_DATA_INN;
    break;
  }
  case BILIA_F_NIN:
  {
    df->kind = BILIA_DATA_NIN;
    break;
  }
  case BILIA_F_SUBSETEQ:
  {
    df->kind = BILIA_DATA_SUBSETEQ;
    break;
  }
  case BILIA_F_NSUBSETEQ:
  {
    df->kind = BILIA_DATA_NSUBSETEQ;
    break;
  }
  case BILIA_F_AND:
  {
    df->kind = BILIA_DATA_AND;
    break;
  }
  case BILIA_F_OR:
  {
    df->kind = BILIA_DATA_OR;
    break;
  }
  case BILIA_F_IMPLIES:
  {
    df->kind = BILIA_DATA_IMPLIES;
    break;
  }
  case BILIA_F_NOT:
  {
    df->kind = BILIA_DATA_NOT;
    break;
  }
  default:
  {
    bilia_error (1, "Building data formula ", "(bad operator)");
    bilia_dform_free (df);
    return NULL;
  }
  }
  switch (e->discr)
  {
    /*If Binary Literal expression*/
  case BILIA_F_EQ:
  case BILIA_F_DISTINCT:
  case BILIA_F_LT:
  case BILIA_F_GT:
  case BILIA_F_LE:
  case BILIA_F_GE:
  case BILIA_F_IN:
  case BILIA_F_NI:
  case BILIA_F_SUBSETEQ:
  case BILIA_F_NSUBSETEQ:
  {
    df->p.targs = bilia_dterm_array_new ();
    df->typ = BILIA_TYP_BOOL;
    bilia_dterm_t *t1 = bilia_exp_push_dterm (e->args[0], lenv);
    bilia_dterm_t *t2 = bilia_exp_push_dterm (e->args[1], lenv);
      if (t1 == NULL || t2 == NULL)
      {
        bilia_error (1, "Building data formula ", "(bad terms)");
        bilia_dform_free (df);
        return NULL;
      }
      /*Check type for NI and IN*/
      if(df->kind == BILIA_DATA_NI || df->kind == BILIA_DATA_IN){
        if(t1->typ != BILIA_TYP_INT || t2->typ != BILIA_TYP_BAGINT){
          bilia_error (1, "Building data formula ", "(bad type for terms)");
          bilia_dform_free (df);
          return NULL;
        }
      }
      /*Check type for subset and nsubset*/
      else if(df->kind == BILIA_DATA_SUBSETEQ || df->kind == BILIA_DATA_NSUBSETEQ){
        if(t1->typ != BILIA_TYP_BAGINT || t2->typ != BILIA_TYP_BAGINT){
          bilia_error (1, "Building data formula ", "(bad type for terms)");
          bilia_dform_free (df);
          return NULL;
        }
      }
      /*Check type for everything else*/
      else if(t1->typ != t2->typ){
        bilia_error (1, "Building data formula ", "(bad type for terms)");
        bilia_dform_free (df);
        return NULL;
      }
    /*Add term to literal*/
    bilia_dterm_array_push (df->p.targs, t1);
    bilia_dterm_array_push (df->p.targs, t2);
    return df;
  }
  /*If ternary literal expression*/
  case BILIA_F_INN:
  case BILIA_F_NIN:
  {
    df->p.targs = bilia_dterm_array_new ();
    df->typ = BILIA_TYP_BOOL;
    bilia_dterm_t *t1 = bilia_exp_push_dterm (e->args[0], lenv);
    bilia_dterm_t *t2 = bilia_exp_push_dterm (e->args[1], lenv);
    bilia_dterm_t *t3 = bilia_exp_push_dterm (e->args[2], lenv);
      if (t1 == NULL || t2 == NULL || t3 == NULL)
      {
        bilia_error (1, "Building data formula ", "(bad terms)");
        bilia_dform_free (df);
        return NULL;
      }
      /*Check type*/
      /*An unexpected behaviour may occur when t3 is in V_{ele}*/
      if(t1->typ != BILIA_TYP_INT || t2->typ != BILIA_TYP_BAGINT || t3->typ != BILIA_TYP_INT){
        bilia_error (1, "Building data formula ", "(bad type for terms)");
        bilia_dform_free (df);
        return NULL;
      }
    /*Add term to litteral*/
    bilia_dterm_array_push (df->p.targs, t1);
    bilia_dterm_array_push (df->p.targs, t2);
    bilia_dterm_array_push (df->p.targs, t3);
    return df;
  }
  /*If boolean expression*/
  case BILIA_F_OR:
  case BILIA_F_AND:
  case BILIA_F_IMPLIES:
  case BILIA_F_NOT:
  {
    df->p.bargs = bilia_dform_array_new ();
    df->typ = BILIA_TYP_BOOL;
    for (size_t i = 0;  i<e->size; i++) {
      /* code */
      bilia_dform_t *f = bilia_exp_push_dform(e->args[i],lenv);
      if (f == NULL)
      {
        bilia_error (1, "Building data formula ", "(bad terms)");
        bilia_dform_free (df);
        return NULL;
      }
      /*Check Type*/
      if(f->typ != BILIA_TYP_BOOL){
        bilia_error (1, "Building data formula ", "(bad type for terms)");
        bilia_dform_free (df);
        return NULL;
      }
      /*Add literal to formula*/
      bilia_dform_array_push(df->p.bargs,f);
    }
    return df;

  }
  default:
    bilia_error (1, "Building data formula ", "(bad terms)");
    bilia_dform_free (df);
    return NULL;
}
}

bilia_dform_t *
bilia_exp_push_top (bilia_context_t * ctx, bilia_exp_t * e)
{
  /*Init the push expr when assert() with the rigth DATA*/
  assert (ctx != NULL);
  assert (e != NULL);
  /// copy variables from context to formula
  if ( bilia_entl_get()->lvars != NULL &&  bilia_entl_get()->lvars != ctx->lvar_env)
    bilia_var_array_delete (bilia_entl_get()->lvars);
  bilia_entl_get()->lvars = ctx->lvar_env;
  //fill other parts of the formula
  switch (e->discr)
  {
  case BILIA_F_OR:
  case BILIA_F_AND:
  case BILIA_F_NOT:
  case BILIA_F_IMPLIES:
  case BILIA_F_EQ:
  case BILIA_F_DISTINCT:
  case BILIA_F_LT:
  case BILIA_F_GT:
  case BILIA_F_LE:
  case BILIA_F_GE:
  case BILIA_F_NSUBSETEQ:
  case BILIA_F_SUBSETEQ:
  case BILIA_F_IN:
  case BILIA_F_NI:
  case BILIA_F_INN:
  case BILIA_F_NIN:
  {
    return bilia_exp_push_dform(e, bilia_entl_get()->lvars);
    break;
  }
  case BILIA_F_PLUS:
  case BILIA_F_MINUS:
  case BILIA_F_BAG:
  case BILIA_F_BAGN:
  case BILIA_F_EMPTYBAG:
  case BILIA_F_BAGUNION:
  case BILIA_F_BAGMINUS:
  case BILIA_F_BAGINTER:
  case BILIA_F_BAGSUM:
  case BILIA_F_MAX:
  case BILIA_F_MIN:
  case BILIA_F_BAGMAX:
  case BILIA_F_BAGMIN:
  case BILIA_F_ITE:
  {
    //this is an error, no translation is possible
    bilia_error (0, "bilia_exp_push_top", "data operation not allowed");
    return NULL;
  }
  default:
  {
    //this is an error, no translation is possible
    bilia_error (0, "bilia_exp_push_top", "space operation not allowed");
    return NULL;
  }
  }
}

void
bilia_exp_push (bilia_context_t * ctx, bilia_exp_t * e)
{
  /*Create a new Dform for each Assert()*/
  if (!e)
    return;
  bilia_dform_array *form = bilia_entl_get_nform();
  bilia_dform_array_push(form,bilia_exp_push_top (ctx, e));
  return;
}
