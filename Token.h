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
	string toString();
	virtual string display();
	virtual string getValue();
};

//���ͳ���
class Integer :public Token
{
public:
	int value;

	Integer(int value) :Token(Tag::INTEGER)
	{
		this->value = value;
	}

	string toString()
	{
		return to_string(value);
	}
	string display()
	{
		return "<TokenName=INTEGER,value=" + toString() + ">";
	}
	string getValue()
	{
		return to_string(value);
	}
};

//�ַ�����
class Character :public Token
{
public:
	char value;
	Character(char value) :Token(Tag::CHARACTER)
	{
		this->value = value;
	}

	string toString()
	{
		string a = "a";
		a[0] = value;
		return a;
	}
	string display()
	{
		return "<TokenName=CHARACTER,value=" + toString() + ">";
	}
	int getWidth()
	{
		return 1;
	};
	string getValue()
	{
		string a = "a";
		a[0] = value;
		return "'" + a + "'";
	}
};

//������:��������TYPEӦ��ΪWord�����࣬ӦΪTYPE���Ǳ�����
class Word :public Token
{
public:
	// �ʷ���Ԫ����
	string lexeme = "";
	Word(string s, int tag) :Token(tag)
	{
		this->lexeme = s;
	}

	Word() {};

	string toString()
	{
		return lexeme;
	}

	string display()
	{
		string TokenName = "";
		switch (Name)
		{
		case 257:TokenName = "INTEGER"; break;
		case 258:TokenName = "CHAR"; break;
		case 259:TokenName = "ID"; break;
		case 260:TokenName = "IF"; break;
		case 261:TokenName = "ELSE"; break;
		case 262:TokenName = "DO"; break;
		case 263:TokenName = "WHILE"; break;
		case 264:TokenName = "AND"; break;
		case 265:TokenName = "OR"; break;
		case 266:TokenName = "EQ"; break;
		case 267:TokenName = "GE"; break;
		case 268:TokenName = "LE"; break;
		case 269:TokenName = "NEQ"; break;
		case 270:TokenName = "INT"; break;
		case 271:TokenName = "CHARACTER"; break;
		}
		return "<TokenName=" + TokenName + "," + "value=" + lexeme + ">";
	}

	string getValue()
	{
		return lexeme;
	}

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

