
CC=tcc
CFLAGS=-ggdb
LDFLAGS=

RM=rm
ECHO=echo

all: server client

server:
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $@.c $(extra)

client:
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $@.c $(extra)

user:
	$(CC) -c server.c
	$(CC) -c client.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $@.c server.o client.o $(extra)

clean:
	@$(ECHO) "$(RM) *.o server client user keycode"
	@-if [ $(CC) == "clang" ]; then $(ECHO) "$(RM) client.dSYM server.dSYM user.dSYM"; $(RM) -rf client.dSYM server.dSYM user.dSYM &>/dev/null; fi
	@-$(RM) *.o server client user 2>/dev/null

keycode:
	$(CC) -o $@ $@.c

.PHONY: *
