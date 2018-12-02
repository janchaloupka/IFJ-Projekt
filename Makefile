NAME=compiler
SRCFOLDER := src
OBJFOLDER := obj
SRCFILES := $(wildcard $(SRCFOLDER)/*.c)
OBJFILES := $(patsubst %.c,$(OBJFOLDER)/%.o,$(notdir $(SRCFILES)))
CC=gcc
CFLAGS= -std=c99 -pedantic -Wall -Wextra -g

# Startovací pravidlo - pro přehlednost
all: $(NAME)

# vzorové pravidlo pro generování všech objektových souborů
$(OBJFOLDER)/%.o : $(SRCFOLDER)/%.c
	mkdir -p $(OBJFOLDER)
	$(CC) $(CFLAGS) -c -o $@ $<


clean:
	rm -rf $(OBJFOLDER)/
	rm -f $(NAME)

test:
	./$(NAME) vita

# Generování závislostí
# při změně souborů spustíme 'make dep'
dep:
	$(CC) -MM $(SRCFOLDER)/*.c >dep.list

-include dep.list

# závěrečné slinkování
$(NAME): $(OBJFILES)
	$(CC) $(CFLAGS) $(OBJFILES) -o $@