CC = gcc
LD = gcc
CFLAGS = -Wall -O2
LDFLAGS = #-lpthread -lm
SRCS := $(wildcard *.c) # wildcard
OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.dep)
EXEC = $(SRCS:.c=)
RM = rm -f

all: $(EXEC)

adaboost: adaboost.o
	$(LD) $(LDFLAGS) -o $@ $^

adaboost.o: adaboost.h calloc_errchk.h adaboost_io.h bit_op.h

clean:
	$(RM) $(OBJS) $(EXEC) *~

.PHONY:
	all clean
