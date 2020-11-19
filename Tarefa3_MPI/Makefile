# GNU Makefile

CC = mpicc
CCFLAGS = -Wall -O3 -lpthread
LDFLAGS = #-pthread
TARGET = mainMPI gera_matrizv3

all: $(TARGET)

%.o: %.c
	$(CC) $(CCFLAGS) -c $<

%: %.o
	$(CC) $(LDFLAGS) $^ -o $@


mainMPI: mainMPI.o matrizv3.o toolsv3.o matriz-operacoesv3.o matriz-operacoes-mpi.o
		$(CC) $(CCFLAGS) matriz-operacoes-mpi.o matriz-operacoesv3.o matrizv3.o toolsv3.o mainMPI.c -o $@ $(LDFLAGS)

gera_matrizv3: matrizv3.o toolsv3.o gera_matrizv3.c
		$(CC) $(CCFLAGS) matrizv3.o toolsv3.o gera_matrizv3.c -o $@ $(LDFLAGS)

help:
	@echo
	@echo
	@echo "####### Exemplo de Execução #######"
	@echo "mpirun -np <nro_de_tarefas> ./mainEx05 <mat_a4x4.example> <mat_b4x4.example>"

clean:
	rm -f *.o *~ $(TARGET) *.map *.result
