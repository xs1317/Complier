#pragma once
#include<iostream>
#include<cstdio>
#include<fstream>
#include<string>
#include<list>
#include<fstream>
#include<vector>
#include<ctype.h>
#include<set>
#include<map>

using namespace std;
class production
{
public:
	int length;
	int id;
	string left;
	vector<string> right;

	production(int i, vector<string>r, string l);
	

	production();

	string toString();

	//所有的产生式都有唯一标号
	bool operator==(const production& p);
};

class WF
{
public:
	set<string> Vt;//终结符集合
	set<string> Vn;//非终结符集合
	map<string, set<string>> First;//First集
	map<string, set<string>> Follow;//Follow集
	map<string, list<production>> split_productions;//分解后的产生式集合
	vector<production> productions;
	string FirstOutPath = "First.txt";
	string FollowOutPath = "Follow.txt";
	string fileName = "Grammar.txt";    //产生式存在data.txt中  以S作为开始符号

	//手动判断哪些符号可以推出空:S,statements,declaration,M,N ;不存在通过推导产生的空,不存在间接左递归
	set<string> nullSymbol{ "S","statements","declaration","M","N" };

	WF(set<string>t, set<string>n, map<string, set<string>>Fi, map<string, set<string>> Fo, map<string, list<production>> sp, vector<production>p);

	void getvalue(set<string>t, set<string>n, map<string, set<string>>Fi, map<string, set<string>> Fo, map<string, list<production>> sp, vector<production>p);


};


extern WF wfdata;
void init();
void getFirst();
void getFollow();
void initWfdata();
bool isVn(string s);
bool isVt(string s);