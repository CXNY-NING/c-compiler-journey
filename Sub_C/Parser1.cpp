#include"ast.h"
#include"token.h"
#include"data.h"
#include<stdlib.h>
#include"scanner.h"
#include"Parser.h"

//加法表达式
//从语法中，不难看出，最简单的加法表达式就是一个乘法表达式，
//那么，在语法树的构造之上，就体现在，加法表达式更靠近根，而乘法表达式更靠近叶子，实现了优先级
Ast_node* add_express(void)
{
	Ast_node* left, *right;
	int op;

	//考虑这颗子树中的乘法语句，提高乘法语句的优先级
	left = mul_express();

	//获取当前的符号
	op = connect(tok.token);
	if (tok.token == end)
	{
		return left;
	}
	tok = scan();

	//通过循环实现左结合
	while (1)
	{
		//先将高优先级的计算
		right = mul_express();
		//建立新的子树
		left = make_node(op, left, NULL, right, 0);
		//读取下一个字符
		op = connect(tok.token);
		//如果读到结束，那么表达式建立完成了
		if (tok.token == end)
		{
			break;
		}
		tok = scan();
	}
	return left;
}

//乘法表达式
Ast_node* mul_express(void)
{
	Ast_node* left, * right;
	int op;

	//左子树一定是基础节点
	left = Primary();
	
	//读取操作符
	tok = scan();
	op = connect(tok.token);
	if (tok.token == end)
	{
		return left;//已经读完了，直接返回左子树即可
	}

	//一直记录乘法表达式，直至出现加法表达式
	while ((op == A_mul_op) || (op == A_div_op))
	{
		tok = scan();
		right = Primary();
		left = make_node(op, left, NULL, right, 0);
		tok = scan();
		op = connect(tok.token);
		if (tok.token == end)
		{
			break;
		}
	}

	return left;
}