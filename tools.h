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
	set<string> Vt;
	set<string> Vn;
	map<string, set<string>> First;
	map<string, set<string>> Follow;				
	map<string, list<production>> split_productions;//分解后的产生式集合
	vector<production> productions;
	string FirstOutPath = "First.txt";
	string FollowOutPath = "Follow.txt";
	string fileName = "Grammar.txt";    //文法产生式所在文件

	//手动判断哪些符号可以推出空:S,statements,declaration,M,N ;不存在通过推导产生的空,不存在间接左递归
	set<string> nullSymbol{ "S","statements","declaration","M","N" };
	int number = 0;                  //记录产生式编号

	WF();

	void outProductions(string outPath);
	bool followEqual(map<string, set<string>> oldM, map<string, set<string>> newM);
	void init();
	void getFirst();
	void getFollow();
	set<string> getSymbolsFollow(list<string> symbols);
	bool isVn(string s);
	bool isVt(string s);
	void getOneFirst(string s);
	bool nullable(string symbol);

};

extern WF wfdata;


//字符串以字符分割
list<string> split(string str, string pattern);

//字符串删除特定字符
string deletmember(string& s, char m);

//求某迭代器后续所有串
list<string> getNextSymbols(vector<string> src, vector<string>::iterator it, vector<string>::iterator end);