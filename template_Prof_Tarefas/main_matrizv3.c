#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <omp.h>
#include <mpi.h>
#include "toolsv3.h"
#include "matrizv3.h"
#include "matriz-operacoesv3.h"
#define DEBUG
#define MPIrun

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int main(int argc, char *argv[]) {

	// %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%
	// DECLARAÇÃO de VARIÁVEIS
	mymatriz mat_a, mat_b, mat_cPar;//, submat_c1, submat_c2;
	mymatriz **mat_soma = NULL, **mat_mult = NULL, **ompMat_mult = NULL, **mat_cFinalOMP = NULL;
	mymatriz *mat_cFinal = NULL;

	char filename[100];
	FILE *fmat;
	int nr_line;
	int *vet_line = NULL;
	int N, M, La, Lb, ntasks = 0;
	double start_time = 0, end_time = 0;
	int tipo_multMat = 2;
	double times[20];
	int contTimes = 0;

#ifdef MPIrun
	int rank, size;
	mymatriz *mat_cMPI = NULL;
#endif

	matriz_bloco_t **vet_submat_a = NULL;
	matriz_bloco_t **vet_submat_b = NULL;

	matriz_bloco_t **Vvsubmat[2];
	mymatriz submat_c[2], *subant = NULL;


	// %%%%%%%%%%%%%%%%%%%%%%%% END %%%%%%%%%%%%%%%%%%%%%%%%

#ifdef MPIrun
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
#endif

	if (argc != 4){
		printf ("ERRO: Numero de parametros %s <matriz_a> <matriz_b> <nro_tarefas>\n", argv[0]);
		exit (1);
	}

#ifdef MPIrun
	if (!rank) {
#endif
		printf("\n \
		%%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%%\n \
		#####  \t Leitura das matrizes A e B (%s)  \t #####\n \
		%%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%%\n",argv[1]);

		// %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%
		//                Leitura da Matriz A (arquivo)
		fmat = fopen(argv[1],"r");
		if (fmat == NULL) {
			printf("Error: Na abertura dos arquivos.");
			exit(1);
		}

		ntasks = atoi(argv[3]);


		extrai_parametros_matriz(fmat, &N, &La, &vet_line, &nr_line);
		mat_a.matriz = NULL;
		mat_a.lin = N;
		mat_a.col = La;
		if (malocar(&mat_a)) {
			printf ("ERROR: Out of memory\n");
		}
		filein_matriz (mat_a.matriz, N, La, fmat, vet_line, nr_line);
		free (vet_line);
		fclose(fmat);

		#ifndef DEBUG
		printf("\n ##### Matriz A (%d,%d) (Lin/Col) #####\n", mat_a.lin, mat_a.col);
		mimprimir(&mat_a);
		#endif

		// %%%%%%%%%%%%%%%%%%%%%%%% END %%%%%%%%%%%%%%%%%%%%%%%%

		// %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%
		//               Leitura da Matriz B (arquivo)
		fmat = fopen(argv[2],"r");
		if (fmat == NULL) {
			printf("Error: Na abertura dos arquivos.");
			exit(1);
		}
		extrai_parametros_matriz(fmat, &Lb, &M, &vet_line, &nr_line);
		mat_b.matriz = NULL;
		mat_b.lin = Lb;
		mat_b.col = M;
		if (malocar(&mat_b)) {
			printf ("ERROR: Out of memory\n");
		}
		filein_matriz (mat_b.matriz, Lb, M, fmat, vet_line, nr_line);
		free (vet_line);
		fclose(fmat);

		#ifndef DEBUG
		printf("\n ##### Matriz B (%d,%d) (Lin/Col) #####\n", mat_b.lin, mat_b.col);
		mimprimir(&mat_b);
		#endif

		// %%%%%%%%%%%%%%%%%%%%%%%% END %%%%%%%%%%%%%%%%%%%%%%%%
#ifdef MPIrun
	}

	if (!rank) {
#endif
		// %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%
		//                 Operações de Adição
		printf("\n \
		%%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%%\n \
		##### \t Operações de soma ##### \t \n \
		%%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%%\n");

		mat_soma = (mymatriz **) calloc (2,sizeof(mymatriz *));
		for (int ii=0; ii < 2; ii++) {
			printf("\n ##### somar_t%d de Matrizes (A + A) #####\n", ii);

			// ### Início do Núcleo	###
			start_time = wtime();
			mat_soma[ii] = msomar(&mat_a, &mat_a, ii);
			end_time = wtime();
			// ### Fim do Núcleo	###

			#ifndef DEBUG
			mimprimir(mat_soma[ii]);
			printf("\tRuntime: %f\n", end_time - start_time);
			#endif

			times[contTimes++] = end_time - start_time;
			sprintf(filename, "soma_t%d.result", ii);
			fmat = fopen(filename,"w");
			fileout_matriz(mat_soma[ii], fmat);
			fclose(fmat);
		}
		// %%%%%%%%%%%%%%%%%%%%%%%% END %%%%%%%%%%%%%%%%%%%%%%%%
#ifdef MPIrun
	}

	if (!rank) {
#endif

		printf("\n \
		%%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%%\n \
		#### \t Operações de multiplicação (Sequencial) #### \t \n \
		%%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%%\n");

		// %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%
		//               Operações de Multiplicação
		mat_mult = (mymatriz **) malloc (sizeof(mymatriz *)*tipo_multMat);
		for (int ii=0; ii < tipo_multMat; ii++) {
			printf("\n ##### multiplicar_t%d de Matrizes #####\n", ii);

			// ### Início do Núcleo	###
			start_time = wtime();
			mat_mult[ii] = mmultiplicar(&mat_a, &mat_b, ii);
			end_time = wtime();
			// ### Fim do Núcleo	###

			#ifndef DEBUG
			printf("\n ##### Matriz C (no main) #####\n");
			mimprimir(mat_mult[ii]);
			printf("\tRuntime: %f\n", end_time - start_time);
			#endif

			times[contTimes++] = end_time - start_time;
			sprintf(filename, "mult_t%d.result", ii);
			fmat = fopen(filename,"w");
			fileout_matriz(mat_mult[ii], fmat);
			fclose(fmat);
		}
		// %%%%%%%%%%%%%%%%%%%%%%%% END %%%%%%%%%%%%%%%%%%%%%%%%
#ifdef MPIrun
	}

	if (!rank) {
#endif
		// %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%
		//               Operações de Multiplicação em bloco
		printf("\n ##### multiplicar_t0 de Matrizes (bloco) #####\n");

		// ### Início do Núcleo	###
		start_time = wtime();
		vet_submat_a = particionar_matrizv3 (&mat_a, 1, ntasks);
		vet_submat_b = particionar_matrizv3 (&mat_b, 0, ntasks);

		for (int i = 0; i < ntasks; i++) {

			// aloca submatriz
			if (i < 2) {
				submat_c[i].matriz = NULL;
				submat_c[i].lin = N;
				submat_c[i].col = M;
				if (malocar(&submat_c[i])) {
					printf ("ERROR: Out of memory\n");
				}
			}

			mzerar(&submat_c[(i%2)]);
			Vvsubmat[(i%2)] = constroi_submatrizv3 (&submat_c[(i%2)], ntasks);
			multiplicar_submatriz (vet_submat_a[i], vet_submat_b[i], *Vvsubmat[(i%2)]);

			if (subant) {
				mat_cFinal = msomar(subant, &submat_c[(i%2)], 1);
				subant = mat_cFinal;
			} else {
				subant = &submat_c[(i%2)];
			}
		}
		end_time = wtime();
		// ### Fim do Núcleo	###

		#ifndef DEBUG
		printf("\n ##### Matriz C (no main) #####\n");
		mimprimir(mat_cFinal);
		printf("\tRuntime: %f\n", end_time - start_time);
		#endif

		times[contTimes++] = end_time - start_time;
		sprintf(filename, "mult_SeqBl0.result");
		fmat = fopen(filename,"w");
		fileout_matriz(mat_cFinal, fmat);
		fclose(fmat);
		// %%%%%%%%%%%%%%%%%%%%%%%% END %%%%%%%%%%%%%%%%%%%%%%%%
#ifdef MPIrun
	}

	if (!rank) {
#endif

		printf("\n \
		%%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%%\n \
		#### \t Operações de multiplicação (Paralelo) \t #### \n \
		%%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%%\n");

#ifdef MPIrun
	}

	if (!rank) {
#endif
		// %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%
		//               Operações de Multiplicação
		printf("\n ##### multiplicarTh_t0 de Matrizes #####\n");

		// ### Início do Núcleo	###
		start_time = wtime();
		mat_cPar.matriz = NULL;
		mat_cPar.lin = N;
		mat_cPar.col = N;
		if (malocar(&mat_cPar)) {
			printf ("ERROR: Out of memory\n");
		}

		mzerar(&mat_cPar);
		if ((ntasks > N) || (ntasks == 0)) {
			ntasks = N;
		}

		printf ("\tNúmero de tarefas = %d\n", ntasks);

		start_time = wtime();
		pthread_t *threads = (pthread_t *) malloc(ntasks * sizeof(pthread_t));
		param_t *args = (param_t *) malloc(ntasks * sizeof(param_t));


		for (int i = 0; i < ntasks; i++)
		{
			args[i].tid = i;
			args[i].ntasks = ntasks;
			args[i].N = N;
			args[i].lini = (N/ntasks)*i;

			if (i == (ntasks-1)) {
				if ((N % ntasks) == 0)
				args[i].lfim = (N/ntasks)+args[i].lini;
				else {
					int lin_resto = (int) (N/ntasks) * ntasks;
					//printf ("%d", lin_resto);
					args[i].lfim = (N/ntasks)+args[i].lini+(N-lin_resto);
				}
			} else {
				args[i].lfim = (N/ntasks)+args[i].lini;
			}


			args[i].L = La;
			args[i].M = M;
			args[i].mat_a = &mat_a;
			args[i].mat_b = &mat_b;
			args[i].mat_cPar = &mat_cPar;
			pthread_create(&threads[i], NULL, multiplicarth, (void *) (args+i));
		}

		for (int i = 0; i < ntasks; i++)
		{
			pthread_join(threads[i], NULL);
		}

		end_time = wtime();
		// ### Fim do Núcleo	###

		#ifndef DEBUG
		mimprimir(&mat_cPar);
		printf("\tRuntime: %f\n", end_time - start_time);
		#endif

		times[contTimes++] = end_time - start_time;
		sprintf(filename, "multth_t0.result");
		fmat = fopen(filename,"w");
		fileout_matriz(&mat_cPar, fmat);
		fclose(fmat);
		// %%%%%%%%%%%%%%%%%%%%%%%% END %%%%%%%%%%%%%%%%%%%%%%%%
#ifdef MPIrun
	}

	// %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%
	//               Operações de Multiplicação (OpenMP)

	if (!rank) {
#endif
		ompMat_mult = (mymatriz **) malloc (sizeof(mymatriz *));
		printf("\n ##### multiplicar_omp0 de Matrizes #####\n");
		omp_set_num_threads(ntasks);
		printf ("\tNúmero de tarefas = %i\n", omp_get_max_threads());

		// ### Início do Núcleo	###
		start_time = wtime();
		(*ompMat_mult) = omp_mmultiplicar(&mat_a, &mat_b, 0);
		end_time = wtime();
		// ### Fim do Núcleo	###

		#ifndef DEBUG
		printf("\n ##### Matriz C (no main) #####\n");
		mimprimir((*ompMat_mult));
		printf("\tRuntime: %f\n", end_time - start_time);
		#endif

		times[contTimes++] = end_time - start_time;
		sprintf(filename, "mult_omp0.result");
		fmat = fopen(filename,"w");
		fileout_matriz((*ompMat_mult), fmat);
		fclose(fmat);

		// %%%%%%%%%%%%%%%%%%%%%%%% END %%%%%%%%%%%%%%%%%%%%%%%%
#ifdef MPIrun
	}

	// %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%
	//               Operações de Multiplicação em bloco OMP
	if (!rank) {
#endif
		mat_cFinalOMP = (mymatriz **) malloc (sizeof(mymatriz *));
		
		subant = NULL;
		printf("\n ##### multiplicar_t0 de Matrizes (bloco OMP) #####\n");
		omp_set_num_threads(ntasks);
		printf ("\tNúmero de tarefas = %i\n", omp_get_max_threads());

		// ### Início do Núcleo	###
		start_time = wtime();
		vet_submat_a = particionar_matrizv3 (&mat_a, 1, ntasks);
		vet_submat_b = particionar_matrizv3 (&mat_b, 0, ntasks);

		for (int i = 0; i < ntasks; i++) {

			// aloca submatriz
			if (i < 2) {
				submat_c[i].matriz = NULL;
				submat_c[i].lin = N;
				submat_c[i].col = M;
				if (malocar(&submat_c[i])) {
					printf ("ERROR: Out of memory\n");
				}
			}

			mzerar(&submat_c[(i%2)]);
			Vvsubmat[(i%2)] = constroi_submatrizv3 (&submat_c[(i%2)], ntasks);
			omp_multiplicar_submatriz (vet_submat_a[i], vet_submat_b[i], *Vvsubmat[(i%2)]);

			if (subant) {
				(*mat_cFinalOMP) = msomar(subant, &submat_c[(i%2)], 1);
				subant = (*mat_cFinalOMP);
			} else {
				subant = &submat_c[(i%2)];
			}
		}

		end_time = wtime();
		// ### Fim do Núcleo	###

		#ifndef DEBUG
		printf("\n ##### Matriz C (no main) #####\n");
		mimprimir((*mat_cFinalOMP));
		printf("\tRuntime: %f\n", end_time - start_time);
		#endif

		times[contTimes++] = end_time - start_time;
		sprintf(filename, "mult_SeqBlomp0.result");
		fmat = fopen(filename,"w");
		fileout_matriz((*mat_cFinalOMP), fmat);
		fclose(fmat);

		// %%%%%%%%%%%%%%%%%%%%%%%% END %%%%%%%%%%%%%%%%%%%%%%%%
#ifdef MPIrun
	}

	// %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%
	//      Operações de Multiplicação em bloco MPI
	// Inicio MPI

	// MULTIPLICAR BLOCO PARALLEL c/ MPI
	if (!rank) {
		printf("\n ##### Multiplicação de Matrizes (bloco MPI) #####\n");
		printf ("\tNúmero de tarefas = %i\n", size);
		start_time = wtime();
	}

	MPI_Barrier (MPI_COMM_WORLD);

	mat_cMPI = mpi_multiplicar (&mat_a, &mat_b);

	MPI_Barrier (MPI_COMM_WORLD);

	if (!rank) {
		end_time = wtime();

		#ifndef DEBUG
		printf("\n ##### Matriz C (no main) #####\n");
		mimprimir(mat_cMPI);
		printf("\tRuntime: %f\n", end_time - start_time);
		#endif

		times[contTimes++] = end_time - start_time;
		sprintf(filename, "mult_MPI0.result");
		fmat = fopen(filename,"w");
		fileout_matriz(mat_cMPI, fmat);
		fclose(fmat);
	}

	MPI_Barrier (MPI_COMM_WORLD);

	// %%%%%%%%%%%%%%%%%%%%%%%% END %%%%%%%%%%%%%%%%%%%%%%%%

	if (!rank) {
#endif
		// %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%
		//              Comparação dos resultados
		printf("\n ##### ##### ##### ##### ##### ##### ##### ##### #####\n");
		printf("\n #####       RELATÓRIO FINAL DAS EXECUÇÕES       #####\n");
		printf("\n ##### ##### ##### ##### ##### ##### ##### ##### #####\n");

		int ctT =0;
		printf("\n ##### Comparação dos resultados da adição de matrizes #####\n");
		printf("\t Tempo de execução 1 : %.4f\n\t[soma_t0 vs soma_t1]\t", times[ctT++]);
		mcomparar (mat_soma[0],mat_soma[1]);
		ctT++;

		printf("\n ##### Comparação dos resultados da Multiplicação de matrizes #####\n");
		for (int i=1; i<tipo_multMat; i++) {
			printf("\t Tempo de execução 1 : %.4f\n\t[mult_t0 vs mult_t%d]\t", times[ctT++], i);
			mcomparar (mat_mult[0],mat_mult[i]);
		}
		ctT++;

		printf("\n ##### Comparação dos resultados da Multiplicação de matrizes bloco #####\n");
		printf("\t Tempo de execução 1 : %.4f\n\t[mult_t0 vs mult_SeqBl0]\t", times[ctT++]);
		mcomparar (mat_mult[0],mat_cFinal);

		printf("\n ##### Comparação dos resultados da Multiplicação de matrizes (thread) #####\n");
		printf("\t Tempo de execução %d : %.4f\n\t[mult_t0 vs mult_tTh]\t", ntasks, times[ctT++]);
		mcomparar (mat_mult[0],&mat_cPar);

		printf("\n ##### Comparação dos resultados da Multiplicação de matrizes (OMP) #####\n");
		printf("\t Tempo de execução %d : %.4f\n\t[mult_t0 vs mult_tOmp0]\t", ntasks, times[ctT++]);
		mcomparar (mat_mult[0],(*ompMat_mult));

		printf("\n ##### Comparação dos resultados da Multiplicação de matrizes (blocoOMP) #####\n");

		printf("\t Tempo de execução %d : %.4f\n\t[mult_t0 vs mult_tBlOmp0]\t", ntasks, times[ctT++]);
		mcomparar (mat_mult[0],(*mat_cFinalOMP));

#ifdef MPIrun
		if (mat_cMPI != NULL) {
			printf("\n ##### Comparação dos resultados da Multiplicação de matrizes (bloco MPI) #####\n");
			printf("\t Tempo de execução %d : %.4f\n\t[mult_t0 vs mult_tMPI0]\t", ntasks, times[ctT++]);
			mcomparar (mat_mult[0],mat_cMPI);
		} else {
			printf("\n ##### Comparação dos resultados da Multiplicação de matrizes (bloco MPI) #####\n");
			printf("\t Não pode ser executado (rank %d)!\t\n\n", rank);
		}

		// %%%%%%%%%%%%%%%%%%%%%%%% END %%%%%%%%%%%%%%%%%%%%%%%%
	}



	if (!rank) {
#endif
		// %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%
		//                   Liberação de memória
		mliberar(&mat_a);
		mliberar(&mat_b);
		mliberar(&mat_cPar);

		for (int ii=0; ii < 2; ii++) {
			mliberar(mat_soma[ii]);
			free (mat_soma[ii]);
		}
		free(mat_soma);

		for (int ii=0; ii < tipo_multMat; ii++) {
			mliberar(mat_mult[ii]);
			free (mat_mult[ii]);
		}
		free(mat_mult);

		mliberar(mat_cFinal);
		free(mat_cFinal);

		mliberar((*ompMat_mult));
		free((*ompMat_mult));

		mliberar((*mat_cFinalOMP));
		free((*mat_cFinalOMP));

#ifdef MPIrun
		mliberar(mat_cMPI);
		free (mat_cMPI);

		// %%%%%%%%%%%%%%%%%%%%%%%% END %%%%%%%%%%%%%%%%%%%%%%%%
	}

	MPI_Finalize();
#endif

	return 0;
}
