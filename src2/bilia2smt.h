

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
 * write smt_file.
 */

#ifndef _BILIA2SMT_H_
#define	_BILIA2SMT_H_

#ifdef	__cplusplus
extern "C"
{
#endif

#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include "bilia_vars.h"
#include "bilia_types.h"
#include "bilia_form.h"
#include "bilia_entl.h"

void bilia2smt();
char * file_name;

#ifdef	__cplusplus
}
#endif

#endif                          /* _BILIA2SMT_H_ */

