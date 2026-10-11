#include"ast.h"

#ifndef Parser
#define Parser
//建立一个完整的抽象语法树
Ast_node* build_Ast(void);

//完整解释树，再返回答案
int InterpretAst(Ast_node* root);
#endif // !Parser

