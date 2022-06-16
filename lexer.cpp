#include"lex.h"

Lexer::Lexer()
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

void  Lexer::init()
{
	line = 1;
	peek = ' ';
	index = -1;
	input = "";
	TokenList.clear();
}

//填Word表
void Lexer::reserve(Word w)
{
	Words.emplace(w.lexeme, w);
}

//获取文件输入
void Lexer::getInput()
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

//读入下一个字符
void Lexer::readch()
{
	index++;
	peek = input[index];
}

//读入下一个字符并判断
bool Lexer::readch(char c)
{
	readch();

	if (peek != c)
		return false;	//无需回退，识别下一个词素还停留在当前位置
	peek = ' ';		//设置为空，下次扫描自动度下一个字符
	return true;
}

//扫描输入，返回下一个词法单元
Token* Lexer::scan()
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
				index -= 2;
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
				} while (isdigit(peek) || peek >= 'A' && peek <= 'F');
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
			t = new Token('|');
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

//扫描所有词法单元
bool Lexer::scanAll()
{
	bool flag = true;
	//写的lexer.scan() 实际上是每次获得一个词法单元
	while (flag)
	{
		Token* temp = scan();
		if (temp != NULL)
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

//显示词法单元序列
void Lexer::showTokenList()
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
			cout << "<未识别词素>";
			outFile << "未识别词素";
		}
	}
	cout << endl;
}
