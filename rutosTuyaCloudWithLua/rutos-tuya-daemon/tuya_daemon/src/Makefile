BIN:= tuya_daemon
CFLAGS := $(CFLAGS) -Wall -g
SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)
LIBLIST := -llink_core -lmiddleware_implementation -lplatform_port -lutils_modules -lubus -lubox -lblobmsg_json -llua5.1#-largp
.PHONY: all clean $(BIN)

all: $(BIN)

$(BIN) : $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBLIST)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) $(OBJS) $(BIN)
