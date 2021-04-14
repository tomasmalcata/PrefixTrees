#include "tree.h"

void init_string(char* string)
{

	string[0] = '\0';
}

void print_menu()
{
	printf("Select number:\n(1)PrintTable()\n(2)LookUp()\n(3)InsertPrefix()\n(4)DeletePrefix()\n(5)CompressTree()\n(6)Exit\n");
}


int main(int argc, char *argv[])
{
char *char_prefix = NULL;
int hop, i, max_bits, menu, print;
tree_node* tree = NULL;
tree_node* tree_copy = NULL;
FILE *file;



if(sscanf(argv[2], "%d", &max_bits) == 0)
{
		exit(-1);
}

max_bits = max_bits + 1;

// Alocação de memória de uma string que irá guardar os prefixos
char_prefix = malloc(sizeof(char) * max_bits);
char_prefix[0] = '\0';
if(char_prefix == NULL)
	{
		printf("Memory error\n"); // Falha na alocação de memória
		exit(-1);
	}

// Abertura do ficheiro de texto com a tabela dos prefixos
file = fopen(argv[1], "r");

// Inicializa-se a árvore
tree = init_tree(max_bits - 1);

// O ciclo while é executado até terem sido lidos todos
// os prefixos no ficheiro de texto
while(1)
{
	init_string(char_prefix);


	if(fscanf(file, "%s %d", char_prefix, &hop) != 2)
	{
		break;
	}
	if(char_prefix[0] == 'e'){
			char_prefix[0] = '\0';
		}

	InsertPrefix(char_prefix, hop, 0, tree);
}

fclose(file);
printf("\nThe prefix table have from the file %s been uploaded\nto the correspondent prefix tree.\n\n",argv[1]);

while(1)
{
	print_menu();
	init_string(char_prefix);
	scanf("%d", &menu);
	printf("\n");

	// 1º Algoritmo -> PrintTable()
	if(menu == 1){
		printf("(1) for original table\n(2) for compressed table\n");
		scanf("%d", &print);
		printf("\n");
		if(print == 1)
			PrintTable(char_prefix ,0, tree);
		if(print == 2){
			if(tree_copy != NULL)
				PrintTable(char_prefix ,0, tree_copy);
			else
				printf("Compressed table has not been done yet\n");
		}
		printf("\n");
	}

	// 2º Algoritmo -> LookUp()
	else if(menu == 2){
		printf("Insert address\n");
		scanf("%s",char_prefix);
		printf("\n");

		if(tree_copy == NULL){
			hop = LookUp(char_prefix, tree, 0, -1);
		}
		else{
			hop = LookUp(char_prefix, tree_copy, 0, -1);
		}
		if(hop == -1 || hop == 0)
			printf("There is not a next hop\n");
		else
			printf("Next hop from address %s is: %d\n\n", char_prefix, hop);
	}

	// 3º Algoritmo -> InsertPrefix()
	else if(menu == 3){
		printf("Insert prefix and hop\n");
		scanf("%s %d", char_prefix, &hop);
		printf("\n");
		if(tree_copy != NULL){
			free_tree(tree_copy);
			tree_copy = NULL;
		}
		InsertPrefix(char_prefix, hop, 0, tree);
	}

	// 4º Algoritmo -> DeletePrefix()
	else if(menu == 4){
		printf("Insert an existent prefix to be deleted:\n");
		scanf("%s", char_prefix);
		printf("\n");
		// Caso a função retorne o valor '-1', signifia que não
		// existe next-hop correspondente a esse prefixo
		if(DeletePrefix(char_prefix, tree, NULL, 0, -1, 1) == -1)
		{
			printf("That prefix does not lead to any possible next-hop.\n\n");
		}
		else
			if(tree_copy != NULL){
				free_tree(tree_copy);
				tree_copy = NULL;
			}
	}

	// 5º Algoritmo -> CompressTree()
	else if(menu == 5){
		if(tree_copy != NULL){
			free_tree(tree_copy);
			tree_copy = NULL;
		}
		tree_copy = init_tree(max_bits - 1);
		copy_tree(tree, tree_copy);
		CompressTree(tree_copy);
		PrintTable(char_prefix ,0, tree_copy);
		printf("\n");
	}

 // 6ª Hipótese -> Exit do programa
	else if(menu == 6){
		break;
	}
}

// Libertação de memória previamente alocada
if(tree_copy != NULL){
	free_tree(tree_copy);
}
free_tree(tree);
free(char_prefix);
}
