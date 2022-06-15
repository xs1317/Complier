#include"tools.h"
using namespace std;



string FirstOutPath = "First.txt";
string FollowOutPath = "Follow.txt";
int number = 0;                  //��¼����ʽ���
set<string> Vt;//�ս������
set<string> Vn;//���ս������
map<string, set<string>> First;//First��
map<string, list<production>> split_productions;//�ֽ��Ĳ���ʽ����
map<string, set<string>> Follow;//Follow��
vector<production> productions;
string fileName = "Grammar.txt";    //����ʽ����data.txt��  ��S��Ϊ��ʼ����
WF wfdata = WF(Vt, Vn, First, Follow, split_productions,productions);



#pragma region Definition
production::production(int i, vector<string>r, string l)
{
	left = l;
	right = r;
	length = r.size();
	//�Ҳ����п��򳤶ȼ���1
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
	//����Ƿ����е�Vn���ӽ�����
	if (oldM.size() == newM.size())
	{
		//���ÿһ��Vn��Follow���Ƿ���������
		for (auto it1 = newM.begin(); it1 != newM.end(); it1++)
		{
			string key = it1->first;
			set<string> value = it1->second;
			//����Ԫ�س���
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

//�ַ������ַ��ָ�
list<string> split(string str, string pattern)
{
	string::size_type pos;
	list<string> result;
	str += pattern;//��չ�ַ����Է������
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

//�ַ���ɾ���ض��ַ�
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
//��ʼ������ʽ��Vt��Vn
void init()
{
	string line;
	ifstream in(fileName);
	while (getline(in, line))
	{
		int position = line.find("->");
		if (position != -1)
		{
			string left = line.substr(0, position);     //��һ���Ƿ��ս����
			deletmember(left, ' ');
			string right = line.substr(position + 2);
			deletmember(right, '\t');
			//�����µķ��ս����
			if(Vn.find(left)==Vn.end())
				Vn.insert(left);

			if (Vn2.find(left) == Vn2.end()) 
				Vn2.insert(left);

			//�ָ��Ҳ�����ӵ�����ʽ
			list<string> splitRight = split(right, " ");
			list<string>::iterator it1;
			//����Ҳ����ŵ�Vn��Vt
			for (it1 = splitRight.begin(); it1 != splitRight.end(); it1++)
			{
				string temp = *it1;
				//�ս����:����''�������ս���� ɾ�����ż���
				if (temp[0] == '\'' || temp=="integer"||temp=="character" ||temp=="ID")
				{
					//�����µ��ս���ţ�ɾ��������
					string tempa = deletmember(temp , '\'');
					*it1 = tempa;
					if(Vt.find(tempa)==Vt.end())
						Vt.insert(tempa);

				}
				else
				{
					//�����µķ��ս��
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

//�ֶ��ж���Щ���ſ����Ƴ���:S,statements,declaration,M,N ;������ͨ���Ƶ������Ŀ�,�����ڼ����ݹ�
set<string> nullSymbol{ "S","statements","declaration","M","N" };
bool nullable(string symbol)
{
	if (nullSymbol.find(symbol) != nullSymbol.end())
		return true;
	else
		return false;
}

//��ĳһ�����ս���ŵ�First���������ж������ʽ��
void getOneFirst(string s)
{

	//�÷��ս�������в���ʽ�Ҳ�
	list<production> productions = split_productions.at(s);
	for (list<production>::iterator pit = productions.begin(); pit != productions.end(); pit++)
	{
		//ȡ��ĳһ������ʽ�Ҳ�
		production ptemp = *pit;
		//�����Ҳ���ÿһ������
		for (vector<string>::iterator sit = ptemp.right.begin(); sit != ptemp.right.end(); sit++)
		{
			string stemp = *sit;
			if (isVt(stemp))
			{
				//���˷ǿղ���ʽ�������ܺ��пգ�����������ս����ֱ�Ӽ���
				if (First[s].find(stemp) == First[s].end())
					First[s].insert(stemp);
				break;  //������һ������ʽ
			}
			else if (isVn(stemp))
			{
				if (stemp != s)
				{
					getOneFirst(stemp);
					//��First(stemp)����First(s)
					First[s].insert(First[stemp].begin(), First[stemp].end());
					//����Vn�����Ƴ��տ�����һ������
					if (nullable(stemp))	
						continue;
					else
						break;
				}
				else if (stemp == s && !nullable(stemp))  //��ݹ飬���ǿգ������First���й���
				{
					break;
				}
				else if (stemp == s && nullable(stemp))	  //��ݹ飬��Ϊ����Ҫ������һ������
				{
					continue;
				}

			}
		}
	}
}

//��ĳ�������������д�
list<string> getNextSymbols(vector<string> src,vector<string>::iterator it,vector<string>::iterator end)
{
	list<string> result;
	for (; it != end; it++)
	{
		result.push_back(*it);
	}
	return result;
}

//��һ�����ŵ�First��
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
				//��Vn��First��������
				if (First[sym].find("#") == First[sym].end())
				{
					result.insert(First[sym].begin(), First[sym].end());
					return result;
				}
				//��Vn��First������,�ѷǿյķ��ż��벢������һ������
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
					//��Vn���п��ҵ�ǰVnΪ���һ������
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

//��ÿ�����ս���ŵ�First��
void getFirst()
{
	ofstream outFirst(FirstOutPath);
	for (set<string>::iterator sit = Vn.begin(); sit != Vn.end(); sit++)
	{
		getOneFirst(*sit);

	}
	cout << "�ķ��������ս���ŵ�First��������ʾ(���� '#' ��ʾ��)��\r\n";
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

//���ÿ�����ս���ŵ�Follow�� �������� ��������ÿ������ʽ
void getFollow()
{
	//�ս���ż��뿪ʼ���ŵ�FOLLOW��
	Follow["S"].insert("!EOF!");
	map<string, set<string>> oldFollow = Follow;
	//�ظ����ж���ֱ��Follow�����ٸı�
	do
	{
		oldFollow = Follow;
		//����ÿһ������ʽ
		for (auto itPr = split_productions.begin(); itPr != split_productions.end(); itPr++)
		{
			string left = itPr->first;

			list<production> rights = itPr->second;
			//���������Ҳ�
			for(production Pright:rights)
			{
				//�����Ҳ�ÿһ������
				for (vector<string>::iterator itR = Pright.right.begin();itR!=Pright.right.end();itR++)
				{
					string Nowsymbol = *itR;
					//����Vt
					if (isVt(Nowsymbol))
					{
						continue;
					}
					else if (isVn(Nowsymbol))
					{
						vector<string>::iterator  itRnext = itR;
						itRnext++;
						//��ǰVn��ĩβ
						if (itRnext == Pright.right.end())
						{
							//Follow(left)����Follow(nowsymbol)
							Follow[Nowsymbol].insert(Follow[left].begin(), Follow[left].end());
						}
						else
						{
							list<string>nextSymbols = getNextSymbols(Pright.right, itRnext,Pright.right.end());
							set<string>nextFirst = getSymbolsFollow(nextSymbols);
							//������ ��First���޿�
							if (nextFirst.find("#") == nextFirst.end())
							{
								Follow[Nowsymbol].insert(nextFirst.begin(), nextFirst.end());
							}
							else
							{
								//����ǿ�
								for (string nf : nextFirst)
								{
									if (nf != "#")
										Follow[Nowsymbol].insert(nf);
								}
								//Follow(left)����Follow(nowsymbol)
								Follow[Nowsymbol].insert(Follow[left].begin(), Follow[left].end());
							}
						}
					}

				}
			}
		}
	} while (!followEqual(oldFollow, Follow));

	ofstream outFollow(FollowOutPath);
	cout << "�ķ��������ս���ŵ�Follow��������ʾ(���� '!EOF!' ��ʾ�ս��)��\r\n";
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


