// 扫描源程序，转化为单词串
#include<iostream>
#include<cstdio>
#include<fstream>
#include<string>
#include<unordered_map>
#include<list>
#include<fstream>
#include<vector>
#include<ctype.h>
using namespace std;

//首先用string串模拟输入
//char input[1024];
string input;
// 用unordered_map构建符号表 快速查找

// 注意小括号、大括号以及单字符运算符不需要加入enum 因为ASCII对其有定义
// INTEGER表示整型常量     CHARACTER为字符常量     CHAR和INT为类型关键字
enum Tag {INTEGER=257, CHAR,ID,IF,ELSE, DO,WHILE,AND,OR,EQ,GE,LE,NEQ,INT,CHARACTER,BASIC};


list <class Token*> TokenList;   //词法单元序列




//这四个类用于管理不同的词法单元
class Token
{
public:
	int Name;    //词法单元表示，从枚举Tag获取值
	Token(int t) 
	{ Name = t; }
	Token() {};
	//重载toString输出信息
	string toString()
	{
		string a = "a";
		if (Name != 0)
			a[0] = char(Name);
		else
			a = "!EOF!";
		return a;
	}
	virtual string display()
	{
		return "<TokenName=" + toString() + "," + "value=Null>";
	}

};
 
//整型常量
class Integer :public Token
{
public:
	int value;

	Integer(int value):Token(Tag::INTEGER)
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

};

//字符常量
class Character :public Token
{
public:
	char value;
	Character(char value) :Token(Tag::CHARACTER)
	{this->value = value;}

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
};

//保留字:变量类型TYPE应当为Word的子类，应为TYPE都是保留字
class Word :public Token
{
public:
	// 词法单元名称
	string lexeme = "";
	Word(string s, int tag) :Token(tag)
	{this->lexeme = s;}

	Word() {};

	string toString() 
	{return lexeme;}

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

};


//基本数据类型:int 和 char,不做存储分配和数组访问 不需要知道大小
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

// 做词法分析处理的类
// 键：词素，值：Token对象
class Lexer
{
public:
	//创建符号表并初始化

	int line = 1;
	//存放下一个输入字符,要么为空要么指向下一个字符；为空时可以略过，自动读入下一个
	char peek =' ';
	int index = -1;
	bool flag = true;

	unordered_map<string, Word>Words ={};

	//将所有关键字和运算符加入Word
	Lexer()
	{
		reserve(Word("if", Tag::IF));
		reserve(Word("else", Tag::ELSE));
		reserve(Word("do", Tag::DO));
		reserve(Word("while", Tag::WHILE));
		reserve(Word("&&", Tag::AND));
		reserve(Word("||", Tag::OR));
		reserve(Word("==", Tag::EQ));
		reserve(Word(">=", Tag::GE));
		reserve(Word("<=", Tag::LE));
		reserve(Word("!=", Tag::NEQ));
		reserve(Type("int", Tag::INT, 4));
		reserve(Type("char", Tag::CHAR, 1));
	}

	void reserve(Word w)
	{
		Words.emplace(w.lexeme, w);
	}

	//读入下一个字符
	void readch() 
	{
		index++;
		peek = input[index];
	}

	//读入下一个字符并判断
	bool readch(char c)
	{
		readch();

		if (peek != c) 
		return false;	//无需回退，识别下一个词素还停留在当前位置
		peek = ' ';		//设置为空，下次扫描自动度下一个字符
		return true;
	}

	Token * scan()
	{
		//略过空白符和注释,先进循环再读，所以每次应指向待识别部分第一个字符
		for (;; readch())
		{
			
			if (peek == ' ' || peek == '\t' || peek == '\r')continue;
			else if (peek == '\n') line = line + 1;
			else if (peek == '/')				//略过注释
			{
				if (readch('*'))
					while (1)
					{
						readch();
						if (peek == '*')
						{
							readch();
							if (peek == '/')
							{
								break;
							}
						}
					}
			}
			else break;
		}
		//整型
		if (isdigit(peek))
		{
			int v = 0;
			do
			{
				v = 10 * v + peek - '0';
				readch();
			} while (isdigit(peek));
			Integer* t = new Integer(v);
			return t;
		}
		//标识符与关键字
		if (isalpha(peek))
		{
			string s = "";
			do
			{
				s = s + peek;
				readch();
			} while (isalpha(peek));

			//检查表项中是否有该关键字
			if (Words.find(s) != Words.end())
			{
				return &Words.at(s);
			}
			else   //必然不是标识符
			{
				Word* w = new Word(s, Tag::ID);
				reserve(*w);
				return w;
			}
		}

		//字符常量:  'x'，其中只允许出现一个字符，注意考虑转义符的情况 \" \'等
		if (peek == '\'')
		{
			char c;
			readch();
			// 非转义符
			if (peek != '\\' and peek != '\'')
			{
				c = peek;
				readch();
			}
			else if (peek == '\'')
			{
				printf("line:%d,带引号的字符至少包含一个符号", line);
				readch();
			}
			else  //转义符识别
			{
				//当前状态： 识别了一个 反斜杠     
				//读入下一个字符
				readch();

				//数字,读入1、2、3位八进制字符
				if (isdigit(peek))
				{
					int tempX = 0;
					do
					{
						tempX = tempX * 8 + peek - '0';
						readch();
					} while (isdigit(peek));
					// 应当回退一个
					if (tempX > 255)
						cout << tempX << "对字符类型太大,line:" << line << endl;
					else
						c = char(tempX);
				}
				else if (peek == 'x')// \xh \xhh 形式
				{
					readch();
					int tempX = 0;
					do
					{
						tempX = tempX * 16 + peek - '0';
						readch();
					} while (isdigit(peek));
					if (tempX > 255)
					{
						cout << tempX << "对字符类型太大,line:" << line << endl;
						return NULL;
					}
					else
						c = char(tempX);
				}
				else
				{
					//后续定义了defalut 所以先进行数字的识别
					switch (peek)
					{
					case '\'':
						c = '\'';
						break;
					case '\"':
						c = '\"';
						break;
					case '\?':
						c = '\?';
						break;
					case '\\':
						c = '\\';
						break;
					case 'a':
						c = '\a';
						break;
					case 'b':
						c = '\b';
						break;
					case 'f':
						c = '\f';
						break;
					case 'n':
						c = '\n';
						break;
					case 'r':
						c = '\r';
						break;
					case 't':
						c = '\t';
						break;
					case 'v':
						c = '\v';
						break;
					default:	//对于未定义的转义符，忽略转义符
						c = peek;
						break;
					}
					readch();

				}
			}
			// 所有的情况都必须再读入一个单引号才能进行
			if (peek = '\'')
			{
				readch();//在识别下一个词素之前应当前进
				Character* t = new Character(c);
				return t;
			}
			else
			{
				cout << "缺失单引号或引号内有多个字符,line:" << line << endl;
			}
		}


		Token* t = NULL;
		//其他情况
		switch (peek)
		{
		
		case '&':
			if (readch('&'))  return &Words.at("&&");     //逻辑与符号
			else // 按位与
			{
				t = new Token('&');
				return t; 				
			}
		case '|':
			if (readch('|')) return &Words.at("||");    //逻辑或
			else                                        //按位或
			{
				t =new Token('|');
				return t;
			}	
		case '=':
			if (readch('=')) return &Words.at("==");    //比较运算：EQ
			else			                            //赋值号
			{
				t = new Token('=');
				return t;
			}
		case '<':
			if (readch('=')) return &Words.at("<=");	//比较:LEQ
			else                    					//比较：L
			{
				t = new Token('<');
				return t;
			}
		case '>':
			if (readch('=')) return &Words.at(">=");	//比较：GEQ
			else                                        //比较：G
			{
				t = new Token('>');
				return t;
			}
		case '!':
			if (readch('=')) return &Words.at("!=");	//比较：NEQ
			else                        				//取反
			{
				t = new Token('!');
				return t;
			}
		//其他单目符号
		case '\0':
			t = new Token(peek);
			return t;
		default: 
			t = new Token(peek);
			readch();
			return t;
		}

	}
};

void showTokenList(string outPath)
{
	ofstream outFile(outPath);


	for (auto t : TokenList)
	{
		if (t != NULL)
		{

			string out = t->display();
			if (t->Name == ';')
				out += "\n";
			cout << out;
			outFile << out;

		}
		else
		{
			cout << "<未识别词素>";
			outFile << "未识别词素";
		}
	}
}

//多遍扫描，构造一个序列存储单词串
int main()
{ 
	//首先测试以字符串作为输入
	ifstream inputFile;
	int datalen = 0;
	inputFile.open("D:\\Code\\lexer\\data.txt");
	string temp;
	while (getline(inputFile, temp))
	{
		input += temp;
		input += '\n';
	}
	Lexer lexer = Lexer();
	bool flag = true;
	//写的lexer.scan() 实际上是每次获得一个词法单元
	while (flag)
	{
		Token* temp = lexer.scan();
		TokenList.push_back(temp);
		if (temp->Name == 0) //获取到了文件结束符
			flag = false;
	}
	string outPath = "D:\\Code\\lexer\\Lex.txt";
	showTokenList(outPath);

	return 0;
}

