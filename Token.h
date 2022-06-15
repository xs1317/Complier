#pragma once
#include<iostream>
#include<string>
#include<list>
#include<unordered_map>
using namespace std;

// 注意小括号、大括号以及单字符运算符不需要加入enum 因为ASCII对其有定义
// INTEGER表示整型常量     CHARACTER为字符常量     CHAR和INT为类型关键字
enum Tag { INTEGER = 257, CHAR, ID, IF, ELSE, DO, WHILE, AND, OR, EQ, GE, LE, NEQ, INT, CHARACTER, BASIC };

//这四个类用于管理不同的词法单元
class Token
{
public:
	int Name;    //词法单元表示，从枚举Tag获取值
	Token(int t);
	Token() ;
	//重载toString输出信息
	string toString();
	virtual string display();
	virtual string getValue();
};

//整型常量
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

//字符常量
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

//保留字:变量类型TYPE应当为Word的子类，应为TYPE都是保留字
class Word :public Token
{
public:
	// 词法单元名称
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

//标识符类没有用到
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

//用于记录标识符环境没有用到
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
//基本数据类型:int 和 char,不做存储分配和数组访问 不需要知道大小,也没有使用

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

	//类型转换
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

