#include "tree.h"

int max_bits = 0;
int max_conj = 0;

/*---------------------------- create_node() ---------------------------------
A função create_node aloca memória para a criação de um nó da árvore e
inicializa o hop desse nó a '-1'.
Esta função retorna o nó criado.
-----------------------------------------------------------------------------*/
tree_node* create_node()
{
	tree_node* aux = NULL;
	aux = (tree_node*)malloc(sizeof(tree_node));
	if(aux == NULL)
	{
		printf("Memory error\n");
		exit(-1);
	}
	// Consideramos que ao inicializar o 'hop' do nó a '-1', estamos
 // a definir um nó que não tenha um next-hop válido.
	aux->hop = -1;
	aux->zero = NULL;
	aux->one = NULL;
	aux->possible_hops = (int*)malloc(sizeof(int)*max_conj);

	// Erro na alocação de memória
	if(aux->possible_hops == NULL)
	{
		printf("Memory error\n");
		exit(-1);
	}
	return aux;
}

/*----------------------------- init_tree() ----------------------------------
A função init_tree recebe como argumento o número máximo de bits para
um endereço de IP e inicializa o inteiro max_conj com o quadrado desse número
(que irá servir para alocar memória para o vetor dos possíveis hops) e, por
fim, retorna o nó criado pela função create_node().
-----------------------------------------------------------------------------*/

tree_node* init_tree(int maxbits)
{
	max_bits = maxbits;
	max_conj = maxbits * maxbits; //This value should be 2^maxbits
	return create_node();
}

void free_node(tree_node* tree){
	free(tree->possible_hops);
	free(tree);
	tree = NULL;
}

/*---------------------------- print_tree() ---------------------------------
Função auxiliar que imprime todos os nós da árvore quer tenham o hop definido
a '-1', quer tenham outro valor qualquer. Serve apenas para testar o código.
-----------------------------------------------------------------------------*/

void print_tree(tree_node *tree)
{
	printf("%d \n",tree->hop);

	if(tree->zero != NULL)
		print_tree(tree->zero);
	if(tree->one != NULL)
		print_tree(tree->one);
}
/*-------------------------- InsertPrefix() ----------------------------
Recebe como parâmetros um prefixo, um hop, um inteiro auxiliar
e um apontador para um nó da árvore.
A função InsertPrefix() vai percorrendo a árvore de acordo
com o valor de cada bit no prefixo e, quando chega ao final,
recorre à função create_node() para criar um nó no sítio devido com o
next-hop adequado.
-----------------------------------------------------------------------*/
void InsertPrefix(char *vect, int hop, int i, tree_node* tree)
{
	if(vect[i] == '\0'){
		tree->hop = hop;
	}

	else{
		if(vect[i] == '0'){
			if(tree->zero == NULL)
				tree->zero = create_node();
			InsertPrefix(vect, hop, i + 1, tree->zero);
			return;
		}


		if(vect[i] == '1'){
			if(tree->one == NULL)
				tree->one = create_node();
			InsertPrefix(vect, hop, i + 1, tree->one);
			return;
	}
}
}

/*---------------------------- LookUp() ---------------------------------
A função LookUp() recebe como argumento um prefixo e retorna o hop desse
prefixo.
-----------------------------------------------------------------------------*/

int LookUp(char* address, tree_node* tree, int i, int hop) {
	if (tree->hop != -1)
		hop = tree->hop;
	if (address[i] == '0' && tree->zero != NULL) {
		hop = LookUp(address, tree->zero, i+1, hop);
	}
	if (address[i] == '1' && tree->one != NULL){
		hop = LookUp(address, tree->one,i+1, hop);
	}
	return hop;
}

/*---------------------------- DeletePrefix() ---------------------------------
A função DeletePrefix recebe uma string com um prefixo e apaga o nó da árvore
correspondente a esse mesmo prefixo. Caso este nó não tenha filhos e os seus
ascendentes sejam nós sem next-hop predefinido, a função vai apagando os nós
até chegar a um que tenha reencaminhamento para o próximo router. Por outro
lado, caso o nó a apagar tenha fihos, o hop é apagado com uma simples troca de
valor do mesmo para -1. No fim, caso o prefixo não exista, a função retornará -1
e é impresso na linha de comandos que o prefixo não leva a nenhum next-hop possível.
------------------------------------------------------------------------------*/

int DeletePrefix(char* prefix_vect, tree_node* tree, tree_node* previous_tree, int i, int aux, int flag)
{
	if((prefix_vect[i] != '\0') && (prefix_vect[i] != 'e'))
	{
		if(prefix_vect[i] == '0'){
			if(tree->zero == NULL){
				flag = -1;
				return flag;
			}
			// A função retorna a "flag" e o seu valor indica se existe um nó (correspondente
			// ao prefixo indicado) com um next-hop válido para apagar ou não
			flag = DeletePrefix(prefix_vect, tree->zero, tree, i + 1, 0, flag);
		}
		else if(prefix_vect[i] == '1'){
			if(tree->one == NULL){
				flag = -1;
				return flag;
			}
			flag = DeletePrefix(prefix_vect, tree->one, tree, i + 1, 1, flag);
		}
		if(tree->zero == NULL && tree->one == NULL && tree->hop == -1){
			free_node(tree);
			if (previous_tree != NULL)
			{
				if(aux == 1)
				{
					previous_tree->one = NULL;
				}
				if(aux == 0)
				{
					previous_tree->zero = NULL;
				}
			}
		}
	}
	else{
		if(tree->zero == NULL && tree->one == NULL){
			free_node(tree);
			if (previous_tree != NULL)
			{
				if(aux == 1)
				{
					previous_tree->one = NULL;
				}
				if(aux == 0)
				{
					previous_tree->zero = NULL;
				}
			}
		}
		else
			tree->hop = -1;
	}
	return flag;
}

/*---------------------------- print_node() ---------------------------------
A função print_node é chamada na próxima função, PrintTable(),  para
simplesmente imprimir o prefixo do nó em questão e o seu next-hop respectivo.
------------------------------------------------------------------------------*/

void print_node(char* aux_vect, int i, int hop)
{
	if (i == 0)
	{
		printf("e ");
	}
	else{
		printf("%s", aux_vect);
	}
	printf(" %d\n", hop);
}

/*---------------------------- PrintTable() ---------------------------------
Tal como o nome indica, a função PrintTable executa um percurso em pré-ordem
da árvore de prefixos e, recorrendo à função print_node(), vai imprimindo
cada nó que tenha um next-hop válido.

Caso esta função imprima o valor '0' como next-hop para um prefixo, significa
que esse mesmo prefixo não tem nenhum next-hop válido. Este valor '0' aparece
nos casos em que a árvore não tinha um next-hop default e foi preciso atribuir
o valor '0' à raiz para poder ser feita a compressão da árvore.
-----------------------------------------------------------------------------*/

void PrintTable(char* aux_vect, int i, tree_node* tree)
{

	if(tree->hop != -1)
		print_node(aux_vect, i, tree->hop);
	if(tree->zero != NULL){
		aux_vect[i] = '0';
		aux_vect[i+1] = '\0';
		PrintTable(aux_vect, i + 1, tree->zero);
	}
	if(tree->one != NULL){
		aux_vect[i] = '1';
		aux_vect[i+1] = '\0';
		PrintTable(aux_vect, i + 1, tree->one);
	}
}

/*---------------------------- Push_to_leaf() ---------------------------------
É o segundo passo da técnica de compressão utilizada neste projecto.
Este passo tem como propósito colocar todos os hops nas folhas da árvore (sem
filhos) e os restantes hops a -1. No final deste passo não existem nós da
árvore com apenas um filho.
-----------------------------------------------------------------------------*/
void Push_to_leaf(tree_node* tree, int hop)
{
	if(tree == NULL){
		return;
	}
	if((tree->zero != NULL || tree->one != NULL) && tree->hop != -1)
	{
		hop = tree->hop;
		tree->hop = -1;
	}
	// Cria um filho esquerdo
	if(tree->zero == NULL && tree->one != NULL){
		tree->zero = create_node();
		tree->zero->hop = hop;
	}
	// Cria um filho direito
	if(tree-> one == NULL && tree->zero != NULL){
		tree->one = create_node();
		tree->one->hop = hop;
	}
	// Utilizando a recursividade percorre-se a árvore
	Push_to_leaf(tree->one, hop);
	Push_to_leaf(tree->zero, hop);
}

/*-------------------- initialize_possible_vects() ----------------------------
Função de inicialização do vetor dos possíveis next-hops para cada nó da
árvore. Esta função precede o terceiro passo da compressão onde irá ser feita
a operação A#B.
-----------------------------------------------------------------------------*/

void initialize_possible_vects(tree_node* tree)
{
	int i;
	if(tree == NULL)
		return;
	for (i = 0; i < max_conj; i++)
	{
		tree->possible_hops[i] = -1;
	}
	initialize_possible_vects(tree->zero);
	initialize_possible_vects(tree->one);
}

/*------------------------------ A_with_B() -----------------------------------
É o terceiro passo da técnica de compressão utilizada neste projecto.
Este passo tem como propósito colocar todos os possíveis next-hops num vetor
para cada nó.
-----------------------------------------------------------------------------*/

void A_with_B(int* A, int* B, int* AB)
{
	int count = 0, i, j;
	for(i = 0; i < max_conj; i++){
		if(A[i] == -1)
			break;
		for(j = 0; j < max_conj; j++){
			if(B[j] == -1)
			{
				break;
			}
			else if(A[i] == B[j]){
				AB[count] = A[i];
				count++;
			}
		}
	}
	if(count == 0){
		for(i = 0; A[i] != -1; i++){
			AB[count] = A[i];
			count ++;
		}
		for(i = 0; B[i] != -1; i++){
			AB[count] = B[i];
			count ++;
		}
	}
}

/*------------------------ put_all_possibilities() ----------------------------
A função put_all_possibilities é chamada pela função de compressão da árvore
e para cada nó da árvore, exceto as folhas, chama a função A_with_B para
que fiquem guardados no vetor "possible_hops", o resultado da operação A#B do
filho esquerdo com o direito.
-----------------------------------------------------------------------------*/

void put_all_possibilities(tree_node* tree)
{
	if(tree == NULL){
		return;
	}
	put_all_possibilities(tree->zero);
	put_all_possibilities(tree->one);

	// Se não existe filho da esquerda, então também não existe o da direita
	// e, por isso, estamos perante uma folha
	if (tree->zero == NULL){
		tree->possible_hops[0] = tree->hop; // Os hops das folhas são guardados no vetor
																			// "possible_hops"
	}
	else{
		A_with_B((tree->zero)->possible_hops, (tree->one)->possible_hops, tree->possible_hops);
	}

}

/*--------------------------- PrintPossibles() --------------------------------
PrintPossibles() é uma função auxiliar muito útil para testarmos o código.
Imprime, para cada nó da árvore, o vetor com os possíveis next-hops.
-----------------------------------------------------------------------------*/

void PrintPossibles(tree_node* tree)
{
	int i;
	for(i = 0; i < max_conj; i++){
		printf("%d ", tree->possible_hops[i]);
	}
	printf("\n");

	if(tree->zero != NULL){
		PrintPossibles(tree->zero);
	}
	if(tree->one != NULL){
		PrintPossibles(tree->one);
	}
}

/*------------------------------ pertence() -----------------------------------
Função auxiliar da função 'choose' que retorna o valor '1' ou '0' caso
o 'hop' de entrada pertenca ao vetor 'possible_hops' ou não, respectivamente.
-----------------------------------------------------------------------------*/

int pertence(int* b, int a){
	int i = 0;
	for(i = 0; i < max_conj; i++){
		if(b[i] == -1)
			return 0;
		if(a == b[i])
			return 1;
	}
	return 0;
}

/*---------------------------- choose() ---------------------------------
É o quarto passo da técnica de compressão utilizada neste projecto.
Este passo tem como propósito escolher, para cada nó, um dos possíveis
hops contidos no vetor 'possible_hops'.
-----------------------------------------------------------------------------*/

void choose(tree_node* tree, int father_hop)
{
	if(tree == NULL)
		return;

	if(pertence(tree->possible_hops, father_hop))
	{
		tree->hop = father_hop;
	}
	else
		tree->hop = tree->possible_hops[0];

	choose(tree->zero, tree->hop);
	choose(tree->one, tree->hop);
}

/*---------------------------- new_eliminate() ---------------------------------
É o quinto e último passo feito pela função de compressão da árvore.
Esta função elimina todos os nós redundantes.
-----------------------------------------------------------------------------*/

void new_eliminate(tree_node* tree, tree_node* prev_tree,int hop, int aux)
{
	if(tree == NULL)
		return;
	int prev_hop = hop;
	if(tree->hop != -1 && tree->hop != hop)
		hop = tree->hop;

	new_eliminate(tree->zero,tree, hop, 0);
	new_eliminate(tree->one,tree, hop, 1);

	if((tree->hop == prev_hop) && (prev_hop != -1)){
		if((tree->one == NULL) && (tree->zero == NULL)){
			free_node(tree);
			if(aux == 1)
				prev_tree->one = NULL;
			if(aux == 0)
				prev_tree->zero = NULL;
		}
		else
			tree->hop = -1;
	}
}

/*---------------------------- CompressTree() ---------------------------------
Esta função realiza a compressão de uma árvore de prefixos que, consequentemente,
resulta numa tabela de prefixos com um menor número de entradas, mas que mantém
o mesmo reecaminhamento que a tabela original.
-----------------------------------------------------------------------------*/
void CompressTree(tree_node* tree)
{
	int i;
	// Para os casos em que a árvore não tenha um next-hop default, é necessário
	// atribuir o valor '0' à raiz para que possa ser feita a compressão.
	// Quando a tabela for impressa, é considerado que todos os prefixos que tenham '0'
	// como hop, não tenham um next-hop válido.
	if (tree->hop == -1)
		tree->hop = 0;
	new_eliminate(tree,tree,-1,-1);
	Push_to_leaf(tree, -1);
	initialize_possible_vects(tree);
	put_all_possibilities(tree);
	choose(tree, tree->possible_hops[0]);
	new_eliminate(tree,tree,-1,-1);
}
/*---------------------------- copy_tree() ---------------------------------
Função auxiliar com o propósito de copiar a árvore de prefixos para uma outra.
-----------------------------------------------------------------------------*/

void copy_tree(tree_node* tree1, tree_node* tree2){
	tree2->hop = tree1->hop;

	if(tree1->zero != NULL){
		tree2->zero = create_node();
		copy_tree(tree1->zero, tree2->zero);
	}
	if(tree1->one != NULL){
		tree2->one = create_node();
		copy_tree(tree1->one, tree2->one);
	}
}

/*---------------------------- free_tree() ---------------------------------
A função free_tree é a última função a ser chamada no main() e liberta
toda a memória previamente alocada para a construção da árvore de prefixos.
-----------------------------------------------------------------------------*/

void free_tree(tree_node* tree)
{
	if(tree == NULL)
		return;
	free_tree(tree->one);
	free_tree(tree->zero);
	free_node(tree);
}
