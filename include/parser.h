#ifndef PARSER_H
#define PARSER_H

#include <expr.h>
#include <stdio.h>

void * takeExpr(Region *, FILE *);
void * takeExprToplevel(Region *, FILE *);

#endif
