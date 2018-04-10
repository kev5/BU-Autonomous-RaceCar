CC= gcc
CFLAGS= -lrt -pthread -lm
DEPS = pid_params.h shared_def.h pid_source.h
ODIR = obj
TARGET = pid_source pid2d_main
INCDIR = ../include

all: $(TARGET)

%.o: %.c $(INCDIR)/$(DEPS)
	$(CC) -I $(INCDIR) -c -o $@ $< $(CFLAGS)

$(TARGET): $(TARGET).o
	gcc -I $(INCDIR) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f ./*.o ./*~ $(TARGET)
