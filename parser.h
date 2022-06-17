#pragma once
#include"Token.h"

using namespace std;

typedef struct t
{
	int status;
	string symbol;

	Token* token;
	//addrΪID��ڻ���
	string addr;
	int temp;
	int instr;
	list<int> nextlist;
	list<int> falselist;
	list<int> truelist;
	//-1��ʾδʹ����ʱ����
	t(int s, string y, Token* t = NULL, int tem = -1) :status(s), symbol(y), token(t), temp(tem) {};
}ParserItem;

typedef struct k
{
	string op;
	string src1;
	string src2;
	string dest;
	k(string o, string sr1, string sr2, string des) :op(o), src1(sr1), src2(sr2), dest(des) {}

	string show()
	{
		return "( " + op + ", " + src1 + ", " + src2 + " ," + dest + " )";
	}

}formula;