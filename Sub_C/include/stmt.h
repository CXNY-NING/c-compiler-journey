#ifndef Stmt
#define Stmt

//输出语句
Ast_node* print_stmt(void);
//定义语句
void var_declaration(void);
//赋值语句
Ast_node* assignment_statement(void);
//if语句
Ast_node* if_stmt(void);
//while循环
Ast_node* while_stmt(void);
//for语句
Ast_node* for_stmt(void);
//简单语句
Ast_node* small_stmt(void);
Ast_node* compound_statement(void);

//函数
Ast_node* function(void);
#endif // !
