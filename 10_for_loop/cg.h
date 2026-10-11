#ifndef cg
#define cg

//加法
int cg_add(int left_reg, int right_reg);
//减法
int cg_sub(int left_reg, int right_reg);
//乘法
int cg_mul(int left_reg, int right_reg);
//除法
int cg_div(int left_reg, int right_reg);
//整数赋值
int cg_int(int value);
//回收所有的寄存器
void free_all_reg(void);

//加载变量
int cg_loadglob(const char* name);

//将值存入变量
int cg_storeglob(int reg, const char* name);

//建立符号表
void cg_globsym(char* sym);

void gen_globsym(char* sym);

// 逻辑运算符 == != > >= < <=
/*
int cg_equal(int left_reg, int right_reg);
int cg_not_equal(int left_reg, int right_reg);
int cg_great_than(int left_reg, int right_reg);
int cg_great_than_equal(int left_reg, int right_reg);
int cg_less_than(int left_reg, int right_reg);
int cg_less_than_equal(int left_reg, int right_reg);

int gen_equal(int left_reg, int right_reg);
int gen_not_equal(int left_reg, int right_reg);
int gen_less_than(int left_reg, int right_reg);
int gen_great_than_equal(int left_reg, int right_reg);
int gen_less_than(int left_reg, int right_reg);
int gen_less_than_equal(int left_reg, int right_reg);
*/
//if语法树的翻译
int get_if_ast(Ast_node* root);

//while语法树的翻译
int get_while_ast(Ast_node* root);

//跳转
int cg_compare_and_jump(int compare_op, int left_reg, int right_reg, int label);

//放置标签
void cg_label(int Label);

//跳转到标签
void cg_jump(int Label);

int cg_compare_and_set(int left_reg, int right_reg, int compare_op);

//语句
void statements(void);

void gen_preamble(void);
//前置代码
void cg_preamble(void);

void gen_printint(int reg);
//输出代码
void cg_printint(int reg);

void gen_postamble(void);
//后置代码
void cg_postamble(void);

void gen_free_all_reg(void);

void free_all_reg(void);

int gen_ast(Ast_node* root);

//生成汇编代码
void general_code(Ast_node* root);
#endif // !cg
