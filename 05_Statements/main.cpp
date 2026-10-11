#include<stdio.h>
#include<stdlib.h>
#include<errno.h>

#include"data.h"
#include"token.h"
#include"scanner.h"
#include"ast.h"
#include"Parser.h"
#include"cg.h"
#include"stmt.h"

int line;
int Putback;
FILE* Target_file;
const char* tokens_type[] = { "end","+","-","*","/","int",";","print"};
token tok;
Ast_node* Root;
FILE* Out_file;
char buffer[Buf_Max + 1];

void init()
{
	line = 1;
	Putback = ' ';
}


int main(int argc, char *argv[])
{
	//输入值检测
	if (argc != 2)
	{
		fprintf(stderr, "error: %s infile", argv[0]);
		exit(1);
	}

	init();
	
	//输入文本信息位置检测
	Target_file = fopen(argv[1], "r");
	if (Target_file == NULL)
	{
		fprintf(stderr, "error: Unable to open: %s : %s", argv[1],strerror(errno));
		exit(1);
	}
	
	//读取token
	scanf_file();
	fclose(Target_file);
/*
	//重新打开文件
	Target_file = fopen(argv[1], "r");
	//建立语法分析树
	tok = scan();
	Root = build_Ast();
	fclose(Target_file);

	//计算对应的值
	printf("第一种语法树：其表达式的值为：%d\n",InterpretAst(Root));
	//释放树的内存占用
	destroy_Ast(Root);

	//重新打开文件
	Target_file = fopen(argv[1], "r");
	//建立语法分析树
	tok = scan();
	Root = add_express();
	fclose(Target_file);

	//计算对应的值
	printf("第二种语法树：其表达式的值为：%d\n", InterpretAst(Root));
	//释放树的内存占用
	destroy_Ast(Root);

	//重新打开文件
	Target_file = fopen(argv[1], "r");
	//建立语法分析树
	tok = scan();
	Root =Prec_express(0);
	fclose(Target_file);

	//计算对应的值
	printf("正确语法树：其表达式的值为：%d\n", InterpretAst(Root));

	//翻译成汇编代码
	Out_file = fopen("output", "w");
	if (!Out_file)
	{
		fprintf(stderr, "error:不正确的输出地址\n");
		exit(1);
	}
	general_code(Root);
	printf("汇编代码已翻译完成\n");
	//释放树的内存占用
	destroy_Ast(Root);
*/


	//重新打开文件
	Target_file = fopen(argv[1], "r");
	tok = scan();
	//翻译成汇编代码
	Out_file = fopen("out.s", "wb");
	if (!Out_file)
	{
		fprintf(stderr, "error:不正确的输出地址\n");
		exit(1);
	}
	gen_preamble();
	stmt_expression();
	gen_postamble();
	printf("汇编代码已翻译完成\n");
	
	return 0;
}
