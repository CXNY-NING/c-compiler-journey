#include<stdlib.h>

#include"ast.h"
#include"data.h"
#include"token.h"
#include"cg.h"
#include"Parser.h"
#include"scanner.h"
#include"sym.h"
#include"stmt.h"

//判断是否是我们所需要的
void match(int type, const char* str)
{
	if (tok.token == type)
	{
		tok = scan();
	}
	else
	{
		if (tok.token != I_value)
		{
			fprintf(stderr, "error:未能找到期望的字符,%s, 期望：%s ,%d行\n", tokens_type[tok.token], str,line);
		}
		else
		{
			fprintf(stderr, "error:未能找到期望的字符,%d, 期望：%s ,%d行\n", tok.int_value, str,line);
			
		}
		exit(1);
	}
}
//分号
void semi()
{
	match(Semicolon, ";");
}

//变量
void ident(void)
{
	match(INT_ident, "ident");
}

//大括号：左、右
void L_big(void)
{
	match(L_big_bracket, "{");	
}

void R_big(void)
{
	match(R_big_bracket, "}");
}

//小括号：左右
void L_small()
{
	match(L_bracket, "(");
}

void R_small()
{
	match(R_bracket, ")");
}

//输出语句
Ast_node* print_stmt(void)
{
	Ast_node* tree;
	int reg;
	//去确认第一个字符是print
	match(print_op, "print");

	//读取这之后的表达式
	tree = Prec_express(0);
	tree=make_node(A_print_op, tree, NULL, NULL, 0);
	semi();
	//释放树的内存占用
	return tree;
}
 
//定义语句
void var_declaration(void)
{
	//确保以int起手
	match(INT, "int");
	//下一个是变量
	ident();
	add_new_sym(buffer);
	//转化为汇编语句
	gen_globsym(buffer);
	
	semi();
}

//赋值语句
Ast_node* assignment_statement(void)
{
	Ast_node* tree,*left,*right;
	int id;
	//确保以变量起手
	ident();

	//赋值之前确保，在符号表中，我们已经有了定义
	if ((id=find_sym(buffer)) == -1)
	{
		fprintf(stderr, "error:该变量未定义,%s,%d行\n", buffer, line);
		exit(1);
	}

	//将变量放在右子树
	right = make_leaf(A_Lident, id);

	//读取=
	match(assign_op, "=");

	//读取表达式
	left = Prec_express(0);
	
	//建立赋值语句完整的语法树
	tree = make_node(A_assign_op, left, NULL,right, 0);

	//结束
	semi();
	return tree;
}

//if语句
Ast_node* if_stmt()
{
	Ast_node* left,*mid,*right=NULL;
	
	//匹配if
	match(if_op, "if");
	L_small();
	//计算判断表达式
	left = Prec_express(0);
	
	if (left->op<A_equal_op || left->op>A_less_then_or_equal_op)
	{
		fprintf(stderr, "error:非法的标识符,%d行\n",line);
		exit(1);
	}
	R_small();
	//再读取第一个语法块
	mid = compound_statement();

	//再寻找是否有else部分
	if (tok.token == else_op)
	{
		tok = scan();
		right = compound_statement();
	}

	left = make_node(A_if, left, mid, right, 0);
	return left;
}


//语句
Ast_node* compound_statement(void)
{
	Ast_node* left=NULL;
	Ast_node* tree;

	//读取左大括号
	L_big();
	while (1)
	{
		switch (tok.token)
		{
		case print_op:
			tree = print_stmt();
			break;
		case INT:
			var_declaration();
			tree = NULL;
			break;
		case INT_ident:
			tree = assignment_statement();
			break;
		case if_op:
			tree = if_stmt();
			break;
		case R_big_bracket:
			//只是一个空的大括号
			R_big();
			return left;
			break;
		default:
			fprintf(stderr, "error:读取到不符合语法规则的符号, %s ,%d行\n", tokens_type[tok.token], line);
			exit(1);
		}
		if (tree != NULL)
		{//如果右边挂了东西，那么再看看左子树
			//如果左子树没有挂东西，那么就放在左子树上，如果有，就做一个新的节点综合这两个子树
			if (left == NULL)
			{
				left = tree;
			}
			else
			{
				left = make_node(A_glue, left, NULL, tree, 0);
			}
		}
	}
}