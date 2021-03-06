#ifndef ANALYZER_H_INCLUDED
#define ANALYZER_H_INCLUDED

#include <stdbool.h>

#define SYM_ERRO            -1
#define SYM_INTEGER         0
#define SYM_FLOAT           1
#define SYM_UNARY_OPERATOR  2
#define SYM_BINARY_OPERATOR 3
#define SYM_COMMAND         4
#define SYM_PARENTESES      5

bool analyse(const char *);

void print_type(char *, int);

int get_type(char *, char *, int, int);

int check_type(char *, char[]);

int valid_last_type(int,int);

int valid_type(char *,int, int, char[], int);

int is_sym_number(char[], char[]);

int is_sym_operator(char[]);

int is_sym_command(char[]);

int is_sym_parenteses(char[]);

bool is_number2(const char[]);

bool get_symbol(char[], char[]);

#endif // ANALYZER_H_INCLUDED
