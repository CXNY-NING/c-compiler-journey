#include<stdlib.h>

#include"ast.h"
#include"data.h"
#include"token.h"
#include"cg.h"
#include"Parser.h"
#include"scanner.h"


//判断是否是我们所需要的
void match(int type, const char* str)
{
	if (tok.token == type)
	{
		tok = scan();
	}
	else
	{
		if (tok.token != INT)
		{
			fprintf(stderr, "error:未能找到期望的字符,%s, 期望：%s\n", tokens_type[tok.token], str);
		}
		else
		{
			fprintf(stderr, "error:未能找到期望的字符,%d, 期望：%s\n", tok.int_value, str);
			
		}
		exit(1);
	}
}

void semi()
{
	match(Semicolon, ";");
}


void stmt_expression(void)
{
	Ast_node* tree;
	int reg;

	while (1)
	{
		//去确认第一个字符是print
		match(print_op, "print");

		//读取这之后的表达式
		tree = Prec_express(0);
		reg = gen_ast(tree);	
		gen_printint(reg);
		gen_free_all_reg();

		semi();
		//释放树的内存占用
		destroy_Ast(tree);
		if (tok.token == end)
		{
			return;
		}
	}
}


