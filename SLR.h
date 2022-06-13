#pragma once
//一个项目：包含一个产生式
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

	//判断是否为规约项目
	bool judgeEnd();

	//点向右移动
	Item moveRight();

	//判断两个项目是否相等
	bool operator==(const Item& item);

	//判断打点的下一个符号
	bool isNext(string symbol);

	string showItem();
};

//op为操作：S为移进   R为规约  ACC为接受
typedef struct m
{
	string op;
	int dest;
	m(string o, int d) :op(o), dest(d) {};
	m() {};
}Actioncell;

//每个状态:有多个项目
class State
{
public:

	//产生式打点项目集
	vector<Item> items;

	//当前状态接受符号string到达的状态序号
	map<string, int> GOTO;

	//当前状态可以接受的符号
	vector<string> nextSymbols;

	State();

	State(Item ite);

	State(vector<Item> ites);

	//求当前项目集的CLOSURE闭包
	void CLOSURE();

	bool isNewItem(Item newItem);

	//当前状态产生式打点的下一个符号

	//当前状态 接收符号symbol前往的位置
	State getGOTO(string symbol);

	string showState(int no);


	//判断两个状态是否相等：若所有的产生式打点项目相等则相等
	bool operator==(const State& s);
};

class SLR
{
public:
	int Stateid = 0;

	string StatesPath = "SLRstates.txt";
	string SLRtablePath = "SLRtable.txt";
	//用Vector存状态，其下标说明序号
	vector<State> SLRstates;

	//GOTO表和ACTION表，用vector下标表示状态标号
	//每一个状态的转移为map，当symbol不在map中进行出错处理,这样减少了空间占用但是时间复杂度提高
	//可以用unorderedmap代替
	vector<unordered_map<string, int>> GOTOtable;
	vector <unordered_map<string, Actioncell>> ACTIONtable;

	void buildstates();
	void showAllstates();
	void buildSLR();
	void showSLR();
};





