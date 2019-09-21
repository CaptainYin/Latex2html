// conpiler.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include<iostream>
#include<fstream>
#include<string>
#include<stack>
#include<sstream>
#include<algorithm>
#include<vector>
#include<fstream>
using namespace std;
const string syntax[11] = { "B", "BB", "B_^{B}{B}", "B^{B}",
"B_{B}", "T{B}{B}{B}", "M{B}{B}{B}",
"i", "n", " ", "(B)" };//T:\int,M:\sum,i:id,n:num
struct item{
	int synind;
	int point_pos;//0表示点再最前面
	void item::print();
	void item::print2file(ofstream &ofile);//输出到文件
};
void item::print(){
	if (synind == 0)
	{
		cout << "S->";
	}
	else
	{
		cout << "B->";
	}
	if (point_pos == 0)
	{//点在第一个
		cout << "·" << syntax[synind] << endl;
		return;
	}
	else
	{
		if (point_pos == syntax[synind].length())
		{//点在最后一个
			cout << syntax[synind] << "·" << endl;
			return;
		}
		else
		{
			for (int i = 0; i < point_pos; i++)
			{
				cout << syntax[synind][i];
			}
			cout << "·";
			for (int i = point_pos; i < syntax[synind].length(); i++)
			{
				cout << syntax[synind][i];
			}
			cout << endl;
		}
	}
}
void item::print2file(ofstream& ofile)
{
	if (synind == 0)
	{
		ofile << "S->";
	}
	else
	{
		ofile << "B->";
	}
	if (point_pos == 0)
	{//点在第一个
		ofile << "·" << syntax[synind] << endl;
		return;
	}
	else
	{
		if (point_pos == syntax[synind].length())
		{//点在最后一个
			ofile << syntax[synind] << "·" << endl;
			return;
		}
		else
		{
			for (int i = 0; i < point_pos; i++)
			{
				ofile << syntax[synind][i];
			}
			ofile << "·";
			for (int i = point_pos; i < syntax[synind].length(); i++)
			{
				ofile << syntax[synind][i];
			}
			ofile << endl;
		}
	}
}
typedef vector<item> setOfItems;
bool exist(setOfItems I, item bgen){
	for each (item var in I)
	{
		if (var.synind == bgen.synind&&var.point_pos == bgen.point_pos)
		{
			return true;
		}
	}
	return false;
}
//因为只有一个非终结符B，因此非核心项目就是B的产生式，·在第一个
void addBgen(setOfItems &I, bool &updateflag)
{
	item bgen;
	bgen.point_pos = 0;
	for (int i = 1; i < 11; i++)
	{
		bgen.synind = i;
		if (exist(I, bgen))
		{
			continue;
		}
		I.push_back(bgen);
		updateflag = true;
	}
}
//求项目集闭包
void closure(setOfItems &I){
	while (true)
	{
		bool updateflag = false;
		for each (item var in I)
		{
			if (syntax[var.synind].length() == var.point_pos)
			{
				continue;
			}
			char ch = syntax[var.synind][var.point_pos];
			if (ch != 'B')
			{
				continue;
			}
			addBgen(I, updateflag);
			break;
		}
		if (updateflag == false)
		{
			break;
		}
	}
}

//DFA转移符号集
const string x = { 'B','(', ')', '{', '}', '_', '^',  'T', 'M', 'n', 'i', ' ' };
const string non_terminal = { '(', ')', '{', '}', '_', '^', 'T', 'M', 'n', 'i', ' ', '$' };
const string follow_B = non_terminal;
//转移函数
setOfItems Goto(const setOfItems I, char x)
{
	setOfItems ret;
	item it;
	for each (item var in I)
	{
		if (syntax[var.synind].length() == var.point_pos)
		{
			continue;
		}
		if (syntax[var.synind][var.point_pos] != x)
		{
			continue;
		}
		it.synind = var.synind;
		it.point_pos = var.point_pos + 1;
		ret.push_back(it);
	}
	closure(ret);
	return ret;
}
struct Gototype{
	char x;
	int toState=-1;
};
struct IGotoX{
	Gototype *toX;
	IGotoX(){
		int length = x.length();
		toX = new Gototype[length];
		for (int i = 0; i < length; i++)
		{
			toX[i].x = x[i];
		}
	}
};
//项目it是否存在于规范族
int ItemsetExist(vector<setOfItems>itemsetf, setOfItems it)
{
	size_t len = it.size();
	for (int i = 0; i < itemsetf.size(); i++)
	{
		setOfItems var = itemsetf[i];
		if (var.size() != len)
		{
			continue;
		}
		bool allin = true;
		for each (item itemvar in it)
		{
			if (!exist(var, itemvar))
			{
				allin = false;
			}
		}
		if (allin == false)
		{
			continue;
		}
		else
		{
			return i;
		}
	}
	return -1;
}
struct ItemSet_Goto{
	vector<setOfItems> ItemSets;
	vector<IGotoX> GotoTable;
};
//输入文法，输出项目集规范族 and goto_table
ItemSet_Goto Items(){
	vector<IGotoX>GotoTable;
	ItemSet_Goto ret;
	vector<setOfItems> itemsetf;
	item init;
	init.synind = 0;
	init.point_pos = 0;
	setOfItems seti;
	seti.push_back(init);//加入S->·B并求闭包
	closure(seti);
	itemsetf.push_back(seti);//加入I0;
	while (true){
		bool updateflag = false;
		setOfItems Ivar;
		IGotoX I2X;
		for (int i = 0; i < itemsetf.size(); i++)
		{
			Ivar = itemsetf[i];
			for (int j = 0; j < x.length(); j++)
			{
				char xchar = x[j];
				seti = Goto(Ivar, xchar);
				int c = ItemsetExist(itemsetf, seti);
				if (!seti.empty() && c == -1)
				{//不为空且不存在
					itemsetf.push_back(seti);
					updateflag = true;
					I2X.toX[j].toState = itemsetf.size() - 1;//此时就是转换到最后一个
				}
				else
				{
					if (seti.empty())
					{//为空，不存在的转化
						I2X.toX[j].toState = -1;
					}
					else
					{//不为空,也就是已经有的state
						I2X.toX[j].toState = c;
					}
				}
			}
		}
		if (!updateflag)
		{
			break;
		}
	}
	ret.GotoTable = GotoTable;
	ret.ItemSets= itemsetf;
	return ret;
}
//输出项目集
void printItems(vector<setOfItems> C)
{
	int c = 0;
	for each (setOfItems itset in C)
	{
		cout << "I" << c << endl;
		c++;
		for each (item var in itset)
		{
			var.print();
		}
	}
}
//输出到文件
void printItems2file(vector<setOfItems> C, char*filename)
{
	ofstream ofile(filename);
	ofile << "LR(0)项目集规范族大小 :" << C.size() << endl;
	int c = 0;
	for each (setOfItems itset in C)
	{
		ofile << "I" << c << endl;
		c++;
		for each (item var in itset)
		{
			var.print2file(ofile);
		}
		ofile << endl;
	}
	ofile.close();
}
struct actiontype{
	char action = ' ';
	//default do nothing,{'a','s','r',' '},'a'=acc,'s'=shift in and goto state
	//'j'=reduce and using syntax[ind] 
	//' '=blank means error;
	int ind;
	int state;
};
//SLR_Analysis_Table consist of action_table and transfer_table
struct SLR_Analysis_Table{
	actiontype** ActionTable;
	int *Tansfer_table;//cause only terminal symbol B
};
SLR_Analysis_Table GenSLR_Action_Table(const ItemSet_Goto A)
{
	SLR_Analysis_Table ret;
	const vector<setOfItems> C = A.ItemSets;
	const vector<IGotoX>goto_t = A.GotoTable;
	int statenum = C.size();
	int *Transfer_table = new int[statenum];
	for (size_t i = 0; i < statenum; i++)
	{
		Transfer_table[i] = -1;
	}
	int non_terminal_num = non_terminal.length();
	actiontype **actiontable = new actiontype*[statenum];
	for (int i = 0; i < statenum; i++)
	{
		actiontable[i] = new actiontype[non_terminal_num];
	}
	for (size_t i = 0; i < statenum; i++)
	{
		setOfItems its = C[i];
		for (size_t j = 0; j < C[i].size(); j++)
		{
			item it = its[j];
			
			if (it.synind == 0)
			{
				if (it.point_pos == 1)
				{
					//if S->B· exist in itemset-i then set action[i,$]=acc;
					//as $ is the last symbol of non_terminal
					actiontable[i][non_terminal.length() - 1].action = 'a';
				}
				else
				{
					Transfer_table[i] = goto_t[i].toX[0].toState;
				}
			}
			else
			{
				if (it.point_pos==syntax[it.synind].length())
				{//点在末尾，归约
					for (size_t k = 0; k < non_terminal.length(); k++)
					{
						actiontable[i][k].action = 'r';//reduce by syntax[it.synind]
						actiontable[i][k].ind = it.synind;
					}
					
				}
				else
				{
					char ch = syntax[it.synind][it.point_pos];
					//P79(a)
				}
			}
		}

	}
	ret.Tansfer_table = Transfer_table;
	ret.ActionTable = actiontable;
	return ret;
}
enum NoteType
{
	id, num
};
struct Notation{
	string note;
	NoteType type;
	int ind;
	Notation(string s, NoteType t) :note(s), type(t){};
};
vector<Notation>NoteList;
//输出符号列表。。。。。？要更换i:id,n:num,好吧
void OutputList(void){
	for each (Notation var in NoteList)
	{
		if (var.type == id)
		{
			cout << "id" << var.ind << '\t' << var.note << endl;

		}
		else
		{
			cout << "num" << var.ind << '\t' << var.note << endl;
		}
	}
}
//
vector<Notation>InstallIntoList(string id_num, NoteType t){
	Notation n(id_num, t);
	int c = 0;
	for each (Notation var in NoteList)
	{
		if (var.note.compare(id_num) == 0)
		{
			if (t == id)
			{
				cout << "id" << c + 1;
			}
			else
			{
				cout << "num" << c + 1;
			}
			return NoteList;
		}
		if (var.type == t)
		{
			c++;
		}
	}
	n.ind = c + 1;
	NoteList.push_back(n);
	if (t == id)
	{
		cout << "id" << c + 1;
	}
	else
	{
		cout << "num" << c + 1;
	}
	return NoteList;
}
//大括号匹配
bool BigBracketMatch(const string B, int pos){
	stack<char> bigbracket;
	size_t length = B.length();
	int ec = 0;
	for (size_t i = 0; i < length; i++)
	{
		if (B[i] == '{')
		{
			bigbracket.push(B[i]);
		}
		else
		{
			if (B[i] == '}')
			{
				if (bigbracket.empty())
				{
					ec = 1;//too much '}'
					cout << "Big Bracket can not match at POSITION:" << pos + i << endl;
					cout << "too much '}'" << endl;
					cout << B << endl;
					return false;
				}
				bigbracket.pop();
			}
		}
	}
	if (!bigbracket.empty())
	{
		ec = 2;//too much '{'
		cout << "Big Bracket can not match at POSITION:" << pos << endl;
		cout << "too much '{'" << endl;
		cout << B << endl;
		return false;
	}
	return true;
}
//小括号匹配
bool LittleBracketMatch(const string B, int pos){
	stack<char> bigbracket;
	size_t length = B.length();
	int ec = 0;
	for (size_t i = 0; i < length; i++)
	{
		if (B[i] == '(')
		{
			bigbracket.push(B[i]);
		}
		else
		{
			if (B[i] == ')')
			{
				if (bigbracket.empty())
				{
					ec = 1;//too much ')'
					cout << "Little Bracket can not match at POSITION:" << pos + i << endl;
					cout << "too much ')'" << endl;
					cout << B << endl;
					return false;
				}
				bigbracket.pop();
			}
		}
	}
	if (!bigbracket.empty())
	{
		ec = 2;//too much '('
		cout << "Little Bracket can not match at POSITION:" << pos << endl;
		cout << "too much '('" << endl;
		cout << B << endl;
		return false;
	}
	return true;
}
bool BracketMatch(const string B){
	return BigBracketMatch(B, 0) && LittleBracketMatch(B, 0);
}
//judge Special charcter \,(,),{,},_,^,'\0','\n'
bool isSpeciNotation(char c){

	if (c == '(' || c == ')' || c == '{' || c == '}' || c == '_' || c == '^' || c == '\\' || c == ' ' || c == '\n')
	{
		return true;
	}
	return false;
}
//judge charcter whether expected
bool NoOtherNotation(const string B){
	char c;
	for (size_t i = 0; i < B.length(); i++)
	{
		c = B.at(i);
		if (!isalnum(c) && !isSpeciNotation(c)){
			//输出意外符号的ascii和位置
			cout << "These is an unexpected notation: ASCII:" << toascii(c) << endl;
			cout << "Position:" << i << endl;
			return false;
		}
	}
	return true;
}
//read sample.txt in
string readFile2String(char*filename){
	ifstream ifile(filename);
	ostringstream buf;
	char ch;
	while (buf&&ifile.get(ch))
		buf.put(ch);
	return buf.str();
}
string MatchNum(const string B, size_t &p){
	string ret;
	if (!isdigit(B[p]))
	{
		cout << "Function Call Error:MatchNum" << endl;
		return ret;
	}
	for (; p < B.length(); p++)
	{
		if (isdigit(B[p])){
			ret.push_back(B[p]);
		}
		else
		{
			break;
		}
	}
	return ret;
}
string MatchId(const string B, size_t &p){
	string ret;
	if (!isalpha(B[p]))
	{
		cout << "Function Call Error:MatchId" << endl;
		return ret;
	}
	for (; p < B.length(); p++)
	{
		if (isalnum(B[p])){
			ret.push_back(B[p]);
		}
		else
		{
			break;
		}
	}
	return ret;
}
bool MatchBlank(const string B, size_t &p){
	if (p + 5 >= B.length())
	{
		return false;
	}
	string b = "blank";
	if (b.compare(B.substr(p + 1, 5)) == 0){
		p += 6;

		return true;
	}
	else
	{
		return false;
	}
}
bool MatchInt(const string B, size_t &p){
	if (p + 3 >= B.length())
	{
		return false;
	}
	string b = "int";
	if (b.compare(B.substr(p + 1, 3)) == 0){
		p += 4;
		return true;
	}
	else
	{
		return false;
	}
}
bool MatchSum(const string B, size_t &p){
	if (p + 3 >= B.length())
	{
		return false;
	}
	string b = "sum";
	if (b.compare(B.substr(p + 1, 3)) == 0){
		p += 4;
		return true;
	}
	else
	{
		return false;
	}
}
vector<Notation> InstallNote(const string B){
	size_t p = 0, length = B.length(); char c; string id_num;
	while (p < length)
	{
		c = B.at(p);
		if (isSpeciNotation(c))
		{
			if (c == '\\')
			{
				if (MatchBlank(B, p))
				{
					cout << " ";
					continue;
				}
				if (MatchInt(B, p))
				{
					cout << "\\int";
					continue;
				}
				if (MatchSum(B, p))
				{
					cout << "\\sum";
					continue;
				}
				cout << "Error at " << p << " ,'\' has wrong follow" << endl;
				p++;
			}
			else
			{
				cout << c;
				p++;
			}
		}
		else
		{
			if (isalpha(c))
			{
				id_num = MatchId(B, p);
				NoteList = InstallIntoList(id_num, id);
			}
			else
			{
				if (isdigit(c))
				{
					id_num = MatchNum(B, p);
					NoteList = InstallIntoList(id_num, num);
				}
				else
				{
					cout << "Error at Postition" << p << endl;
				}
			}
		}
	}
	cout << endl;
	return NoteList;
}
int _tmain(int argc, _TCHAR* argv[])
{
	string input = readFile2String("sample06.txt");
	cout << input << endl;
	if (isspace(input.back()))
	{
		input.erase(input.end() - 1);
	}
	input.erase(input.begin());
	input.erase(input.end() - 1);
	cout << input << endl;
	if (!BracketMatch(input) || !NoOtherNotation(input))
	{
		//括号不匹配或者有意外字符结束进程
		return 0;
	}
	NoteList = InstallNote(input);
	OutputList();
	
	//求出LR(0)项目集规范族
	ItemSet_Goto C_gototabale = Items();
	
	printItems2file(C_gototabale.ItemSets, "Items.txt");
	return 0;
}

