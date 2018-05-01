/**************************************************************************/
/*                                                                        */
/*  BaTS decision procedure                                               */
/*                                                                        */
/*  you can redistribute it and/or modify it under the terms of the GNU   */
/*  Lesser General Public License as published by the Free Software       */
/*  Foundation, version 3.                                                */
/*                                                                        */
/*  It is distributed in the hope that it will be useful,                 */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of        */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         */
/*  GNU Lesser General Public License for more details.                   */
/*                                                                        */
/*  See the GNU Lesser General Public License version 3.                  */
/*  for more details (enclosed in the file LICENSE).                      */
/*                                                                        */
/**************************************************************************/

#ifndef BILIA_SAT_H_
#define BILIA_SAT_H_

#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>

#include "bilia_vars.h"
#include "bilia_types.h"
#include "bilia_form.h"
// #include "bilia2sat.h"
#include "bilia_rewrite.h"
#include "bilia_step2.h"

/* ====================================================================== */
/* Sat checking and diagnosis */
/* ====================================================================== */

int bilia_sat_solve ();
/* Check satisfiability and print diagnosis if required */

/* ====================================================================== */
/* Utilities */
/* ====================================================================== */

int
time_difference (struct timeval *result, struct timeval *t2,
                 struct timeval *t1);


#endif /* BILIA_SAT_H_ */
