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
 * STEP2.
 */

#ifndef _BILIA_Z3_API_H_
#define	_BILIA_Z3_API_H_

#ifdef	__cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <memory.h>
#include <setjmp.h>
#include <z3.h>
#include "bilia_vars.h"
#include "bilia_types.h"
#include "bilia_form.h"
#include "bilia_entl.h"

//BILIA_VECTOR_DECLARE(Z3_ast_array,Z3_ast);

void bilia_z3_api();

#ifdef	__cplusplus
}
#endif

#endif                          /* _BILIA_STEP2_H_ */
