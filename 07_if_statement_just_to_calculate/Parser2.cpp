#include"ast.h"
#include"data.h"
#include"Parser.h"
#include"scanner.h"

#include<stdlib.h>

/*
	显然，在前一种方法中，我们能够看到，编写递归函数，少了很多麻烦，但是，
	在复杂的表达式面前，我们有一个无法避免的问题，那就是，递归调用次数过多！
	一旦递归调用次数过多，就会爆栈，并且在未来加入新的表达式，例如：乘方，对数，等等，
	就需要重新设计递归函数。
	于是，我们引入优先级（虽然也有，在前面），通过手动设置优先级，让计算机通过优先级，自动的去完成
	计算，不仅设计量小，同时，方便后期扩展。
*/

//首先，定义一些基础的优先级

static int OpPrec[] = {
						0,//end
						10,10,//+ -
						20,20,// * /
						30,30,//== !=
						40,40,40,40// lt,le,gt,ge
					  };//end  +  - * / 优先级依次递增

//绑定优先级
static int connect_Prec(token T)
{
	if (T.token >= end && T.token < I_value)
	{
		return OpPrec[T.token];
	}
	fprintf(stderr, "error:语法错误，输入符号有误，%d行\n",line);
	exit(1);
	return OpPrec[T.token];
}

//传入一个优先级，通过这个优先级，判断，我们读取的这一部分是高还是低优先级
Ast_node* Prec_express(int low_Prec)
{
	Ast_node* left, * right;
	int op;
	int now_Prec;

	//先完成对数字的读取
	left = Primary();

	//在观察符号
	tok = scan();
	if (tok.token == end || tok.token==Semicolon)
	{
		return left;//读完了 ;
	}
	op = connect(tok.token);
	now_Prec = connect_Prec(tok);

	while (now_Prec > low_Prec)
	{
		tok = scan();
		//先去计算高优先级的部分
		right = Prec_express(now_Prec);
		
		//再链接回来
		left = make_node(op, left, right, 0);
		if (tok.token == end || tok.token == Semicolon)
		{
			return left;
		}
		op = connect(tok.token);
		now_Prec = connect_Prec(tok);
	}
	return left;
}



