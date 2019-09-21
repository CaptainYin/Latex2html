
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
	int PrintStream::genLeft(int low, int up, int left);
	stack<symbol>analysisstack;
	void PrintStream::updatec();
	symbol c;
	int Sl_ind;
	int PrintStream::length_idornum(symbol s);
	int PrintStream::getlowbound(int up);
};
int PrintStream::getlowbound(int up)
{
	stack<char>s;
	char t;
	for (int i = up; i >= 0; i--)
	{
		t = ret[i].ch;
		if (t == '{')
		{
			s.push(t);
		}
		else
		{
			if (t == '}')
			{
				if (!s.empty() && s.top() == '{')
				{
					s.pop();
				}
				else
				{
					return i + 1;
				}
			}
		}
	}
}
int PrintStream::length_idornum(symbol s)
{
	if (s.ch == 'i')
	{
		for each (Notation var in NoteList)
		{
			if (var.type == id || var.ind == s.id_num_ind)
			{
				return var.note.length();
			}
		}
	}
	else
	{
		if (s.ch == 'n')
		{
			for each (Notation var in NoteList)
			{
				if (var.type == num || var.ind == s.id_num_ind)
				{
					return var.note.length();
				}
			}
		}
		else
		{
			cout << "ERROR:length_idornum" << endl;
			return -1;
		}
	}

}
int PrintStream::genLeft(int low, int up, int left)
{

	for (int i = up; i >= low; i--)
	{
		symbol &k = ret[i];
		if (k.ch == '_'&&ret[i - 1].ch == '^')
		{
			left = left - ret[i + 1].front_size / 2;
			int up1 = i - 3;
			int low1 = getlowbound(up1);
			int sub = genLeft(low1, up1, left);
			up1 = low1 - 3;
			low1 = getlowbound(up1);
			int sup = genLeft(low1, up1, left);
			i = low1 - 1;
			left = sup > sub ? sup : sub;
			continue;
		}
		if (k.ch == 'T' || k.ch == 'M')
		{

			k.left = left;
			if (k.ch == 'T')
			{
				left = left + k.front_size / 2;
			}
			else
			{
				left = left + k.front_size;
			}

			int up1 = i - 2;
			int low1 = getlowbound(up1);
			int sub = genLeft(low1, up1, left);
			up1 = low1 - 3;
			low1 = getlowbound(up1);
			int sup = genLeft(low1, up1, left);
			i = low1 - 1;
			left = sup > sub ? sup : sub;
			continue;
		}
		if (k.ch == '(' || k.ch == ')' || k.ch == ' ')
		{
			k.left = left;
			left = left + k.front_size;
		}
		else
		{
			if (k.ch == 'i' || k.ch == 'n')
			{
				k.left = left;
				left = left + k.front_size*length_idornum(k);
			}
			else
			{
				if (k.ch == '_' || k.ch == '^')
				{
					left -= ret[i + 1].front_size / 2;
				}
			}
		}
	}
	return left;
}
void PrintStream::updatec()
{
	c = analysisstack.top();
	analysisstack.pop();
}
void PrintStream::extend()
{
	switch (syntaxind)
	{
	case 1:action1(); break;
	case 2:action2(); break;
	case 3:action3(); break;
	case 4:action4(); break;
	case 5:action5(); break;
	case 6:action6(); break;
	case 7:action7(); break;
	case 8:action8(); break;
	case 9:action9(); break;
	case 10:action10(); break;

	default:cout << "ERROR:no case" << endl;
		break;
	}
}
PrintStream::PrintStream(stack<actiontypeSingle> s, SymboList L)
{
	as = s;
	SL = L;
	symbol c;
	analysisstack.push(c);
	Sl_ind = SL.size() - 1;
}
void PrintStream::genPrintStream()
{
	while (!analysisstack.empty())
	{

		updatec();
		if (c.ch != 'B')
		{//terminal

			ret.push_back(c);
			Sl_ind--;
		}
		else
		{//B

			while (as.top().action != 'r')
			{
				as.pop();
				if (as.empty())
				{
					cout << Sl_ind << "ERROR for as.top() " << endl;
					return;
				}
			}
			syntaxind = as.top().ind;
			as.pop();//使用过的pop掉
			extend();//根据产生式syntaxind的语法制导定义展开B并且计算除left外所有属性，加入ret
		}
	}
	genLeft(0, ret.size() - 1, 100);
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
	c1.top = c.top + c.front_size * 3 / 4;
	analysisstack.push(c1);
	c1.ch = '}';
	analysisstack.push(c1);
	c1.ch = '{';
	analysisstack.push(c1);
	c1.ch = 'B';
	c1.front_size = c.front_size / 2;
	c1.top = c.top - c.front_size * 1 / 3;
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
	c1.top = c.top - c.front_size / 3;
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
	c1.top = c.top + c.front_size * 3 / 4;
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
	c1.top = c.top - c.front_size / 2;
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
	c1.top = c.top - c.front_size / 2;
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
	c1.id_num_ind = SL[Sl_ind].id_num_ind;
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
	for each (symbol c in ps)
	{
		c.printsymbol(out);
	}
	out << "</body></html>" << endl;
}