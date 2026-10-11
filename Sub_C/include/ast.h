#ifndef Ast
#define Ast

typedef struct Ast_node
{
	int op;//操作 + - * / int semicolon
	struct Ast_node* left_node;//左子树
	struct Ast_node* mid_node;
	struct Ast_node* right_node;//右子树
	union
	{
		int int_value;//存放的int值，若op为int，则这里存放对应值
		int id;
	}v;
}Ast_node;

/*
enum Ast_op_type//+ - * / int semicolon ident = ;
{
	ast_end, int_add, int_sub, int_mul, int_div, int_node, A_sem, A_ident, A_Lident, A_Assign
};
*/


// 运算符号 + - * / 
// 逻辑运算符 == != > >= < <=
// 值 int常量，int变量
// 标识符 ; print int 赋值符号 被赋值的符号 融合节点（为了方便后期if语句的翻译）
enum Ast_op_type
{
	A_add_op=1, A_sub_op, A_mul_op, A_div_op,
	A_equal_op, A_not_equal_op, A_great_then_op, A_great_then_or_equal_op, A_less_then_op, A_less_then_or_equal_op,
	A_I_value, A_INT_ident,
	A_Semicolon, A_print_op, A_INT , A_assign_op, A_Lident,A_glue,A_if,
	A_while_op,A_for,A_do,A_void
};


//建立Ast节点（二元）
Ast_node* make_node(int op, Ast_node* left_node, Ast_node* mid_node, Ast_node* right_node, int value);

//建立Ast节点（零元）
Ast_node* make_leaf(int op, int value);

//建立Ast节点（一元）
Ast_node* make_only(int op, Ast_node* left, int value);

//释放Ast
void destroy_Ast(Ast_node* root);

#endif // !Ast
