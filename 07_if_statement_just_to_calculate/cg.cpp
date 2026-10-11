#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>

#include"data.h"

static bool register_busy[4];
static const char* register_list[] = { "%r8","%r9","%r10" ,"%r11" };
static const char* bregister_list[] = { "%r8b","%r9b","%r10b" ,"%r11b" };

//分配一个寄存器
int alloc_reg(void)
{
	for (int i = 0; i < 4; i++)
	{
		if (!register_busy[i])
		{
			register_busy[i] = true;
			return i;
		}
	}
	fprintf(stderr, "error:当前已无寄存器可分配\n");
	exit(1);
}

//回收一个寄存器
void free_reg(int reg_name)
{
	register_busy[reg_name] = false;
}

//回收所有的寄存器
void free_all_reg(void)
{
	for (int i = 0; i < 4; i++)
	{
		register_busy[i] = false;
	}
}
//加法
int cg_add(int left_reg, int right_reg)
{
	//输出代码
	fprintf(Out_file, "\taddq\t%s,%s\n", register_list[left_reg], register_list[right_reg]);
	//释放第一个寄存器，返回第二个寄存器
	free_reg(left_reg);
	return right_reg;
}

//减法
int cg_sub(int left_reg, int right_reg)
{
	//输出代码
	fprintf(Out_file, "\tsubq\t%s,%s\n", register_list[right_reg], register_list[left_reg]);
	//释放第二个寄存器，返回第一个寄存器
	free_reg(right_reg);
	return left_reg;
}
//乘法
int cg_mul(int left_reg, int right_reg)
{
	//输出代码
	fprintf(Out_file, "\timulq\t%s,%s\n", register_list[left_reg], register_list[right_reg]);
	//释放第一个寄存器，返回第二个寄存器
	free_reg(left_reg);
	return right_reg;
}
//除法
int cg_div(int left_reg, int right_reg)
{
	//除数放在%rax寄存器里面
	fprintf(Out_file, "\tmovq\t%s,%%rax\n", register_list[left_reg]);
	//再将%rax扩展成128位，即%rdx:%rax	
	fprintf(Out_file, "\tcqo\n");
	//%rdx:%rax除以right_reg中的除数
	fprintf(Out_file, "\tidivq\t%s\n", register_list[right_reg]);
	//商在%rax，余数在%rdx
	//再把商拷贝回去
	fprintf(Out_file, "\tmovq\t%%rax,%s\n", register_list[left_reg]);
	//释放另一个寄存器
	free_reg(right_reg);
	
	return left_reg;
}
//整数赋值
int cg_int(int value)
{
	//先由寄存器分配得到一个寄存器
	int reg = alloc_reg();

	//输出代码
	fprintf(Out_file, "\tmovq\t$%d,%s\n", value, register_list[reg]);
	
	//再返回这个已经赋值后的寄存器名称
	return reg;
}


//加载变量
int cg_loadglob(const char* name)
{
	//得到一个寄存器
	int reg = alloc_reg();

	//把变量放入寄存器
	fprintf(Out_file, "\tmovq\t%s(%%rip), %s\n", name, register_list[reg]);
	return reg;
}

//将值存入变量
int cg_storeglob(int reg, const char* name)
{
	//把变量
	fprintf(Out_file, "\tmovq\t%s, %s(%%rip)\n", register_list[reg], name);
	return reg;
}

//建立符号表
void cg_globsym(char* sym)
{
	fprintf(Out_file, "\t.comm\t%s,8,8\n", sym);
}

//由于汇编代码，在比较时只处理低八位，但是我们最后读取的时候是观察所有位数的，
//因此，我们需要将高56位，全部置0，所以，我们在输出比较指令时，需要有置0操作
static int cg_compare(int left_reg, int right_reg, const char* how_to_do)
{
	fprintf(Out_file, "\tcmpq\t%s, %s\t\n",register_list[right_reg],register_list[left_reg]);
	fprintf(Out_file, "\t%s\t%s\n", how_to_do, bregister_list[right_reg]);
	fprintf(Out_file, "\tandq\t$255,%s\n", register_list[right_reg]);
	free_reg(left_reg);
	return right_reg;
}

// 逻辑运算符 == != > >= < <=
int cg_equal(int left_reg, int right_reg)
{
	return cg_compare(left_reg, right_reg, "sete");
}

int cg_not_equal(int left_reg, int right_reg)
{
	return cg_compare(left_reg, right_reg, "setne");
}
int cg_great_than(int left_reg, int right_reg)
{
	{
		return cg_compare(left_reg, right_reg, "setg");
	}
}
int cg_great_than_equal(int left_reg, int right_reg)
{
	{
		return cg_compare(left_reg, right_reg, "setge");
	}
}
int cg_less_than(int left_reg, int right_reg)
{
	return cg_compare(left_reg, right_reg, "setl");
}
int cg_less_than_equal(int left_reg, int right_reg)
{
	return cg_compare(left_reg, right_reg, "setle");
}


//前置代码
void cg_preamble()
{
	free_all_reg();
	fputs(
		"\t.text\n"
		".LC0:\n"
		"\t.string\t\"%d\\n\"\n"
		"printint:\n"
		"\tpushq\t%rbp\n"
		"\tmovq\t%rsp, %rbp\n"
		"\tsubq\t$16, %rsp\n"
		"\tmovl\t%edi, -4(%rbp)\n"
		"\tmovl\t-4(%rbp), %eax\n"
		"\tmovl\t%eax, %esi\n"
		"\tleaq	.LC0(%rip), %rdi\n"
		"\tmovl	$0, %eax\n"
		"\tcall	printf@PLT\n"
		"\tnop\n"
		"\tleave\n"
		"\tret\n"
		"\n"
		"\t.globl\tmain\n"
		"\t.type\tmain, @function\n"
		"main:\n"
		"\tpushq\t%rbp\n"
		"\tmovq	%rsp, %rbp\n",
		Out_file);
}

//输出值
void cg_printint(int reg) 
{
	fprintf(Out_file, "\tmovq\t%s, %%rdi\n", register_list[reg]);
	fprintf(Out_file, "\tcall\tprintint\n");
	free_reg(reg);
}

//后置代码
void cg_postamble()
{
	fputs(
		"\tmovl	$0, %eax\n"
		"\tpopq	%rbp\n"
		"\tret\n",
		Out_file);
}