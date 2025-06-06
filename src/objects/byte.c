#include <stdio.h>
#include <ctype.h>

#include <common.h>

#include <objects/byte.h>

static size_t showByte(char * buf, size_t size, void * o) {
    char c = BYTE(o)->value;

    if (isprint(c)) return snprintf(buf, size, "'%c'", c);

    if (size <= 6) return ellipsis(buf);
    return snprintf(buf, size, "'\\x%02hhX'", c);
}

static bool equalByte(void * o1, void * o2) {
    return BYTE(o1)->value == BYTE(o2)->value;
}

ExprTag exprByteTag = {
    .eval   = trivEval,
    .apply  = applyThrowError,
    .show   = showByte,
    .delete = trivDelete,
    .move   = trivMove,
    .equal  = equalByte,
    .size   = sizeof(ExprByte)
};

void initByteTag(Region * region) {
    UNUSED(region);

    newExprImmortal(&exprTag, &exprByteTag, NULL);
    setVar(region->scope, "byte", &exprByteTag);
}
