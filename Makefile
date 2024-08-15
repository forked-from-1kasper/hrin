CC     = cc
CFLAGS = -g -Iinclude/ -std=c99 -Wall -Wextra -pedantic
BINARY = hrin
CFILES = src/io.c src/lexer.c src/parser.c src/avl.c src/trie.c src/array.c\
         src/objects/nil.c src/objects/ident.c src/objects/list.c src/objects/integer.c\
         src/objects/string.c src/objects/lambda.c src/objects/extern.c src/objects/boolean.c\
         src/expr.c src/error.c src/hrin.c
OFILES = $(CFILES:.c=.o)
DEPEND = Makefile.depend

all: $(BINARY)

.SUFFIXES: .c .o

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

$(BINARY): $(OFILES)
	$(CC) -o $@ $(OFILES)

clean:
	rm -f $(BINARY) $(OFILES)

depend:
	echo > $(DEPEND)
	makedepend -I`$(CC) --print-file-name=include` -f$(DEPEND) -- $(CFLAGS) $(CFILES)

$(DEPEND):
	touch $(DEPEND)

include $(DEPEND)