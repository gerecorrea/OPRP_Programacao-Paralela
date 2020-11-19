/* ANOTAÇÕES DO CÓDIGO E DESCRIÇÕES
	Autores: Beatriz e Geremias

	V12 - Versão atual:
		Começamos na kkn(), mas lá é deixada só a mestre (thread 0) posicionar os cavalos. Ao terminar de posicionar todos (k=0) chamamos a queens
		Dentro da queens temos todas as threads trabalhando simultaneamente (o que não intrfere), apenas para chegarem na attackqueens(), que é a de maior tempo de execução, a única difernete de 0.00
		Là dentro separamos as threads nos 8 diferentes whiles, cada thread pega uma parte igual (basicamente)
		Mutexes estão corretos, tanto para board quanto k. eu acho.
		Importante:
			Retorna o resultado válido e com numero max de Q somente com 1 threads
			Nas outras o resultado tá inválido, por padrão. 
			Muito provavelmente o erro ocorre no agir em conjunto das threads, algo de verificação, porém todas as tentativas ficaram com retornos inconsistentes.
		
	Observações mais antigas:
		Início da parelização, declarando algumas como globais e declarando o básico de thread no main
		A função kkn() é a principal/primária das threads, ela recebe o identificador delas como long
		Analisar criticamente e efetuar o impacto das novas threads em interferência nas posições
		Obs: criei uma função inútil de thread pra lançar o create_threads lá quando quisermos testar o kkn() padrão 

	Informações e anotações úteis do comportamento e funcionamento do código:
		A função sequencial que mais consome tempo é a attackqueens(), apenas para saber na parte de paralelizar
		attack() é chamado por attackqueens(), que por sua vez é chamada por place()
		place() é chamada pela queens() e pela kkn()
		kkn() é a primeira chamada e chama canPlace(), place(). Além de também queens() e displayResult(), ambas uma vez (para exibição dos resultados).
		Para efeito de comparação: Com entrada 100 50 chamou a attackqueens() 9886 vezes, kkn() 1 vezes, attack() 50 vezes (chama qtd de cavalos existentes).

	Para testes e cálculo de tempo: 
		Usar somente via gprof, via clock() não é confiável!!! 
		Além disso cuidar dos prints sendo mostrados, eles podem influeciar no tempo de saída
		Antes da paralelização, tempo do código em matrizes estava dando para 300 50 de entrada, algo como 9.1 a 9.4 de tempo
		Infelizmente, o tempo já estava bem próximo do código original
		Vamos ver paralelizando.
		IMPORTANTE: O professor irá executar/testar na máquina ens4. Devemos testar lá tbm.
*/

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <algorithm>
#include <pthread.h>
#include <math.h>
using namespace std;

//http://mathworld.wolfram.com/KnightsProblem.html

/* m*n is the board dimension
k is the number of knights to be placed on board
count is the number of possible solutions */
//clock_t tempo_inicial, tempo_final;

//Variáveis globais:
int m;//quantidade de linhas da matriz
int n;//quantidade de colunas da matriz (é quadrada)
int k; //quantidade de cavalos a serem colocados na board
int num_threads; //quantidade de threads
char** board; //board contendo as rainhas e cavalos.

pthread_mutex_t mutex_board = PTHREAD_MUTEX_INITIALIZER; //para board
//pthread_mutex_t mutex_new_board = PTHREAD_MUTEX_INITIALIZER; //para new_board, tentativa que não gerou modificações nos resultados na interpretação diferente.
pthread_mutex_t mutex_k = PTHREAD_MUTEX_INITIALIZER; //para o k (qtd cavalos)
pthread_barrier_t barreira;
pthread_cond_t esperaMestre = PTHREAD_COND_INITIALIZER;

void makeBoard(){
    //Aqui inicializo as posições com '_' com o uso do fill. O tempo de código não alterou muito.
    //fill(*board, *board + m*n, '_');
	
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++) {
			board[i][j] = '_';
		}
	}
}

/* Imprimir tabuleiro */
/*
void displayBoard(char** board){
	cout << endl;
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++) {
			cout << "\t" << board[i][j] << " ";
		}
		cout << endl;
	}
}
*/

/* Imprimir tabuleiro com coordenadas*/
void displayBoardCoordenatesMatrix(char** board){
	cout << endl;
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++) {
			if ((board[i][j] != 'K') && (board[i][j] != 'Q')) {
				cout << "\t" << (i*m)+(j+1) << ";" ;
			} else {
				cout << "\t" << board[i][j] << ";" ;
			}
		}
		cout << endl;
	}
	cout << endl;
}

/* Imprimir resultado final */
void displayResult(char** board){
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++) {
			if (board[i][j] == 'K' || board[i][j] == 'Q') {
				cout << board[i][j] << (i*m)+(j+1) << ";" ;
			}			
		}
	}
	cout << endl;
}


/* Imprimir coordenadas do tabuleiro */
/*void displayBoardCoordenates(char** board){
	cout << endl;
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++) {
			if ((board[i][j] == 'K') || (board[i][j] == 'Q')) {
				//cout << m << board[i][j] << i << j << (i*m)+(j+1) << ";" ;
				cout << (i*m)+(j+1) << ";" ;
			}
		}
	}
	cout << endl;
}
*/


/* Marca todas as posições de ataque de um cavaleiro colocado na posição [i][j]*/
void attack(int i, int j, char a, char** board){
	//Vou mandar só a thread master pra cá, não tem pq mandar mais.


	/* Condições para garantir que o bloco a ser verificado esteja dentro do tabuleiro
    void displayBoardCoordenatesMatrix(char** board, char a) */
	pthread_mutex_lock(&mutex_board);
    if ((i + 2) < m && (j - 1) >= 0) { /*baixo 2 esquerda 1*/
		board[i + 2][j - 1] = a;
	}
	if ((i - 2) >= 0 && (j - 1) >= 0) { /*cima 2 esquerda 1*/
		board[i - 2][j - 1] = a;
	}
	if ((i + 2) < m && (j + 1) < n) { /* baixo 2 direita 1*/
		board[i + 2][j + 1] = a;
	}
	if((i - 2) >= 0 && (j + 1) < n) { /* cima 2 direita 1*/
		board[i - 2][j + 1] = a;
	}
	if ((i + 1) < m && (j + 2) < n) { /* baixo 1 direita 2 */
		board[i + 1][j + 2] = a;
	}
	if ((i - 1) >= 0 && (j + 2) < n) { /* cima 1 direita 2 */
		board[i - 1][j + 2] = a;
	}
	if ((i + 1) < m && (j - 2) >= 0) { /* baixo 1 esquerda 2 */
		board[i + 1][j - 2] = a;
	}
	if ((i - 1) >= 0 && (j - 2) >= 0) { /*cima 1 esquerda 2*/
		board[i - 1][j - 2] = a;
	}
	pthread_mutex_unlock(&mutex_board);
}

/*Marca todas as posições de ataque de uma rainha colocada na posição [i][j] */
int attackqueens(int oi, int oj, char a, char** board, long id_thread){
	int i,j;
	int val_thread_min = 0, val_thread_max = 0;

	// São 8 whiles, tentar fazer as divisões e aplicar aqui um para cada
	if(num_threads<=4){
		val_thread_min = id_thread * ceil(8/num_threads) ; //8 pq é a qtd de while, vamos dividir entre eles
		val_thread_max = (id_thread+1) * ceil(8/num_threads) - 1;
		if(id_thread == num_threads - 1) //se é a ultima thread e a divisão não era exata, então a ultima thread trabalha mais.
			val_thread_max = 7; 
	}
	else if(num_threads<=8)
	{
		val_thread_min = id_thread * floor(8/num_threads) ; //8 pq é a qtd de while, vamos dividir entre eles
		val_thread_max = (id_thread+1) * floor(8/num_threads);
	}

	//Isso aqui é só para fazer funfar, o certo é não ter os limitantes forçados. Ainda sim dá deadlock ou algusn erros vez ou outra.
	//val_thread_min = 0;
	//val_thread_max = 7;
	

	//Se num_threads > 8 não é pra entrar em nenhum if, não tem o que ele fazer.

	//Sendo assim, com 4 threads:
	//se id 0 vai de 0 a 1
	//se id 1 vai de 2 a 3 
	//se id 2 vai de 4 a 5
	//se id 3 vai de 6 a 7 (limite)

	//Com 3 threads:
	//se id 0 vai de 0 a 2
	//se id 1 vai de 3 a 5
	//se id 2 vai de 6 a 7
	//nessa lógica, caso a divisão não seja exata, a última thread terá um trabalho diminuido
	//obs: ceil pega o inteiro acima da divisao quebrada


    /*vertical esquerda cima*/
	//fazer if com sti aqui!!!!!!!!! fica dentro do sti e stiLimite o 0 pois esse é o primeiro while
	if(0 >= val_thread_min && 0 <= val_thread_max){
		i = oi;
		j = oj;
		while ((i > 0) && (j > 0)) {
			//if ((board[i-1][j-1] != 'K') || (board[i-1][j-1] != 'A') || (board[i-1][j-1] != 'Q') || (board[i-1][j-1] != 'q')){
			pthread_mutex_lock(&mutex_board);
			if ((board[i-1][j-1] != 'K') || (board[i-1][j-1] != 'A') || (board[i-1][j-1] != 'Q')){
				if ((board[i-1][j-1] != 'K')) {
					board[i-1][j-1] = a;
					pthread_mutex_unlock(&mutex_board);
				} else {
					pthread_mutex_unlock(&mutex_board);
					return(1);				
				}
			}
			i--;
			j--;
		}
	//	printf ("thread%ld aqui no attack\n",id_thread);
	}

    /*cima*/
	if(1 >= val_thread_min && 1 <= val_thread_max){
		i = oi;
		j = oj;
		while (i > 0) {
			pthread_mutex_lock(&mutex_board);
			if (board[i-1][j] != 'Q' && board[i-1][j] != 'K') {
				board[i-1][j] = a;
				pthread_mutex_unlock(&mutex_board);
			} else {
				pthread_mutex_unlock(&mutex_board);
				return(1);
			}
			i--;
		}
		//printf ("thread%ld aqui no attack\n",id_thread);
	}

    /*vertical cima direita*/
	if(2 >= val_thread_min && 2 <= val_thread_max){
		i = oi;
		j = oj;
		while ((i > 0) && (j < n)) {
			pthread_mutex_lock(&mutex_board);
			if (board[i-1][j+1] != 'Q' && board[i-1][j+1] != 'K') {
				board[i-1][j+1] = a;
				pthread_mutex_unlock(&mutex_board);
			}else {
				pthread_mutex_unlock(&mutex_board);
				return(1);
			}
			i--;
			j++;
		}
		//printf ("thread%ld aqui no attack\n",id_thread);
	}

    /*lado esquerdo*/
	if(3 >= val_thread_min && 3 <= val_thread_max){
		i = oi;
		j = oj;
		while (j > 0) {
			pthread_mutex_lock(&mutex_board);
			if (board[i][j-1] != 'Q' && board[i][j-1] != 'K') {
				board[i][j-1] = a;   
				pthread_mutex_unlock(&mutex_board);
			}else {
				pthread_mutex_unlock(&mutex_board);
				return(1);	
			}
			j--;
		}
		//printf ("thread%ld aqui no attack\n",id_thread);
	}

    /*lado direito*/
	if(4 >= val_thread_min && 4 <= val_thread_max){
		i = oi;
		j = oj;
		while (j+1 < n) {
			pthread_mutex_lock(&mutex_board);
			if (board[i][j+1] != 'Q' && board[i][j+1] != 'K') {
				board[i][j+1] = a;
			pthread_mutex_unlock(&mutex_board);
			}else {
				pthread_mutex_unlock(&mutex_board);
				return(1);
			}
			j++;
		}
		//printf ("thread%ld aqui no attack\n",id_thread);
	}

    /*vertical esquerda baixo*/
	if(5 >= val_thread_min && 5 <= val_thread_max){
		i = oi;
		j = oj;
		while ((i+1 < m) && (j > 0)) {
			pthread_mutex_lock(&mutex_board);
			if (board[i+1][j-1] != 'Q' && board[i+1][j-1] != 'K') {
				board[i+1][j-1] = a;
				pthread_mutex_unlock(&mutex_board);
			}else {
			pthread_mutex_unlock(&mutex_board);
				return(1);
			}
			i++;
			j--;
		}
		//printf ("thread%ld aqui no attack\n",id_thread);
	}

    /*baixo*/
	if(6 >= val_thread_min && 6 <= val_thread_max){
		i = oi;
		j = oj;
		while (i+1 < m) {
			pthread_mutex_lock(&mutex_board);
			if (board[i+1][j] != 'Q' && board[i+1][j] != 'K') {
				board[i+1][j] = a;
				pthread_mutex_unlock(&mutex_board);
			}else {
				pthread_mutex_unlock(&mutex_board);
				return(1);
			}
			i++;
		}
		//printf ("thread%ld aqui no attack\n",id_thread);
	}

    /*vertical direita baixo*/
	if(7>= val_thread_min && 7 <= val_thread_max){
		i = oi;
		j = oj;
		while ((i+1 < m) && (j+1 < n)) {
			/* antes da inversão para o if possuir a maior ocorrência
			if (board[i+1][j+1] != 'Q' && board[i+1][j+1] != 'K') {
				return(1);
			}else {
				board[i+1][j+1] = a;
			}
			*/
		pthread_mutex_lock(&mutex_board);
			if (board[i+1][j+1] != 'Q' && board[i+1][j+1] != 'K') {
				board[i+1][j+1] = a;
				pthread_mutex_unlock(&mutex_board);
			}else {
				pthread_mutex_unlock(&mutex_board);
				return(1);
			}
			i++;
			j++;
		}
		//printf ("thread%ld aqui no attack\n",id_thread);
	}
	
	//barreira não tá dando bom, mas precisamos de algum jeito aqui só poder retornar quando todas as threads chegarem nesse ponto, sem nenhuma ficar pra tás
	pthread_barrier_wait(&barreira);
	return(0);
}


/* Verifica se a posição está vazia para colocar o cavaleiro */
bool canPlace(int i, int j, char** board){
	//Aqui é um acesso compartilhado, uma thread pode agir enquanto realiza tal verificação, bom ser uma rc de acesso a board (mutex t1)
	pthread_mutex_lock(&mutex_board);
	if (board[i][j] == '_'){
		pthread_mutex_unlock(&mutex_board);
	    return true;
	}	
	else{
		pthread_mutex_unlock(&mutex_board);
	    return false;
	}
}

/* Coloca o cavaleiro na posição [i][j] no tabuleiro */
int place(int i, int j, char k, char a, char** board, char** new_board, long id_thread){
	//Como new_board não é global, mas sim criado nas funções, não tem pq ter mutex, acredito.
	
	/* Copia as configurações do antigo tabuleiro para o novo tabuleiro */
	for (int y = 0; y < m; y++) {
		for (int z = 0; z < n; z++) {
			//teria q ver esse new_board que ta sendo passado por parametro
			//pthread_mutex_lock(&mutex_new_board);
			pthread_mutex_lock(&mutex_board);
			new_board[y][z] = board[y][z];
			//pthread_mutex_unlock(&mutex_new_board);
			pthread_mutex_unlock(&mutex_board);
		}
	}

	/* Coloca o cavaleiro na posição [i][j] no novo tabuleiro */
	//pthread_mutex_lock(&mutex_new_board);
	new_board[i][j] = k;
	//pthread_mutex_unlock(&mutex_new_board);	

	/* Marca todas as opções de ataque do cavaleiro recém-colocado no novo tabuleiro */
	pthread_mutex_lock(&mutex_k);
	if (k == 'K') {
		pthread_mutex_unlock(&mutex_k);
		if(id_thread==0)
			attack(i, j, a, new_board);
	} else {
		pthread_mutex_unlock(&mutex_k);
		return(attackqueens(i, j, a, new_board, id_thread)); //aqui irá começar a parada da paralelização.
	}
	return (0);
}

/*Encontra as rainhas e coloca elas no tabuleiro*/
void queens(int qui, int quj, char ** board, long id_thread) {
	//passo o id_thread pra frente.

	if ((qui*m)+(quj+1) >= (m*n)) {
		//displayBoard(board);
		//displayBoardCoordenates(board,'Q');
	} else {
		//Aqui era um possível ponto de paralelização, mas da forma tentada deu ruim e retornou resultados inválidos tbm!
		for (int i = qui; i < m; i++) {
			for (int j = quj; j < n; j++) {
                
				if (canPlace(i,j,board)) {
				    /* Cria um novo tabuleiro e coloca a nova rainha nele */
					char** new_board = new char*[m];
					for (int x = 0; x < m; x++) {
						new_board[x] = new char[n];
					}

					if (place(i, j, 'Q', 'a', board, new_board, id_thread)) {    
					    /* Função recursiva para encontrar outras rainhas*/
					    pthread_mutex_lock(&mutex_board);
						board[i][j] = '*';
						pthread_mutex_unlock(&mutex_board);
						queens(i, j, board, id_thread);
					} else {
						/* Função recursiva para encontrar outras rainhas*/
						//printf("Place queen on %d:%d\n", i,j);
                        pthread_mutex_lock(&mutex_board);
						board[i][j] = 'Q';
						pthread_mutex_unlock(&mutex_board);
                       // displayBoard(new_board);
						queens(i, j, new_board, id_thread);
					}
					//displayBoard(board);

					/* Deleta o novo tabuleiro*/
					for (int x = 0; x < m; x++) {
						delete[] new_board[x];
                    }
					delete[] new_board;

				}
			}
		}
	}
}

/* Coloca os cavaleiros no tabuleiro de modo que eles não se atacam - É a função principal chamada pelas threads do main*/
void *kkn(void* argp){
	/*primeira função chamada, nelo pego o id_thread e vou passando pra frente, nessa tentativa só tento paralelizar mesmo na attackqueens.*/ 
	//Lembrando que k e board são globais!!

	int sti = 0, stj = 0;
	int liberado=0;

	long id_thread = (long)argp; //id da thread
	//printf("oi1-kkn-thread%ld\n",id_thread);
	/* Loop para verificar todas as posições no tabuleiro */
	//for (int i = sti; i < stiLimite; i++) { //antes i < m e sti = 0.
	for (int i = sti; i < m; i++) { //antes i < m e sti = 0.
		for (int j = stj; j < n; j++) {

			/* É possível colocar o cavaleiro na posição [i][j]? */
			//if(id_thread == 0){
				if (canPlace(i, j, board)) {
					//printf("oi2-kkn-entrei-thread%ld\n",id_thread);

					/* Cria um novo tabuleiro e coloca o cavaleiro na posição */
					char** new_board = new char*[m];
					
						for (int x = 0; x < m; x++) {
							new_board[x] = new char[n];
						}
					

					//pthread_mutex_lock(&mutex_board);
					if(id_thread == 0){ //somente thread master entra, pois estamos só colocando os cavalos
						place(i, j, 'K', 'A', board, new_board, id_thread); //lá dentro ele chamará a attackqueens tbm!
						//printf("oi3-kkn-fizoplace-thread%ld\n",id_thread);
					}
					//pthread_mutex_unlock(&mutex_board);
					

					/* Função recursiva para os outros cavaleiros - retirada*/
					//kkn(k - 1, i, j, new_board);
					
					//como só id thread = 0 entrou, só thread master irá decrescer ele.
					if(id_thread==0){
						k = k-1;
						//printf("oi4-kkn-kdecresc-thread%ld\n",id_thread);
					}
					sti = i;
					stj = j;
					i = sti;

					if(id_thread != 0){ //meu cond wait manual pq não funfou com ele normal :/
						pthread_mutex_lock(&mutex_k);
						while(k > 0){
							pthread_cond_wait(&esperaMestre,&mutex_k);	
						}
					}


					//Aqui a thread master pega o newboard e passa pra board com o novo cavalo
					if(id_thread == 0){
						//printf("oi5-kkn-nova_board-thread%ld\n",id_thread);
						for (int y = 0; y < m; y++) {
							for (int z = 0; z < n; z++) {
								//pthread_mutex_lock(&mutex_board);
								pthread_mutex_lock(&mutex_board); //Apenas por segurança, supostamente tanto faz aqui
								board[y][z] = new_board[y][z]; //auqui temos um ponto crítico, acesso compartilhado ao board.
								pthread_mutex_unlock(&mutex_board);
								//pthread_mutex_unlock(&mutex_board);
							}
						}
					}
					//printf("k (qtd cavalos) que faltam = %d\n", k);

					/* Se não tem cavaleiros, passamos a botar as rainhas, podendo chamar as threads em espera*/
					if (k == 0) {
						//Como k==0, posso reativar todas as trhreads com o signal e todas entrarem na queens, para paralelizarmos na attackqueens elas.
						pthread_cond_signal(&esperaMestre);
						pthread_mutex_unlock(&mutex_k);

						//printf("oi6-kkn-k=0-thread%ld\n",id_thread);
						
						pthread_barrier_wait(&barreira);
						//Uma possibilidade seria separar as threads aqui, mas mesmo eu separando dentro da queens gerou problemas (o que daria no mesmo).
						queens(0,0,board, id_thread); //aqui teríamos que passar o id da thread e ver como controlar ela lá dentro. só é chamada após não ter mais cavalos pra botar.
						
						//para garantir que todos terminaram o queens e podemos dispor dos resultados então.
						pthread_barrier_wait(&barreira); //- retirei porque trava ele ligado. No formato atual não precisa mais, elas terminam juntas na mesma chamada sempre.
						if(id_thread == 0){
							//displayBoardCoordenatesMatrix(board); //mostra a matriz com Q K e casas vazias. NÃO MANTERÁ NO ENVIO FINAL
							//tempo_final = clock();
							//cout<<"tempo final = "<< (tempo_final - tempo_inicial) / (double)CLOCKS_PER_SEC << endl;
							displayResult(board); //mostra de forma mais simples a saída. MANTERÁ NO ENVIO FINAL
							//pthread_mutex_unlock(&mutex_board);
							exit (1);
						}
					}					

					/* Delete the new board	to free up the memory */
					/*for (int x = 0; x < m; x++) {
						delete[] new_board[x];
					}
					delete[] new_board; não estava sendo utilizado*/
				}
			//}
			//else{
				//while(k!=0){
				//}
				//Se não é mestre, faço esperar o sinal de que todos os cavalos foram colocados.
				//pthread_cond_wait(&esperaMestre,&mutex_k);		
			//}
			
		}
		stj = 0; 
	}
	
}

// Main
int main( int argc, char *argv[]){
	
	//tempo_inicial = clock();

	if (argc != 4 ) {
		printf("./knight <linha_tabuleiro> <nro_cavalos> <num_threads>\n");
		return -1;
	}

	m = atoi(argv[1]);
	n = m;
	k = atoi(argv [2]);

	//THREADS:
	num_threads = atoi(argv [3]);

	/* Criando um tabuleiro m*n */
	//char** board = new char*[m];
	board = new char*[m];
    for (int i = 0; i < m; i++) {
		board[i] = new char[n];
	}

	pthread_t *threads; //declaro as threads
	threads = (pthread_t *)malloc(sizeof(pthread_t) * (num_threads)); //aloco a quantidade de threads baseado na entrada.
	
	//Verificando se tudo certo na alocação.
	if(threads==NULL){
			printf ("Erro na alocação das threads.\n");
			exit(1);
	}

	//Verificação da inicialização correta dos mutexes.
	if (pthread_mutex_init(&mutex_board, NULL) != 0 || pthread_mutex_init(&mutex_k,NULL)!=0){
			printf("Erro inicializando mutex t\n");
	}

	//Inicialização das barreiras.
	int rc = pthread_barrier_init(&barreira, NULL, num_threads);
		    if (rc != 0) {
		  	perror("erro em pthread_barrier_init()");
		  	exit(1);
	}

	/* Preenche as casas do tabuleiro com "_" */
	makeBoard();

	//Inicialização das threads
	long d;
	for (int i = 0; i < num_threads; i++){
		d = i;
		//pthread_create(&threads[i], NULL, kkn, (void *) d); //lanço id da thread junto
		pthread_create(&threads[i], NULL, kkn, (void *) d); 
	}

	for (int i = 0; i < num_threads; i++){
	    rc = pthread_join(threads[i], NULL);
		//pthread_join(threads[i], NULL);
	}

	/*cout << endl
	<< "Total number of solutions : "
	<< count << endl;*/

	pthread_cond_destroy(&esperaMestre);
	pthread_mutex_destroy(&mutex_board);
	pthread_mutex_destroy (&mutex_k);

	free(threads);

	return 0;
}