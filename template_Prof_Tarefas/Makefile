# GNU Makefile

#CC = gcc
CC = mpicc
CCFLAGS = -Wall -O3 -fopenmp -lpthread #-llikwid #-funroll-all-loops
LDFLAGS =# -fopenmp -lpthread
TARGET = main_matrizv3 gera_matrizv3 help
EXE = ./gera_matrizv3

all: $(TARGET)

%.o: %.c
	$(CC) $(CCFLAGS) -c $<

%: %.o
	$(CC) $(LDFLAGS) $^ -o $@

main_matriz-bloco: main_matriz-bloco.c matrizv2.o toolsv2.o matriz-operacoes.o
	$(CC) $(CCFLAGS) matriz-operacoes.o matrizv2.o toolsv2.o main_matriz-bloco.c -o $@ $(LDFLAGS)

main_matriz: main_matriz.c matriz.o toolsv2.o matriz-operacoes.o
	$(CC) $(CCFLAGS) matriz-operacoes.o matriz.o toolsv2.o main_matriz.c -o $@ $(LDFLAGS)

main_matrizv3: main_matrizv3.c matrizv3.o toolsv3.o matriz-operacoesv3.o
		$(CC) $(CCFLAGS) matriz-operacoesv3.o matrizv3.o toolsv3.o main_matrizv3.c -o $@ $(LDFLAGS)

gera_matriz2: matrizv2.o toolsv2.o gera_matriz2.c
	$(CC) $(CCFLAGS) matrizv2.o toolsv2.o gera_matriz2.c -o $@ $(LDFLAGS)

gera_matrizv3: matrizv3.o toolsv3.o gera_matrizv3.c
		$(CC) $(CCFLAGS) matrizv3.o toolsv3.o gera_matrizv3.c -o $@ $(LDFLAGS)

help:
	@echo
	@echo
	@echo "####### Exemplo de Execução #######"
	@echo "./main_matrizv3 mat_a3x4.example mat_b4x3.example 2"

clean:
	rm -f *.o *~ $(TARGET) *.map *.result
