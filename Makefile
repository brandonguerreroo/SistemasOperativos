OBJS = lab_sis.c

# Nombre del ejecutable
TARGET = SO

all: SO

SO: $(OBJS)
	gcc -o SO $(OBJS) -lncurses
