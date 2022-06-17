#pragma once
#include<iostream>
#include<string>
#include<list>
#include<unordered_map>
using namespace std;

// ע��С���š��������Լ����ַ����������Ҫ����enum ��ΪASCII�����ж���
// INTEGER��ʾ���ͳ���     CHARACTERΪ�ַ�����     CHAR��INTΪ���͹ؼ���
enum Tag { INTEGER = 257, CHAR, ID, IF, ELSE, DO, WHILE, AND, OR, EQ, GE, LE, NEQ, INT, CHARACTER, BASIC };


//���ĸ������ڹ���ͬ�Ĵʷ���Ԫ
class Token
{
public:
	int Name;    //�ʷ���Ԫ��ʾ����ö��Tag��ȡֵ
	Token(int t);
	Token() ;
	//����toString�����Ϣ
	virtual string display();
	virtual string getValue();
};

//���ͳ���
class Integer :public Token
{
public:
	int value;

	Integer(int value);

	string display();
	string getValue();
};

//�ַ�����
class Character :public Token
{
public:
	char value;
	Character(char value);

	string display();
	int getWidth();
	string getValue();
};

//������:��������TYPEӦ��ΪWord�����࣬ӦΪTYPE���Ǳ�����
class Word :public Token
{
public:
	// �ʷ���Ԫ����
	string lexeme = "";

	Word(string s, int tag);

	Word();


	string display();

	string getValue();

};

//��ʶ����û���õ�
class Id :Word
{
public:
	string type;
	int width;
	string lexeme;

	Id() :Word()
	{}

	Id(string le) :Word(le, Tag::ID)
	{
		lexeme = le;
	}
};

//���ڼ�¼��ʶ������û���õ�
class Env
{
public:
	unordered_map<string, class Id> Table;
	Env* parent = NULL;

	Env()
	{
		parent = NULL;
	}

	Env(Env& v)
	{
		parent = &v;
	}

	bool put(string key, Id i)
	{
		if (Table.find(key) == Table.end())
		{
			Table[key] = i;
		}
		else
		{
			return false;
		}
	}

	Id* find(string key)
	{
		for (Env* e = this; e != NULL; e = e->parent)
		{
			if (e->Table.find(key) != Table.end())
			{
				Id* f = &e->Table.at(key);
				return f;
			}
			else
			{
				return NULL;
			}
		}
	}
};
//������������:int �� char,�����洢������������ ����Ҫ֪����С,Ҳû��ʹ��

class Type :public Word
{
public:
	int width = 0;
	Type(string s, int tag, int w) :Word(s, tag)
	{
		this->width = w;
	}

	bool operator==(const Type t)
	{
		return(width == t.width && Name == t.Name && lexeme == t.lexeme);
	}

	//����ת��
	static bool numeric(Type p)
	{
		Type Int = Type("int", INT, 4);
		Type Char = Type("char", CHAR, 1);
		if (p == Int || p == Char)
			return true;
		else
			return false;
	}

	static Type max(Type p1, Type p2)
	{
		if (!numeric(p1) || !numeric(p2)) return Type("error", BASIC, 4);
		else if (p1.Name == INT || p2.Name == CHAR)
			return Type("int", INT, 4);
		else
			return Type("char", CHAR, 1);
	}
};

//ʹname�ܺ���������ƥ�䣬תΪ�ַ���
string convertName(int name);