// ɨ��Դ����ת��Ϊ���ʴ�
#include"lex.h"
#include"SLR.h"
#include<stack>
using namespace std;

bool flag = false;
// ��unordered_map�������ű� ���ٲ���

// ע��С���š��������Լ����ַ����������Ҫ����enum ��ΪASCII�����ж���
// INTEGER��ʾ���ͳ���     CHARACTERΪ�ַ�����     CHAR��INTΪ���͹ؼ���
enum Tag {INTEGER=257, CHAR,ID,IF,ELSE, DO,WHILE,AND,OR,EQ,GE,LE,NEQ,INT,CHARACTER,BASIC};

//ʹname�ܺ���������ƥ�䣬תΪ�ַ���
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

//���ĸ������ڹ���ͬ�Ĵʷ���Ԫ
class Token
{
public:
	int Name;    //�ʷ���Ԫ��ʾ����ö��Tag��ȡֵ
	Token(int t) 
	{ Name = t; }
	Token() {};
	//����toString�����Ϣ
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
 
//���ͳ���
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

//�ַ�����
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

//������:��������TYPEӦ��ΪWord�����࣬ӦΪTYPE���Ǳ�����
class Word :public Token
{
public:
	// �ʷ���Ԫ����
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


//������������:int �� char,�����洢������������ ����Ҫ֪����С
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

// ���ʷ������������
// �������أ�ֵ��Token����
class Lexer
{
public:
	//�������ű���ʼ��

	int line = 1;
	//�����һ�������ַ�,ҪôΪ��Ҫôָ����һ���ַ���Ϊ��ʱ�����Թ����Զ�������һ��
	char peek =' ';
	int index = -1;
	bool flag = true;
	string sourceCodePath = "sourceCode1.txt";
	string tokenOutPath = "Lex.txt";
	string input;
	vector <class Token*> TokenList;   //�ʷ���Ԫ����
	unordered_map<string, Word>Words ={};

	//�����йؼ��ֺ����������Word
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

	//������һ���ַ�
	void readch() 
	{
		index++;
		peek = input[index];
	}

	//������һ���ַ����ж�
	bool readch(char c)
	{
		readch();

		if (peek != c) 
		return false;	//������ˣ�ʶ����һ�����ػ�ͣ���ڵ�ǰλ��
		peek = ' ';		//����Ϊ�գ��´�ɨ���Զ�����һ���ַ�
		return true;
	}

	Token * scan()
	{
		//�Թ��հ׷���ע��,�Ƚ�ѭ���ٶ�������ÿ��Ӧָ���ʶ�𲿷ֵ�һ���ַ�
		for (;; readch())
		{
			
			if (peek == ' ' || peek == '\t' || peek == '\r')continue;
			else if (peek == '\n') line = line + 1;
			else if (peek == '/')				//�Թ�ע��
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
		//����
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
		//��ʶ����ؼ���
		if (isalpha(peek))
		{
			string s = "";
			do
			{
				s = s + peek;
				readch();
			} while (isalpha(peek));

			//���������Ƿ��иùؼ���
			if (Words.find(s) != Words.end())
			{
				return &Words.at(s);
			}
			else   //��Ȼ���Ǳ�ʶ��
			{
				Word* w = new Word(s, Tag::ID);
				reserve(*w);
				return w;
			}
		}

		//�ַ�����:  'x'������ֻ�������һ���ַ���ע�⿼��ת�������� \" \'��
		if (peek == '\'')
		{
			char c;
			readch();
			// ��ת���
			if (peek != '\\' and peek != '\'')
			{
				c = peek;
				readch();
			}
			else if (peek == '\'')
			{
				printf("line:%d,�����ŵ��ַ����ٰ���һ������", line);
				readch();
			}
			else  //ת���ʶ��
			{
				//��ǰ״̬�� ʶ����һ�� ��б��     
				//������һ���ַ�
				readch();

				//����,����1��2��3λ�˽����ַ�
				if (isdigit(peek))
				{
					int tempX = 0;
					do
					{
						tempX = tempX * 8 + peek - '0';
						readch();
					} while (isdigit(peek));
					// Ӧ������һ��
					if (tempX > 255)
					{
						cout << tempX << "���ַ�����̫��,line:" << line << endl;
						return NULL;
					}
					else
						c = char(tempX);
				}
				else if (peek == 'x')// \xh \xhh ��ʽ
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
						cout << tempX << "���ַ�����̫��,line:" << line << endl;
						return NULL;
					}
					else
						c = char(tempX);
				}
				else
				{
					//����������defalut �����Ƚ������ֵ�ʶ��
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
					default:	//����δ�����ת���������ת���
						c = peek;
						break;
					}
					readch();

				}
			}
			// ���е�����������ٶ���һ�������Ų��ܽ���
			if (peek == '\'')
			{
				readch();//��ʶ����һ������֮ǰӦ��ǰ��
				Character* t = new Character(c);
				return t;
			}
			else
			{
				cout << "ȱʧ�����Ż��������ж���ַ�,line:" << line << endl;
				return NULL;	
			}
		}


		Token* t = NULL;
		//�������
		switch (peek)
		{
		
		case '&':
			if (readch('&'))  return &Words.at("&&");     //�߼������
			else // ��λ��
			{
				t = new Token('&');
				return t; 				
			}
		case '|':
			if (readch('|')) return &Words.at("||");    //�߼���
			else                                        //��λ��
			{
				t =new Token('|');
				return t;
			}	
		case '=':
			if (readch('=')) return &Words.at("==");    //�Ƚ����㣺EQ
			else			                            //��ֵ��
			{
				t = new Token('=');
				return t;
			}
		case '<':
			if (readch('=')) return &Words.at("<=");	//�Ƚ�:LEQ
			else                    					//�Ƚϣ�L
			{
				t = new Token('<');
				return t;
			}
		case '>':
			if (readch('=')) return &Words.at(">=");	//�Ƚϣ�GEQ
			else                                        //�Ƚϣ�G
			{
				t = new Token('>');
				return t;
			}
		case '!':
			if (readch('=')) return &Words.at("!=");	//�Ƚϣ�NEQ
			else                        				//ȡ��
			{
				t = new Token('!');
				return t;
			}
		//������Ŀ����
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
				cout << "<δʶ�����>" ;
				outFile << "δʶ�����";
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
		//д��lexer.scan() ʵ������ÿ�λ��һ���ʷ���Ԫ
		while (flag)
		{
			Token* temp = scan();
			if(temp != NULL)
				TokenList.push_back(temp);
			else
			{
				cout << "�ʷ�����ʧ��" << endl;
				return false;
			}
			if (temp->Name == 0) //��ȡ�����ļ�������
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
	vector <class Token*> TokenList;   //�ʷ���Ԫ����
	vector<unordered_map<string, int>> GOTOtable;
	vector <unordered_map<string, Actioncell>> ACTIONtable;
	
	Parser(vector<class Token*> T,SLR s)
	{
		TokenList = T;
		GOTOtable = s.GOTOtable;
		ACTIONtable = s.ACTIONtable;
	}


	//����ACTIONtable��GOTOtable�Դʷ�������õĵ��ʴ����з���
	//��ӡ��Լ����
	void parsing()
	{
		int Tokenindex = 0;
		if (TokenList.size() == 0)
			return;
		stack<ParserItem> parseStack;
		//����״̬0���ս��
		parseStack.push(ParserItem(0, "!EOF!"));

		cout << "------------------�﷨����-------------------" << endl;

		//ȡ�׸�����
		Token a = *TokenList[Tokenindex++];
		while (true)
		{
			ParserItem s = parseStack.top();
			//���Ҷ���
			if ( ACTIONtable[s.status].find(convertName(a.Name)) != ACTIONtable[s.status].end())
			{
				Actioncell act = ACTIONtable[s.status].at(convertName(a.Name));
				//�ƽ�����
				if (act.op == "S")
				{
					parseStack.push(ParserItem(act.dest, convertName(a.Name)));
					a = *TokenList[Tokenindex++];
					cout << act.op<< act.dest << endl;
				}
				//��Լ����
				else if (act.op == "R")
				{
					//��ȡ��Լ�õĲ���ʽ
					production p = wfdata.productions[act.dest];
					for (int i = 0; i < p.length; i++)
						parseStack.pop();
					//��ȡ��ǰջ��
					ParserItem nowTop = parseStack.top();
					parseStack.push(ParserItem( GOTOtable[nowTop.status][p.left] , p.left));
					cout << p.toString() << endl;;

				}
				else if (act.op == "ACC")
				{
					cout << "�﷨�������"<<endl;
					break;
				}
			}
			else
			{
				cout << "����" << endl;
				break;
			}
		}

	}
};

vector<string> sourcePath = {"sourceCode.txt","sourceCode1.txt","sourceCode2.txt","sourceCode3.txt" ,"sourceCode4.txt" ,"sourceCode5.txt" };
vector<string> tokenPath = { "Token.txt","Token1.txt","Token2.txt","Token3.txt" ,"Token4.txt" ,"Token5.txt" };
//���ɨ�裬����һ�����д洢���ʴ�
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
	cout << "\n\n\n����SLR������\n";
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

