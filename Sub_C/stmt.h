#ifndef Stmt
#define Stmt

//Êä³öÓï¾ä
Ast_node* print_stmt(void);
//¶¨ÒåÓï¾ä
void var_declaration(void);
//¸³ÖµÓï¾ä
Ast_node* assignment_statement(void);
//ifÓï¾ä
Ast_node* if_stmt(void);
//whileÑ­»·
Ast_node* while_stmt(void);
//forÓï¾ä
Ast_node* for_stmt(void);
//¼òµ¥Óï¾ä
Ast_node* small_stmt(void);
Ast_node* compound_statement(void);

//º¯Êı
Ast_node* function(void);
#endif // !
