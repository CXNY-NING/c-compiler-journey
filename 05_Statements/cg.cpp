#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>

#include"data.h"

static bool register_busy[4];
static const char* register_list[] = { "%r8","%r9","%r10" ,"%r11" };

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