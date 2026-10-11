#include <stdio.h>
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

#endif // !Data
