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

	//���еĲ���ʽ����Ψһ���
	bool operator==(const production& p);
};

class WF
{
public:
	set<string> Vt;
	set<string> Vn;
	map<string, set<string>> First;
	map<string, set<string>> Follow;				
	map<string, list<production>> split_productions;//�ֽ��Ĳ���ʽ����
	vector<production> productions;
	string FirstOutPath = "First.txt";
	string FollowOutPath = "Follow.txt";
	string fileName = "Grammar.txt";    //�ķ�����ʽ�����ļ�

	//�ֶ��ж���Щ���ſ����Ƴ���:S,statements,declaration,M,N ;������ͨ���Ƶ������Ŀ�,�����ڼ����ݹ�
	set<string> nullSymbol{ "S","statements","declaration","M","N" };
	int number = 0;                  //��¼����ʽ���

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


//�ַ������ַ��ָ�
list<string> split(string str, string pattern);

//�ַ���ɾ���ض��ַ�
string deletmember(string& s, char m);

//��ĳ�������������д�
list<string> getNextSymbols(vector<string> src, vector<string>::iterator it, vector<string>::iterator end);