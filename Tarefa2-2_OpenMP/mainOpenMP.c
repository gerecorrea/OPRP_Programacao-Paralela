#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "toolsv3.h"
#include "matrizv3.h"
#include "matriz-operacoesv3.h"

//Novax inclusões:
#include <pthread.h>
#include <omp.h>

//Variáveis globais:
int qtdParticoes = 3; //qtd global de divisores pras matrizes em blocos

int tamLoop = 10; //tamanho do loop, para facilitar a alteração em todos os testes

// // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int main(int argc, char *argv[]) {

// // %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%
// 	// DECLARAÇÃO de VARIÁVEIS

    mymatriz mat_a, mat_b; //matriz lida
    FILE *fmat;
	char filename[100];
    int nr_line;
	int *vet_line = NULL;
	int N, M, La, Lb; //N = qtd Linhas mat_a; M = qtd linhas mat_b
	int ntasks; //qtd de threads
	double start_time, end_time;


    double mediaMultMatSeq; //media de tempo da multiplicação das matrizes sequenciais.
    double mediaMultBlocoSeq;//media de tempo da multiplicação dos blocos sequenciais.


	//MATRIZES RESULTANTE DE CADA CASO
	mymatriz **mat_multSeq = NULL; //Caso sequencial normal
	mymatriz **mat_multBlocoSeq = NULL; //caso bloco normal
	mymatriz **ompMat_mult = NULL;
	mymatriz **mat_cFinalOMP = NULL;
	

// // %%%%%%%%%%%%%%%%%%%%%%%% END %%%%%%%%%%%%%%%%%%%%%%%%

	if (argc != 4){
        //arg 1 é o executável
        //arg 2 matriz 1
        //arg 3 matriz 2
		//arg 4 é a qtd de threads
		printf ("ERRO: Numero de parametros %s <matriz_a> <matriz_b> <n_threads>\n", argv[0]);
		exit (1);
	}

	ntasks = atoi(argv[3]);//qtd de threads
	// não tá pegando direito, por enquanto definir como
	//ntasks = 6;
	printf ("Nº processadores de entrada: %d\n", ntasks);

    // %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%
    //                Leitura da Matriz A (arquivo)
	fmat = fopen(argv[1],"r");
	if (fmat == NULL) {
		printf("Error: Na abertura dos arquivos.");
		exit(1);
	}
	extrai_parametros_matriz(fmat, &N, &La, &vet_line, &nr_line);
	
	//Para definir um numero máximo de threads lógico - isso era pra pthread
	//if((ntasks > N) || (ntasks == 0)){
	//	ntasks = N;
	//}

	mat_a.matriz = NULL;
	mat_a.lin = N;

	mat_a.col = La;
	if (malocar(&mat_a)) {
		printf ("ERROR: Out of memory\n");
	}
	filein_matriz (mat_a.matriz, N, La, fmat, vet_line, nr_line);
	free (vet_line);
	fclose(fmat);
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
	// %%%%%%%%%%%%%%%%%%%%%%%% END %%%%%%%%%%%%%%%%%%%%%%%%


// // %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%
	// // Multiplicação Sequencial (sem permutação)
	//  LOOP (10x)
	//DEPOIS MUDAR PARA TIRAR PERMUTAÇÃO QNDO TIVER OK.
	mat_multSeq = (mymatriz **) malloc (sizeof(mymatriz *) * 6);

	start_time = wtime();
	for(int ii=0; ii<tamLoop; ii++){
		for(int i=0;i<6; i++){	//permutação de tipos
			//fazer com as 6 permutações. -- ARRUMAR
			mat_multSeq[i] = mmultiplicar(&mat_a, &mat_b, i);
			//vai ficar reescrevedo nos arquivos os mesmos dados, mas sem problema.
		}
	}
	end_time = wtime();

	//Mesmo com a permutação, salvar as matrizes fora da contagem do tempo, para não influencair nos resultados
	for(int ii=0;ii<6; ii++){//Armazenando as 6 matrizes geradas
		sprintf(filename, "mult_t%d_SeqNormal.result", ii);
		fmat = fopen(filename,"w");
		fileout_matriz(mat_multSeq[ii], fmat);
		fclose(fmat);
	}
	
	mediaMultMatSeq = (end_time - start_time) / tamLoop;
	
// // %%%%%%%%%%%%%%%%%%%%%%%% END %%%%%%%%%%%%%%%%%%%%%%%%

// // %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%

// // Multiplicação Sequencial em Bloco

    int divisor = qtdParticoes; //qtd de particoes da matriz (2 a 20, segundo o professor)
	mat_multBlocoSeq = (mymatriz **) malloc (sizeof(mymatriz *) * 6);

    start_time = wtime();
	for(unsigned short ii=0; ii<tamLoop; ii++){//loop de 2
		for(int i=0; i<6 ;i++){ //permutação de tipos
			mat_multBlocoSeq[i] = opera_submatrizes(&mat_a, &mat_b, divisor, i);
		}
	}
	end_time = wtime();

	for(unsigned int ii=0; ii<6; ii++){//armazenando todas as matrizes geradas
		sprintf(filename, "mult_t%d_BlocoNormal.result", ii);
		fmat = fopen(filename,"w");
		fileout_matriz(mat_multBlocoSeq[ii], fmat);
		fclose(fmat);
	}

    mediaMultBlocoSeq = (end_time - start_time)/tamLoop;
	
// // %%%%%%%%%%%%%%%%%%%%%%%% END %%%%%%%%%%%%%%%%%%%%%%%%

// // %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%

// // Multiplicação normal com OpenMP - Tudo ok - tempos aleatórios às vezes
		ompMat_mult = (mymatriz **) malloc (sizeof(mymatriz *) * 6);
		omp_set_num_threads(ntasks);
		printf ("Número de tarefas setado pelo OMP: %d\n", omp_get_max_threads());
		int tid = 0;
		
		//Para dividir as tarefas de acordo com a quantidade de threads
		int vet[7];
		vet[0] = 0;
		if(ntasks >= 6)
			for(int i=1;i<ntasks+1;i++) vet[i] = i;
		else if(ntasks == 5) 
			for(int i=1;i<ntasks+1;i++) vet[i] = i + 1;
		else if (ntasks == 4)
			for(int i=1;i<ntasks+1;i++) vet[i] = i + 2;
		else if (ntasks == 3)
			for(int i=1;i<ntasks+1;i++) vet[i] = i*2;
		else if (ntasks == 2)
			for(int i=1;i<ntasks+1;i++) vet[i] = i*3;
		else if (ntasks == 1)
			for(int i=1;i<ntasks+1;i++) vet[i] = 6;

		start_time = wtime();
		//testar com pragma Scheduler (estático ou dinâmico sla com passo 1. (ex: Dynamic, 7))
		//Se passar o estático sem passar o passo ele divide igual, e a ultima fica com o resto. (melhor)
		#pragma omp parallel num_threads(ntasks)
		{
			for(int i=0; i<tamLoop; i++){
				tid = omp_get_thread_num();
				if(tid < 6){
					for(int ii=vet[tid]; ii<vet[tid+1]; ii++){ //para pegar as diferentes permutações
						ompMat_mult[ii] = multiplicar_OMP(&mat_a, &mat_b,ii);
					}
				}
				if(i == tamLoop - 1){
					for(int ii=vet[tid]; ii<vet[tid+1]; ii++){ 
						sprintf(filename, "mult_omp%d_normal.result", ii);
						fmat = fopen(filename,"w");
						fileout_matriz(ompMat_mult[ii], fmat);
						fclose(fmat);
					}
				}
			}
		}
		end_time = wtime();
		#ifndef DEBUG
		const float mediaNormalOpenMP = (end_time-start_time)/tamLoop;
		#endif
// // %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%

// // Multiplicação MultiThreads em Bloco com OpenMP:
		mat_cFinalOMP = (mymatriz **) malloc (sizeof(mymatriz *) * 6);
		omp_set_num_threads(ntasks);
		start_time = wtime();
		#pragma omp parallel num_threads(ntasks)
			for(int i=0; i<tamLoop; i++){
				tid = omp_get_thread_num();
				if(tid < 6){
					for(int ii=vet[tid]; ii<vet[tid+1]; ii++){ //para pegar as diferentes permutações 
						mat_cFinalOMP[ii] = multiplicaPorBlocos_OMP(&mat_a, &mat_b, qtdParticoes, ii);
						//Obs: nessa função, em matriz-operacoesv3.c, realizo todo o processo de submatrizes
						//e a multiplicação por blocos. Lá dentro essa setado o corte em A com vertical e B com horizontal, mas pode ser inverso sem problema.
					}
				}
				if(i == tamLoop - 1){
					for(int ii=vet[tid]; ii<vet[tid+1]; ii++){ 
						sprintf(filename, "mult_omp%d_Bloco.result", ii);
						fmat = fopen(filename,"w");
						fileout_matriz(mat_cFinalOMP[ii], fmat);
						fclose(fmat);
					}
				}
			}
		end_time = wtime();
		#ifndef DEBUG
		const float mediaBlocoOpenMP = (end_time-start_time)/tamLoop;
		#endif
		//*opera_submatrizes(mymatriz *mat_a, mymatriz *mat_b, int divisor, unsigned short tipo);

	

// %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%
	//COMPARAÇÕES

	//uma matriz de cada tipo de cada forma:
	printf ("\n##-------- SAÍDA DOS DADOS -------##:\n\n");
	printf ("Matriz sequencial normal de posição 0:\n");
	mimprimir(mat_multSeq[0]);
	printf ("Matriz em bloco sequencial de posição 0:\n");
	mimprimir(mat_multBlocoSeq[0]);		
	printf ("Matriz normal com openMP de posição 0:\n");
	mimprimir(ompMat_mult[0]);
	printf ("Matriz em bloco com OpenMP de posição 0:\n");
	mimprimir(mat_cFinalOMP[0]);
	//printf("Matriz em bloco com thread de posição 0:\n");


	/* apenas para teste
	printf ("\n");
	for(int i=1; i<6; i++){
		printf("matriz sequencial normal 0 vs matriz sequencial normal %d:", i);
		mcomparar(mat_multSeq[0], mat_multSeq[i]);			
	}

	printf ("\n");
	for(int i=1; i<6; i++){
		printf("matriz sequencial por blocos 0 vs matriz sequencial por blocos %d:", i);
		mcomparar(mat_multBlocoSeq[0], mat_multBlocoSeq[i]);			
	}

	printf("\n");
	for(int i=1; i<ntasks; i++){
		printf("matriz normal thread 0 vs matriz normal thread %d:", i);
		mcomparar(args[0].mat_c, args[i].mat_c);			
	}

	printf("\n");
	for(int i=1; i<ntasks; i++){
		printf("matriz em bloco com thread 0 vs matriz em bloco com thread %d:", i);
		mcomparar(argsBloco[0].mat_c, argsBloco[i].mat_c);			
	}
	*/

	//comparações das 4 formas de multiplicação:
	printf ("\nmatriz sequencial normal vs matriz sequencial por blocos:");
	mcomparar(mat_multSeq[0], mat_multBlocoSeq[0]);

	printf ("matriz sequencial normal vs matriz normal com OpenMP:\t");
	mcomparar(mat_multSeq[0], ompMat_mult[0]);

	printf ("matriz sequencial normal vs matriz por bloco com OpenMP:"); //arrumar esse ainda
	mcomparar(mat_multSeq[0], mat_cFinalOMP[0]);

	//Tempos:
	printf ("\nTempo médio matriz sequencial normal:\t\t%.10lf seconds\n", mediaMultMatSeq);
	printf ("Tempo médio matriz em bloco sequencial:\t\t%.10lf seconds\n", mediaMultBlocoSeq);
	printf ("Tempo médio matriz com %d núcleos por OpenMP:\t%.10lf seconds\n", ntasks, mediaNormalOpenMP);
	printf ("Tempo médio matriz com %d núcleos em bloco por OpenMP:\t%.10lf seconds\n", ntasks, mediaBlocoOpenMP);

	printf ("\nSpeedUp matriz normal: %.2lf\n", mediaMultMatSeq/mediaNormalOpenMP);
	printf ("SpeedUp matriz em bloco: %.2lf\n", mediaMultBlocoSeq/mediaBlocoOpenMP);

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% END %%%%%%%%%%%%%%%%%%%%%%%%%

//   // %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%

// // LIBERAR MEMÓRIA
	free(mat_multSeq);
	free(mat_multBlocoSeq);
	//free(threads);
	//free(threadsBloco);

	//free(argsBloco);

// 	LIBERAR MATRIZES
	mliberar(&mat_a);
	mliberar(&mat_b);

	mliberar((*ompMat_mult));
	free((*ompMat_mult));

	mliberar((*mat_cFinalOMP));
	free((*mat_cFinalOMP));

// 	FECHAR ARQUIVOS

// // %%%%%%%%%%%%%%%%%%%%%%%% END %%%%%%%%%%%%%%%%%%%%%%%%

 	return 0;

}
