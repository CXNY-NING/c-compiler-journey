#include<stdlib.h>

#include"ast.h"
#include"data.h"
#include"token.h"
#include"cg.h"
#include"Parser.h"
#include"scanner.h"
#include"sym.h"

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

//输出语句
void print_stmt(void)
{
	Ast_node* tree;
	int reg;
	//去确认第一个字符是print
	match(print_op, "print");

	//读取这之后的表达式
	tree = Prec_express(0);
	reg = gen_ast(tree,-1);	
	gen_printint(reg);
	gen_free_all_reg();

	semi();
	//释放树的内存占用
	destroy_Ast(tree);
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
void assignment_statement(void)
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
	tree = make_node(A_assign_op, left, right, 0);

	//翻译赋值语句
	gen_ast(tree, -1);

	//结束
	semi();
}

//语句
void statements(void)
{
	while (1)
	{
		switch (tok.token)
		{
			case print_op:
				print_stmt();
				break;
			case INT:
				var_declaration();
				break;
			case INT_ident:
				assignment_statement();
				break;
			case end:
				return;
			default:
				fprintf(stderr, "error:读取到不符合语法规则的符号, %s ,%d行\n", tokens_type[tok.token], line);
				exit(1);
		}
	}
}