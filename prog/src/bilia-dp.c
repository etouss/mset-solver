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

#include <stdio.h>
#include "smtlib2bilia.h"
#include "bilia_option.h"
#include <libgen.h>

/* ====================================================================== */
/* MAIN/Main/main */
/* ====================================================================== */

/**
 * Print informations on usage.
 */

void
print_help (void)
{
        printf ("bats: decision procedure for ????, version 0.1\n");
        printf ("Usage: bats [options] file [output]\n");
        bilia_option_print (stdout);
        printf ("  file   input file in SMTLIB2 format\n");
        printf ("  output proof of sat/unsat\n");
}

/**
 * Entry of the decision procedure.
 * @requires: only one problem per file
 *
 * Call: bats [-t|-b|-v|-uf] file.smt2
 */
int
main (int argc, char **argv)
{
        // Step 0: Check the arguments
        if (argc <= 1)
        {
                print_help ();
                return 1;
        }
        int arg_file = 1;
        while (arg_file < argc)
        {
                int opt = bilia_option_set (argv[arg_file]);
                if (opt == 1)
                        arg_file++;
                else if (opt == -1)
                {
                        print_help ();
                        return 1;
                }
                else
                        break;
        }
        if (arg_file >= argc)
        {

                printf ("no input file\n");
                print_help ();
                return 1;
        }

        if (bilia_option_get_verb () > 0)
                fprintf (stdout, "bats on file %s\n", argv[arg_file]);

        // Step 1: Parse the file and initialize the problem
        // pre: the file shall exists.
        FILE *f = fopen (argv[arg_file], "r");
        if (!f)
        {
                printf ("File %s not found!\nquit.", argv[arg_file]);
                return 1;
        }
        extern char *file_name;
        file_name = basename(argv[arg_file]);

        // initialize the problem
        bilia_entl_init ();
        bilia_entl_set_fname (argv[arg_file]);
        if ((arg_file + 1) < argc)
                bilia_entl_set_foutput (argv[arg_file + 1]);

        if (bilia_option_get_verb () > 0)
                fprintf (stdout, "  > parse file %s\n", argv[arg_file]);
        // call the parser
        smtlib2_bilia_parser *sp = smtlib2_bilia_parser_new ();
        smtlib2_abstract_parser_parse ((smtlib2_abstract_parser *) sp, f);

        // Step 2: call the solving execute the commands in the file (check-sat)
        // done in (bilia.c) bilia_check
        // also sets the smtlib2 parser result

        // Step 3: finish (free memory, etc.)
        /* bilia_entl_fprint(stdout); */
        free(file_name);
        smtlib2_bilia_parser_delete (sp);
        fclose (f);
        bilia_entl_free ();
        return 0;
}
