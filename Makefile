CC=g++
CFLAGS=-O3 -funroll-loops -c

WARNFLAGS = -W -Wall -Wextra -Wno-unused

LDFLAGS=-O2 -lm
SOURCES=Bot.cc MyBot.cc State.cc Square.cc
OBJECTS=$(SOURCES:.cc=.o)
EXECUTABLE=MyBot

#Uncomment the following to enable debugging
CFLAGS+=-g -DDEBUG

all: $(OBJECTS) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(WARNFLAGS) $(OBJECTS) -o $@

.cc.o: *.h
	$(CC) $(WARNFLAGS) $(CFLAGS) $< -o $@

tests: Tests.cc
	$(CC) $(LDFLAGS) $(WARNFLAGS) Tests.cc -o $@

clean: 
	-rm -f ${EXECUTABLE} ${OBJECTS} *.d
	-rm -f debug.txt

.PHONY: all clean

