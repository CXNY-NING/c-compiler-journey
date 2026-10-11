#include <stdio.h>
#include"sym.h"
#include"token.h"
#include"ast.h"

#ifndef Data
#define Data
extern int line;//确定行数
extern int Putback;//取回符号
extern FILE* Target_file;//读取目标文件
extern token tok;
extern Ast_node* Root;
extern FILE* Out_file;//输出文件
#define Buf_Max 512
extern char buffer[Buf_Max + 1];//缓存区
#define Gsymnum 1024
extern symbol_table Global_sym[Gsymnum];
static int G_sym_num = 0;

#endif // !Data
