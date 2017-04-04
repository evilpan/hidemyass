CC=gcc
CFLAGS=-ggdb -Wall

all : hidemyass
test:
	$(MAKE) -C unit_test

hidemyass : main.c option_parser.c core.c 
	$(CC) $(CFLAGS) $^ -o $@ 

clean:
	rm -rf hidemyass
