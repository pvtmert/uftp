
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
	@$(ECHO) "rm *.o server client user"
	@-$(RM) *.o server client user 2>/dev/null
	
.PHONY: *
