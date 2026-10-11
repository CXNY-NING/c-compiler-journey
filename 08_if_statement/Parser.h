#include"ast.h"

#ifndef Parser
#define Parser
//将token中的类型与ast中的操作数链接起来 token->ast_op
int connect(int tok);

//基本的抽象语法树单元
Ast_node* Primary(void);

//建立一个完整的抽象语法树
Ast_node* build_Ast(void);

//完整解释树，再返回答案
int InterpretAst(Ast_node* root);

//加法语句
Ast_node* add_express(void);

//乘法语句
Ast_node* mul_express(void);

//传入一个优先级，通过这个优先级，判断，我们读取的这一部分是高还是低优先级
Ast_node* Prec_express(int low_Prec);

#endif // !Parser

