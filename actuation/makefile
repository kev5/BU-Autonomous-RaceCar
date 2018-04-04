CC= gcc
CFLAGS= -lrt -pthread -lm
DEPS = setpoint.h
ODIR = obj
TARGET = command
INCDIR = ../include

all: $(TARGET)

%.o: %.c $(INCDIR)/$(DEPS)
	$(CC) -I $(INCDIR) -c -o $@ $< $(CFLAGS)

$(TARGET): $(TARGET).o
	gcc -I $(INCDIR) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f ./*.o ./*~ $(TARGET)
