#pragma once
//һ����Ŀ������һ������ʽ
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
#include<unordered_map>>
#include"tools.h"
class Item
{
public:
	int position;
	int length;
	production p;

	Item(production pt, int po);

	//�ж��Ƿ�Ϊ��Լ��Ŀ
	bool judgeEnd();

	//�������ƶ�
	Item moveRight();

	//�ж�������Ŀ�Ƿ����
	bool operator==(const Item& item);

	//�жϴ�����һ������
	bool isNext(string symbol);

	string showItem();
};

//opΪ������SΪ�ƽ�   RΪ��Լ  ACCΪ����
typedef struct m
{
	string op;
	int dest;
	m(string o, int d) :op(o), dest(d) {};
	m() {};
}Actioncell;

//ÿ��״̬:�ж����Ŀ
class State
{
public:

	//����ʽ�����Ŀ��
	vector<Item> items;

	//��ǰ״̬���ܷ���string�����״̬���
	map<string, int> GOTO;

	//��ǰ״̬���Խ��ܵķ���
	vector<string> nextSymbols;

	State();

	State(Item ite);

	State(vector<Item> ites);

	//��ǰ��Ŀ����CLOSURE�հ�
	void CLOSURE();

	bool isNewItem(Item newItem);

	//��ǰ״̬����ʽ������һ������

	//��ǰ״̬ ���շ���symbolǰ����λ��
	State getGOTO(string symbol);

	string showState(int no);


	//�ж�����״̬�Ƿ���ȣ������еĲ���ʽ�����Ŀ��������
	bool operator==(const State& s);
};

class SLR
{
public:
	int Stateid = 0;

	string StatesPath = "SLRstates.txt";
	string SLRtablePath = "SLRtable.txt";
	//��Vector��״̬�����±�˵�����
	vector<State> SLRstates;

	//GOTO���ACTION����vector�±��ʾ״̬���
	//ÿһ��״̬��ת��Ϊmap����symbol����map�н��г�����,���������˿ռ�ռ�õ���ʱ�临�Ӷ����
	//������unorderedmap����
	vector<unordered_map<string, int>> GOTOtable;
	vector <unordered_map<string, Actioncell>> ACTIONtable;

	void buildstates();
	void showAllstates();
	void buildSLR();
	void showSLR();
};





