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

//对现有的标识符编号 + - * / int ; print
enum tokens
{
	end, add_op, sub_op, mul_op, div_op, I_value, Semicolon, print_op, INT, equal_op, INT_ident
};

extern const char* tokens_type[];

#endif // !Token
