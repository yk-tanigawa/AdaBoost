CC = gcc
LD = gcc
CFLAGS = -Wall -O2
LDFLAGS = #-lpthread -lm
SRCS := $(wildcard *.c) # wildcard
OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.dep)
EXEC = $(SRCS:.c=)
RM = rm -f


all: adaboost

adaboost: adaboost.o adaboost.h calloc_errchk.h adaboost_io.h
	$(LD) $(LDFLAGS) -o $@ $^

clean:
	$(RM) $(OBJS) $(EXEC) *~

.PHONY:
	all clean
