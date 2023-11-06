CFLAGS := $(CFLAGS) -Wall -g 
BIN := esp_controller
LIBS := -lserialport -lubus -lubox -lblobmsg_json
SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)


.PHONY: all $(BIN) clean

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)


%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) $(OBJS) $(BIN)