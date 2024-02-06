MOD_DIR = ../../mod
CC = gcc  # C compiler
CFLAGS = -fPIC -Wall -Wextra -O2 -g  # C flags
LDFLAGS = -shared   # linking flags
RM = rm -f   # rm command
TARGET_LIB = $(MOD_DIR)/libaudit.so  # target lib

SRCS = dllc.c  # source files
OBJS = $(SRCS:.c=.o)

$(TARGET_LIB): $(OBJS)
	$(CC) ${LDFLAGS} -o $@ $^

$(SRCS:.c=.d):%.d:%.c
	$(CC) $(CFLAGS) -MM $< >$@

include $(SRCS:.c=.d)

.PHONY: all
all: ${TARGET_LIB}

.PHONY: clean
clean:
	-${RM} ${TARGET_LIB} ${OBJS} $(SRCS:.c=.d)