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
	//遍历每一个项目
	auto end = items.end();
	auto begin = items.begin();
	for (int i = 0; i < items.size(); i++)
	{
		Item tempitem = items[i];
		//点在末尾考虑下一个项目
		if (tempitem.position == tempitem.length)
			continue;

		//取出下一个符号
		string nextSymbol = tempitem.p.right[tempitem.position];
		nextSymbols.push_back(nextSymbol);
		if (isVt(nextSymbol))
		{
			continue;
		}
		//下一个符号为Vn 则加入以Vn为左部的所有产生式
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
	//查看每一个项目对symbol的接受情况
	for (Item oldItem : items)
	{
		//判断下一个符号是否为symbol
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
	result += "  产生式打点项目:\n";
	for (Item it : items)
	{
		result += "     " + it.showItem();
	}
	result += "  GOTO状态转换:\n";
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
		//在s中搜索当前状态的所有项目
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
			//未找到当前项目
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
	//已到末尾
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
			result = result + " ·";
		result = result + " " + p.right[i];
	}

	if (position == length)
	{
		result += " ·";
	}

	result += "\n";
	return result;
}


#pragma endregion

void SLR::buildstates()
{
	//加入第一条产生式,初始化I0
	production startproduction = *wfdata.split_productions.at("S").begin();
	State startState = State(Item(startproduction, 0));
	startState.CLOSURE();
	SLRstates.push_back(startState);
	
	int oldStatesNum = SLRstates.size();
	//遍历一遍SLRstates求每个项集对的GOTO
	do
	{
		oldStatesNum = SLRstates.size();
		//遍历每一个状态
		for (int i = 0;i<SLRstates.size();i++)
		{
			State nowState = SLRstates[i];
			State newState;
			//遍历每一个可接受的后续状态
			for (string nextSymbol : nowState.nextSymbols)
			{
				newState = nowState.getGOTO(nextSymbol);
				//检查newState是否在SLRstates中
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
				//不存在的状态
				if (exsited == false)
				{
					SLRstates.push_back(newState);
					//GOTO指向新产生状态
					nowState.GOTO[nextSymbol] = SLRstates.size() - 1;

				}
				else
				{
					nowState.GOTO[nextSymbol] = exIndex;
				}

			}
			//GOTO集发生变化，加进去
			SLRstates[i] = nowState;
		}

	} while (SLRstates.size() > oldStatesNum);    //重复循环直至无新状态加入SLRstates
}

void SLR::showAllstates()
{
	ofstream outSLRstates(StatesPath);

	for (int i = 0;i<SLRstates.size();i++)
	{
		string i1 = "状态I" + to_string(i) + ":\n";
		cout << i1;
		outSLRstates << i1;
		string i2 = SLRstates[i].showState(i);
		cout << i2;
		outSLRstates << i2;
		cout << "\n\n";
		outSLRstates << "\n\n";
	}
}

//S->statements .  应当对!EOF!置规约
//根据SLRStates构建SLR分析表
//对于冲突项目应该提示解决
void SLR::buildSLR()
{
	//SLRstates和table均以下标表示状态
	for (int sindex =0;sindex<SLRstates.size();sindex++)
	{
		State s = SLRstates[sindex];
		unordered_map<string, int> VnGoto;
		unordered_map<string, Actioncell> VtACTION;
		//添加GOTO表,获取s.GOTO中非终结符号转换
		for (auto i = s.GOTO.begin(); i != s.GOTO.end(); i++)
		{
			string symbol = i->first;
			int dest = i->second;
			//当前项通过Vn转换
			if (isVn(symbol))
			{
				VnGoto[symbol] = dest;
			}
			//先加入移进项目再考虑规约冲突
			else if(isVt(symbol))
			{

				Actioncell newaction = Actioncell("S",dest);
				VtACTION[symbol] = newaction;
			}
		}

		//状态s的GOTO项,不会产生冲突
		GOTOtable.push_back(VnGoto);

		//考察状态s的每一个打点项目，找到规约项目
		for (Item item : s.items)
		{
			//该项目点在尾部,置Follow(left)的每一个符号为规约
			if (item.judgeEnd())
			{
				string left = item.p.left;
				int productionNo = item.p.id;
				//对Follow(left)每一个符号置规约
				for (string i : wfdata.Follow[left])
				{
					//无冲突 直接加入
					Actioncell newAction = Actioncell("R", productionNo);
					if (VtACTION.find(i) == VtACTION.end())
					{
						VtACTION[i] = newAction;
					}
					//VtACTION[i]有已定义动作由具体文法决定，为了方便测试此处直接写入冲突解决规则
					else
					{
						//原项目
						Actioncell oldAction = VtACTION.at(i);
						cout << "解决冲突:" << endl;
						cout << "产生冲突的状态:" << endl;
						cout << s.showState(sindex);

						cout << "旧项 ACTION[" << sindex << "," << i << "]=" << oldAction.op << oldAction.dest<< endl;
						if (oldAction.op == "R")
						{
							cout << "	R" << oldAction.dest << "=" << wfdata.productions[oldAction.dest].toString() << endl;;
						}
						cout << "新项 ACTION[" << sindex << "," << i << "]=" << newAction.op << newAction.dest << endl;
						if (newAction.op == "R")
						{
							cout << "	R"<< newAction.dest<<"=" << wfdata.productions[newAction.dest].toString() << endl;;
						}

						cout << "请选择:0->保留旧项目,1->选取新项目" << endl;
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
								cout << "输入错误，重新输入" << endl;
							}
						}

					}
				}
			}
		}
		ACTIONtable.push_back(VtACTION);
	}
	
	//置ACC,I0为开始，由于采用拓广文法，结束状态一定在I1
	ACTIONtable[1]["!EOF!"] = Actioncell("ACC", -1);
}


void SLR::showSLR()
{
	ofstream outSLRtable(SLRtablePath);
	for (int i = 0; i < GOTOtable.size(); i++)
	{
		string tableItem = "";
		tableItem = tableItem + "State" + to_string(i) +":\n";
		//ACTION项
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

	cout << "\n\n\n构造SLR分析表\n";
	mySLR.buildSLR();
	mySLR.showSLR();

	return 0;
}