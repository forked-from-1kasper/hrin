#ifndef ERROR_H
#define ERROR_H

typedef char * ErrorTag;

void * throw(ErrorTag tag, const char * format, ...);

const char * getErrorBuffer();
ErrorTag getThrownError();

extern ErrorTag EOFErrorTag, SyntaxErrorTag, ApplyErrorTag, UnknownErrorTag, NameErrorTag, OOMErrorTag, TypeErrorTag, RegionErrorTag;

#define ARITY(expected, given) if ((expected) != (given)) return throw(TypeErrorTag, "expected %zu argument(s) but %zu were given", (expected), (given));

#endif