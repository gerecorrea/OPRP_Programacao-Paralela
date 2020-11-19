#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "toolsv3.h"
#include "matrizv3.h"
#include "matriz-operacoesv3.h"

//Novax inclusões:
#include <pthread.h>

//Variáveis globais:
int qtdParticoes = 2; //qtd global de divisores pras matrizes em blocos

int tamLoop = 2; //tamanho do loop, para facilitar a alteração em todos os testes

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
    double mediaMultBlocoSeq;//media de tempo da multiplicação dos blocos sequenciais.]
	double mediaMultThread; //media de tempo da multiplicação da matriz normal com threads
	double mediaMultBlocoThread; //medio de tempo da multiplicação da matriz em bloco com threads

	//MATRIZES RESULTANTE DE CADA CASO
	mymatriz **mat_multSeq = NULL; //Caso sequencial normal
	mymatriz **mat_multBlocoSeq = NULL; //caso bloco normal
	mymatriz   mat_multThread; //caso sequencial com thread; //ele não usou ponteiros
	mymatriz   mat_multBlocoThread; //caso bloco com thread;

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
	printf ("Nº threads de entrada: %d\n", ntasks);

    // %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%
    //                Leitura da Matriz A (arquivo)
	fmat = fopen(argv[1],"r");
	if (fmat == NULL) {
		printf("Error: Na abertura dos arquivos.");
		exit(1);
	}
	extrai_parametros_matriz(fmat, &N, &La, &vet_line, &nr_line);
	
	//Para definir um numero máximo de threads lógico
	if((ntasks > N) || (ntasks == 0)){
		ntasks = N;
	}

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

// // Multiplicação MultiThread (sem blocos):
	
	divisor = qtdParticoes;
	mat_multThread.matriz = NULL; //mat_c do professor
	mat_multThread.lin = N; //N é qtd lihas matriz A
	mat_multThread.col = M; //não deveria ser M?
	if(malocar(&mat_multThread)){
		printf ("ERROR\n");
	}
	//mzerar(&mat_multThread);

	pthread_t *threads = NULL;
	threads = (pthread_t *) malloc(ntasks * sizeof(pthread_t));
   	param_t *args = NULL;
	args = (param_t *) malloc(ntasks * sizeof(param_t));
	
	start_time = wtime();
	for(int ii = 0; ii < tamLoop; ii++){
		for(int iii=0; iii<6; iii++){ //permutação
			mzerar(&mat_multThread);
			for(int i = 0; i< ntasks; i++){
				args[i].tid = i;
				args[i].ntasks = ntasks;
				args[i].N = N; 
				args[i].lini = (N/ntasks) * i;
				args[i].tipo = iii; //Tipo para futura permutação

				if(i == (ntasks-1)){
					if((N % ntasks) == 0)
						args[i].lfim = (N/ntasks) + args[i].lini;
					else{
						int lin_resto = (int) (N/ntasks) * ntasks;
						args[i].lfim = (N/ntasks)+args[i].lini+(N - lin_resto);
					}
				}
				else{
					args[i].lfim = (N / ntasks) + args[i].lini;
				}

				args[i].L = La;
				args[i].M = M;
				args[i].mat_a = &mat_a;
				args[i].mat_b = &mat_b;
				args[i].mat_c = &mat_multThread;
				pthread_create(&threads[i], NULL, multiplicarThread, (void *) (args+i));
			}
			for(int i=0; i< ntasks; i++){
				pthread_join(threads[i], NULL);
			}
		}
	}
	end_time = wtime();
	mediaMultThread = (end_time - start_time)/tamLoop; //arrumar divisão dps
	
	for(int i=0; i<ntasks; i++){
		sprintf(filename, "mult_t%d_ThreadNormal.result", i);
		fmat = fopen(filename,"w");
		fileout_matriz(args[i].mat_c, fmat);
		fclose(fmat);	
	}



// // %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%

// // Multiplicação MultiThreads em Bloco

	divisor = qtdParticoes;
	mat_multBlocoThread.matriz = NULL;
	mat_multBlocoThread.lin = N; //N é qtd lihas matriz A
	mat_multBlocoThread.col = M; //não deveria ser M?
	if(malocar(&mat_multBlocoThread)){
		printf ("ERROR\n");
	}

	pthread_t *threadsBloco = NULL;
	threadsBloco = (pthread_t *) malloc(ntasks * sizeof(pthread_t));
   	param_t *argsBloco = NULL;
	argsBloco = (param_t *) malloc(ntasks * sizeof(param_t));
	
	start_time = wtime();
	for(int ii = 0; ii < tamLoop; ii++){
		for(int iii=0; iii<6; iii++){ //permutação
			mzerar(&mat_multBlocoThread);
			for(int i = 0; i< ntasks; i++){
				argsBloco[i].tid = i;
				argsBloco[i].ntasks = ntasks;
				argsBloco[i].N = N; 
				argsBloco[i].lini = (N/ntasks) * i;
				argsBloco[i].tipo = iii; //Tipo para futura permutação
				argsBloco[i].divisor = divisor; //qtd divisoes para acesso posterior pelas threads

				if(i == (ntasks-1)){
					if((N % ntasks) == 0)
						argsBloco[i].lfim = (N/ntasks) + argsBloco[i].lini;
					else{
						int lin_resto = (int) (N/ntasks) * ntasks;
						argsBloco[i].lfim = (N/ntasks)+argsBloco[i].lini+(N - lin_resto);
					}
				}
				else{
					argsBloco[i].lfim = (N / ntasks) + argsBloco[i].lini;
				}

				argsBloco[i].L = La;
				argsBloco[i].M = M;
				argsBloco[i].mat_a = &mat_a;
				argsBloco[i].mat_b = &mat_b;
				argsBloco[i].mat_c = &mat_multBlocoThread;
				pthread_create(&threadsBloco[i], NULL, multiplicarThread, (void *) (argsBloco+i));
			}
		
			for(int i=0; i< ntasks; i++){
				pthread_join(threadsBloco[i], NULL);
			}
		}
	}
	end_time = wtime();
	mediaMultBlocoThread = (end_time - start_time)/tamLoop; //arrumar divisão dps
	
	for(int i=0; i<ntasks; i++){
		sprintf(filename, "mult_t%d_ThreadBloco.result", i);
		fmat = fopen(filename,"w");
		fileout_matriz(argsBloco[i].mat_c, fmat);
		fclose(fmat);	
	}

// %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%
	//COMPARAÇÕES

	//uma matriz de cada tipo de cada forma:
	printf ("Matriz sequencial normal de posição 0:\n");
	mimprimir(mat_multSeq[0]);
	printf ("Matriz em bloco sequencial de posição 0:\n");
	mimprimir(mat_multBlocoSeq[0]);		
	printf ("Matriz normal com thread de posição 0:\n");
	mimprimir(args[0].mat_c);
	printf ("Matriz em bloco com thread de posição 0:\n");
	mimprimir(argsBloco[0].mat_c);
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

	printf ("matriz sequencial normal vs matriz normal com thread:\t");
	mcomparar(mat_multSeq[0], args[0].mat_c);

	printf ("matriz sequencial normal vs matriz por bloco com thread:");
	mcomparar(mat_multSeq[0], argsBloco[0].mat_c);

	//Tempos:
	printf ("\nTempo médio matriz sequencial normal:\t\t%.10lf seconds\n", mediaMultMatSeq);
	printf ("Tempo médio matriz em bloco sequencial:\t\t%.10lf seconds\n", mediaMultBlocoSeq);
	printf ("Tempo médio matriz com %d threads normal:\t%.10lf seconds\n", ntasks, mediaMultThread);
	printf ("Tempo médio matriz com %d threads em bloco:\t%.10lf seconds\n", ntasks, mediaMultBlocoThread);

	printf ("\nSpeedUp matriz normal: %.2lf\n", mediaMultMatSeq/mediaMultThread);
	printf ("SpeedUp matriz em bloco: %.2lf\n", mediaMultBlocoSeq/mediaMultBlocoThread);

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% END %%%%%%%%%%%%%%%%%%%%%%%%%

//   // %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%

// // LIBERAR MEMÓRIA
	free(mat_multSeq);
	free(mat_multBlocoSeq);
	free(threads);
	free(threadsBloco);
	free(args);
	free(argsBloco);

// 	LIBERAR MATRIZES
	mliberar(&mat_a);
	mliberar(&mat_b);

// 	FECHAR ARQUIVOS

// // %%%%%%%%%%%%%%%%%%%%%%%% END %%%%%%%%%%%%%%%%%%%%%%%%

 	return 0;

}
