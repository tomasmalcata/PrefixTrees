#include <stdlib.h>
#include <stdio.h>


// Estrutura dos nós da árvore
typedef struct tree_node{

	int hop;
	int* possible_hops;	
	struct tree_node* zero; // apontador para o filho da esquerda
	struct tree_node* one; // apontador para o filho da direita
}tree_node;

// Declaração de funções
void print_tree(tree_node *tree);
tree_node* init_tree(int max_bits);
tree_node* create_node();
void InsertPrefix(char *vect,int hop, int i, tree_node* tree);
void free_tree(tree_node* tree);
int LookUp(char* address, tree_node* tree, int i, int hop);
void PrintTable(char* aux_vect,int i, tree_node* tree);
int DeletePrefix(char* prefix_vect, tree_node* tree, tree_node* previous_tree, int i, int aux, int flag);
void CompressTree(tree_node* tree);
void copy_tree(tree_node* tree1, tree_node* tree2);
