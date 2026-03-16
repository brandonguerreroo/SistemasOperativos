OBJS = lab_sis.c listas_l.c

# Nombre del ejecutable
TARGET = SO

all: SO

SO: $(OBJS)
	gcc -o SO $(OBJS) -lncurses
