// 扫描源程序，转化为单词串
#include"lex.h"
#include"SLR.h"
#include<stack>
using namespace std;

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

typedef struct t
{
	int status;
	string symbol;
	t(int s, string y) :status(s), symbol(y) {};
}ParserItem;


typedef struct k
{


};

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
		Token a = *TokenList[Tokenindex++];
		while (true)
		{
			ParserItem s = parseStack.top();
			//查找动作
			if ( ACTIONtable[s.status].find(convertName(a.Name)) != ACTIONtable[s.status].end())
			{
				Actioncell act = ACTIONtable[s.status].at(convertName(a.Name));
				//移进动作
				if (act.op == "S")
				{
					parseStack.push(ParserItem(act.dest, convertName(a.Name)));
					a = *TokenList[Tokenindex++];
					cout << act.op<< act.dest << endl;
				}
				//规约动作
				else if (act.op == "R")
				{
					//获取规约用的产生式
					production p = wfdata.productions[act.dest];
					for (int i = 0; i < p.length; i++)
						parseStack.pop();
					//获取当前栈顶
					ParserItem nowTop = parseStack.top();
					parseStack.push(ParserItem( GOTOtable[nowTop.status][p.left] , p.left));
					cout << p.toString() << endl;;

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
				break;
			}
		}

	}
};

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
	SLR mySLR;
	mySLR.buildstates();
	mySLR.showAllstates();
	cout << "\n\n\n构造SLR分析表\n";
	mySLR.buildSLR();
	mySLR.showSLR();

	Lexer lexer = Lexer();

	Parser parser(lexer.TokenList, mySLR);

	while (true)
	{
		lexer.init();
		lexer.getInput();
		if (lexer.scanAll()) 
		{
			lexer.showTokenList();
			parser.TokenList = lexer.TokenList;
			parser.parsing();
		}
		int selection;
		cout << endl;
		cin >> selection;
		if (selection < sourcePath.size())
		{
			lexer.sourceCodePath = sourcePath[selection];
			lexer.tokenOutPath = tokenPath[selection];
		}

	}

	return 0;
}

