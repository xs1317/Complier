// ɨ��Դ����ת��Ϊ���ʴ�
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

//������string��ģ������
//char input[1024];
string input;
// ��unordered_map�������ű� ���ٲ���

// ע��С���š��������Լ����ַ����������Ҫ����enum ��ΪASCII�����ж���
// INTEGER��ʾ���ͳ���     CHARACTERΪ�ַ�����     CHAR��INTΪ���͹ؼ���
enum Tag {INTEGER=257, CHAR,ID,IF,ELSE, DO,WHILE,AND,OR,EQ,GE,LE,NEQ,INT,CHARACTER,BASIC};


list <class Token*> TokenList;   //�ʷ���Ԫ����




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
						cout << tempX << "���ַ�����̫��,line:" << line << endl;
					else
						c = char(tempX);
				}
				else if (peek == 'x')// \xh \xhh ��ʽ
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
			if (peek = '\'')
			{
				readch();//��ʶ����һ������֮ǰӦ��ǰ��
				Character* t = new Character(c);
				return t;
			}
			else
			{
				cout << "ȱʧ�����Ż��������ж���ַ�,line:" << line << endl;
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
			cout << "<δʶ�����>";
			outFile << "δʶ�����";
		}
	}
}

//���ɨ�裬����һ�����д洢���ʴ�
int main()
{ 
	//���Ȳ������ַ�����Ϊ����
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
	//д��lexer.scan() ʵ������ÿ�λ��һ���ʷ���Ԫ
	while (flag)
	{
		Token* temp = lexer.scan();
		TokenList.push_back(temp);
		if (temp->Name == 0) //��ȡ�����ļ�������
			flag = false;
	}
	string outPath = "D:\\Code\\lexer\\Lex.txt";
	showTokenList(outPath);

	return 0;
}

