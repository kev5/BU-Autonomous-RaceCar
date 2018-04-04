CC= gcc
CFLAGS= -lrt -pthread -lm
DEPS = setpoint.h
ODIR = obj
OBJ = pid_source.o pid2d_main.o 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

pid2d_test: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(odir)/*.o *~ core $(INCDIR)/*~
