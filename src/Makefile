CC=gcc
CFLAGS=-O3 -Wall
LDFLAGS=-Wall
EXEC=st25dv_prog

all: $(EXEC)

srv_nfc_term.o: srv_nfc_term.c
	$(CC) -o srv_nfc_term.o -c srv_nfc_term.c $(CFLAGS)

exec_rw_lib.o: exec_rw_lib.c
	$(CC) -o exec_rw_lib.o -c exec_rw_lib.c $(CFLAGS)

st25dv_util.o: st25dv_util.c
	$(CC) -o st25dv_util.o -c st25dv_util.c $(CFLAGS)

main.o: main.c
	$(CC) -o main.o -c main.c $(CFLAGS)

st25dv_prog: main.o st25dv_util.o exec_rw_lib.o srv_nfc_term.o
	$(CC) -o $(EXEC) main.o st25dv_util.o exec_rw_lib.o srv_nfc_term.o $(LDFLAGS)

clean:
	rm -rf *.o $(EXEC)

run: all
	./$(EXEC)
