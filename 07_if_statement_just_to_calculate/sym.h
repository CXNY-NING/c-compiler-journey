#ifndef Sym
#define Sym

typedef struct symbol
{
	char* name;
}symbol_table;

//查找一个一个变量在符号表中的位置，在返回对应下标，不在，返回-1
int find_sym(const char* name);

//加入一个新的符号
void add_new_sym(const char* name);
#endif // 
