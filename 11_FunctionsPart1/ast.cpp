#include"ast.h"
#include<stdlib.h>
#include<stdio.h>


//建立Ast节点（二元）
Ast_node* make_node(int op, Ast_node* left_node, Ast_node* mid_node, Ast_node* right_node, int value)
{
	Ast_node* temp_ast_node = (Ast_node*)malloc(sizeof(Ast_node));
	if (temp_ast_node == NULL)
	{
		fprintf(stderr, "error:内存不足\n");
		exit(1);
	}

	temp_ast_node->op = op;
	temp_ast_node->left_node = left_node;
	temp_ast_node->mid_node = mid_node;
	temp_ast_node->right_node = right_node;
	temp_ast_node->v.int_value = value;
	return temp_ast_node;
}

//建立Ast节点（零元）
Ast_node* make_leaf(int op,int value)
{
	return make_node(op, NULL, NULL, NULL, value);
}

/*
Ast_node* make_leaf(int value)
{
	Ast_node* temp_ast_node = (Ast_node*)malloc(sizeof(Ast_node));
	if (temp_ast_node == NULL)
	{
		fprintf(stderr, "error:内存不足\n");
		exit(1);
	}

	temp_ast_node->op = int_node;
	temp_ast_node->left_node = NULL;
	temp_ast_node->right_node = NULL;
	temp_ast_node->int_value = value;
	return temp_ast_node;
}
*/

//建立Ast节点（一元）
Ast_node* make_only(int op, Ast_node* left, int value)
{
	return make_node(op, left, NULL, NULL, value);
}

//释放Ast
void destroy_Ast(Ast_node* root)
{
	//先释放左子树
	if (root->left_node!=NULL)
	{
		destroy_Ast(root->left_node);
	}

	//再释放右子树
	if (root->right_node != NULL)
	{
		destroy_Ast(root->right_node);
	}

	//最后释放当前节点
	free(root);
}