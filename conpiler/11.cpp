#include"stdafx.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stack>
using namespace std;
struct Node1
{
	char vn;
	char vt;
	char s[10];
}MAP[20];//存储分析预测表每个位置对应的终结符，非终结符，产生式 
int k;
//用R代表E',W代表T',e代表空
char G[10][10] = { "E->TR", "R->+TR", "R->e", "T->FW", "W->*FW", "W->e", "F->(E)", "F->i" };//存储文法中的产生式 
char VN[6] = { 'E', 'R', 'T', 'W', 'F' };//存储非终结符 
char VT[6] = { 'i', '+', '*', '(', ')', '#' };//存储终结符 
char SELECT[10][10] = { "(,i", "+", "),#", "(,i", "*", "+,),#", "(", "i" };//存储文法中每个产生式对应的SELECT集 
char Right[10][8] = { "->TR", "->+TR", "->e", "->FW", "->*FW", "->e", "->(E)", "->i" };
stack <char> stak, stak1, stak2;
bool compare(char *a, char *b)
{
	int i, la = strlen(a), j, lb = strlen(b);
	for (i = 0; i<la; i++)
	for (j = 0; j<lb; j++)
	{
		if (a[i] == b[j])
			return 1;
	}
	return 0;
}
char *Find(char vn, char vt)
{
	int i;
	for (i = 0; i<k; i++)
	{
		if (MAP[i].vn == vn && MAP[i].vt == vt)
			return MAP[i].s;
	}
	return "error";
}
char * Analyse(char * word)
{
	char p, action[10], output[10];
	int i = 1, j, l = strlen(word), k = 0, l_act, m;
	while (!stak.empty())
		stak.pop();
	stak.push('#');
	stak.push('E');
	printf("________________________________________________________________________________\n");
	printf("\n                    对符号串%s的分析过程\n", word);
	printf("    步骤        栈顶元素        剩余输入串        推到所用产生式或匹配\n");
	p = stak.top();
	while (p != '#')
	{
		printf("%7d         ", i++);
		p = stak.top();
		stak.pop();
		printf("%6c          ", p);
		for (j = k, m = 0; j<l; j++)
			output[m++] = word[j];
		output[m] = '\0';
		printf("%10s", output);
		if (p == word[k])
		{
			if (p == '#')
			{
				printf("            接受\n");
				return "SUCCESS";
			}
			printf("          “%c”匹配\n", p);
			k++;
		}
		else
		{
			strcpy_s(action, Find(p, word[k]));
			if (strcmp(action, "error") == 0)
			{
				printf("        没有可用的产生式\n");
				return "ERROR";
			}
			printf("            %c%s\n", p, action);
			int l_act = strlen(action);
			if (action[l_act - 1] == 'e')
				continue;
			for (j = l_act - 1; j>1; j--)
				stak.push(action[j]);
		}
	}
	if (strcmp(output, "#") != 0)
		return "ERROR";
}
int main()
{
	freopen("in.txt", "r", stdin);
	//freopen("out.txt","w",stdout);
	char source[100];
	int i, j, flag, l, m;
	printf("\n*****为了方便编写程序，用R代表E',W代表T',e代表空*****\n\n");
	printf("该文法的产生式如下：\n");
	for (i = 0; i<8; i++)
		printf("   %s\n", G[i]);
	printf("________________________________________________________________________________\n");
	printf("\n该文法的SELECT集如下：\n");
	for (i = 0; i<8; i++)
	{
		printf("   SELECT(%s) = { %s }\n", G[i], SELECT[i]);
	}
	printf("________________________________________________________________________________\n");
	//判断是否是LL(1)文法 
	flag = 1;
	for (i = 0; i<8; i++)
	{
		for (j = i + 1; j<8; j++)
		{
			if (G[i][0] == G[j][0])
			{
				if (compare(SELECT[i], SELECT[j]))
				{
					flag = 0; break;
				}
			}
		}
		if (j != 8)
			break;
	}
	if (flag)
		printf("\n有相同左部产生式的SELECT集合的交集为空，所以文法是LL（1）文法。\n");
	else
		printf("\n有相同左部产生式的SELECT集合的交集不为空，所以文法不是LL（1）文法。\n");
	printf("________________________________________________________________________________\n");
	//预测分析表
	for (i = 0, k = 0; i<8; i++)
	{
		l = strlen(SELECT[i]);
		for (j = 0; j<l; j += 2)
		{
			MAP[k].vn = G[i][0];
			MAP[k].vt = SELECT[i][j];
			strcpy(MAP[k].s, Right[i]);
			k++;
		}
	}
	printf("\n表达式文法的预测分析表如下：\n\n");
	printf("          ");
	for (i = 0; i<6; i++)
		printf("%10c", VT[i]);
	printf("\n");
	for (i = 0; i<5; i++)
	{
		printf("        ---------------------------------------------------------------\n");
		printf("%10c", VN[i]);
		for (j = 0; j<6; j++)
		{
			for (m = 0; m<k; m++)
			{
				if (VN[i] == MAP[m].vn && VT[j] == MAP[m].vt)
				{
					printf("%10s", MAP[m].s);
					break;
				}
			}
			if (m == k)
				printf("          ");
		}
		printf("\n");
	}
	/*预测分析程序
	Analyse函数*/
	//输入源文件串 
	while (cin >> source)
	{
		printf("\n分析结果：%s\n\n", Analyse(source));
	}
	while (1);
	return 0;
}