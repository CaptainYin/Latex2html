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
}MAP[20];//�洢����Ԥ���ÿ��λ�ö�Ӧ���ս�������ս��������ʽ 
int k;
//��R����E',W����T',e�����
char G[10][10] = { "E->TR", "R->+TR", "R->e", "T->FW", "W->*FW", "W->e", "F->(E)", "F->i" };//�洢�ķ��еĲ���ʽ 
char VN[6] = { 'E', 'R', 'T', 'W', 'F' };//�洢���ս�� 
char VT[6] = { 'i', '+', '*', '(', ')', '#' };//�洢�ս�� 
char SELECT[10][10] = { "(,i", "+", "),#", "(,i", "*", "+,),#", "(", "i" };//�洢�ķ���ÿ������ʽ��Ӧ��SELECT�� 
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
	printf("\n                    �Է��Ŵ�%s�ķ�������\n", word);
	printf("    ����        ջ��Ԫ��        ʣ�����봮        �Ƶ����ò���ʽ��ƥ��\n");
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
				printf("            ����\n");
				return "SUCCESS";
			}
			printf("          ��%c��ƥ��\n", p);
			k++;
		}
		else
		{
			strcpy_s(action, Find(p, word[k]));
			if (strcmp(action, "error") == 0)
			{
				printf("        û�п��õĲ���ʽ\n");
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
	printf("\n*****Ϊ�˷����д������R����E',W����T',e�����*****\n\n");
	printf("���ķ��Ĳ���ʽ���£�\n");
	for (i = 0; i<8; i++)
		printf("   %s\n", G[i]);
	printf("________________________________________________________________________________\n");
	printf("\n���ķ���SELECT�����£�\n");
	for (i = 0; i<8; i++)
	{
		printf("   SELECT(%s) = { %s }\n", G[i], SELECT[i]);
	}
	printf("________________________________________________________________________________\n");
	//�ж��Ƿ���LL(1)�ķ� 
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
		printf("\n����ͬ�󲿲���ʽ��SELECT���ϵĽ���Ϊ�գ������ķ���LL��1���ķ���\n");
	else
		printf("\n����ͬ�󲿲���ʽ��SELECT���ϵĽ�����Ϊ�գ������ķ�����LL��1���ķ���\n");
	printf("________________________________________________________________________________\n");
	//Ԥ�������
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
	printf("\n���ʽ�ķ���Ԥ����������£�\n\n");
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
	/*Ԥ���������
	Analyse����*/
	//����Դ�ļ��� 
	while (cin >> source)
	{
		printf("\n���������%s\n\n", Analyse(source));
	}
	while (1);
	return 0;
}