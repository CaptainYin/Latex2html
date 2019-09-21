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
const string terminal = { '(', ')', '{', '}', '_', '^', 'T', 'M', 'n', 'i', ' ', '$' };
const string follow_B = terminal;
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
			GotoTable.push_back(I2X);
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
	char action2 = ' ';//slave action
	int ind2;
	int state2;
};
struct actiontypeSingle{
	char action = ' ';
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
	int terminal_num = terminal.length();
	actiontype **actiontable = new actiontype*[statenum];
	for (int i = 0; i < statenum; i++)
	{
		actiontable[i] = new actiontype[terminal_num];//row is terminal
	}
	for (size_t i = 0; i < statenum; i++)
	{//for each itemset
		setOfItems its = C[i];
		for (size_t j = 0; j < C[i].size(); j++)
		{//for each item
			item it = its[j];
			if (it.synind == 0)
			{
				if (it.point_pos == 1)
				{
					//if S->B· exist in itemset-i then set action[i,$]=acc;
					//as $ is the last symbol of non_terminal
					actiontable[i][terminal.length() - 1].action = 'a';
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
					for (size_t k = 0; k <terminal.length(); k++)
					{
						if (actiontable[i][k].action == 's')
						{//检测是否有移进归约冲突
							cout << "ERROR1" << endl;
						}
						if (actiontable[i][k].ind!= it.synind && actiontable[i][k].action == 'r')
						{//检测是否有归约归约冲突
							cout << "ERROR3" << endl;
						}
						actiontable[i][k].action = 'r';//reduce by syntax[it.synind]
						actiontable[i][k].ind = it.synind;
					}
				}
				else
				{
					char ch = syntax[it.synind][it.point_pos];
					
					//P79(a)
					if (ch=='B')
					{
						
						Transfer_table[i] = goto_t[i].toX[0].toState;

					}
					else
					{
						int k = 0;
						for ( k = 0; k < terminal.length(); k++)
						{
							if (ch == terminal[k])
							{
								break;
							}
						}
						int xp = 0;
						for (; xp < x.length(); xp++)
						{
							if (ch ==x[xp])
							{
								break;
							}
						}
						if (actiontable[i][k].action == 'r')
						{
							//检测是否有移进归约冲突
							cout << "Items: " << i << " terminal:" << ch << endl;
							cout << "using syntax No."<<actiontable[i][k].ind << endl;
							cout << "移进归约冲突" << endl;
							actiontable[i][k].action2 = 's';
							actiontable[i][k].state2 = goto_t[i].toX[xp].toState;
							continue;
						}
						if (actiontable[i][k].state != goto_t[i].toX[xp].toState&& actiontable[i][k].action == 's')
						{//检测是否有移进移进冲突
							cout << "ERROR4" << endl;
						}
						actiontable[i][k].action = 's';
						actiontable[i][k].state = goto_t[i].toX[xp].toState;
					}
				}
			}
		}
	}
	ret.Tansfer_table = Transfer_table;
	ret.ActionTable = actiontable;
	return ret;
}

class Analysis
{
public:
	stack<int> state;
	stack<actiontypeSingle>List;
	int pos=0;
	Analysis(const string t, const SLR_Analysis_Table T);
	Analysis(const stack<int> s, const stack<actiontypeSingle>L, const int p, const string t);
	~Analysis();
	int Analysis::generateActList();//1:acc,0:fork,-1:error
	//m_s 1: for master 2 for slave
	int Analysis::actionBy(const int m_s);//1:acc,0:fork,-1:error

private:
	string text;
	int ind_ter;
	int currentState;
	void Analysis::updateind_ter();
	void Analysis::updatecurrentState();
	SLR_Analysis_Table SLR_T;
};
void Analysis::updatecurrentState()
{
	currentState = state.top();
}
int Analysis::actionBy(const int m_s)
{
	 actiontype** ActionTable = SLR_T.ActionTable;
	 int *Tansfer_table = SLR_T.Tansfer_table;
	actiontypeSingle act;
	if (m_s==1)
	{//master
		act.action = ActionTable[currentState][ind_ter].action;
		act.ind = ActionTable[currentState][ind_ter].ind;
		act.state = ActionTable[currentState][ind_ter].state;
	}
	else
	{//slave
		act.action = ActionTable[currentState][ind_ter].action2;
		act.ind = ActionTable[currentState][ind_ter].ind2;
		act.state = ActionTable[currentState][ind_ter].state2;
	}
	if (act.action == 'a')return 1;//acc
	if (act.action == 's')
	{
		state.push(act.state);
		updatecurrentState();
		List.push(act);
	}
	else
	{//reduce
		size_t length = syntax[act.ind].length();
		for (size_t i = 0; i < length; i++)
		{
			state.pop();
			if (state.empty())cout << "err:pop too much" << endl;
			updatecurrentState();
		}
		int toState = Tansfer_table[currentState];
		state.push(toState);
		updatecurrentState();
		List.push(act);
	}
	return 0;//normal
}
int Analysis::generateActList(){
	 actiontype** ActionTable=SLR_T.ActionTable;
	 int *Tansfer_table = SLR_T.Tansfer_table;
	while (true)
	{
		updateind_ter();
		updatecurrentState();
		if (ActionTable[currentState][ind_ter].action == ' ')return -1;//error
		else
		{
			if (ActionTable[currentState][ind_ter].action2 != ' ')return 0;//fork
			int ret=actionBy(1);
			if (ret==1)
			{
				return 1;
			}
			pos++;
			if (pos==text.length())
			{
				List.pop();//没有必要，-1就直接忽略
				return -1;
			}
		}
	}
	
}
Analysis::Analysis(stack<int> s, stack<actiontypeSingle>L, int p, string t)
{
	List = L;
	state = s;
	pos = p;
	text = t;
	char ch = text[pos];
	int i = 0;
	for (; i < terminal.length(); i++)
	{
		if (ch == terminal[i])
		{
			break;
		}
	}
	ind_ter = i;
}
Analysis::Analysis(const string t, const SLR_Analysis_Table T)
{
	state.push(0);
	text = t;
	SLR_T = T;
}
void Analysis::updateind_ter()
{
	char ch = text[pos];
	int i = 0;
	for (; i < terminal.length(); i++)
	{
		if (ch == terminal[i])
		{
			break;
		}
	}
	ind_ter = i;
}
Analysis::~Analysis()
{
	
}
class AnalysisTree
{
public:
	
	stack<Analysis> ATree;
	stack<actiontypeSingle> AnalysisTree::Grow();
	AnalysisTree(const string t, const SLR_Analysis_Table T);
	~AnalysisTree();
	

private:
	 string input;
	void AnalysisTree::init();
	 SLR_Analysis_Table SLR_T;
};
stack<actiontypeSingle>AnalysisTree::Grow(){
	init();
	while (true)
	{
		if (ATree.empty())
		{
			Analysis activityNode(input,SLR_T);
			cout << "FAILED" <<endl;
			return activityNode.List;
		}
		Analysis activityNode = ATree.top();
		ATree.pop();
		int retsta=activityNode.generateActList();
		if (retsta==-1)
		{
			//?
			activityNode = ATree.top();
			ATree.pop();

		}
		else
		{
			if (retsta==1)
			{
				return activityNode.List;
			}
			else
			{
				//resta==0 fork
				Analysis activityNode1 = activityNode;
				
				if (activityNode1.actionBy(1) == 1)
				{
					return activityNode1.List;
				}
				ATree.push(activityNode1);
				if (activityNode.actionBy(2) == 1)
				{
					return activityNode.List;
				}
				ATree.push(activityNode);
			}
		}
	}
}
void AnalysisTree::init()
{

	Analysis root(input, SLR_T);
	ATree.push(root);

}
AnalysisTree::AnalysisTree(const string t, const SLR_Analysis_Table T)
{
	input = t;
	SLR_T = T;

}

AnalysisTree::~AnalysisTree()
{
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
vector<Notation>InstallIntoList(string id_num, NoteType t,string &out){
	Notation n(id_num, t);
	int c = 0;
	for each (Notation var in NoteList)
	{
		if (var.note.compare(id_num) == 0)
		{
			if (t == id)
			{
				out.push_back('i');
				out.push_back(c+1+'0');
				
			}
			else
			{
				out.push_back('n');
				out.push_back(c + 1 + '0');
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
		out.push_back('i');
		out.push_back(c + 1 + '0');
	}
	else
	{
		out.push_back('n');
		out.push_back(c + 1 + '0');
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
vector<Notation> InstallNote(const string B,string &out){
	out = "";//clear out
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
					out.push_back(' ');
					continue;
				}
				if (MatchInt(B, p))
				{
					out.push_back('T');
					continue;
				}
				if (MatchSum(B, p))
				{
					out.push_back('M');
					continue;
				}
				cout << "Error at " << p << " ,'\' has wrong follow" << endl;
				p++;
			}
			else
			{
				out.push_back(c);
				p++;
			}
		}
		else
		{
			if (isalpha(c))
			{
				id_num = MatchId(B, p);
				NoteList = InstallIntoList(id_num, id,out);
			}
			else
			{
				if (isdigit(c))
				{
					id_num = MatchNum(B, p);
					NoteList = InstallIntoList(id_num, num,out);
				}
				else
				{
					cout << "Error at Postition" << p << endl;
				}
			}
		}
	}
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
	string output;
	NoteList = InstallNote(input,output);
	cout << output << endl;
	OutputList();
	//求出LR(0)项目集规范族
	ItemSet_Goto A = Items();
	printItems2file(A.ItemSets, "Items.txt");
	SLR_Analysis_Table SLR_Table= GenSLR_Action_Table(A);
	//SLR_Table 中有冲突地方action！=‘ ’&&action2!=' '  出错回溯
	//input+$
	input = "M{i}{i}{i}$";
	AnalysisTree Anatree(input, SLR_Table);
	stack<actiontypeSingle> List = Anatree.Grow();

	return 0;
}

