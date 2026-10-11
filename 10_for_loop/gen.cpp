#include"data.h"
#include"ast.h"
#include"cg.h"

//翻译AST
static int get_ans(Ast_node* root ,int reg,int upper_op)
{
	int left_reg, right_reg;
	int op = root->op;
	switch (op)
	{
		case A_if:
			return get_if_ast(root);
			break;
		case A_while_op:
			return get_while_ast(root);
			break;
		case A_glue:
			get_ans(root->left_node, -1, op);
			free_all_reg();
			get_ans(root->right_node, -1, op);
			free_all_reg();
			return -1;
	}

	if (root->left_node)
	{
		left_reg = get_ans(root->left_node,-1,op);
	}
	if (root->right_node)
	{
		right_reg = get_ans(root->right_node,left_reg,op);
	}
	switch (op)//+ - * / int semicolon ident = 
	{
		case A_add_op:
			return cg_add(left_reg, right_reg);
			break;
		case A_sub_op:
			return cg_sub(left_reg, right_reg);
			break;
		case A_mul_op:
			return cg_mul(left_reg, right_reg);
			break;
		case A_div_op:
			return cg_div(left_reg, right_reg);
			break;
		case A_I_value:
			return cg_int(root->v.int_value);
			break;
		case A_INT_ident:
			return cg_loadglob(Global_sym[root->v.id].name);
			break;
		case A_Lident:
			return cg_storeglob(reg, Global_sym[root->v.id].name);
			break;
		case A_assign_op:
			return right_reg;
			break;
		case A_equal_op:
		case A_not_equal_op:			
		case A_great_then_op:		
		case A_great_then_or_equal_op:			
		case A_less_then_op:			
		case A_less_then_or_equal_op:
			if (upper_op == A_if|| upper_op == A_while_op)
			{
				return cg_compare_and_jump(root->op, left_reg, right_reg, reg);
			}
			else
			{
				return cg_compare_and_set(root->op, left_reg, right_reg);
			}
		case A_print_op:
			cg_printint(left_reg);
			free_all_reg();
			return -1;
			break;
		default:
			fprintf(stderr, "error:检测到未能识别的内容 %d\n", op);
	}
}

//if语句翻译的的前置工作
static int New_label(void)
{
	return label++;
}


//if语法树的翻译
int get_if_ast(Ast_node* root)
{
	//我们需要两个出口，if之后紧跟的语法块中的结尾位置，和else结束之后的语法块位置
	int Lfalse, Lend;

	Lfalse = New_label();
	//如果有else
	if (root->right_node)
	{
		Lend = New_label();
	}

	//计算判断语句,并且跳转到不符合该条件的位置去
	get_ans(root->left_node, Lfalse, root->op);
	free_all_reg();

	//翻译if之后的语法块
	get_ans(root->mid_node, -1, root->op);
	free_all_reg();

	//若有else，一旦进入这个语法块，就要跳转到else语法块执行完的地方
	if (root->right_node)
	{
		cg_jump(Lend);
	}

	//现在放下为否的标签
	cg_label(Lfalse);

	//如果有else，则继续翻译else部分
	if (root->right_node)
	{
		get_ans(root->right_node, -1,root->op);
		free_all_reg();
		cg_label(Lend);
	}
	return -1;
}

//while语法树的翻译
int get_while_ast(Ast_node* root)
{
	//我们需要两个标签，lstart：当条件为真时，自动跳转到这个位置。lend:当条件为假时，直接继续后边的代码
	int lstart, lend;
	lstart = New_label();
	lend = New_label();

	//先放置入口label，再计算表达式
	cg_label(lstart);
	get_ans(root->left_node, lend, root->op);
	free_all_reg();
	//再翻译条件为真时的语句
	get_ans(root->mid_node, -1, root->op);
	free_all_reg();
	//跳转至开头
	cg_jump(lstart);

	//放置出口
	cg_label(lend);

	return -1;
}


int gen_ast(Ast_node* root)
{
	return get_ans(root, -1, 0);
}

void gen_printint(int reg)
{
	 cg_printint(reg);
}

void gen_postamble(void)
{
	cg_postamble();
}

void gen_preamble(void)
{
	cg_preamble();
}

void gen_free_all_reg(void)
{
	free_all_reg();
}

void gen_globsym(char* sym)
{
	cg_globsym(sym);
}


//汇总函数
void general_code(Ast_node* root)
{
	//最终答案的目标寄存器
	int reg;

	//前置代码
	gen_preamble();

	reg=get_ans(root,-1,root->op);
	//输出最终寄存器的值
	gen_printint(reg);

	//后置代码
	gen_postamble();
}