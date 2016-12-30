all: outrun

CFLAGS += -g
CC = gcc


outrun: output
	example    -n 10 -s hello   -d 3.14 -f
	example.sh -n 20 -s goodbye -d 2.18 -f

output: optrun
	$(CC) $(CFLAGS) example.c -o example

optrun:
	optionalize -c example.opt > example.c
	optionalize -b example.opt > example.sh
	optionalize -m example.opt > example.1
	chmod 755 example.sh


clean:
	$(RM) example example.c example.sh example.1
