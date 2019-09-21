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
#include"wordAnalysis.h"
const string syntax[11] = { "B", "BB", "B_^{B}{B}", "B^{B}",
"B_{B}", "T{B}{B}{B}", "M{B}{B}{B}",
"i", "n", " ", "(B)" };//T:\int,M:\sum,i:id,n:num
//DFA转移符号集
const string x = { 'B', '(', ')', '{', '}', '_', '^', 'T', 'M', 'n', 'i', ' ' };
const string terminal = { '(', ')', '{', '}', '_', '^', 'T', 'M', 'n', 'i', ' ', '$' };//length：12
const string follow_B = terminal;
#include"Item_GotoTable.h"
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
				if (it.point_pos == syntax[it.synind].length())
				{//点在末尾，归约
					for (size_t k = 0; k < terminal.length(); k++)
					{
						if (actiontable[i][k].action == 's')
						{//检测是否有移进归约冲突
							cout << "ERROR1" << endl;
						}
						if (actiontable[i][k].ind != it.synind && actiontable[i][k].action == 'r')
						{//检测是否有归约归约冲突
							cout << "ERROR3" << endl;
						}
						actiontable[i][k].action = 'r';//reduce by syntax[it.synind]
						actiontable[i][k].ind = it.synind;
					}
				}
				else
				{//点不在末尾，移进
					char ch = syntax[it.synind][it.point_pos];
					if (ch == 'B')
					{//第一个就是B
						Transfer_table[i] = goto_t[i].toX[0].toState;
					}
					else
					{
						int k = 0;
						for (k = 0; k < terminal.length(); k++)
						{
							if (ch == terminal[k])
							{
								break;
							}
						}
						int xp = 0;
						for (; xp < x.length(); xp++)
						{
							if (ch == goto_t[i].toX[xp].x)
							{
								break;
							}
						}
						if (actiontable[i][k].action == 'r')//已经有归约操作了
						{
							//检测是否有移进归约冲突
							cout << "Items: " << i << " terminal:" << ch << endl;
							cout << "using syntax No." << actiontable[i][k].ind << endl;
							cout << "移进归约冲突" << endl;
							actiontable[i][k].action2 = 's';
							if (goto_t[i].toX[xp].toState == -1)cout << ch << endl;
							actiontable[i][k].state2 = goto_t[i].toX[xp].toState;
							continue;
						}
						if (actiontable[i][k].state != goto_t[i].toX[xp].toState&& actiontable[i][k].action == 's')
						{//检测是否有移进移进冲突
							cout << "ERROR4" << endl;
						}
						actiontable[i][k].action = 's';//
						if (goto_t[i].toX[xp].toState == -1)cout << ch << endl;
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
	int pos = 0;
	Analysis(const string t, const SLR_Analysis_Table T);
	Analysis(const stack<int> s, const stack<actiontypeSingle>L, const int p, const string t);

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
	if (state.top() == -1)cout << "why -1" << endl;
	currentState = state.top();
}
int Analysis::actionBy(const int m_s)
{
	actiontype** ActionTable = SLR_T.ActionTable;
	int *Tansfer_table = SLR_T.Tansfer_table;
	actiontypeSingle act;
	if (m_s == 1)
	{//master
		act.action = ActionTable[currentState][ind_ter].action;
		if (act.action == ' ')
		{
			cout << "master" << endl;
		}
		act.ind = ActionTable[currentState][ind_ter].ind;
		act.state = ActionTable[currentState][ind_ter].state;
	}
	else
	{//slave
		cout << "slave" << endl;
		act.action = ActionTable[currentState][ind_ter].action2;
		act.ind = ActionTable[currentState][ind_ter].ind2;
		act.state = ActionTable[currentState][ind_ter].state2;
	}
	if (act.action == 'a')
	{
		List.push(act);
		return 1;//acc
	}
		
	if (act.action == 's')
	{
		//debug 2016年4月21日17:49:23
		if (act.state == -1)
		{
			cout << currentState << " " << ind_ter << " " << terminal[ind_ter] << endl;
			cout << act.action << endl;
			cout << "why 1" << endl;
		}
		state.push(act.state);
		pos++;//如果是r就不用++，2016年4月21日21:26:50
		if (pos == text.length())
		{
			return -1;
		}
		updatecurrentState();
		List.push(act);
	}
	else
	{//reduce
		//debug 2016年4月21日17:48:54
		if (act.action == ' ')
		{
			cout << "eeeeeeee" << endl;
		}
		size_t length = syntax[act.ind].length();
		for (size_t i = 0; i < length; i++)
		{
			state.pop();
			if (state.empty())cout << "err:pop too much" << endl;
			updatecurrentState();
		}
		int toState = Tansfer_table[currentState];
		state.push(toState);
		if (toState == -1)
		{
			cout << "why 2" << endl;
		}
		updatecurrentState();
		List.push(act);
	}
	return 0;//normal
}
int Analysis::generateActList(){
	actiontype** ActionTable = SLR_T.ActionTable;
	int *Tansfer_table = SLR_T.Tansfer_table;
	while (true)
	{
		updateind_ter();
		updatecurrentState();
		if (ActionTable[currentState][ind_ter].action == ' ')
		{
			cout << "wwwwwwwww" << endl;
			return -1;//error
		}
		else
		{
			if (ActionTable[currentState][ind_ter].action2 != ' ')return 0;//fork
			int ret = actionBy(1);//1 or 0 or -1
			if (ret != 0)
			{// return 1 or -1
				return ret;
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
	if (ind_ter == terminal.length())
	{
		cout << "ind_ter exceed" << endl;
	}
	ind_ter = i;
}
class AnalysisTree
{
public:

	stack<Analysis> ATree;
	stack<actiontypeSingle> AnalysisTree::Grow();
	AnalysisTree(const string t, const SLR_Analysis_Table T);
	void AnalysisTree::printList();
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
			Analysis activityNode(input, SLR_T);
			cout << "FAILED" << endl;
			return activityNode.List;
		}
		Analysis activityNode = ATree.top();
		ATree.pop();
		int retsta = activityNode.generateActList();
		if (retsta == -1)
		{//Failed
			if (ATree.empty())
			{
				cout << "Fail all,no result" << endl;
				stack<actiontypeSingle> ret;
				return ret;
			}
			activityNode = ATree.top();
			ATree.pop();

		}
		else
		{
			if (retsta == 1)
			{//acc
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
void printActionList(stack<actiontypeSingle> s, char *FileName)
{
	actiontypeSingle a;
	ofstream out(FileName);
	while (!s.empty())
	{
		a = s.top();
		s.pop();
		out << a.action;
		if (a.action == 's')
		{
			out << a.state<<'\t'<<"Shift in";
		}
		if (a.action == 'r')
		{
			out << a.ind<<'\t'<<"B->"<<syntax[a.ind];
		}
		if (a.action == 'a')
		{
			out << '\t' << "S->$B$";
		}
		out << endl;
	}
}

class PrintStream
{
public:
	SymboList ret;//和ps逆序
	PrintStream(stack<actiontypeSingle> s, SymboList L);
	void PrintStream::genPrintStream();

private:
	stack<actiontypeSingle> as;
	SymboList SL;
	int syntaxind;
	void PrintStream::extend();
	void PrintStream::action1();
	void PrintStream::action2();
	void PrintStream::action3();
	void PrintStream::action4();
	void PrintStream::action5();
	void PrintStream::action6();
	void PrintStream::action7();
	void PrintStream::action8();
	void PrintStream::action9();
	void PrintStream::action10();
	void PrintStream::genLeft();
	stack<symbol>analysisstack;
	void PrintStream::updatec();
	symbol c;
	int Sl_ind;
};
void PrintStream::updatec()
{
	c = analysisstack.top();
	analysisstack.pop();
}
void PrintStream::extend()
{
	switch (syntaxind)
	{
	case 1:action1();
	case 2:action2();
	case 3:action3();
	case 4:action4();
	case 5:action5();
	case 6:action6();
	case 7:action7();
	case 8:action8();
	case 9:action9();
	case 10:action10();

	default:
		break;
	}
}
PrintStream::PrintStream(stack<actiontypeSingle> s, SymboList L)
{
	as = s;
	SL = L;
	symbol c;
	analysisstack.push(c);
	Sl_ind = SL.size()-1;
}
void PrintStream::genPrintStream()
{
	
	while (!analysisstack.empty())
	{
		updatec();
		if (c.ch!='B')
		{//terminal
			ret.push_back(c);
			Sl_ind--;
		}
		else
		{//B
			while (as.top().action!='r')
			{
				as.pop();
			}
			syntaxind = as.top().ind;
			as.pop();//使用过的pop掉
			extend();//根据产生式syntaxind的语法制导定义展开B并且计算除left外所有属性，加入ret
			
			
		}
	}
	genLeft();
}
void PrintStream::action1(){
	//"BB",
	analysisstack.push(c);
	analysisstack.push(c);
}
void PrintStream::action2(){
	//  "B_^{B}{B}", 
	symbol c1;
	analysisstack.push(c);
	c1.ch = '_';
	analysisstack.push(c1);
	c1.ch = '^';
	analysisstack.push(c1);
	c1.ch = '{';
	analysisstack.push(c1);
	c1.ch = 'B';
	c1.front_size = c.front_size / 2;
	c1.top = c.top + c.front_size;
	analysisstack.push(c1);
	c1.ch = '}';
	analysisstack.push(c1);
	c1.ch = '{';
	analysisstack.push(c1);
	c1.ch = 'B';
	c1.front_size = c.front_size / 2;
	c1.top = c.top - c.front_size;
	analysisstack.push(c1);
	c1.ch = '}';
	analysisstack.push(c1);
}
void PrintStream::action3(){
	//"B^{B}",
	symbol c1;
	analysisstack.push(c);
	c1.ch = '^';
	analysisstack.push(c1);
	c1.ch = '{';
	analysisstack.push(c1);
	c1.ch = 'B';
	c1.front_size = c.front_size / 2;
	c1.top = c.top - c.front_size;
	analysisstack.push(c1);
	c1.ch = '}';
	analysisstack.push(c1);
}
void PrintStream::action4(){
	//"B_{B}",
	symbol c1;
	analysisstack.push(c);
	c1.ch = '_';
	analysisstack.push(c1);
	c1.ch = '{';
	analysisstack.push(c1);
	c1.ch = 'B';
	c1.front_size = c.front_size / 2;
	c1.top = c.top + c.front_size;
	analysisstack.push(c1);
	c1.ch = '}';
	analysisstack.push(c1);
}
void PrintStream::action5(){
	// "T{B}{B}{B}",
	symbol c1;

	c1.ch = 'T';
	c1.printable = true;
	c1.front_size = c.front_size;
	c1.top = c.top;
	analysisstack.push(c1);

	c1.ch = '{';
	c1.printable = false;
	analysisstack.push(c1);

	c1.ch = 'B';//下限
	c1.front_size = c.front_size/2;
	c1.top = c.top+c.front_size;
	analysisstack.push(c1);

	c1.ch = '}';
	c1.printable = false;
	analysisstack.push(c1);

	c1.ch = '{';
	c1.printable = false;
	analysisstack.push(c1);

	c1.ch = 'B';//上限
	c1.front_size = c.front_size/2;
	c1.top = c.top-c.front_size ;
	analysisstack.push(c1);

	c1.ch = '}';
	c1.printable = false;
	analysisstack.push(c1);

	c1.ch = '{';
	c1.printable = false;
	analysisstack.push(c1);

	c1.ch = 'B';//运算内容
	c1.front_size = c.front_size;
	c1.top = c.top;
	analysisstack.push(c1);

	c1.ch = '}';
	c1.printable = false;
	analysisstack.push(c1);
}
void PrintStream::action6(){
	// "M{B}{B}{B}",
	symbol c1;

	c1.ch = 'M';
	c1.printable = true;
	c1.front_size = c.front_size;
	c1.top = c.top;
	analysisstack.push(c1);

	c1.ch = '{';
	c1.printable = false;
	analysisstack.push(c1);

	c1.ch = 'B';//下限
	c1.front_size = c.front_size / 2;
	c1.top = c.top + c.front_size;
	analysisstack.push(c1);

	c1.ch = '}';
	c1.printable = false;
	analysisstack.push(c1);

	c1.ch = '{';
	c1.printable = false;
	analysisstack.push(c1);

	c1.ch = 'B';//上限
	c1.front_size = c.front_size / 2;
	c1.top = c.top - c.front_size;
	analysisstack.push(c1);

	c1.ch = '}';
	c1.printable = false;
	analysisstack.push(c1);

	c1.ch = '{';
	c1.printable = false;
	analysisstack.push(c1);

	c1.ch = 'B';//运算内容
	c1.front_size = c.front_size;
	c1.top = c.top;
	analysisstack.push(c1);

	c1.ch = '}';
	c1.printable = false;
	analysisstack.push(c1);
}
void PrintStream::action7(){
	//"i",
	symbol c1;
	c1.ch = 'i';
	c1.printable = true;
	c1.font_style = "oblique";
	c1.id_num_ind=SL[Sl_ind].id_num_ind;
	c1.front_size = c.front_size;
	c1.top = c.top;
	analysisstack.push(c1);
}
void PrintStream::action8(){
	//"n", 
	symbol c1;
	c1.ch = 'n';
	c1.printable = true;
	c1.front_size = c.front_size;
	c1.id_num_ind = SL[Sl_ind].id_num_ind;;
	c1.top = c.top;
	analysisstack.push(c1);
}
void PrintStream::action9(){
	//" ", 
	symbol c1;
	c1.ch = ' ';
	c1.printable = true;
	c1.front_size = c.front_size;
	c1.top = c.top;
	analysisstack.push(c1);
}
void PrintStream::action10(){
	//"(B)"
	symbol c1;
	c1.ch = '(';
	c1.printable = true;
	c1.front_size = c.front_size;
	c1.top = c.top;
	analysisstack.push(c1);

	c1.ch = 'B';
	c1.front_size = c.front_size;
	c1.top = c.top;
	analysisstack.push(c1);

	c1.ch = ')';
	c1.printable = true;
	c1.front_size = c.front_size;
	c1.top = c.top;
	analysisstack.push(c1);

}

void genHTML(const SymboList ps, char *FileName)
{
	ofstream out(FileName);
	out << "<html><head><meta content=\"text / html; charset = gb2312\"> </head> <body>" << endl;
	symbol c;
	c.printsymbol(out);
	
	out << "</body></html>" << endl;
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
	SymboList SL;
	NoteList = InstallNote(input, SL);
	OutputList();
	//求出LR(0)项目集规范族
	ItemSet_Goto A = Items();
	printItems2file(A.ItemSets, "Items.txt");
	SLR_Analysis_Table SLR_Table = GenSLR_Action_Table(A);
	printGototable2file(A.GotoTable, "GotoTable.txt");
	//SLR_Table 中有冲突地方action！=‘ ’&&action2!=' '  出错回溯
	//input+$
	string no_ind = covert2string(SL);
	cout << no_ind << endl;
	AnalysisTree Anatree(no_ind, SLR_Table);
	stack<actiontypeSingle> ActList = Anatree.Grow();
	printActionList(ActList, "actionlist.txt");


	SymboList ps;
	genHTML(ps, "sample.html");
	
	return 0;
}
