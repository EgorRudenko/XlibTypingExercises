C_COMP = gcc
MAIN = main.c
OUTPUT = main

all: clean main

main:
	$(C_COMP) -o $(OUTPUT) $(MAIN) -lX11 -lstdc++

clean:
	rm $(OUTPUT)
