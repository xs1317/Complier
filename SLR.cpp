#include"SLR.h"
using namespace std;

#pragma region Definition

State::State() {}
State::State(vector<Item> ites)
{
	items = ites;
}
State::State(Item ite)
{
	items.push_back(ite);
}
void State::CLOSURE()
{
	//����ÿһ����Ŀ
	auto end = items.end();
	auto begin = items.begin();
	for (int i = 0; i < items.size(); i++)
	{
		Item tempitem = items[i];
		//����ĩβ������һ����Ŀ
		if (tempitem.position == tempitem.length)
			continue;

		//ȡ����һ������
		string nextSymbol = tempitem.p.right[tempitem.position];
		nextSymbols.push_back(nextSymbol);
		if (isVt(nextSymbol))
		{
			continue;
		}
		//��һ������ΪVn �������VnΪ�󲿵����в���ʽ
		else if (isVn(nextSymbol))
		{
			list<production>productions = wfdata.split_productions[nextSymbol];
			for (production p : productions)
			{
				Item newitem = Item(p, 0);
				if (isNewItem(newitem))
					items.push_back(newitem);
			}
		}
	}
}
bool State::isNewItem(Item newItem)
{
	for (Item i : items)
	{
		if (i == newItem)
			return false;
	}
	return true;
}
State State::getGOTO(string symbol)
{
	vector<Item> newItems;
	//�鿴ÿһ����Ŀ��symbol�Ľ������
	for (Item oldItem : items)
	{
		//�ж���һ�������Ƿ�Ϊsymbol
		if (oldItem.isNext(symbol))
		{
			newItems.push_back(oldItem.moveRight());
		}
	}

	State newState = State(newItems);
	newState.CLOSURE();
	return newState;
}
string State::showState(int no)
{
	string result = "";
	result += "  ����ʽ�����Ŀ:\n";
	for (Item it : items)
	{
		result += "     " + it.showItem();
	}
	result += "  GOTO״̬ת��:\n";
	for (auto i = GOTO.begin(); i != GOTO.end(); i++)
	{
		string symbol = i->first;
		int dest = i->second;
		result += "     GOTO(" + to_string(no) + "," + symbol + ")=" + to_string(dest) + "\n";
	}
	return result;
}
bool State::operator==(const State& s)
{
	if (s.items.size() == items.size())
	{
		//��s��������ǰ״̬��������Ŀ
		for (int i = 0; i < items.size(); i++)
		{
			bool find = false;
			for (int j = 0; j < s.items.size(); j++)
			{
				if (items[i] == s.items[j])
				{
					find = true;
					continue;
				}
			}
			//δ�ҵ���ǰ��Ŀ
			if (find == false)
			{
				return find;
			}
		}
		return true;
	}
	else
	{
		return false;
	}
}


Item::Item(production pt, int po)
{
	position = po;
	p = pt;
	length = pt.length;
}
bool Item::judgeEnd()
{
	if (position < length)
		return false;
	else
		return true;
}
Item Item::moveRight()
{
	Item temp = Item(p, position);
	if (temp.position < temp.length)
		temp.position += 1;
	return temp;
}
bool Item::operator==(const Item& item)
{
	return this->position == item.position && this->length == item.length && this->p == item.p;
}
bool Item::isNext(string symbol)
{
	//�ѵ�ĩβ
	if (judgeEnd())
	{
		return false;
	}
	else
	{
		if (p.right[position] == symbol)
			return true;
	}
	return false;
}
string Item::showItem()
{
	string result = "";
	result = result + p.left + " ->";
	for (int i = 0; i < length; i++)
	{
		if (i == position)
			result = result + " ��";
		result = result + " " + p.right[i];
	}

	if (position == length)
	{
		result += " ��";
	}

	result += "\n";
	return result;
}


#pragma endregion

void SLR::buildstates()
{
	//�����һ������ʽ,��ʼ��I0
	production startproduction = *wfdata.split_productions.at("S").begin();
	State startState = State(Item(startproduction, 0));
	startState.CLOSURE();
	SLRstates.push_back(startState);
	
	int oldStatesNum = SLRstates.size();
	//����һ��SLRstates��ÿ����Ե�GOTO
	do
	{
		oldStatesNum = SLRstates.size();
		//����ÿһ��״̬
		for (int i = 0;i<SLRstates.size();i++)
		{
			State nowState = SLRstates[i];
			State newState;
			//����ÿһ���ɽ��ܵĺ���״̬
			for (string nextSymbol : nowState.nextSymbols)
			{
				newState = nowState.getGOTO(nextSymbol);
				//���newState�Ƿ���SLRstates��
				bool exsited = false;
				int exIndex = 0;
				for (int j = 0; j < SLRstates.size(); j++)
				{
					if (SLRstates[j] == newState)
					{
						exsited = true;
						exIndex = j;
						break;
					}
				}
				//�����ڵ�״̬
				if (exsited == false)
				{
					SLRstates.push_back(newState);
					//GOTOָ���²���״̬
					nowState.GOTO[nextSymbol] = SLRstates.size() - 1;

				}
				else
				{
					nowState.GOTO[nextSymbol] = exIndex;
				}

			}
			//GOTO�������仯���ӽ�ȥ
			SLRstates[i] = nowState;
		}

	} while (SLRstates.size() > oldStatesNum);    //�ظ�ѭ��ֱ������״̬����SLRstates
}

void SLR::showAllstates()
{
	ofstream outSLRstates(StatesPath);

	for (int i = 0;i<SLRstates.size();i++)
	{
		string i1 = "״̬I" + to_string(i) + ":\n";
		cout << i1;
		outSLRstates << i1;
		string i2 = SLRstates[i].showState(i);
		cout << i2;
		outSLRstates << i2;
		cout << "\n\n";
		outSLRstates << "\n\n";
	}
}

//S->statements .  Ӧ����!EOF!�ù�Լ
//����SLRStates����SLR������
//���ڳ�ͻ��ĿӦ����ʾ���
void SLR::buildSLR()
{
	//SLRstates��table�����±��ʾ״̬
	for (int sindex =0;sindex<SLRstates.size();sindex++)
	{
		State s = SLRstates[sindex];
		unordered_map<string, int> VnGoto;
		unordered_map<string, Actioncell> VtACTION;
		//���GOTO��,��ȡs.GOTO�з��ս����ת��
		for (auto i = s.GOTO.begin(); i != s.GOTO.end(); i++)
		{
			string symbol = i->first;
			int dest = i->second;
			//��ǰ��ͨ��Vnת��
			if (isVn(symbol))
			{
				VnGoto[symbol] = dest;
			}
			//�ȼ����ƽ���Ŀ�ٿ��ǹ�Լ��ͻ
			else if(isVt(symbol))
			{

				Actioncell newaction = Actioncell("S",dest);
				VtACTION[symbol] = newaction;
			}
		}

		//״̬s��GOTO��,���������ͻ
		GOTOtable.push_back(VnGoto);

		//����״̬s��ÿһ�������Ŀ���ҵ���Լ��Ŀ
		for (Item item : s.items)
		{
			//����Ŀ����β��,��Follow(left)��ÿһ������Ϊ��Լ
			if (item.judgeEnd())
			{
				string left = item.p.left;
				int productionNo = item.p.id;
				//��Follow(left)ÿһ�������ù�Լ
				for (string i : wfdata.Follow[left])
				{
					//�޳�ͻ ֱ�Ӽ���
					Actioncell newAction = Actioncell("R", productionNo);
					if (VtACTION.find(i) == VtACTION.end())
					{
						VtACTION[i] = newAction;
					}
					//VtACTION[i]���Ѷ��嶯���ɾ����ķ�������Ϊ�˷�����Դ˴�ֱ��д���ͻ�������
					else
					{
						//ԭ��Ŀ
						Actioncell oldAction = VtACTION.at(i);
						cout << "�����ͻ:" << endl;
						cout << "������ͻ��״̬:" << endl;
						cout << s.showState(sindex);

						cout << "���� ACTION[" << sindex << "," << i << "]=" << oldAction.op << oldAction.dest<< endl;
						if (oldAction.op == "R")
						{
							cout << "	R" << oldAction.dest << "=" << wfdata.productions[oldAction.dest].toString() << endl;;
						}
						cout << "���� ACTION[" << sindex << "," << i << "]=" << newAction.op << newAction.dest << endl;
						if (newAction.op == "R")
						{
							cout << "	R"<< newAction.dest<<"=" << wfdata.productions[newAction.dest].toString() << endl;;
						}

						cout << "��ѡ��:0->��������Ŀ,1->ѡȡ����Ŀ" << endl;
						while (true)
						{
							int select;
							cin >> select;
							if (select == 0)
							{
								break;
							}
							else if (select == 1)
							{
								VtACTION[i] = newAction;
								break;
							}
							else
							{
								cout << "���������������" << endl;
							}
						}

					}
				}
			}
		}
		ACTIONtable.push_back(VtACTION);
	}
	
	//��ACC,I0Ϊ��ʼ�����ڲ����ع��ķ�������״̬һ����I1
	ACTIONtable[1]["!EOF!"] = Actioncell("ACC", -1);
}


void SLR::showSLR()
{
	ofstream outSLRtable(SLRtablePath);
	for (int i = 0; i < GOTOtable.size(); i++)
	{
		string tableItem = "";
		tableItem = tableItem + "State" + to_string(i) +":\n";
		//ACTION��
		tableItem = tableItem + "    ACTION:\n";
		
		for (auto itA = ACTIONtable[i].begin(); itA != ACTIONtable[i].end(); itA++)
		{
			string symbol = itA->first;
			Actioncell ac = itA->second;
			tableItem = tableItem + "           " + symbol + "  :  " + ac.op + to_string(ac.dest) + "\n";
		}
		tableItem = tableItem + "    GOTO:\n";
		for (auto itG = GOTOtable[i].begin(); itG != GOTOtable[i].end(); itG++)
		{
			string symbol = itG->first;
			int dest = itG->second;
			tableItem = tableItem + "           " + symbol + "  :  "+ to_string(dest) + "\n";
		}
		cout << tableItem << endl;
		outSLRtable << tableItem << endl;
	}
}


int main2()
{
	init();
	getFirst();
	cout << endl; cout << endl; cout << endl; cout << endl;
	getFollow();

	initWfdata();

	SLR mySLR;

	mySLR.buildstates();

	mySLR.showAllstates();

	cout << "\n\n\n����SLR������\n";
	mySLR.buildSLR();
	mySLR.showSLR();

	return 0;
}