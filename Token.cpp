#include"Token.h"
//定义了各种词法单元


Token::Token(int t)
{
		Name = t;
}
Token::Token() {}
string Token:: toString()
{
	string a = "a";
	if (Name != 0)
		a[0] = char(Name);
	else
		a = "!EOF!";
	return a;
}
string Token::display()
{
	return "<TokenName=" + toString() + "," + "value=Null>";
}
string Token::getValue(){return "";};

