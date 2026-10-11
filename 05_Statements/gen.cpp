#include"data.h"
#include"ast.h"
#include"cg.h"

//翻译AST
static int get_ans(Ast_node* root)
{
	int left_reg, right_reg;
	int op = root->op;

	//依次计算左右子树计算出的值放入的寄存器
	if (root->left_node != NULL)
	{//左
		left_reg = get_ans(root->left_node);
	}
	if (root->right_node != NULL)
	{//右
		right_reg = get_ans(root->right_node);
	}
	
	switch (op)
	{
		case int_add:
			return cg_add(left_reg, right_reg);
			break;
		case int_sub:
			return cg_sub(left_reg, right_reg);
			break;
		case int_mul:
			return cg_mul(left_reg, right_reg);
			break;
		case int_div:
			return cg_div(left_reg, right_reg);
			break;
		case int_node:
			return cg_int(root->int_value);
		default:
			fprintf(stderr, "error:检测到未能识别的内容 %d\n", op);
	}
}

int gen_ast(Ast_node* root)
{
	return get_ans(root);
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
//汇总函数
void general_code(Ast_node* root)
{
	//最终答案的目标寄存器
	int reg;

	//前置代码
	gen_preamble();

	reg=get_ans(root);
	//输出最终寄存器的值
	gen_printint(reg);

	//后置代码
	gen_postamble();
}