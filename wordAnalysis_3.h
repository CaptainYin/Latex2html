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
const string T = "&#8747";
const string M = "&#8721";
struct symbol
{
	char ch = 'B';//T,M,i,n,(,),' '
	int id_num_ind;
	bool printable = false;
	int top = 200;
	int left = 200;
	int front_size = 60;
	symbol() = default;
	string font_style = "normal";//oblique ,normal
	void symbol::printsymbol(ofstream&out)const;
};
void symbol::printsymbol(ofstream&out)const
{
	if (!printable)
	{//{,},^,_,$
		return;
	}
	out << "<div style=\"position: absolute;top: " << top << "px; left:" << left << "px;\"><";
	out << "span style=\"font-size:" << front_size << "px; font-style:" << font_style << ";";
	out << "line-height:100%;\">";
	if (ch == 'i')
	{
		string not;
		for each (Notation var in NoteList)
		{
			if (var.type == id&&var.ind == id_num_ind)
			{
				not = var.note;
				break;
			}
		}
		out << not;
	}
	else
	{

		if (ch == 'n')
		{
			string not;
			for each (Notation var in NoteList)
			{
				if (var.type == num&&var.ind == id_num_ind)
				{
					not = var.note;
					break;
				}
			}
			out << not;
		}
		else
		{
			if (ch == 'T')
			{
				out << T;
			}
			else
			{
				if (ch == 'M')
				{
					out << M;
				}
				else
				{
					out << ch;//(,),' '
				}
			}
		}
	}

	out << "</span></div>" << endl;
}
typedef vector<symbol>SymboList;
vector<Notation>InstallIntoList(string id_num, NoteType t, SymboList &out){
	Notation n(id_num, t);
	int c = 0;
	for each (Notation var in NoteList)
	{
		if (var.note.compare(id_num) == 0)
		{
			if (t == id)
			{
				symbol e;
				e.ch = 'i';
				e.id_num_ind = c + 1;
				out.push_back(e);
			}
			else
			{
				symbol e;
				e.ch = 'n';
				e.id_num_ind = c + 1;
				out.push_back(e);
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
		symbol e;
		e.ch = 'i';
		e.id_num_ind = c + 1;
		out.push_back(e);
	}
	else
	{
		symbol e;
		e.ch = 'n';
		e.id_num_ind = c + 1;
		out.push_back(e);
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
vector<Notation> InstallNote(const string B, SymboList &out){
	
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
					symbol e;
					e.ch = ' ';
					out.push_back(e);
					continue;
				}
				if (MatchInt(B, p))
				{
					symbol e;
					e.ch = 'T';
					out.push_back(e);
					continue;
				}
				if (MatchSum(B, p))
				{
					symbol e;
					e.ch = 'M';
					out.push_back(e);
					continue;
				}
				cout << "Error at " << p << " ,'\' has wrong follow" << endl;
				p++;
			}
			else
			{
				symbol e;
				e.ch = c;
				out.push_back(e);
				p++;
			}
		}
		else
		{
			if (isalpha(c))
			{
				id_num = MatchId(B, p);
				NoteList = InstallIntoList(id_num, id, out);
			}
			else
			{
				if (isdigit(c))
				{
					id_num = MatchNum(B, p);
					NoteList = InstallIntoList(id_num, num, out);
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
string noindex(const string text)
{
	string ret;
	for each (char var in text)
	{
		if (!isdigit(var))ret.push_back(var);
	}
	return ret;
}
string covert2string(const SymboList sl)
{
	string ret;
	for each (symbol var in sl)
	{
		ret.push_back(var.ch);
	}
	ret.push_back('$');
	return ret;
}
