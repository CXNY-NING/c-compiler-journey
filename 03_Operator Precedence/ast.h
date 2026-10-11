#ifndef Ast
#define Ast

typedef struct Ast_node
{
	int op;//操作 + - * / int
	struct Ast_node* left_node;//左子树
	struct Ast_node* right_node;//右子树
	int int_value;//存放的int值，若op为int，则这里存放对应值
}Ast_node;

enum Ast_op_type//+ - * / int
{
	ast_end,int_add, int_sub, int_mul, int_div,int_node
};

//建立Ast节点（二元）
Ast_node* make_node(int op, Ast_node* left_node, Ast_node* right_node, int value);

//建立Ast节点（零元）
Ast_node* make_leaf(int value);

//建立Ast节点（一元）
Ast_node* make_only(int op, Ast_node* left, int value);

//释放Ast
void destroy_Ast(Ast_node* root);

#endif // !Ast
