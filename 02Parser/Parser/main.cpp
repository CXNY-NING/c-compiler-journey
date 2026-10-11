#include<stdio.h>
#include<stdlib.h>
#include<errno.h>

#include"data.h"
#include"token.h"
#include"scanner.h"
#include"ast.h"
#include"Parser.h"

int line;
int Putback;
FILE* Target_file;
const char* tokens_type[] = { "end","+","-","*","/","int" };
token tok;
Ast_node* Root;

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
	
	//重新打开文件
	Target_file = fopen(argv[1], "r");
	//建立语法分析树
	tok = scan();
	Root = build_Ast();
	fclose(Target_file);

	//计算对应的值
	printf("其表达式的值为：%d\n",InterpretAst(Root));
	//释放树的内存占用
	destroy_Ast(Root);

	return 0;
}
