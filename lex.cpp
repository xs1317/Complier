// 扫描源程序，转化为单词串
#include"lex.h"
#include"SLR.h"
#include<stack>
#include<functional>
using namespace std;

string formulaPath = "formula.txt";

bool flag = false;
// 用unordered_map构建符号表 快速查找

// 注意小括号、大括号以及单字符运算符不需要加入enum 因为ASCII对其有定义
// INTEGER表示整型常量     CHARACTER为字符常量     CHAR和INT为类型关键字
enum Tag {INTEGER=257, CHAR,ID,IF,ELSE, DO,WHILE,AND,OR,EQ,GE,LE,NEQ,INT,CHARACTER,BASIC};

//使name能和其他部分匹配，转为字符串
string convertName(int name)
{
	if (name == 0)
	{
		return "!EOF!";
	}
	else if(name <= 256)
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

#pragma region Token



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
	virtual string getValue()
	{
		return "";
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
	string getValue()
	{
		return to_string(value);
	}
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
#pragma endregion


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
	string sourceCodePath = "sourceCode1.txt";
	string tokenOutPath = "Lex.txt";
	string input;
	vector <class Token*> TokenList;   //词法单元序列
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

	void init()
	{
		line = 1;
		peek = ' ';
		index = -1;
		input = "";
		TokenList.clear();
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
				{
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
				//应该为除号
				else
				{
					index -=2;
					readch();
					break;
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
					{
						cout << tempX << "对字符类型太大,line:" << line << endl;
						return NULL;
					}
					else
						c = char(tempX);
				}
				else if (peek == 'x')// \xh \xhh 形式
				{
					readch();
					int tempX = 0;
					do
					{
						if (isdigit(peek))
							tempX = tempX * 16 + peek - '0';
						else
							tempX = tempX * 16 + peek - 'A' + 10;
						readch();
					} while (isdigit(peek)|| peek>='A' && peek<='F');
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
			if (peek == '\'')
			{
				readch();//在识别下一个词素之前应当前进
				Character* t = new Character(c);
				return t;
			}
			else
			{
				cout << "缺失单引号或引号内有多个字符,line:" << line << endl;
				return NULL;	
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
	void showTokenList()
	{
		ofstream outFile(tokenOutPath);

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
				cout << "<未识别词素>" ;
				outFile << "未识别词素";
			}
		}
		cout << endl;
	}

	void getInput()
	{
		ifstream inputFile(sourceCodePath);
		input = "";
		int datalen = 0;
		string temp;
		while (getline(inputFile, temp))
		{
			input += temp;
			input += '\n';
		}
	}

	bool scanAll()
	{
		bool flag = true;
		//写的lexer.scan() 实际上是每次获得一个词法单元
		while (flag)
		{
			Token* temp = scan();
			if(temp != NULL)
				TokenList.push_back(temp);
			else
			{
				cout << "词法分析失败" << endl;
				return false;
			}
			if (temp->Name == 0) //获取到了文件结束符
				flag = false;
		}
		return true;
	}
};



//为了方便起见，加入所有需要的属性
vector<int> tempV;
//找到一个可以用的
int findTempV()
{
	for (int i = 0; i < tempV.size(); i++)
	{
		if (tempV[i] == 0)
		{
			tempV[i] = 1;
			return i;
		}
	}
	
	tempV.push_back(1);
	return tempV.size() - 1;
}
void releaseTempV(int i)
{
	if(i!=-1)
		tempV[i] = 0;
}
typedef struct t
{
	int status;
	string symbol;
	
	Token* token;
	//addr为ID入口或常量
	string addr;
	int temp;
	//-1表示未使用临时变量
	t(int s, string y,Token* t=NULL,int tem = -1) :status(s), symbol(y),token(t),temp(tem){};
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

//标识符环境
Env *env;
vector<formula> code;

//用于传递继承属性
string type;
int width;
int nextquad;
void gen(int nextquad, string op, string src1, string src2, string dest)
{
	code.push_back(formula(op, src1, src2, dest));
}

#pragma region  SDTaction
ParserItem transP0(production p, vector<ParserItem> items)
{
	cout << "	调用了一个语义分析命令" << endl;
	return ParserItem(-1, "");
}
ParserItem transP1(production p, vector<ParserItem> items)
{
	cout << "	调用了一个语义分析命令" << endl;
	ParserItem pit(-1, "");
	pit.addr = items[0].token->getValue();
	return pit;
}

ParserItem transP2(production p, vector<ParserItem> items)
{
	cout << "	调用了一个语义分析命令" << endl;
	ParserItem pit(-1, "");
	pit.addr = items[0].token->getValue();
	return pit;
}

ParserItem transP15(production p, vector<ParserItem> items)
{
	cout << "	调用了一个语义分析命令" << endl;
	type = "int";
	width = 4;
	return ParserItem(-1, "");
}

ParserItem transP16(production p, vector<ParserItem> items)
{
	cout << "	调用了一个语义动作" << endl;
	type = "char";
	width = 1;
	return ParserItem(-1, "");
}

ParserItem transP22(production p, vector<ParserItem> items)
{
	cout << "	调用了一个语义动作" << endl;
	ParserItem pit(-1, "");
	pit.addr = items[0].addr;
	return pit;
}

ParserItem transP22(production p, vector<ParserItem> items)
{
	cout << "	调用了一个语义动作" << endl;
	ParserItem pit(-1, "");
	pit.addr = items[0].addr;
	return pit;
}

//逗号表达式，取最左边的传递
ParserItem transP23(production p, vector<ParserItem> items)
{
	cout << "	调用了一个语义动作" << endl;
	ParserItem pit(-1, "");
	pit.addr = items[0].addr;
	return pit;
}

ParserItem transP24(production p, vector<ParserItem> items)
{
	cout << "	调用了一个语义动作" << endl;
	ParserItem pit(-1, "");
	pit.addr = items[0].addr;
	return pit;
}

ParserItem transP25(production p, vector<ParserItem> items)
{
	cout << "	调用了一个语义动作" << endl;
	ParserItem pit(-1, "");
	//无需生成临时变量
	pit.addr = items[0].token->getValue();
	
	//生成四元式
	gen(nextquad++, "=", items[2].addr, "", items[0].token->getValue());

	//销毁临时变量
	releaseTempV(items[2].temp);
	return pit;
}

ParserItem transP26(production p, vector<ParserItem> items)
{
	cout << "	调用了一个语义动作" << endl;
	ParserItem pit(-1, "");
	pit.addr = items[0].addr;
	return pit;
}

ParserItem transP27(production p, vector<ParserItem> items)
{
	cout << "	调用了一个语义动作" << endl;
	ParserItem pit(-1, "");
	//生成新的临时变量
	pit.temp = findTempV();
	string addr = "temp" + to_string(pit.temp);
	pit.addr = addr;
	//生成四元式
	gen(nextquad++, "or", items[0].addr, items[2].addr, addr);

	//销毁临时变量
	releaseTempV(items[0].temp);
	releaseTempV(items[2].temp);
	return pit;
}

ParserItem transP28(production p, vector<ParserItem> items)
{
	cout << "	调用了一个语义动作" << endl;
	ParserItem pit(-1, "");
	pit.addr = items[0].addr;
	return pit;
}

ParserItem transP29(production p, vector<ParserItem> items)
{
	cout << "	调用了一个语义动作" << endl;
	ParserItem pit(-1, "");
	//生成新的临时变量
	pit.temp = findTempV();
	string addr = "temp" + to_string(pit.temp);
	pit.addr = addr;
	//生成四元式
	gen(nextquad++, "and", items[0].addr, items[2].addr, addr);

	//销毁临时变量
	releaseTempV(items[0].temp);
	releaseTempV(items[2].temp);
	return pit;
}

ParserItem transP30(production p, vector<ParserItem> items)
{
	cout << "	调用了一个语义动作" << endl;
	ParserItem pit(-1, "");
	pit.addr = items[0].addr;
	return pit;
}

ParserItem transP31(production p, vector<ParserItem> items)
{
	cout << "	调用了一个语义动作" << endl;
	ParserItem pit(-1, "");
	//生成新的临时变量
	pit.temp = findTempV();
	string addr = "temp" + to_string(pit.temp);
	pit.addr = addr;
	//生成四元式
	gen(nextquad++, "==", items[0].addr, items[2].addr, addr);

	//销毁临时变量
	releaseTempV(items[0].temp);
	releaseTempV(items[2].temp);
	return pit;
}

ParserItem transP32(production p, vector<ParserItem> items)
{
	cout << "	调用了一个语义动作" << endl;
	ParserItem pit(-1, "");
	//生成新的临时变量
	pit.temp = findTempV();
	string addr = "temp" + to_string(pit.temp);
	pit.addr = addr;
	//生成四元式
	gen(nextquad++, "!=", items[0].addr, items[2].addr, addr);

	//销毁临时变量
	releaseTempV(items[0].temp);
	releaseTempV(items[2].temp);
	return pit;
}

ParserItem transP33(production p, vector<ParserItem> items)
{
	cout << "	调用了一个语义动作" << endl;
	ParserItem pit(-1, "");
	pit.addr = items[0].addr;
	return pit;
}

ParserItem transP34(production p, vector<ParserItem> items)
{
	cout << "	调用了一个语义动作" << endl;
	ParserItem pit(-1, "");
	//生成新的临时变量
	pit.temp = findTempV();
	string addr = "temp" + to_string(pit.temp);
	pit.addr = addr;
	//生成四元式
	gen(nextquad++, "<", items[0].addr, items[2].addr, addr);

	//销毁临时变量
	releaseTempV(items[0].temp);
	releaseTempV(items[2].temp);
	return pit;
}

ParserItem transP35(production p, vector<ParserItem> items)
{
	cout << "	调用了一个语义动作" << endl;
	ParserItem pit(-1, "");
	//生成新的临时变量
	pit.temp = findTempV();
	string addr = "temp" + to_string(pit.temp);
	pit.addr = addr;
	//生成四元式
	gen(nextquad++, "<=", items[0].addr, items[2].addr, addr);

	//销毁临时变量
	releaseTempV(items[0].temp);
	releaseTempV(items[2].temp);
	return pit;
}

ParserItem transP36(production p, vector<ParserItem> items)
{
	cout << "	调用了一个语义动作" << endl;
	ParserItem pit(-1, "");
	//生成新的临时变量
	pit.temp = findTempV();
	string addr = "temp" + to_string(pit.temp);
	pit.addr = addr;
	//生成四元式
	gen(nextquad++, ">", items[0].addr, items[2].addr, addr);

	//销毁临时变量
	releaseTempV(items[0].temp);
	releaseTempV(items[2].temp);
	return pit;
}

ParserItem transP37(production p, vector<ParserItem> items)
{
	cout << "	调用了一个语义动作" << endl;
	ParserItem pit(-1, "");
	//生成新的临时变量
	pit.temp = findTempV();
	string addr = "temp" + to_string(pit.temp);
	pit.addr = addr;
	//生成四元式
	gen(nextquad++, ">=", items[0].addr, items[2].addr, addr);

	//销毁临时变量
	releaseTempV(items[0].temp);
	releaseTempV(items[2].temp);
	return pit;
}

ParserItem transP38(production p, vector<ParserItem> items)
{
	cout << "	调用了一个语义动作" << endl;
	ParserItem pit(-1, "");
	pit.addr = items[0].addr;
	return pit;
}

ParserItem transP39(production p, vector<ParserItem> items)
{
	cout << "	调用了一个语义动作" << endl;
	ParserItem pit(-1, "");
	//生成新的临时变量
	pit.temp = findTempV();
	string addr = "temp" + to_string(pit.temp);
	pit.addr = addr;
	//生成四元式
	gen(nextquad++, "+", items[0].addr, items[2].addr, addr);

	//销毁临时变量
	releaseTempV(items[0].temp);
	releaseTempV(items[2].temp);
	return pit;
}

ParserItem transP40(production p, vector<ParserItem> items)
{
	cout << "	调用了一个语义动作" << endl;
	ParserItem pit(-1, "");
	//生成新的临时变量
	pit.temp = findTempV();
	string addr = "temp" + to_string(pit.temp);
	pit.addr = addr;
	//生成四元式
	gen(nextquad++, "-", items[0].addr, items[2].addr, addr);

	//销毁临时变量
	releaseTempV(items[0].temp);
	releaseTempV(items[2].temp);
	return pit;
}

ParserItem transP41(production p, vector<ParserItem> items)
{
	cout << "	调用了一个语义动作" << endl;
	ParserItem pit(-1, "");
	pit.addr = items[0].addr;
	return pit;
}

ParserItem transP42(production p, vector<ParserItem> items)
{
	cout << "	调用了一个语义动作" << endl;
	ParserItem pit(-1, "");
	//生成新的临时变量
	pit.temp = findTempV();
	string addr = "temp" + to_string(pit.temp);
	pit.addr = addr;
	//生成四元式
	gen(nextquad++, "*", items[0].addr, items[2].addr, addr);

	//销毁临时变量
	releaseTempV(items[0].temp);
	releaseTempV(items[2].temp);
	return pit;
}

ParserItem transP43(production p, vector<ParserItem> items)
{
	cout << "	调用了一个语义动作" << endl;
	ParserItem pit(-1, "");
	//生成新的临时变量
	pit.temp = findTempV();

	string addr = "temp" + to_string(pit.temp);
	pit.addr = addr;
	//生成四元式
	gen(nextquad++, "/", items[0].addr, items[2].addr, addr);

	//销毁临时变量
	releaseTempV(items[0].temp);
	releaseTempV(items[2].temp);
	return pit;
}

ParserItem transP44(production p, vector<ParserItem> items)
{
	cout << "	调用了一个语义动作" << endl;
	ParserItem pit(-1, "");
	pit.addr = items[0].addr;
	return pit;
}

ParserItem transP45(production p, vector<ParserItem> items)
{
	cout << "	调用了一个语义动作" << endl;
	ParserItem pit(-1, "");
	pit.temp = findTempV();

	string addr = "temp" + to_string(pit.temp);
	pit.addr = addr;
	gen(nextquad++, "-", items[1].addr, "", addr);
	return pit;
}

ParserItem transP46(production p, vector<ParserItem> items)
{
	cout << "	调用了一个语义动作" << endl;
	ParserItem pit(-1, "");
	pit.temp = findTempV();

	string addr = "temp" + to_string(pit.temp);
	pit.addr = addr;
	gen(nextquad++, "+", items[1].addr, "", addr);
	return pit;
}

ParserItem transP47(production p, vector<ParserItem> items)
{
	cout << "	调用了一个语义动作" << endl;
	ParserItem pit(-1, "");
	pit.addr = items[0].token->getValue();
	return pit;
}

ParserItem transP48(production p, vector<ParserItem> items)
{
	cout << "	调用了一个语义动作" << endl;
	ParserItem pit(-1, "");
	pit.addr = items[0].addr;
	return pit;
}

ParserItem transP49(production p, vector<ParserItem> items)
{
	cout << "	调用了一个语义动作" << endl;
	ParserItem pit(-1, "");
	pit.addr = items[1].addr;
	return pit;
}
#pragma endregion


vector< ParserItem(*)(production, vector<ParserItem>)> trans;
void initTrans()
{
	trans.resize(wfdata.productions.size());
	trans[0] = transP0;
	trans[1] = transP1;
	trans[2] = transP2;
	trans[22] = transP22;
	trans[23] = transP23;
	trans[24] = transP24;
	trans[25] = transP25;
	trans[26] = transP26;
	trans[27] = transP27;
	trans[28] = transP28;
	trans[29] = transP29;
	trans[30] = transP30;
	trans[31] = transP31;
	trans[32] = transP32;
	trans[33] = transP33;
	trans[34] = transP34;
	trans[35] = transP35;
	trans[36] = transP36;
	trans[37] = transP37;
	trans[38] = transP38;
	trans[39] = transP39;
	trans[40] = transP40;
	trans[41] = transP41;
	trans[42] = transP42;
	trans[43] = transP43;
	trans[44] = transP44;
	trans[45] = transP45;
	trans[46] = transP46;
	trans[47] = transP47;
	trans[48] = transP48;
	trans[49] = transP49;
}


ParserItem SDT(production p, vector<ParserItem> items)
{
	auto t = trans[p.id];
	if (t != NULL)
		return t(p, items);
	else
		return ParserItem(-1, "");
}

//语法分析器
class Parser
{
public:
	vector <class Token*> TokenList;   //词法单元序列
	vector<unordered_map<string, int>> GOTOtable;
	vector <unordered_map<string, Actioncell>> ACTIONtable;



	Parser(vector<class Token*> T,SLR s)
	{
		TokenList = T;
		GOTOtable = s.GOTOtable;
		ACTIONtable = s.ACTIONtable;
	}

	//利用ACTIONtable和GOTOtable对词法分析获得的单词串进行分析
	//打印规约过程
	void parsing()
	{
		int Tokenindex = 0;
		if (TokenList.size() == 0)
			return;
		stack<ParserItem> parseStack;
		//加入状态0和终结符
		parseStack.push(ParserItem(0, "!EOF!"));

		cout << "------------------语法分析-------------------" << endl;

		//取首个符号
		Token* aptr = TokenList[Tokenindex];
		Token a = *TokenList[Tokenindex++];

		while (true)
		{
			ParserItem s = parseStack.top();
			//查找动作

			if ( ACTIONtable[s.status].find(convertName(a.Name)) != ACTIONtable[s.status].end())
			{
				Actioncell act = ACTIONtable[s.status].at(convertName(a.Name));
				//移进动作,引进来的一定是Vt
				if (act.op == "S")
				{
					parseStack.push(ParserItem(act.dest, convertName(a.Name),aptr));
					aptr = TokenList[Tokenindex];
					a = *TokenList[Tokenindex++];
					cout << act.op<< act.dest << endl;
					//大括号改变 标识符环境

				}
				//规约动作
				else if (act.op == "R")
				{

					//获取规约用的产生式
					production p = wfdata.productions[act.dest];
					vector<ParserItem> tempItems;
					for (int i = 0; i < p.length; i++)
					{
						ParserItem topItem = parseStack.top();
						tempItems.push_back(topItem);
						parseStack.pop();
					}
					cout << p.toString() << endl;;
					//获取当前栈顶
					ParserItem nowTop = parseStack.top();
					//求解左部属性（有副作用）
					reverse(tempItems.begin(), tempItems.end());
					ParserItem newParserItem = SDT(p, tempItems);
					//查分析表获得左部的状态与符号
					newParserItem.status = GOTOtable[nowTop.status][p.left];
					newParserItem.symbol = p.left;
					//插入分析栈
					parseStack.push(newParserItem);
					

				}
				else if (act.op == "ACC")
				{
					cout << "语法分析完成"<<endl;
					break;
				}
			}
			else
			{
				cout << "出错" << endl;
				if (a.Name == 270)
				{
					cout << "意外的declaration语句" << endl;
				}
				break;
			}
		}
	}

};

void showFormula()
{
	ofstream outFirst(formulaPath);
	cout << "\n\n部分语句四元式如下:\n";

	for (auto i : code)
	{
		string temp = "(" + i.op + " , " + i.src1 + " , " + i.src2 + " , " + i.dest + " )";
		outFirst << temp << endl;
		cout << temp << endl;;
	}
}

vector<string> sourcePath = {"sourceCode.txt","sourceCode1.txt","sourceCode2.txt","sourceCode3.txt" ,"sourceCode4.txt" ,"sourceCode5.txt" };
vector<string> tokenPath = { "Token.txt","Token1.txt","Token2.txt","Token3.txt" ,"Token4.txt" ,"Token5.txt" };
//多遍扫描，构造一个序列存储单词串
int main()
{ 
	init();
	getFirst();
	cout << endl; cout << endl; cout << endl; cout << endl;
	getFollow();
	initWfdata();
	wfdata.outProductions("productionsId.txt");
	SLR mySLR;
	mySLR.buildstates();
	mySLR.showAllstates();
	cout << "\n\n\n构造SLR分析表\n";
	mySLR.buildSLR();
	mySLR.showSLR();
	initTrans();
	Lexer lexer = Lexer();

	Parser parser(lexer.TokenList, mySLR);

	while (true)
	{
		//重建环境
		env = NULL;
		code.clear();
		nextquad = 0;

		int selection;
		cout << endl;
		cin >> selection;
		if (selection < sourcePath.size())
		{
			lexer.sourceCodePath = sourcePath[selection];
			lexer.tokenOutPath = tokenPath[selection];
		}
		lexer.init();

		lexer.getInput();
		if (lexer.scanAll()) 
		{
			lexer.showTokenList();
			parser.TokenList = lexer.TokenList;
			parser.parsing();
			showFormula();
		}


	}

	return 0;
}

