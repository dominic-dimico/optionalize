#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>

#define SIZE1 32
#define SIZE2 256

struct options {
 bool python;
 bool bash;
 bool C;
 bool man;
};

void usage(char *progname) {
  printf("usage: %s [OPTIONS]\n\n", progname);
  printf(" -p    --python \n         translate into Python\n\n");
  printf(" -b    --bash \n         translate into BaSH script\n\n");
  printf(" -c    --C \n         translate into C language\n\n");
  printf(" -m    --man \n         translate into manual page\n\n");
}

int countlines(char *filename) {
  FILE *file = fopen(filename, "r");
  char ch;
  int lines = 0;
  while (!feof(file)) {
    ch = fgetc(file);
    if (ch == '\n')
      lines++;
  }
  fclose(file);
  return lines;
}


int main (int argc, char **argv) {

  int i=0, j=0;
  int ret;

  bool python = 0;
  bool bash = 0;
  bool C = 0;
  bool man = 0;

  static const struct option long_options[] = {
    {"python", no_argument, 0, 'p'},
    {"bash", no_argument, 0, 'b'},
    {"C", no_argument, 0, 'c'},
    {"man", no_argument, 0, 'm'}
  };

  int c=0;
  int optdex=0;
  while ((c = getopt_long (argc, argv, "pbcm", long_options, &optdex)) != -1)
    switch (c) {
     case 'p':
      python = 1;
      break;
     case 'b':
      bash = 1;
      break;
     case 'c':
      C = 1;
      break;
     case 'm':
      man = 1;
      break;
     case '?':
      usage(argv[0]);
      exit(1);
     default:
      exit(1);
  }

  if (C || man || bash || python);
  else { usage(argv[0]); exit(1); }
  
  int num = countlines(argv[argc-1]);
  FILE *optfile = fopen(argv[argc-1], "r");

  char option[num];
  char options[num][5][SIZE1];
  char descriptions[num][SIZE2];
  char ch;

    for (i=0; i<num; i++) {
      for (j=0; j<4; j++) {
        if (fscanf(optfile, "%s", option));
        strcpy(options[i][j], option);
      }
      do { ch = fgetc(optfile); }
      while (ch == ' ' || ch == '\t');
      ungetc(ch, optfile);
      if (fgets(descriptions[i], SIZE2, optfile));
    }
    fclose(optfile);

    for (i=0; i<num; i++) {
      j=-1;
      while (descriptions[i][++j] != '\0');
      descriptions[i][j-1] = '\0';
    }

  if (C) {

    printf("\
#include <stdio.h>\n\
#include <string.h>\n\
#include <stdlib.h>\n\
#include <getopt.h>\n\
#include <stdbool.h>\n\n\
");


    printf("struct options {\n");
    for (i=0; i<num; i++) {
      for (j=0; j<2; j++)
        printf(" %s", options[i][j]);
      printf(";\n");
    }
    printf("};\n\n");


    printf("void usage(char *progname) {\n");
    printf("  printf(\"usage: %%s [OPTIONS]\\n\\n\", progname);\n");
    for (i=0; i<num; i++) {
      printf("  printf(\"");
      printf(" -%s    ", options[i][2]);
      printf("--%s ", options[i][3]);
      printf("\\n         %s\\n\\n", descriptions[i]);
      printf("\");\n");
    }
    printf("}\n\n");


    printf("void test(struct options *opts) {\n");
    for (i=0; i<num; i++) {
      printf("  printf(\"");
      printf(" %s: %%", options[i][1]);
      if (0==strcmp(options[i][0], "char*"))
           printf("s");
      else if (0==strcmp(options[i][0], "float"))
           printf("f");
      else printf("d");
      printf("\\n\", opts->%s", options[i][1]);
      printf(");\n");
    }
    printf("}\n\n");


    printf("int main(int argc, char **argv) {\n\n");

    for (i=0; i<num; i++) {
      for (j=0; j<2; j++)
        printf(" %s", options[i][j]);
      if (0==strcmp(options[i][0], "char*"))
           printf(" = calloc(1, 256)");
      else printf(" = 0");
      printf(";\n");
    }


    printf("\n static const struct option long_options[] = {\n");
    for (i=0; i<num; i++) {
      printf("  {\"%s\", ", options[i][3]);
      if (0==strcmp(options[i][0], "bool"))
           printf("no_argument, 0, ");
      else printf("required_argument, 0, ");
      printf("'%s'}", options[i][2]);
      if (i<num-1) printf(",\n");
      else         printf("\n };\n");
    }


    printf("\n int c=0;\n int optdex=0;\n while ((c = getopt_long (argc, argv, \"");
    for (i=0; i<num; i++) {
      printf("%s", options[i][2]);
      if (0!=strcmp(options[i][0], "bool"))
         printf(":");
    }
    printf("\", long_options, &optdex)) != -1)\n  switch (c) {\n");

    for (i=0; i<num; i++) {
      printf("   case '%s':\n    ", options[i][2]);
      if (0==strcmp(options[i][0], "bool"))
        printf("%s = 1;\n", options[i][1]);
      else if (0==strcmp(options[i][0], "int"))
        printf("%s = atoi(optarg);\n", options[i][1]);
      else if (0==strcmp(options[i][0], "float"))
        printf("%s = atof(optarg);\n", options[i][1]);
      else if (0==strcmp(options[i][0], "char*"))
        printf("strcpy(%s, optarg);\n", options[i][1]);
      printf("    break;\n");
    }

    printf("   case '?':\n");
    printf("    usage(argv[0]);\n");
    printf("    exit(1);\n");
    
    printf("   default:\n    exit(1);\n  }");
    printf("\n\n");

    printf(" struct options *opts = (struct options *) malloc(sizeof(struct options));\n");
    for (i=0; i<num; i++) {
      printf(" opts->%s = %s;\n", options[i][1], options[i][1]);
    }

    printf("\n test(opts);\n\n");

    for (i=0; i<num; i++) 
      if (0==strcmp(options[i][0], "char*"))
        printf(" free(%s);\n", options[i][1]);

    printf("\n\n");
    printf("}\n");


  } else if (bash) {

    printf("#!/bin/bash\n\n");


    for (i=0; i<num; i++) {
      printf("%s=", options[i][1]);
      if (0==strcmp(options[i][0], "char*"))
        printf("\"\"");
      else printf("0");
      printf(";\n");
    }

    printf("usage() {\n");
    for (i=0; i<num; i++) {
      printf("  echo -ne \"");
      printf(" -%s    ", options[i][2]);
      printf("--%s ", options[i][3]);
      printf("\\n         %s\\n", descriptions[i]);
      printf("\";\n");
    }
    printf("}\n\n");

    printf("test() {\n");
    for (i=0; i<num; i++) {
      printf("  echo \"%s: $%s\"\n", options[i][1], options[i][1]);
    }
    printf("}\n\n");

    printf("\nwhile getopts \"");
    for (i=0; i<num; i++) {
      printf("%s", options[i][2]);
      if (0!=strcmp(options[i][0], "bool"))
         printf(":");
    }
    printf("\" opt; do\n case $opt in");
    for (i=0; i<num; i++) {
      printf("\n  %s)\n   ", options[i][2]);
      if (0==strcmp(options[i][0], "bool"))
        printf("%s=true;", options[i][1]);
      else printf("%s=$OPTARG;", options[i][1]);
      printf("\n   ;;");
    }
    printf("\n esac\ndone\n\n");


  } else if (python) {

 
   /*
   printf("import getopt, sys");

   printf("def main():");
   printf(" try:");

   printf("  opts, args = getopt.getopt(sys.argv[1:], \"");
   //ho:v
   printf(\", [");
   //"help", "output=
   printf("\"])");

   printf(" except getopt.GetoptError as err:");
   printf("  print str(err)");
   printf("  usage()");
   printf("  sys.exit(2)");

   // declare

   printf(" for o, a in opts:");

   printf("  if o == "-v":");
   printf("   verbose = True");

   // loop
   for (i=0; i<num; i++) {
     printf("  elif o in (\"%s\", \"--%s\"):", 
       options[i][2], options[i][3]);
     printf("   %s = a", options[i][1]);
   }

   printf("        else:");
   printf("            assert False, "unhandled option"");
   printf("    # ...");
   printf("");
   printf("if __name__ == "__main__":");
   printf("    main()");

   */


  } else if (man) {

   printf(".TH cmdname 1\n");
   printf(".SH NAME\n");
   printf("cmdname \\- short description\n");
   printf(".SH SYNOPSIS\n");
   printf(".B cmdname\n");
   printf("[OPTIONS]\n");
   printf(".SH DESCRIPTION\n");
   printf("Long description.\n");
   printf(".SH OPTIONS\n");

   for (i=0; i<num; i++) 
     printf(".TP\n.BR \\-%s \", \" \\-\\-%s \" \" \\fI%s\\fR\n%s\n",
       options[i][2], options[i][3], options[i][0], descriptions[i]
     );

   printf(".SH AUTHOR\n");
   printf("Written by Dominic Dimico <dominic.dimico@gmail.com>.\n");

  }

}

