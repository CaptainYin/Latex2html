struct item{
	int synind;
	int point_pos;//0��ʾ������ǰ��
	void item::print();
	void item::print2file(ofstream &ofile);//������ļ�
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
	{//���ڵ�һ��
		cout << "��" << syntax[synind] << endl;
		return;
	}
	else
	{
		if (point_pos == syntax[synind].length())
		{//�������һ��
			cout << syntax[synind] << "��" << endl;
			return;
		}
		else
		{
			for (int i = 0; i < point_pos; i++)
			{
				cout << syntax[synind][i];
			}
			cout << "��";
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
	{//���ڵ�һ��
		ofile << "��" << syntax[synind] << endl;
		return;
	}
	else
	{
		if (point_pos == syntax[synind].length())
		{//�������һ��
			ofile << syntax[synind] << "��" << endl;
			return;
		}
		else
		{
			for (int i = 0; i < point_pos; i++)
			{
				ofile << syntax[synind][i];
			}
			ofile << "��";
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
//��Ϊֻ��һ�����ս��B����˷Ǻ�����Ŀ����B�Ĳ���ʽ�����ڵ�һ��
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
//����Ŀ���հ�
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

//ת�ƺ���
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
//��Ŀit�Ƿ�����ڹ淶��
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
//�����ķ��������Ŀ���淶�� and goto_table(44*x.length())
ItemSet_Goto Items(){
	vector<IGotoX>GotoTable;
	ItemSet_Goto ret;
	vector<setOfItems> itemsetf;
	item init;
	init.synind = 0;
	init.point_pos = 0;
	setOfItems seti;
	seti.push_back(init);//����S->��B����հ�
	closure(seti);
	itemsetf.push_back(seti);//����I0;
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
			{//��Ϊ���Ҳ�����
				itemsetf.push_back(seti);
				updateflag = true;
				I2X.toX[j].toState = itemsetf.size() - 1;//��ʱ����ת�������һ��
			}
			else
			{
				if (seti.empty())
				{//Ϊ�գ������ڵ�ת��

					I2X.toX[j].toState = -1;
				}
				else
				{//��Ϊ��,Ҳ�����Ѿ��е�state

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
//���Gototable
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
//�����Ŀ��
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
//������ļ�
void printItems2file(vector<setOfItems> C, char*filename)
{
	ofstream ofile(filename);
	ofile << "LR(0)��Ŀ���淶���С :" << C.size() << endl;
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