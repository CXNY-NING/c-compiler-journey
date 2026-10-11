#include<stdio.h>
#include<string.h>

#ifndef Token
#define Token
//暂时仅支持标识符 + - * / int值(0~9组成的数字) ; print int = 
//token 的定义
typedef struct token
{
	int token;//确定是哪一种表示符
	int int_value;//若是数字，则记录下值
}token;

//对现有的标识符编号 
// 结束符 end
// 运算符号 + - * / 
// 逻辑运算符 == != > >= < <=
// 值 int常量，int变量
// 标识符 ; print int 赋值符号 { } ( ) if else while for
enum tokens
{
	end,
	add_op, sub_op, mul_op, div_op,
	equal_op, not_equal_op, great_then_op, great_then_or_equal_op, less_then_op, less_then_or_equal_op,
	I_value, INT_ident,
	Semicolon, print_op, INT, assign_op, L_big_bracket, R_big_bracket, L_bracket, R_bracket, if_op, else_op,
	while_op, for_op, do_op, void_op
};

extern const char* tokens_type[];

#endif // !Token
