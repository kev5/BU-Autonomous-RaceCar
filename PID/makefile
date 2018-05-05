CC= g++
CFLAGS= -lrt -pthread -lm
DEPS = pid_params.h shared_def.h pid_source.h
INCDIR = ../include
TARGET = pid2d_main
OBJS = $(TARGET).o pid_source.o

all: $(TARGET) $(OBJS)

%.o: %.c $(INCDIR)/$(DEPS)
	$(CC) -I $(INCDIR) -c -o $@ $< $(CFLAGS)

$(TARGET): $(TARGET).o $(OBJS)
	$(CC) -I $(INCDIR) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f ./*.o ./*~ $(TARGET)
