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

#include <stdlib.h>
#include <string.h>
#include "smtlib2bilia.h"

/**
 * Mapping of types from smtlib2abstractparser (file smtlib2types.h)
 * to types from BILIA parser (files bilia.h and bilia_types.h):
 * - smtlib2_term (void*)  to     bilia_exp_t*
 */

/* =========================================================================
 * Forward declaration of
 * functions overriding the abstract parser functions.
 * ========================================================================= */

/* Commands */

static void smtlib2_bilia_parser_set_logic (smtlib2_parser_interface * p,
                                           const char *logic);
static void smtlib2_bilia_parser_declare_function (smtlib2_parser_interface *
                                                  p, const char *name,
                                                  smtlib2_sort sort);
static void smtlib2_bilia_parser_assert_formula (smtlib2_parser_interface * p,
                                                smtlib2_term term);
static void smtlib2_bilia_parser_check_sat (smtlib2_parser_interface * p);

/* Sorts */

static smtlib2_sort smtlib2_bilia_parser_make_sort (smtlib2_parser_interface *
                                                   p, const char *sortname,
                                                   smtlib2_vector * index);
/* Terms */

static void smtlib2_bilia_parser_declare_variable (smtlib2_parser_interface *
                                                  parser, const char *name,
                                                  smtlib2_sort sort);

static smtlib2_term smtlib2_bilia_parser_mk_function (smtlib2_context ctx,
                                                     const char *symbol,
                                                     smtlib2_sort sort,
                                                     smtlib2_vector * index,
                                                     smtlib2_vector * args);
static smtlib2_term smtlib2_bilia_parser_mk_number (smtlib2_context ctx,
                                                   const char *rep,
                                                   unsigned int width,
                                                   unsigned int base);
#define SMTLIB2_BILIA_DECLHANDLER(name)				      \
    static smtlib2_term smtlib2_bilia_parser_mk_ ## name (              \
        smtlib2_context ctx,                                            \
        const char *symbol,                                             \
        smtlib2_sort sort,                                              \
        smtlib2_vector *idx,                                            \
        smtlib2_vector *args)
SMTLIB2_BILIA_DECLHANDLER (and);
SMTLIB2_BILIA_DECLHANDLER (or);
SMTLIB2_BILIA_DECLHANDLER (not);
SMTLIB2_BILIA_DECLHANDLER (implies);
SMTLIB2_BILIA_DECLHANDLER (eq);
SMTLIB2_BILIA_DECLHANDLER (distinct);
SMTLIB2_BILIA_DECLHANDLER (ite);
SMTLIB2_BILIA_DECLHANDLER (lt);
SMTLIB2_BILIA_DECLHANDLER (gt);
SMTLIB2_BILIA_DECLHANDLER (le);
SMTLIB2_BILIA_DECLHANDLER (ge);
SMTLIB2_BILIA_DECLHANDLER (plus);
SMTLIB2_BILIA_DECLHANDLER (minus);
SMTLIB2_BILIA_DECLHANDLER (bag);
SMTLIB2_BILIA_DECLHANDLER (bagn);
SMTLIB2_BILIA_DECLHANDLER (bagunion);
SMTLIB2_BILIA_DECLHANDLER (bagminus);
SMTLIB2_BILIA_DECLHANDLER (baginter);
SMTLIB2_BILIA_DECLHANDLER (bagsum);
SMTLIB2_BILIA_DECLHANDLER (nsubseteq);
SMTLIB2_BILIA_DECLHANDLER (subseteq);
SMTLIB2_BILIA_DECLHANDLER (in);
SMTLIB2_BILIA_DECLHANDLER (ni);
SMTLIB2_BILIA_DECLHANDLER (inn);
SMTLIB2_BILIA_DECLHANDLER (nin);
SMTLIB2_BILIA_DECLHANDLER (max);
SMTLIB2_BILIA_DECLHANDLER (min);
SMTLIB2_BILIA_DECLHANDLER (bagmax);
SMTLIB2_BILIA_DECLHANDLER (bagmin);

#define SMTLIB2_BILIA_SETHANDLER(tp, s, name) \
    smtlib2_term_parser_set_handler(tp, s, smtlib2_bilia_parser_mk_ ## name)

/* =========================================================================
 * BILIA parser creation/destruction.
 * ========================================================================= */

#define bilia_ctx(p) (((smtlib2_bilia_parser *)(p))->ctx_)
#define bilia_sorts(p) (((smtlib2_bilia_parser *)(p))->sorts_)
#define bilia_funs(p) (((smtlib2_bilia_parser *)(p))->funs_)

smtlib2_bilia_parser *
smtlib2_bilia_parser_new (void)
{
  smtlib2_bilia_parser *ret =
    (smtlib2_bilia_parser *) malloc (sizeof (smtlib2_bilia_parser));
  smtlib2_parser_interface *pi;
  smtlib2_term_parser *tp;
  smtlib2_abstract_parser *ap;

  ret->ctx_ = bilia_mk_context ();
  smtlib2_abstract_parser_init ((smtlib2_abstract_parser *) ret,
                                (smtlib2_context) ret);
  ret->sorts_ =
    smtlib2_hashtable_new (smtlib2_hashfun_str, smtlib2_eqfun_str);
  ret->funs_ = smtlib2_hashtable_new (smtlib2_hashfun_str, smtlib2_eqfun_str);

  /* initialize the parser interface and override virtual methods */
  pi = SMTLIB2_PARSER_INTERFACE (ret);
  /* Commands */
  pi->set_logic = smtlib2_bilia_parser_set_logic;
  pi->declare_function = smtlib2_bilia_parser_declare_function;
  pi->assert_formula = smtlib2_bilia_parser_assert_formula;
  pi->check_sat = smtlib2_bilia_parser_check_sat;
  /* Terms */
  pi->declare_variable = smtlib2_bilia_parser_declare_variable;
  /* Sorts */
  pi->make_sort = smtlib2_bilia_parser_make_sort;
  /* Term parser */
  tp = ((smtlib2_abstract_parser *) ret)->termparser_;
  /* for symbols and user-defined function application */
  smtlib2_term_parser_set_function_handler (tp,
                                            smtlib2_bilia_parser_mk_function);
  smtlib2_term_parser_set_number_handler (tp, smtlib2_bilia_parser_mk_number);

  /* for logic pre-defined operators */
  SMTLIB2_BILIA_SETHANDLER (tp, "or", or);
  SMTLIB2_BILIA_SETHANDLER (tp, "and", and);
  SMTLIB2_BILIA_SETHANDLER (tp, "not", not);
  SMTLIB2_BILIA_SETHANDLER (tp, "=>", implies);
  SMTLIB2_BILIA_SETHANDLER (tp, "=", eq);
  SMTLIB2_BILIA_SETHANDLER (tp, "distinct", distinct);
  SMTLIB2_BILIA_SETHANDLER (tp, "<>", distinct);
  SMTLIB2_BILIA_SETHANDLER (tp, "ite", ite);
  SMTLIB2_BILIA_SETHANDLER (tp, "<", lt);
  SMTLIB2_BILIA_SETHANDLER (tp, ">", gt);
  SMTLIB2_BILIA_SETHANDLER (tp, "<=", le);
  SMTLIB2_BILIA_SETHANDLER (tp, ">=", ge);
  SMTLIB2_BILIA_SETHANDLER (tp, "+", plus);
  SMTLIB2_BILIA_SETHANDLER (tp, "-", minus);
  SMTLIB2_BILIA_SETHANDLER (tp, "bag", bag);
  SMTLIB2_BILIA_SETHANDLER (tp, "bagn", bagn);
  /* SMTLIB2_BILIA_SETHANDLER (tp, "emptybag", emptybag); */
  SMTLIB2_BILIA_SETHANDLER (tp, "bagunion", bagunion);
  SMTLIB2_BILIA_SETHANDLER (tp, "bagminus", bagminus);
  SMTLIB2_BILIA_SETHANDLER (tp, "baginter", baginter);
  SMTLIB2_BILIA_SETHANDLER (tp, "bagsum", bagsum);
  /* SMTLIB2_BILIA_SETHANDLER (tp, "subset", subset);  */
  SMTLIB2_BILIA_SETHANDLER (tp, "subset", subseteq);
  SMTLIB2_BILIA_SETHANDLER (tp, "nsubset", nsubseteq);
  SMTLIB2_BILIA_SETHANDLER (tp, "in", in);
  SMTLIB2_BILIA_SETHANDLER (tp, "inn", inn);
  SMTLIB2_BILIA_SETHANDLER (tp, "ni", ni);
  SMTLIB2_BILIA_SETHANDLER (tp, "nin", nin);
  SMTLIB2_BILIA_SETHANDLER (tp,"max",max);
  SMTLIB2_BILIA_SETHANDLER (tp,"min",min);
  SMTLIB2_BILIA_SETHANDLER (tp,"bagmax",bagmax);
  SMTLIB2_BILIA_SETHANDLER (tp,"bagmin",bagmin);

  /* Initialize the logic basic sorts */
  extern bilia_type_t * tybool;
  tybool = bilia_mk_type_bool ();
  smtlib2_hashtable_set (ret->sorts_,
                         (intptr_t) (void *) smtlib2_strdup ("Bool"),
                         (intptr_t) (void *) tybool);
  /* set options in the abstract parser */
  ap = (smtlib2_abstract_parser *) pi;
  ap->print_success_ = false;

  return ret;
}

void
smtlib2_bilia_parser_delete (smtlib2_bilia_parser * p)
{
  smtlib2_hashtable_delete (bilia_sorts (p), NULL, NULL);
  smtlib2_hashtable_delete (bilia_funs (p), NULL, NULL);
  smtlib2_abstract_parser_deinit (&(p->parent_));
  bilia_del_context (bilia_ctx (p));
  free (p);
}

/* =========================================================================
 * Commands parsing.
 * ========================================================================= */

/**
 * Command (set-logic logic)
 * Logics supported:
 * QF_BILIA : theory of ESOP'13 paper
 * QF_SLL  : theory of APLAS'14 paper
 * QF_SLRD : theory of SL-COMP'14
 * QF_SLRDI: theory with integer data
 */
static void
smtlib2_bilia_parser_set_logic (smtlib2_parser_interface * p,
                               const char *logic)
{
  smtlib2_abstract_parser *ap = (smtlib2_abstract_parser *) p;

  /* fix logic only one time */
  if (ap->response_ != SMTLIB2_RESPONSE_ERROR)
    {
      /* check that the logic is supported */

      switch (bilia_set_logic (bilia_ctx (p), logic))
        {
        case BILIA_LOGIC_OTHER:
          {
            ap->response_ = SMTLIB2_RESPONSE_ERROR;
            ap->errmsg_ =
              smtlib2_sprintf ("logic `%s' is not supported", logic);
            break;
          }
        case BILIA_LOGIC_SLRDI:
          {
            /* declare theory primitive sorts Int and BagInt */
            extern bilia_type_t *tyint;
            tyint = bilia_mk_type_int ();
            smtlib2_hashtable_set (bilia_sorts (p),
                                   (intptr_t) (void *) smtlib2_strdup ("Int"),
                                   (intptr_t) (void *) tyint);
            extern bilia_type_t *tybag;
            tybag = bilia_mk_type_bagint ();
            smtlib2_hashtable_set (bilia_sorts (p),
                                   (intptr_t) (void *)
                                   smtlib2_strdup ("BagInt"),
                                   (intptr_t) (void *) tybag);
            // break; // TODO NEW: put break because SetLoc is not useful in SLRDI
          }
        default:
          {
            /* declare theory primitive sort SetLoc */
              break;
          }
        }
    }
  else
    bilia_error (0, "smtlib2parser_set_logic", "previous syntax error");
}


/** Command (declare-fun name args res)
 *  Used to declare BILIA fields and variables (location or set of locations).
 *  The args shall be empty.
 * @param name the name of the function declared
 * @param sort the res sort,
 *             already checked that args = () @see make_function_sort
 *
 */
static void
smtlib2_bilia_parser_declare_function (smtlib2_parser_interface * p,
                                      const char *name, smtlib2_sort sort)
{
  smtlib2_bilia_parser *sp = (smtlib2_bilia_parser *) p;
  smtlib2_abstract_parser *ap = (smtlib2_abstract_parser *) p;

  if (ap->response_ != SMTLIB2_RESPONSE_ERROR)
    {
      /* check that it is not already defined */
      intptr_t k;
      if (smtlib2_hashtable_find (bilia_funs (p), (intptr_t) name, &k))
        {
          ap->response_ = SMTLIB2_RESPONSE_ERROR;
          ap->errmsg_ = smtlib2_sprintf ("function `%s' already declared",
                                         name);
        }
      else
        {
          /* check that the function is supported */
          bilia_type_t *ty =
            bilia_mk_fun_decl (bilia_ctx (p), name, (bilia_type_t *) sort);
          if (ty != NULL)
            {
              ap->response_ = SMTLIB2_RESPONSE_SUCCESS;
              smtlib2_hashtable_set (sp->funs_,
                                     (intptr_t) (void *)
                                     smtlib2_strdup (name),
                                     (intptr_t) (void *) ty);
            }
          else
            {
              ap->response_ = SMTLIB2_RESPONSE_ERROR;
              ap->errmsg_ =
                smtlib2_sprintf ("function declaration `%s' not supported",
                                 name);
            }
        }
    }
  else
    bilia_error (0, "smtlib2parser_declare_fun", "previous syntax error");
}

/** Term (exists ...), (forall ...), (let ...), and
 *  function definition.
 *  Used to declare local variables.
 *  The variable declared is pushed in the local context.
 *
 *  @param name variable name; it shall start with ?
 *  @param sort variable type; it shall be a record
 */
static void
smtlib2_bilia_parser_declare_variable (smtlib2_parser_interface * p,
                                      const char *name, smtlib2_sort sort)
{
  smtlib2_abstract_parser *ap = (smtlib2_abstract_parser *) p;

  if (ap->response_ != SMTLIB2_RESPONSE_ERROR)
    {
      bilia_type_t *ty = (bilia_type_t *) sort;
      if (ty != NULL)
        {
          // variable declaration
          // check that this name starts with  ?
          if (name[0] != '?')
            {
              ap->response_ = SMTLIB2_RESPONSE_ERROR;
              ap->errmsg_ =
                smtlib2_sprintf ("local variable `%s' is not a constant.",
                                 name);
            }
          else if (bilia_type_is_vartype (ty) == false)
            {
              ap->response_ = SMTLIB2_RESPONSE_ERROR;
              ap->errmsg_ =
                smtlib2_sprintf
                ("local variable `%s' is not of primitive sort.", name);
            }
          else
            {
              bilia_push_var (bilia_ctx (p), name, ty);
            }
        }
    }
  else
    bilia_error (0, "smtlib2parser_declare_var", "previous syntax error");
}


static void
smtlib2_bilia_parser_assert_formula (smtlib2_parser_interface * p,
                                    smtlib2_term term)
{
  smtlib2_abstract_parser *ap = (smtlib2_abstract_parser *) p;

  if (ap->response_ != SMTLIB2_RESPONSE_ERROR)
    {
      /* bilia_entl_fprint(stdout); */
      // check also that assert has a good formula
      if (bilia_assert (bilia_ctx (p), (bilia_exp_t *) term) == false)
        {
          ap->response_ = SMTLIB2_RESPONSE_ERROR;
          ap->errmsg_ = smtlib2_strdup ("assert not a BILIA formula");
        }
    }
  else
    bilia_error (0, "smtlib2parser_assert", "previous syntax error");
}

static void
smtlib2_bilia_parser_check_sat (smtlib2_parser_interface * p)
{
  smtlib2_abstract_parser *ap = (smtlib2_abstract_parser *) p;

  if (ap->response_ != SMTLIB2_RESPONSE_ERROR)
    {
      int s = bilia_check (bilia_ctx (p));
      // returns status of phi1 /\ not(phi2)
      ap->response_ = SMTLIB2_RESPONSE_STATUS;
      switch (s)
        {
        case 1:
          ap->status_ = SMTLIB2_STATUS_SAT;
          break;
        case 0:
          ap->status_ = SMTLIB2_STATUS_UNSAT;
          break;
        default:
          ap->status_ = SMTLIB2_STATUS_UNKNOWN;
          break;
        }
      ap->status_ = SMTLIB2_STATUS_UNKNOWN;
    }
  else
    bilia_error (0, "smtlib2parser_check_sat", "previous syntax error");
}

/* =========================================================================
 * Sorts parsing.
 * ========================================================================= */

/** Called for a sort use.
 */
static smtlib2_sort
smtlib2_bilia_parser_make_sort (smtlib2_parser_interface * p,
                               const char *sortname, smtlib2_vector * index)
{
  smtlib2_abstract_parser *ap = (smtlib2_abstract_parser *) p;

  bilia_type_t *ret = NULL;

  if (ap->response_ != SMTLIB2_RESPONSE_ERROR)
    {
      intptr_t k;
      /* check that the sort is already declared */
      if (!smtlib2_hashtable_find (bilia_sorts (p), (intptr_t) sortname, &k))
        {
          ap->response_ = SMTLIB2_RESPONSE_ERROR;
          ap->errmsg_ = smtlib2_sprintf ("sort `%s' not declared", sortname);
        }
      else if (index != NULL)
        {
          ap->response_ = SMTLIB2_RESPONSE_ERROR;
          ap->errmsg_ = smtlib2_sprintf ("unknown sort `%s' with index != 0",
                                         sortname);
        }
      else
        ret = (bilia_type_t *) k;
    }
  else
    bilia_error (0, "smtlib2parser_make_sort", "previous syntax error");
  return (smtlib2_sort) ret;
}

/** Used for function application term (symbol args).
 *
 * @param ctx    context of call
 * @param symbol name of the function
 * @param sort   type annotation NOT SUPPORTED
 * @param index  index annotation NOT SUPPORTED
 * @param args   arguments of call
 * @return term built with these arguments
 */
static smtlib2_term
smtlib2_bilia_parser_mk_function (smtlib2_context ctx,
                                 const char *symbol, smtlib2_sort sort,
                                 smtlib2_vector * index,
                                 smtlib2_vector * args)
{
  if (&sort != &sort)
    {
      assert (false);
    }

  bilia_context_t *sctx = bilia_ctx (ctx);
  bilia_exp_t *res = NULL;
  if (index)
    // indexed terms or as terms not supported
    return (smtlib2_term) NULL;
  if (NULL != args)
    // n-ary functions
    res = bilia_mk_app (sctx, symbol,
                       (bilia_exp_t **) (smtlib2_vector_array (args)),
                       smtlib2_vector_size (args));
  else
    // constant, variable or quantified variable
    res = bilia_mk_app (sctx, symbol, NULL, 0);
  // no way to return a message using the context
  return (smtlib2_term) res;
}

static smtlib2_term
smtlib2_bilia_parser_mk_number (smtlib2_context ctx,
                               const char *rep,
                               unsigned int width, unsigned int base)
{
  bilia_context_t *sctx = bilia_ctx (ctx);
  bilia_exp_t *ret = NULL;
  if ((width == 0) && (base == 10))
    {
      // parse only only base 10 numbers, not arrays
      ret = bilia_mk_number (sctx, rep);
    }
  return (smtlib2_term) ret;
}

SMTLIB2_BILIA_DECLHANDLER (and)
{
  if (symbol != symbol && sort != sort && idx != idx)
    {
      assert (0);               // to remove warnings in unsed parameters
    }
  if (args == NULL)
    return (smtlib2_term) NULL;
  return bilia_mk_and (bilia_ctx (ctx),
                      (bilia_exp_t **) (smtlib2_vector_array (args)),
                      smtlib2_vector_size (args));
}

SMTLIB2_BILIA_DECLHANDLER (or)
{
  if (symbol != symbol && sort != sort && idx != idx)
    {
      assert (0);               // to remove warnings in unsed parameters
    }
  if (args == NULL)
    return (smtlib2_term) NULL;
  return bilia_mk_or (bilia_ctx (ctx),
                      (bilia_exp_t **) (smtlib2_vector_array (args)),
                      smtlib2_vector_size (args));
}

SMTLIB2_BILIA_DECLHANDLER (not)
{
  if (symbol != symbol && sort != sort && idx != idx)
    {
      assert (0);               // to remove warnings in unsed parameters
    }
  if (args == NULL)
    return (smtlib2_term) NULL;
  return bilia_mk_not (bilia_ctx (ctx),
                      (bilia_exp_t **) (smtlib2_vector_array (args)),
                      smtlib2_vector_size (args));
}

SMTLIB2_BILIA_DECLHANDLER (implies)
{
  if (symbol != symbol && sort != sort && idx != idx)
    {
      assert (0);               // to remove warnings in unsed parameters
    }
  if (args == NULL)
    return (smtlib2_term) NULL;
  return bilia_mk_implies (bilia_ctx (ctx),
                          (bilia_exp_t **) (smtlib2_vector_array (args)),
                          smtlib2_vector_size (args));
}

SMTLIB2_BILIA_DECLHANDLER (eq)
{
  if (symbol != symbol && sort != sort && idx != idx)
    {
      assert (0);               // to remove warnings in unsed parameters
    }
  if (args == NULL)
    return (smtlib2_term) NULL;
  return bilia_mk_eq (bilia_ctx (ctx),
                     (bilia_exp_t **) (smtlib2_vector_array (args)),
                     smtlib2_vector_size (args));
}

SMTLIB2_BILIA_DECLHANDLER (distinct)
{
  if (symbol != symbol && sort != sort && idx != idx)
    {
      assert (0);               // to remove warnings in unsed parameters
    }
  if (args == NULL)
    return (smtlib2_term) NULL;
  return bilia_mk_distinct (bilia_ctx (ctx),
                           (bilia_exp_t **) (smtlib2_vector_array (args)),
                           smtlib2_vector_size (args));
}

SMTLIB2_BILIA_DECLHANDLER (ite)
{
  if (symbol != symbol && sort != sort && idx != idx)
    {
      assert (0);               // to remove warnings in unsed parameters
    }
  if (args == NULL)
    return (smtlib2_term) NULL;
  return bilia_mk_ite (bilia_ctx (ctx),
                      (bilia_exp_t **) (smtlib2_vector_array (args)),
                      smtlib2_vector_size (args));
}

SMTLIB2_BILIA_DECLHANDLER (lt)
{
  if (symbol != symbol && sort != sort && idx != idx)
    {
      assert (0);               // to remove warnings in unsed parameters
    }
  if (args == NULL)
    return (smtlib2_term) NULL;
  return bilia_mk_lt (bilia_ctx (ctx),
                     (bilia_exp_t **) (smtlib2_vector_array (args)),
                     smtlib2_vector_size (args));
}

SMTLIB2_BILIA_DECLHANDLER (gt)
{
  if (symbol != symbol && sort != sort && idx != idx)
    {
      assert (0);               // to remove warnings in unsed parameters
    }
  if (args == NULL)
    return (smtlib2_term) NULL;
  return bilia_mk_gt (bilia_ctx (ctx),
                     (bilia_exp_t **) (smtlib2_vector_array (args)),
                     smtlib2_vector_size (args));
}

SMTLIB2_BILIA_DECLHANDLER (le)
{
  if (symbol != symbol && sort != sort && idx != idx)
    {
      assert (0);               // to remove warnings in unsed parameters
    }
  if (args == NULL)
    return (smtlib2_term) NULL;
  return bilia_mk_le (bilia_ctx (ctx),
                     (bilia_exp_t **) (smtlib2_vector_array (args)),
                     smtlib2_vector_size (args));
}

SMTLIB2_BILIA_DECLHANDLER (ge)
{
  if (symbol != symbol && sort != sort && idx != idx)
    {
      assert (0);               // to remove warnings in unsed parameters
    }
  if (args == NULL)
    return (smtlib2_term) NULL;
  return bilia_mk_ge (bilia_ctx (ctx),
                     (bilia_exp_t **) (smtlib2_vector_array (args)),
                     smtlib2_vector_size (args));
}

SMTLIB2_BILIA_DECLHANDLER (plus)
{
  if (symbol != symbol && sort != sort && idx != idx)
    {
      assert (0);               // to remove warnings in unsed parameters
    }
  if (args == NULL)
    return (smtlib2_term) NULL;
  return bilia_mk_plus (bilia_ctx (ctx),
                       (bilia_exp_t **) (smtlib2_vector_array (args)),
                       smtlib2_vector_size (args));
}

SMTLIB2_BILIA_DECLHANDLER (minus)
{
  if (symbol != symbol && sort != sort && idx != idx)
    {
      assert (0);               // to remove warnings in unsed parameters
    }
  if (args == NULL)
    return (smtlib2_term) NULL;
  return bilia_mk_minus (bilia_ctx (ctx),
                        (bilia_exp_t **) (smtlib2_vector_array (args)),
                        smtlib2_vector_size (args));
}

SMTLIB2_BILIA_DECLHANDLER (bag)
{
  if (symbol != symbol && sort != sort && idx != idx)
    {
      assert (0);               // to remove warnings in unsed parameters
    }
  if (args == NULL)
    return (smtlib2_term) NULL;
  return bilia_mk_bag (bilia_ctx (ctx),
                      (bilia_exp_t **) (smtlib2_vector_array (args)),
                      smtlib2_vector_size (args));
}

SMTLIB2_BILIA_DECLHANDLER (bagn)
{
  if (symbol != symbol && sort != sort && idx != idx)
    {
      assert (0);               // to remove warnings in unsed parameters
    }
  if (args == NULL)
    return (smtlib2_term) NULL;
  return bilia_mk_bagn (bilia_ctx (ctx),
                      (bilia_exp_t **) (smtlib2_vector_array (args)),
                      smtlib2_vector_size (args));
}


SMTLIB2_BILIA_DECLHANDLER (bagunion)
{
  if (symbol != symbol && sort != sort && idx != idx)
    {
      assert (0);               // to remove warnings in unsed parameters
    }
  if (args == NULL)
    return (smtlib2_term) NULL;
  return bilia_mk_bagunion (bilia_ctx (ctx),
                           (bilia_exp_t **) (smtlib2_vector_array (args)),
                           smtlib2_vector_size (args));
}

SMTLIB2_BILIA_DECLHANDLER (bagminus)
{
  if (symbol != symbol && sort != sort && idx != idx)
    {
      assert (0);               // to remove warnings in unsed parameters
    }
  if (args == NULL)
    return (smtlib2_term) NULL;
  return bilia_mk_bagminus (bilia_ctx (ctx),
                           (bilia_exp_t **) (smtlib2_vector_array (args)),
                           smtlib2_vector_size (args));
}

SMTLIB2_BILIA_DECLHANDLER (bagsum)
{
  if (symbol != symbol && sort != sort && idx != idx)
    {
      assert (0);               // to remove warnings in unsed parameters
    }
  if (args == NULL)
    return (smtlib2_term) NULL;
  return bilia_mk_bagsum (bilia_ctx (ctx),
                           (bilia_exp_t **) (smtlib2_vector_array (args)),
                           smtlib2_vector_size (args));
}

SMTLIB2_BILIA_DECLHANDLER (baginter)
{
  if (symbol != symbol && sort != sort && idx != idx)
    {
      assert (0);               // to remove warnings in unsed parameters
    }
  if (args == NULL)
    return (smtlib2_term) NULL;
  return bilia_mk_baginter (bilia_ctx (ctx),
                           (bilia_exp_t **) (smtlib2_vector_array (args)),
                           smtlib2_vector_size (args));
}

SMTLIB2_BILIA_DECLHANDLER (subseteq)
{
  if (symbol != symbol && sort != sort && idx != idx)
    {
      assert (0);               // to remove warnings in unsed parameters
    }
  if (args == NULL)
    return (smtlib2_term) NULL;
  return bilia_mk_subseteq (bilia_ctx (ctx),
                         (bilia_exp_t **) (smtlib2_vector_array (args)),
                         smtlib2_vector_size (args));
}

SMTLIB2_BILIA_DECLHANDLER (nsubseteq)
{
  if (symbol != symbol && sort != sort && idx != idx)
    {
      assert (0);               // to remove warnings in unsed parameters
    }
  if (args == NULL)
    return (smtlib2_term) NULL;
  return bilia_mk_nsubseteq (bilia_ctx (ctx),
                         (bilia_exp_t **) (smtlib2_vector_array (args)),
                         smtlib2_vector_size (args));
}

SMTLIB2_BILIA_DECLHANDLER (inn)
{
  if (symbol != symbol && sort != sort && idx != idx)
    {
      assert (0);               // to remove warnings in unsed parameters
    }
  if (args == NULL)
    return (smtlib2_term) NULL;
  return bilia_mk_inn (bilia_ctx (ctx),
                         (bilia_exp_t **) (smtlib2_vector_array (args)),
                         smtlib2_vector_size (args));
}

SMTLIB2_BILIA_DECLHANDLER (in)
{
  if (symbol != symbol && sort != sort && idx != idx)
    {
      assert (0);               // to remove warnings in unsed parameters
    }
  if (args == NULL)
    return (smtlib2_term) NULL;
  return bilia_mk_in (bilia_ctx (ctx),
                         (bilia_exp_t **) (smtlib2_vector_array (args)),
                         smtlib2_vector_size (args));
}

SMTLIB2_BILIA_DECLHANDLER (nin)
{
  if (symbol != symbol && sort != sort && idx != idx)
    {
      assert (0);               // to remove warnings in unsed parameters
    }
  if (args == NULL)
    return (smtlib2_term) NULL;
  return bilia_mk_nin (bilia_ctx (ctx),
                         (bilia_exp_t **) (smtlib2_vector_array (args)),
                         smtlib2_vector_size (args));
}

SMTLIB2_BILIA_DECLHANDLER (ni)
{
  if (symbol != symbol && sort != sort && idx != idx)
    {
      assert (0);               // to remove warnings in unsed parameters
    }
  if (args == NULL)
    return (smtlib2_term) NULL;
  return bilia_mk_ni (bilia_ctx (ctx),
                         (bilia_exp_t **) (smtlib2_vector_array (args)),
                         smtlib2_vector_size (args));
}

SMTLIB2_BILIA_DECLHANDLER (max)
{
  if (symbol != symbol && sort != sort && idx != idx)
    {
      assert (0);               // to remove warnings in unsed parameters
    }
  if (args == NULL)
    return (smtlib2_term) NULL;
  return bilia_mk_max (bilia_ctx (ctx),
                         (bilia_exp_t **) (smtlib2_vector_array (args)),
                         smtlib2_vector_size (args));
}
SMTLIB2_BILIA_DECLHANDLER (min)
{
  if (symbol != symbol && sort != sort && idx != idx)
    {
      assert (0);               // to remove warnings in unsed parameters
    }
  if (args == NULL)
    return (smtlib2_term) NULL;
  return bilia_mk_min (bilia_ctx (ctx),
                         (bilia_exp_t **) (smtlib2_vector_array (args)),
                         smtlib2_vector_size (args));
}
SMTLIB2_BILIA_DECLHANDLER (bagmax)
{
  if (symbol != symbol && sort != sort && idx != idx)
    {
      assert (0);               // to remove warnings in unsed parameters
    }
  if (args == NULL)
    return (smtlib2_term) NULL;
  return bilia_mk_bagmax (bilia_ctx (ctx),
                         (bilia_exp_t **) (smtlib2_vector_array (args)),
                         smtlib2_vector_size (args));
}
SMTLIB2_BILIA_DECLHANDLER (bagmin)
{
  if (symbol != symbol && sort != sort && idx != idx)
    {
      assert (0);               // to remove warnings in unsed parameters
    }
  if (args == NULL)
    return (smtlib2_term) NULL;
  return bilia_mk_bagmin (bilia_ctx (ctx),
                         (bilia_exp_t **) (smtlib2_vector_array (args)),
                         smtlib2_vector_size (args));
}
