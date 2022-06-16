#include"Token.h"
//定义了各种词法单元
using namespace std;

Token::Token(int t)
{
		Name = t;
}
Token::Token() {}
string Token::display()
{
	string a = "a";
	if (Name != 0)
		a[0] = char(Name);
	else
		a = "!EOF!";
	return "<TokenName=" + a + "," + "value=Null>";
}
string Token::getValue(){

string a = "a";
if (Name != 0)
	a[0] = char(Name);
else
	a = "!EOF!";
return a;
};



Integer::Integer(int value):Token(Tag::INTEGER)
{
	this->value = value;
}
string Integer::display()
{
	return "<TokenName=INTEGER,value=" + getValue() + ">";
}
string Integer::getValue()
{return to_string(value);
}

Character::Character(char value) :Token(Tag::CHARACTER)
{
	this->value = value;
}
string Character::display()
{
	return "<TokenName=CHARACTER,value=" + getValue() + ">";
}
int Character::getWidth()
{
	return 1;
};
string Character::getValue()
{
	string a = "a";
	a[0] = value;
	return "'" + a + "'";
}


Word::Word(string s, int tag) :Token(tag)
{
	this->lexeme = s;
}
Word::Word() {}
string  Word::display()
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
string Word::getValue()
{
	return lexeme;
}


//转换函数，将enum下标转化为字符串
string convertName(int name)
{
	if (name == 0)
	{
		return "!EOF!";
	}
	else if (name <= 256)
	{
		string a = "0";
		a[0] = char(name);
		return a;
	}
	else
	{
		string result;
		switch (name)
		{
		case 257:result = "integer"; break;
		case 258:result = "char"; break;
		case 259:result = "ID"; break;
		case 260:result = "if"; break;
		case 261:result = "else"; break;
		case 262:result = "do"; break;
		case 263:result = "while"; break;
		case 264:result = "&&"; break;
		case 265:result = "||"; break;
		case 266:result = "=="; break;
		case 267:result = ">="; break;
		case 268:result = "<="; break;
		case 269:result = "!="; break;
		case 270:result = "int"; break;
		case 271:result = "character"; break;
		}
		return result;
	}
}