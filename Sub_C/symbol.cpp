#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include"token.h"
#include"sym.h"
#include"data.h"
//增、删、改、查

//加入一个新的符号
int add_new_sym(const char* name)
{
	//若以前定义过了，直接返回
	int name_point = find_sym(name);
	if (name_point != -1)
	{
		return name_point;
	}
	//有位置，就加入，没有，则报错
	if (G_sym_num < Gsymnum)
	{
		Global_sym[G_sym_num].name = _strdup(name);
		G_sym_num++;
		return G_sym_num - 1;
	}
	else
	{
		fprintf(stderr, "error:符号表已满，%d行\n", line);
		exit(1);
	}
}

//查找一个一个变量在符号表中的位置，在返回对应下标，不在，返回-1
int find_sym(const char* name)
{
	for (int i = 0; i < G_sym_num; i++)
	{
		if (strcmp(name, Global_sym[i].name)==0)
		{//找到了
			return i;
		}
	}
	return -1;
}


