NAME=main
OBJFILES=$(NAME).o scanner.o

CC=gcc
CFLAGS= -std=c99 -pedantic -Wall -Wextra -g

# vzorové pravidlo pro generování všech objektových souborů
%.o : %.c
	$(CC) $(CFLAGS) -c $<

# Startovací pravidlo - pro přehlednost
all: $(NAME)

clean:
	rm -f *.o
	rm -f *.out
	rm -f $(NAME)

# Generování závislostí
# při změně souborů spustíme 'make dep'
dep:
	$(CC) -MM *.c >dep.list

-include dep.list

# závěrečné slinkování
$(NAME): $(OBJFILES)
	$(CC) $(CFLAGS) $(OBJFILES) -o $@