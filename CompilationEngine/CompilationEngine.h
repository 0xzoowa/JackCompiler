#ifndef COMPILATIONENGINE_H
#define COMPILATIONENGINE_H

#define MAX_CHAR 256

#include <stdio.h>

void create_engine(char *in, char *out); //.xml
void destroy_engine(void);
void compile_class(void);
void compile_class_var_dec(void);
void compile_subroutine(void);
void compile_parameter_list(void);
void compile_subroutine_body(void);
void compile_var_dec(void);
void compile_statements(void);
void compile_let(void);
void compile_if(void);
void compile_while(void);
void compile_do(void);
void compile_return(void);
void compile_expression(void);
void compile_term(void);
int compile_expression_list(void);

#endif