#include"tools.h"
using namespace std;



string FirstOutPath = "First.txt";
string FollowOutPath = "Follow.txt";
int number = 0;                  //记录产生式编号
set<string> Vt;//终结符集合
set<string> Vn;//非终结符集合
map<string, set<string>> First;//First集
map<string, list<production>> split_productions;//分解后的产生式集合
map<string, set<string>> Follow;//Follow集
vector<production> productions;
string fileName = "Grammar.txt";    //产生式存在data.txt中  以S作为开始符号
WF wfdata = WF(Vt, Vn, First, Follow, split_productions,productions);



#pragma region Definition
production::production(int i, vector<string>r, string l)
{
	left = l;
	right = r;
	length = r.size();
	//右部含有空则长度减少1
	if (find(right.begin(), right.end(), "#") != right.end())
		length = length - 1;
	id = i;
}

production::production() {}

string production::toString()
{
	string result = "";
	result = result + left + " ->";
	for (auto r : right)
	{
		result = result + " " + r;
	}
	return result;
}

bool production::operator==(const production& p)
{
	return this->id == p.id;
}

WF::WF(set<string>t, set<string>n, map<string, set<string>>Fi, map<string, set<string>> Fo, map<string, list<production>> sp, vector<production>p)
{
	Vt = t;
	Vn = n;
	First = Fi;
	Follow = Fo;
	split_productions = sp;
	productions = p;
}

void WF::getvalue(set<string>t, set<string>n, map<string, set<string>>Fi, map<string, set<string>> Fo, map<string, list<production>> sp, vector<production>p)
{
	Vt = t;
	Vn = n;
	First = Fi;
	Follow = Fo;
	split_productions = sp;
	productions = p;
}

void WF::outProductions(string outPath)
{
	ofstream outProductions(outPath);
	for (auto i : productions)
	{
		outProductions << i.id <<"  :  " << i.toString() << endl;
	}
}

#pragma endregion



#pragma region tools

void initWfdata()
{
	wfdata.getvalue(Vt, Vn, First, Follow, split_productions, productions);
}


bool followEqual(map<string, set<string>> oldM, map<string, set<string>> newM)
{
	//检查是否所有的Vn都加进来了
	if (oldM.size() == newM.size())
	{
		//检查每一个Vn的Follow集是否有所增长
		for (auto it1 = newM.begin(); it1 != newM.end(); it1++)
		{
			string key = it1->first;
			set<string> value = it1->second;
			//有新元素出现
			if (oldM.find(key) == oldM.end())
			{
				return false;
			}
			else
			{
				if (value.size() != oldM[key].size())
				{
					return false;
				}
			}
			
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool isVn(string s)
{
	if (Vn.find(s) != Vn.end())
		return true;
	return false;
}

bool isVt(string s)
{
	if (Vt.find(s) != Vt.end())
		return true;
	return false;
}

//字符串以字符分割
list<string> split(string str, string pattern)
{
	string::size_type pos;
	list<string> result;
	str += pattern;//扩展字符串以方便操作
	int size = str.size();

	for (int i = 0; i < size; i++)
	{
		pos = str.find(pattern, i);
		if (pos < size)
		{
			string s = str.substr(i, pos - i);
			if(s!=""&&s!=" ")
				result.push_back(s);
			i = pos + pattern.size() - 1;
		}
	}
	return result;
}

//字符串删除特定字符
string deletmember(string &s, char m) {

	string::iterator it = s.begin();
	for (it=s.begin(); it != s.end();) 
	{
		if (*it == m)it = s.erase(it);
		else it++;
	}
	return s;
}

set<string> Vn2;
//初始化产生式、Vt、Vn
void init()
{
	string line;
	ifstream in(fileName);
	while (getline(in, line))
	{
		int position = line.find("->");
		if (position != -1)
		{
			string left = line.substr(0, position);     //左部一定是非终结符号
			deletmember(left, ' ');
			string right = line.substr(position + 2);
			deletmember(right, '\t');
			//加入新的非终结符号
			if(Vn.find(left)==Vn.end())
				Vn.insert(left);

			if (Vn2.find(left) == Vn2.end()) 
				Vn2.insert(left);

			//分割右部并添加到产生式
			list<string> splitRight = split(right, " ");
			list<string>::iterator it1;
			//添加右部符号到Vn和Vt
			for (it1 = splitRight.begin(); it1 != splitRight.end(); it1++)
			{
				string temp = *it1;
				//终结符号:对于''包括的终结符号 删除引号加入
				if (temp[0] == '\'' || temp=="integer"||temp=="character" ||temp=="ID")
				{
					//加入新的终结符号，删除单引号
					string tempa = deletmember(temp , '\'');
					*it1 = tempa;
					if(Vt.find(tempa)==Vt.end())
						Vt.insert(tempa);

				}
				else
				{
					//加入新的非终结符
					if (Vn.find(left) != Vn.end())
						Vn.insert(temp);
				}

			}
			vector<string> sr(splitRight.begin(), splitRight.end());
			production P = production(number++, sr,left);
			productions.push_back(P);
			split_productions[left].push_back(P);
		}
	}
	Vt.insert("!EOF!");
}

//手动判断哪些符号可以推出空:S,statements,declaration,M,N ;不存在通过推导产生的空,不存在间接左递归
set<string> nullSymbol{ "S","statements","declaration","M","N" };
bool nullable(string symbol)
{
	if (nullSymbol.find(symbol) != nullSymbol.end())
		return true;
	else
		return false;
}

//求某一个非终结符号的First集（可能有多个产生式）
void getOneFirst(string s)
{

	//该非终结符号所有产生式右部
	list<production> productions = split_productions.at(s);
	for (list<production>::iterator pit = productions.begin(); pit != productions.end(); pit++)
	{
		//取出某一条产生式右部
		production ptemp = *pit;
		//遍历右部的每一个符号
		for (vector<string>::iterator sit = ptemp.right.begin(); sit != ptemp.right.end(); sit++)
		{
			string stemp = *sit;
			if (isVt(stemp))
			{
				//除了非空产生式，不可能含有空，所以如果是终结符号直接加入
				if (First[s].find(stemp) == First[s].end())
					First[s].insert(stemp);
				break;  //考虑下一条产生式
			}
			else if (isVn(stemp))
			{
				if (stemp != s)
				{
					getOneFirst(stemp);
					//将First(stemp)接入First(s)
					First[s].insert(First[stemp].begin(), First[stemp].end());
					//若该Vn可以推出空考察下一个符号
					if (nullable(stemp))	
						continue;
					else
						break;
				}
				else if (stemp == s && !nullable(stemp))  //左递归，但非空，不会对First集有贡献
				{
					break;
				}
				else if (stemp == s && nullable(stemp))	  //左递归，可为空需要考察下一个符号
				{
					continue;
				}

			}
		}
	}
}

//求某迭代器后续所有串
list<string> getNextSymbols(vector<string> src,vector<string>::iterator it,vector<string>::iterator end)
{
	list<string> result;
	for (; it != end; it++)
	{
		result.push_back(*it);
	}
	return result;
}

//求一串符号的First集
set<string> getSymbolsFollow(list<string> symbols)
{

	set<string> result;
	if (symbols.size() == 0)
	{
		result.insert("#");
		return result;
	}
	else
	{

		for (auto it = symbols.begin(); it != symbols.end(); it++)
		{
			string sym = *it;
			if (isVt(sym))
			{
				result.insert(sym);
				return result;
			}
			else if (isVn(sym))
			{
				//该Vn的First集不含空
				if (First[sym].find("#") == First[sym].end())
				{
					result.insert(First[sym].begin(), First[sym].end());
					return result;
				}
				//该Vn的First集含空,把非空的符号加入并考虑下一个符号
				else
				{
					for (auto itIn = First[sym].begin(); itIn != First[sym].end(); itIn++)
					{
						string f = *itIn;
						if (f != "#")
						{
							result.insert(f);
						}
					}
					//若Vn含有空且当前Vn为最后一个符号
					auto itJudgeEnd = it;
					itJudgeEnd++;
					if (itJudgeEnd == symbols.end())
					{
						result.insert("#");
					}
				}
			}
		}
	}
	return result;
}

//求每个非终结符号的First集
void getFirst()
{
	ofstream outFirst(FirstOutPath);
	for (set<string>::iterator sit = Vn.begin(); sit != Vn.end(); sit++)
	{
		getOneFirst(*sit);

	}
	cout << "文法各个非终结符号的First集如下所示(其中 '#' 表示空)：\r\n";
	for (auto it = First.begin(); it != First.end(); it++)
	{
		string key = it->first;
		set<string> F = it->second;
		string outS = "First(" + key + ")={";
		auto it2 = F.begin();
		outS += " '" + *it2+"'";
		it2++;
		for (; it2 != F.end(); it2++)
		{
			outS = outS + ", '" + *it2 + "' ";
		}
		outS = outS + " }\r\n";
		cout << outS;
		outFirst << outS;
	}
}

//求解每个非终结符号的Follow集 三个步骤 反复考察每个产生式
void getFollow()
{
	//终结符号加入开始符号的FOLLOW集
	Follow["S"].insert("!EOF!");
	map<string, set<string>> oldFollow = Follow;
	//重复下列动作直到Follow集不再改变
	do
	{
		oldFollow = Follow;
		//遍历每一条产生式
		for (auto itPr = split_productions.begin(); itPr != split_productions.end(); itPr++)
		{
			string left = itPr->first;

			list<production> rights = itPr->second;
			//遍历所有右部
			for(production Pright:rights)
			{
				//遍历右部每一个符号
				for (vector<string>::iterator itR = Pright.right.begin();itR!=Pright.right.end();itR++)
				{
					string Nowsymbol = *itR;
					//忽视Vt
					if (isVt(Nowsymbol))
					{
						continue;
					}
					else if (isVn(Nowsymbol))
					{
						vector<string>::iterator  itRnext = itR;
						itRnext++;
						//当前Vn在末尾
						if (itRnext == Pright.right.end())
						{
							//Follow(left)加入Follow(nowsymbol)
							Follow[Nowsymbol].insert(Follow[left].begin(), Follow[left].end());
						}
						else
						{
							list<string>nextSymbols = getNextSymbols(Pright.right, itRnext,Pright.right.end());
							set<string>nextFirst = getSymbolsFollow(nextSymbols);
							//后续串 的First集无空
							if (nextFirst.find("#") == nextFirst.end())
							{
								Follow[Nowsymbol].insert(nextFirst.begin(), nextFirst.end());
							}
							else
							{
								//加入非空
								for (string nf : nextFirst)
								{
									if (nf != "#")
										Follow[Nowsymbol].insert(nf);
								}
								//Follow(left)加入Follow(nowsymbol)
								Follow[Nowsymbol].insert(Follow[left].begin(), Follow[left].end());
							}
						}
					}

				}
			}
		}
	} while (!followEqual(oldFollow, Follow));

	ofstream outFollow(FollowOutPath);
	cout << "文法各个非终结符号的Follow集如下所示(其中 '!EOF!' 表示终结符)：\r\n";
	for (auto it = Follow.begin(); it != Follow.end(); it++)
	{
		string key = it->first;
		set<string> F = it->second;
		string outS = "Follow(" + key + ")={";
		auto it2 = F.begin();
		outS += " '" + *it2 + "'";
		it2++;
		for (; it2 != F.end(); it2++)
		{
			outS = outS + ", '" + *it2 + "' ";
		}
		outS = outS + " }\r\n";
		cout << outS;
		outFollow << outS;
	}
}
#pragma endregion


