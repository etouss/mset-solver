#include <stdlib.h>
#include <stdio.h>
#include <libgen.h>

int main(int argc ,char **argv){

  for(int i=1;i<10;i++){

  //char *file_name = basename(argv[i]);
  printf("%s\n",argv[i]);


  char *command = (char *) malloc (1024 * sizeof (char));
  //Z3
  sprintf (command, "../src2/bats %s",argv[i] );
  system(command);
  wait(NULL);
  //t
  sprintf (command, "../src2/bats -ele %s",argv[i] );
  system(command);
  wait(NULL);
  //b
  sprintf (command, "../src2/bats -b %s",argv[i] );
  system(command);
  wait(NULL);
  //u
  sprintf (command, "../src2/bats -u %s",argv[i] );
  system(command);
  wait(NULL);
  //tb
  sprintf (command, "../src2/bats -ele -b %s",argv[i] );
  system(command);
  wait(NULL);
  //ut
  sprintf (command, "../src2/bats -u -ele %s",argv[i] );
  system(command);
  wait(NULL);
  //ub
  sprintf (command, "../src2/bats -u -b %s",argv[i] );
  system(command);
  wait(NULL);
  //utb
  sprintf (command, "../src2/bats -u -ele -b %s",argv[i] );
  system(command);
  wait(NULL);

  sprintf (command, "../src2/bats -nr %s",argv[i] );
  system(command);
  wait(NULL);
  //t
  sprintf (command, "../src2/bats -ele -nr %s",argv[i] );
  system(command);
  wait(NULL);
  //b
  sprintf (command, "../src2/bats -b -nr %s",argv[i] );
  system(command);
  wait(NULL);
  //u
  sprintf (command, "../src2/bats -u -nr %s",argv[i] );
  system(command);
  wait(NULL);
  //tb
  sprintf (command, "../src2/bats -ele -b -nr %s",argv[i] );
  system(command);
  wait(NULL);
  //ut
  sprintf (command, "../src2/bats -u -ele -nr %s",argv[i] );
  system(command);
  wait(NULL);
  //ub
  sprintf (command, "../src2/bats -u -b -nr %s",argv[i] );
  system(command);
  wait(NULL);
  //utb
  sprintf (command, "../src2/bats -u -ele -b -nr %s",argv[i] );
  system(command);
  wait(NULL);

  //CVC4
  // sprintf (command, "../src/bats    -cvc4 %s ",argv[i] );
  // system(command);
  // wait(NULL);
  // //t
  // sprintf (command, "../src/bats   -cvc4  -ele %s ",argv[i] );
  // system(command);
  // wait(NULL);
  // //b
  // sprintf (command, "../src/bats   -cvc4  -b %s ",argv[i] );
  // system(command);
  // wait(NULL);
  // //u
  // sprintf (command, "../src/bats   -cvc4  -u %s ",argv[i] );
  // system(command);
  // wait(NULL);
  // //tb
  // sprintf (command, "../src/bats   -cvc4 -ele -b  %s ",argv[i] );
  // system(command);
  // wait(NULL);
  // //ut
  // sprintf (command, "../src/bats   -cvc4 -u -ele %s ",argv[i] );
  // system(command);
  // wait(NULL);
  // //ub
  // sprintf (command, "../src/bats   -cvc4  -u -b %s ",argv[i] );
  // system(command);
  // wait(NULL);
  // //utb
  // sprintf (command, "../src/bats   -cvc4  -u -ele -b %s",argv[i] );
  // system(command);
  // wait(NULL);
  free(command);
  }

  return 1;
}
