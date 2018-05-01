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

/**
 * Options for the decision procedure.
 */

#ifndef BILIA_OPTION_H_
#define BILIA_OPTION_H_

#include <stdbool.h>
#include <stdio.h>

/* ====================================================================== */
/* Getting/Setting/resetting options */
/* ====================================================================== */


/**
 * @brief Trigger verbosity level.
 *
 * Default is 0 (i.e., no message).
 */
void bilia_option_set_uinterpreted ();
void bilia_option_set_nr ();
void bilia_option_set_verb(int);

void bilia_option_set_optim_bag ();
int bilia_option_get_optim_bag ();
void bilia_option_set_optim_ele ();
int bilia_option_get_optim_ele ();
void bilia_option_set_cvc4 ();
int bilia_option_get_cvc4 ();



/**
 * @brief Level of verbosity.
 */
int bilia_option_get_verb (void);
int bilia_option_get_uinterpreted();
int bilia_option_get_nr();

/**
 * @brief Set option using the input string of the form '-'optioncode.
 */
int bilia_option_set (char *option);

/**
 * @brief Print options.
 */
void bilia_option_print (FILE * f);



#endif /* BILIA_OPTION_H_ */
