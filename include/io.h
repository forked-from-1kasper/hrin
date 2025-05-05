#ifndef IO_H
#define IO_H

#include <stdio.h>

void fpush(char);
int fnext(FILE *);
char * fdup();
void fdrop();

#endif
