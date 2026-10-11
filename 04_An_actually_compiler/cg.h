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

//前置代码
void cg_preamble(void);

//输出代码
void cg_printint(int reg);

//后置代码
void cg_postamble(void);

//生成汇编代码
void general_code(Ast_node* root);
#endif // !cg
