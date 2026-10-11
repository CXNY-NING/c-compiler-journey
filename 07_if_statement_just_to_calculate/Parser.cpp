#include"ast.h"
#include"token.h"
#include"data.h"
#include<stdlib.h>
#include"scanner.h"

/*
//将token中的类型与ast中的操作数链接起来 token->ast_op
int connect(int tok)
{
	int res;
	switch (tok)
	{
		case add_op:
			res = int_add;// +
			break;
		case sub_op:
			res = int_sub;// -
			break;
		case mul_op:
			res = int_mul;// *
			break;
		case div_op:
			res = int_div;// /
			break;
		case I_value:
			res = int_node;//int
			break;
		case end:
			res = ast_end;//end
			break;
		case Semicolon:
			res = A_sem;
			break;
		case INT_ident:
			res = A_ident;
			break;
		case assign_op:
			res = A_Assign;
			break;
		default:
			//未知符号
			fprintf(stderr, "error:未知操作数,在%d行\n", line);
			exit(1);
	}

	return res;
}
*/

int connect(int tok)
{
	if (tok > 0 && tok < I_value)
	{
		return tok;
	}
	fprintf(stderr, "error:语法错误,%d行\n",line);
	exit(1);
}




//基本的抽象语法树单元
Ast_node* Primary(void)
{
	Ast_node* temp_ast_node;
	int id;

	switch (tok.token)
	{
		case I_value://零元操作码
			temp_ast_node = make_leaf(A_I_value,tok.int_value);
			break;
		case INT_ident:
			id = find_sym(buffer);
			if (id == -1)
			{
				fprintf(stderr, "error:未定义的元素%s ,%d行\n", buffer, line);
				exit(1);
			}
			temp_ast_node = make_leaf(A_INT_ident, id);
			break;
		default://不符合现有语法
			fprintf(stderr, "error:出现了不符合现有语法的语句，出现在第%d行\n", line);
			exit(1);
	}
	return temp_ast_node;
}

//建立一个完整的抽象语法树
Ast_node* build_Ast(void)
{
	Ast_node* root, *left, *right;
	int op;
	printf("%d \n", tok.int_value);
	//先处理左子树，一个合法的表达式当然是以数字开头，当然，我们这里先不考虑括号问题
	left = Primary();

	//再读取下一个字符，符号
	tok = scan();
	if (tok.token == end)
	{//就把左子树直接返回
		return left;
	}
	printf("%s", tokens_type[tok.token]);
	op = connect(tok.token);


	//再处理右子树,先读取一个数字
	tok = scan();
	right = build_Ast();

	//最后将左右子树挂在当前根节点的左右即可，
	root = make_node(op, left, right, 0);
	return root;
}
//注：这样写，当然是错的，我们现在不考虑优先级，默认大家所有人的优先级的是一致的
//并且，我们会发现，这样的递归函数，越早出现的数字会越晚去进行计算，每个节点的左子树只有一层
//但是右子树，会有很多层

//完整解释树，再返回答案
int InterpretAst(Ast_node* root)
{
	//先处理左子树的答案
	int left_ans;
	if (root->left_node != NULL)
	{//如果有左子树，那么，递归计算
		left_ans = InterpretAst(root->left_node);
	}

	//再处理右子树
	int right_ans;
	if(root->right_node!=NULL)
	{//如果有右子树，那么，递归计算
		right_ans = InterpretAst(root->right_node);
	}

	//当，左右子树的值都计算完毕时，我们根据当前节点的op来确定计算方法
	int ans;
	switch (root->op)
	{
		case A_add_op:
			ans = left_ans + right_ans;
			printf("%d + %d\n", left_ans, right_ans);
			break;
		case A_sub_op:
			ans = left_ans - right_ans;
			printf("%d - %d\n", left_ans, right_ans);
			break;
		case A_mul_op:
			ans = left_ans * right_ans;
			printf("%d * %d\n", left_ans, right_ans);
			break;
		case A_div_op:
			ans = left_ans / right_ans;
			printf("%d / %d\n", left_ans, right_ans);
			break;
		case A_I_value:
			ans = root->v.int_value;
			break;
		default:
			fprintf(stderr, "error:遇到了无法识别的表示符 %d", root->op);
			exit(1);
	}
	return ans;
}