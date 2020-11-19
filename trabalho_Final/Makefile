# GNU Makefile
# Revisão: ago/2019

CC = g++
CCFLAGS = -Wall -O3 
LDFLAGS = -pthread -pg
TARGET = main help

all: $(TARGET)

%.o: %.c
	$(CC) $(CCFLAGS) -c $<

%: %.o
	$(CC) $(LDFLAGS) $^ -o $@

main: main.cpp
			$(CC) $(CCFLAGS) main.cpp -o $@ $(LDFLAGS)


help:
	@echo
	@echo
	@echo "####### Exemplo de Execução #######"
	@echo "./main <nro_linhas> <nro_cavalos> <nro_threads>"

clean:
	rm -f *.o *.out *~ $(TARGET)
