#include"token.h"
#include"data.h"
#include<ctype.h>
#include<string.h>
#include<stdlib.h>

static int next(void)
{
	//临时存储
	int cur;

	//若有多读取的情况，则先读取这个多读的东西
	if (Putback)
	{
		cur = Putback;
		Putback = 0;
		return cur;
	}

	//没有多读的情况，直接从已有的文本中在读取一个字符
	cur = fgetc(Target_file);
	//如果读到的这个字符是换行符，行数加一
	if (cur == '\n')
	{
		line++;
	}
	return cur;

}

static void putback(int cur)
{
	Putback = cur;
}

//跳过一些文本自带的无意义的符号
static int skip(void)
{
	int cur;
	
	//先读取一个符号
	cur = next();
	//在判断是否需要跳过，若需要，在读一个符号
	while(cur == '\n' || cur == '\t' || cur == '\r' || cur == '\f' || cur == ' ')
	{
		cur = next();
	}

	return cur;

}

//自动计算距离
static int distance(const char str[], char cur)
{
	int len = strlen(str);

	//一个一个比对，找到就返回下标，即数值
	for (int i = 0; i < len; i++)
	{
		if (cur == str[i])
		{
			return i;
		}
	}
	//没找到，则返回-1
	return -1;
}


//检测到数字，自动转化为完整int
static int scan_int(int cur)
{
	int value=0,temp;

	while ((temp=distance("0123456789", cur)) >= 0)
	{
		value = value * 10 + temp;
		cur = next();
	}
	
	putback(cur);
	return value;
}

//检测到英文，自动收集完整的英文单词
static void collect_word(int cur, char* buf, int Max_len)
{
	int i = 0;
	//循环检测，直至不是我们想要的字符
	while (isalpha(cur) || isdigit(cur) || cur == '_')
	{
		//不要超出缓存区(buf_len-1,还要留一个位置给\0)
		if (i < Max_len - 1)
		{
			buf[i] = cur;
			cur = next();
			i++;
		}
		else
		{
			fprintf(stderr, "error:过长的名称,%d 行\n", line);
			exit(1);
		}
	}

	//放回多余的字符
	putback(cur);
	//补上\0
	buf[i] = '\0';
}

//判断这段字符是不是我们的标识符
static int compare_keyword(char* buf)
{
	switch (*buf)
	{
		case 'e':
			if (strcmp(buf, "else") == 0)
			{
				return else_op;
				break;
			}
		case 'f':
			if (strcmp(buf, "for") == 0)
			{
				return for_op;
				break;
			}
		case 'i':
			if (strcmp(buf, "int") == 0)
			{
				return INT;
				break;
			}
			else if (strcmp(buf, "if") == 0)
			{
				return if_op;
				break;
			}
		case 'p':
			if (strcmp(buf, "print") == 0)
			{
				return print_op;
				break;
			}
		case 'w':
			if (strcmp(buf, "while") == 0)
			{
				return while_op;
				break;
			}
		default:
			return INT_ident;
	}
}

//读取文本，返回token
token scan(void)
{
	int cur;
	cur = skip();
	token temp;

	//判断符号
	switch (cur)
	{
		case EOF:
			temp.token = end;
			break;
		case '+':
			temp.token = add_op;
			break;
		case '-':
			temp.token = sub_op;
			break;
		case '*':
			temp.token = mul_op;
			break;
		case '/':
			temp.token = div_op;
			break;
		case ';':
			temp.token = Semicolon;
			break;
		case '{':
			temp.token = L_big_bracket;
			break;
		case '}':
			temp.token = R_big_bracket;
			break;
		case '(':
			temp.token = L_bracket;
			break;
		case ')':
			temp.token = R_bracket;
			break;	
		case '=':
			cur = next();
			//先判断等于逻辑判断符号
			if (cur != '=')
			{
				putback(cur);
				temp.token = assign_op;
			}
			else
			{
				temp.token = equal_op;
			}
			
			break;
		case '>':
			cur = next();
			//先判断等于逻辑判断符号
			if (cur != '=')
			{
				putback(cur);
				temp.token = great_then_op;
			}
			else
			{
				temp.token = great_then_or_equal_op;
			}
			
			break;
		case '<':
			cur = next();
			//先判断等于逻辑判断符号
			if (cur != '=')
			{
				putback(cur);
				temp.token = less_then_op;
			}
			else
			{
				temp.token = less_then_or_equal_op;
			}
			break;
		case '!':
			cur = next();
			if (cur == '=')
			{
				temp.token = not_equal_op;
			}
			else
			{
				fprintf(stderr, "error:出现了未曾设想到的字符，%c;期望：=\n", cur);
				exit(1);
			}
			break;
		default:
			//如果是数字，则记录下完整的数字
			if (isdigit(cur))
			{
				temp.token = I_value;
				temp.int_value = scan_int(cur);
				break;
			}
			//如果是英文，我们需要将这一整个英文单词读出来
			else if (isalpha(cur)||cur=='_')
			{
				collect_word(cur, buffer, Buf_Max);
				temp.token=compare_keyword(buffer);
				break;
			}
			//在识别不出来，那就是错误的输入
			temp.token = -1;
			temp.int_value = cur;
	}

	return temp;
}

//读取文本
void scanf_file()
{
	token temp;
	
	do
	{
		temp = scan();
		if (temp.token == -1)
		{
			printf("第%d行: 无法识别的字符: %c",line,temp.int_value);
			exit(1);
		}
		printf("第%d行: %s ",line, tokens_type[temp.token]);
		if (temp.token == I_value)
		{
			printf("%d", temp.int_value);
		}  
		else if (temp.token == INT_ident)
		{
			printf("%s", buffer);
		}
		printf("\n");
	} while (temp.token != end);
}