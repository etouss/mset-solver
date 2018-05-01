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
 * Defines the problem for the decision procedure.
 */

#ifndef BILIA_ENTL_H_
#define BILIA_ENTL_H_

#include <stdio.h>
#include "bilia_form.h"
// #include "bilia2sat.h"
#include "bilia.h"
#include "bilia_option.h"
#include "bilia_form.h"



/* ====================================================================== */
/* Datatypes */
/* ====================================================================== */

/** Entailment problems
 *          phi ==> (\/_j psi_j)    valid
 *  read from the input file in the form
 *          phi /\ ! (\/_j psi_j)   unsat
 *  more precisely in SMTLIB2 format
 *          assert (phi)
 *          ;; for all j
 *          assert (not (psi_j))
 *          check-sat
 *
 *  Additional informations for this entailment solving are
 *  stored as:
 *  - boolean abstract of positive/negative formulae
 *  - abstract graphs of positive/negative formulae
 */

typedef struct bilia_entl_t
{
  char *smt_fname;              // smt file with entailment
  char *output_fname;           // output file with proof
  bilia_logic_t logic;           // theory used in formulas
  bilia_var_array *lvars;        // local variables V_{int}
  bilia_var_array *wvars;         //w_a,x variable
  bilia_dform_array *nform;           // array of asserted formula
  bilia_dform_array *rform;           // array of rewritting constraint
  bilia_dform_array *wform;           // array of constraint over w_a,x
  bilia_form_kind_t cmd;         // command given: check-sat (default),
  //                check-unsat
} bilia_entl_t;

#include "bilia_sat.h"


/* ====================================================================== */
/* Globals */
/* ====================================================================== */

extern bilia_entl_t *bilia_prob;  // problem of entailment in bilia

/* ====================================================================== */
/* Constructors/destructors */
/* ====================================================================== */

/* Initialization/Deallocation of formulas */
void bilia_entl_init (void);
void bilia_entl_free (void);

/* ====================================================================== */
/* Getters */
/* ====================================================================== */

//bilia_form_t *bilia_entl_get_pform (void);
/* Get positive formula */
// bilia_form_t *bilia_entl_get_nform_last (void);
/* Get last negative formulae */
bilia_dform_t *bilia_entl_get_nform_last (void);
bilia_dform_array * bilia_entl_get_nform ();
bilia_dform_array *bilia_entl_get_rform (void);
bilia_dform_array *bilia_entl_get_wform (void);
bilia_entl_t *bilia_entl_get (void);
bilia_var_array *bilia_entl_get_var_array(void);
bilia_var_array *bilia_entl_get_wvar_array(void);
/* Get all the set of negative formulae */

/* ====================================================================== */
/* Setters */
/* ====================================================================== */

void bilia_entl_set_fname (char *fname);
/* Set source file information */
void bilia_entl_set_foutput (char *fname);
/* Set output file information */
void bilia_entl_set_cmd (bilia_form_kind_t pb);

/* ====================================================================== */
/* Predicates */
/* ====================================================================== */

int bilia_entl_is_sat (void);
/* Test if it is a satisfiability problem, i.e., empty negative formulas  */

/* ====================================================================== */
/* Printers */
/* ====================================================================== */

void bilia_entl_fprint (FILE * f);

/* ====================================================================== */
/* Solver */
/* ====================================================================== */

int bilia_entl_type (void);
/* Type the predicates, fields, formulas in bilia_prob */

int bilia_entl_solve (void);
/* Check the global problem in bilia_prob */

#endif /* BILIA_ENTL_H_ */
