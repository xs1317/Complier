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
	set<string> Vt;//�ս������
	set<string> Vn;//���ս������
	map<string, set<string>> First;//First��
	map<string, set<string>> Follow;//Follow��
	map<string, list<production>> split_productions;//�ֽ��Ĳ���ʽ����
	vector<production> productions;
	string FirstOutPath = "First.txt";
	string FollowOutPath = "Follow.txt";
	string fileName = "Grammar.txt";    //����ʽ����data.txt��  ��S��Ϊ��ʼ����

	//�ֶ��ж���Щ���ſ����Ƴ���:S,statements,declaration,M,N ;������ͨ���Ƶ������Ŀ�,�����ڼ����ݹ�
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