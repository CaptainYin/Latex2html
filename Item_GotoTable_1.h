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
	int toState = -1;
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
//输入文法，输出项目集规范族 and goto_table(44*x.length())
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
	//while (true){
	bool updateflag = false;
	setOfItems Ivar;

	for (int i = 0; i < itemsetf.size(); i++)
	{
		IGotoX I2X;
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
	//if (!updateflag)
	//	{
	//		break;
	//}
	//}

	ret.GotoTable = GotoTable;
	ret.ItemSets = itemsetf;
	return ret;
}
//输出Gototable
void printGototable2file(const vector<IGotoX>GotoT, char*FileName)
{
	ofstream out(FileName);
	out << '\t';
	for each (char var in x)
	{
		out << var << '\t';
	}
	out << endl;
	int itemind = 0;
	for each (IGotoX var in GotoT)
	{
		out << itemind << '\t';
		itemind++;
		for (size_t i = 0; i < x.length(); i++)
		{
			out << var.toX[i].toState << '\t';
		}
		out << endl;
	}
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