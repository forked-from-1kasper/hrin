#ifndef ERROR_H
#define ERROR_H

typedef char * ErrorTag;

void * throw(ErrorTag tag, const char * format, ...);

const char * getErrorBuffer();
ErrorTag getThrownError();

extern ErrorTag EOFErrorTag, SyntaxErrorTag, ApplyErrorTag, UnknownErrorTag, NameErrorTag, OOMErrorTag, TypeErrorTag;

#endif