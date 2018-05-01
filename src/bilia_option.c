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

#include "bilia_option.h"
#include <string.h>


/* ====================================================================== */
/* Verbosity. */
/* ====================================================================== */


int verbosity_level = 0;

void
bilia_option_set_verb (int level)
{
  verbosity_level = (level > 0) ? level : 0;
}

int
bilia_option_get_verb (void)
{
  return verbosity_level;
}

int uinterpreted = 0;

void
bilia_option_set_uinterpreted ()
{
  uinterpreted = 1;
}

int
bilia_option_get_uinterpreted (void)
{
  return uinterpreted;
}

int nr = 0;

void
bilia_option_set_nr ()
{
  nr = 1;
}

int
bilia_option_get_nr (void)
{
  return nr;
}

int obag = 0;

void
bilia_option_set_optim_bag ()
{
  obag = 1;
}

int
bilia_option_get_optim_bag ()
{
  return obag;
}

int oele = 0;

void
bilia_option_set_optim_ele ()
{
  oele = 1;
}

int
bilia_option_get_optim_ele ()
{
  return oele;
}

int cvc4 = 0;

void
bilia_option_set_cvc4 ()
{
  cvc4 = 1;
}

int
bilia_option_get_cvc4 ()
{
  return cvc4;
}


/* ====================================================================== */
/* Set/Print. */
/* ====================================================================== */

int
bilia_option_set (char *option)
{
   if (strcmp (option, "-v") == 0)
    {
      bilia_option_set_verb (1); /* verbosity level */
      return 1;
    }
  if (strcmp (option, "-u") == 0)
    {
      bilia_option_set_uinterpreted ();
      return 1;
    }
  if (strcmp (option, "-b") == 0)
    {
      bilia_option_set_optim_bag ();
      return 1;
    }
  if (strcmp (option, "-nr") == 0)
    {
      bilia_option_set_nr ();
      return 1;
    }
  if (strcmp (option, "-ele") == 0)
    {
      bilia_option_set_optim_ele ();
      return 1;
    }
  if (strcmp (option, "-cvc4") == 0)
    {
      bilia_option_set_cvc4 ();
      return 1;
    }
  if (option != NULL && option[0] == '-')
    {
      printf ("Unknown option: %s! ignore.\n", option);
      return -1;
    }
  return 0;
}


void
bilia_option_print (FILE * f)
{

  fprintf (f, "Options:\n");
  fprintf (f, "  -v     verbose messages\n");
  fprintf (f, "  -b     bag optimisation\n");
  fprintf (f, "  -ele   element optimisation\n");
  fprintf (f, "  -nr   n-aire optimisation\n");
  fprintf (f, "  -u    QFUFLIA logic\n");
  fprintf (f, "  -cvc4  CVC4 parser\n");
}
