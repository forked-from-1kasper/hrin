#include <stdarg.h>
#include <stdio.h>

#include <error.h>

static ErrorTag availableErrorTag = 0;

ErrorTag newErrorTag(void) {
    return availableErrorTag++;
}

static char errorMessageBuffer[2048] = {0};
static ErrorTag thrownError;

void * throw(ErrorTag tag, const char * format, ...) {
    thrownError = tag;

    if (format == NULL) {
        errorMessageBuffer[0] = '\0';
    } else {
        va_list argv;

        va_start(argv, format);
        vsnprintf(errorMessageBuffer, sizeof(errorMessageBuffer), format, argv);
        va_end(argv);
    }

    return NULL;
}

const char * getErrorBuffer(void) {
    return errorMessageBuffer;
}

ErrorTag getThrownError(void) {
    ErrorTag retval = thrownError;

    thrownError = NULL;
    return retval;
}

ErrorTag EOFErrorTag     = "EOFError";
ErrorTag SyntaxErrorTag  = "SyntaxError";
ErrorTag ApplyErrorTag   = "ApplyError";
ErrorTag UnknownErrorTag = "UnknownError";
ErrorTag NameErrorTag    = "NameError";
ErrorTag OOMErrorTag     = "OOMError";
ErrorTag TypeErrorTag    = "TypeError";
ErrorTag RegionErrorTag  = "RegionError";
