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

	return tree;
}

//if语句
Ast_node* if_stmt(void)
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

//while循环
Ast_node* while_stmt(void)
{
	//读取while字符
	match(while_op, "while");

	//读取左括号
	L_small();

	Ast_node* condition;
	//读取布尔表达式
	condition = Prec_express(0);

	if (condition->op<equal_op || condition->op>less_then_or_equal_op)
	{
		fprintf(stderr, "error:在while语句建立的时候出现了表达式读取错误，请检查布尔表达式是否正确,%d行\n", line);
		exit(1);
	}

	//读取右括号
	R_small();

	Ast_node* stmt;
	stmt = compound_statement();
	Ast_node* res = make_node(A_while_op, condition, stmt, NULL, 0);

	return res;
}

//for语句
//我们将其拆解成两个部分，前置定义部分，while循环部分
Ast_node* for_stmt(void)
{
	//读取for (
	match(for_op, "for");
	L_small();

	//读取前置条件
	Ast_node* preop_stmt;
	preop_stmt = small_stmt();
	semi();
	//读取判断条件
	Ast_node* condition;
	condition = Prec_express(0);
	semi();

	//读取后置条件
	Ast_node* postop_stmt;
	postop_stmt = small_stmt();
	R_small();

	//执行语句
	Ast_node* run_stmt;
	run_stmt = compound_statement();

	//先将执行语句+后置条件打包起来,再将条件语句+它们以while的形式打包起来
	run_stmt = make_node(A_glue, run_stmt, NULL, postop_stmt, 0);
	run_stmt = make_node(A_while_op, condition, run_stmt, NULL, 0);

	//最后再将for打包起来
	Ast_node* tree;
	tree = make_node(A_glue, preop_stmt, NULL, run_stmt, 0);
	return tree;
}

//do_while循环
Ast_node* do_while_stmt(void)
{
	//读取do
	match(do_op, "do");

	//读取循环体
	Ast_node *run_stmt;
	run_stmt = compound_statement();

	//读取while (
	match(while_op, "while");
	L_small();

	//读取判断条件
	Ast_node *condition;
	condition = Prec_express(0);
	if (condition->op<equal_op || condition->op>less_then_or_equal_op)
	{
		fprintf(stderr, "error:在do_while语句建立的时候出现了表达式读取错误，请检查布尔表达式是否正确,%d行\n", line);
		exit(1);
	}

	//右括号 ;
	R_small();
	semi();

	Ast_node* tree;
	tree = make_node(A_do, condition, run_stmt, NULL, 0);
	return tree;
}

//简单语句
Ast_node* small_stmt(void)
{
	Ast_node* tree;
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
	case while_op:
		tree = while_stmt();
		break;
	case for_op:
		tree = for_stmt();
		break;
	case do_op:
		tree = do_while_stmt();
		break;
	default:
		fprintf(stderr, "error:读取到不符合语法规则的符号, %s ,%d行\n", tokens_type[tok.token], line);
		exit(1);
	}
	return tree;
}

//复杂语句
Ast_node* compound_statement(void)
{
	Ast_node* left=NULL;
	Ast_node* tree;

	//读取左大括号
	L_big();
	while (1)
	{
		if (tok.token == R_big_bracket)
		{
			//只是一个空的大括号
			R_big();
			return left;
		}
		
		tree = small_stmt();
		if (tree != NULL && (tree->op == A_print_op || tree->op == A_assign_op))
		{
			semi();
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

//函数
Ast_node* function(void)
{
	//读取void
	match(void_op, "void");

	//变量名
	ident();
	int name_point=add_new_sym(buffer);

	//()
	L_small();
	R_small();

	//读取函数内容
	Ast_node* tree = compound_statement();


	tree = make_only(A_void, tree, name_point);
	return tree;

}