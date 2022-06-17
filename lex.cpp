// ɨ��Դ����ת��Ϊ���ʴ�
#include"lex.h"
#include"SLR.h"
#include"Token.h"
#include<stack>
#include<functional>
using namespace std;

string formulaPath = "formula.txt";




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
				//Ӧ��Ϊ����
				else
				{
					index -=2;
					readch();
					break;
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



//Ϊ�˷������������������Ҫ������
vector<int> tempV;
//�ҵ�һ�������õ�
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
	//addrΪID��ڻ���
	string addr;
	int temp;
	int instr;
	list<int> nextlist;
	list<int> falselist;
	list<int> truelist;
	//-1��ʾδʹ����ʱ����
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

//��ʶ������
Env *env;
vector<formula> code;

//���ڴ��ݼ̳�����
string type;
int width;
int nextquad;
void gen(int nextquad, string op, string src1, string src2, string dest)
{
	code.push_back(formula(op, src1, src2, dest));
}

list<int> merge(list<int>l1, list<int>l2)
{
	list<int> result;
	for (auto i : l1)
		result.push_back(i);
	for(auto i:l2)
		result.push_back(i);
	return result;
}
void backpatch(list<int> l, int instr)
{
	for (int i : l)
	{
		code[i].dest = to_string(instr);
	}
}


#pragma region  SDTaction
ParserItem transP0(production p, vector<ParserItem> items)
{
	cout << "	������һ�������������" << endl;
	ParserItem pit(-1, "");

	backpatch(items[0].nextlist, nextquad);
	return pit;



}
ParserItem transP1(production p, vector<ParserItem> items)
{
	cout << "	������һ�������������" << endl;
	ParserItem pit(-1, "");

	pit.addr = items[0].token->getValue();
	return pit;
}

ParserItem transP2(production p, vector<ParserItem> items)
{
	cout << "	������һ�������������" << endl;
	ParserItem pit(-1, "");
	pit.addr = items[0].token->getValue();
	return pit;
}

ParserItem transP3(production p, vector<ParserItem> items)
{
	cout << "	������һ�������������" << endl;
	ParserItem pit(-1, "");

	backpatch(items[3].truelist, items[5].instr);
	backpatch(items[6].nextlist, items[1].instr);
	pit.nextlist = items[3].falselist;
	gen(nextquad++, "GOTO", "", "", to_string(items[1].instr));
	return pit;
}
ParserItem transP4(production p, vector<ParserItem> items)
{
	cout << "	������һ�������������" << endl;
	ParserItem pit(-1, "");

	backpatch(items[2].truelist, items[4].instr);
	pit.nextlist = merge(items[2].falselist, items[5].nextlist);
	return pit;
}
ParserItem transP5(production p, vector<ParserItem> items)
{
	cout << "	������һ�������������" << endl;
	ParserItem pit(-1, "");

	backpatch(items[2].truelist, items[4].instr);
	backpatch(items[2].falselist, items[8].instr);
	list<int> temp = merge(items[5].nextlist, items[6].nextlist);
	pit.nextlist = merge(temp, items[9].nextlist);

	pit.nextlist = items[0].nextlist;
	return pit;
}


ParserItem transP6(production p, vector<ParserItem> items)
{
	cout << "	������һ�������������" << endl;
	ParserItem pit(-1, "");

	pit.nextlist = items[0].nextlist;
	return pit;
}

ParserItem transP7(production p, vector<ParserItem> items)
{
	cout << "	������һ�������������" << endl;
	ParserItem pit(-1, "");

	pit.nextlist = items[0].nextlist;
	return pit;
}

ParserItem transP8(production p, vector<ParserItem> items)
{
	cout << "	������һ�������������" << endl;
	ParserItem pit(-1, "");


	pit.nextlist = items[0].nextlist;
	return pit;
}

ParserItem transP9(production p, vector<ParserItem> items)
{
	cout << "	������һ�������������" << endl;
	ParserItem pit(-1, "");

	pit.nextlist = items[0].nextlist;

	return pit;
}


ParserItem transP12(production p, vector<ParserItem> items)
{
	cout << "	������һ�������������" << endl;
	ParserItem pit(-1, "");
	
	pit.nextlist = items[2].nextlist;

	return pit;
}



//statements -> statements1 M statement   ��M������statements1��next��,������statement��next��
ParserItem transP13(production p, vector<ParserItem> items)
{
	cout << "	������һ�������������" << endl;
	ParserItem pit(-1, "");

	backpatch(items[0].nextlist, items[1].instr);
	pit.nextlist = items[2].nextlist;

	return pit;
}


ParserItem transP14(production p, vector<ParserItem> items)
{
	cout << "	������һ�������������" << endl;
	ParserItem pit(-1, "");

	return pit;
}

ParserItem transP15(production p, vector<ParserItem> items)
{
	cout << "	������һ�������������" << endl;
	type = "int";
	width = 4;
	return ParserItem(-1, "");
}

ParserItem transP16(production p, vector<ParserItem> items)
{
	cout << "	������һ�����嶯��" << endl;
	type = "char";
	width = 1;
	return ParserItem(-1, "");
}

//����û�й�����ű���������ģ��һ��
ParserItem transP20(production p, vector<ParserItem> items)
{
	cout << "	������һ�����嶯��" << endl;
	ParserItem pit(-1, "");
	pit.addr = items[0].addr;
	return pit;
}

ParserItem transP21(production p, vector<ParserItem> items)
{
	cout << "	������һ�����嶯��" << endl;
	ParserItem pit(-1, "");
	pit.addr = items[0].token->getValue();

	return pit;
}

ParserItem transP22(production p, vector<ParserItem> items)
{
	cout << "	������һ�����嶯��" << endl;
	ParserItem pit(-1, "");
	pit.addr = items[0].addr;
	return pit;
}

//���ű��ʽ��ȡ����ߵĴ���
ParserItem transP23(production p, vector<ParserItem> items)
{
	cout << "	������һ�����嶯��" << endl;
	ParserItem pit(-1, "");
	pit.addr = items[0].addr;
	return pit;
}

ParserItem transP24(production p, vector<ParserItem> items)
{
	cout << "	������һ�����嶯��" << endl;
	ParserItem pit(-1, "");
	pit.addr = items[0].addr;
	return pit;
}

ParserItem transP25(production p, vector<ParserItem> items)
{
	cout << "	������һ�����嶯��" << endl;
	ParserItem pit(-1, "");
	//����������ʱ����
	pit.addr = items[0].token->getValue();
	
	//������Ԫʽ
	gen(nextquad++, "=", items[2].addr, "", items[0].token->getValue());

	//������ʱ����
	releaseTempV(items[2].temp);
	return pit;
}

ParserItem transP26(production p, vector<ParserItem> items)
{
	cout << "	������һ�����嶯��" << endl;
	ParserItem pit(-1, "");
	pit.addr = items[0].addr;
	return pit;
}

ParserItem transP27(production p, vector<ParserItem> items)
{
	cout << "	������һ�����嶯��" << endl;
	ParserItem pit(-1, "");
	//�����µ���ʱ����
	pit.temp = findTempV();
	string addr = "temp" + to_string(pit.temp);
	pit.addr = addr;
	//������Ԫʽ
	gen(nextquad++, "or", items[0].addr, items[2].addr, addr);

	//������ʱ����
	releaseTempV(items[0].temp);
	releaseTempV(items[2].temp);
	return pit;
}

ParserItem transP28(production p, vector<ParserItem> items)
{
	cout << "	������һ�����嶯��" << endl;
	ParserItem pit(-1, "");
	pit.addr = items[0].addr;
	return pit;
}

ParserItem transP29(production p, vector<ParserItem> items)
{
	cout << "	������һ�����嶯��" << endl;
	ParserItem pit(-1, "");
	//�����µ���ʱ����
	pit.temp = findTempV();
	string addr = "temp" + to_string(pit.temp);
	pit.addr = addr;
	//������Ԫʽ
	gen(nextquad++, "and", items[0].addr, items[2].addr, addr);

	//������ʱ����
	releaseTempV(items[0].temp);
	releaseTempV(items[2].temp);
	return pit;
}

ParserItem transP30(production p, vector<ParserItem> items)
{
	cout << "	������һ�����嶯��" << endl;
	ParserItem pit(-1, "");
	pit.addr = items[0].addr;
	return pit;
}

ParserItem transP31(production p, vector<ParserItem> items)
{
	cout << "	������һ�����嶯��" << endl;
	ParserItem pit(-1, "");
	//�����µ���ʱ����
	pit.temp = findTempV();
	string addr = "temp" + to_string(pit.temp);
	pit.addr = addr;
	//������Ԫʽ
	gen(nextquad++, "==", items[0].addr, items[2].addr, addr);

	//������ʱ����
	releaseTempV(items[0].temp);
	releaseTempV(items[2].temp);
	return pit;
}

ParserItem transP32(production p, vector<ParserItem> items)
{
	cout << "	������һ�����嶯��" << endl;
	ParserItem pit(-1, "");
	//�����µ���ʱ����
	pit.temp = findTempV();
	string addr = "temp" + to_string(pit.temp);
	pit.addr = addr;
	//������Ԫʽ
	gen(nextquad++, "!=", items[0].addr, items[2].addr, addr);

	//������ʱ����
	releaseTempV(items[0].temp);
	releaseTempV(items[2].temp);
	return pit;
}

ParserItem transP33(production p, vector<ParserItem> items)
{
	cout << "	������һ�����嶯��" << endl;
	ParserItem pit(-1, "");
	pit.addr = items[0].addr;
	return pit;
}

ParserItem transP34(production p, vector<ParserItem> items)
{
	cout << "	������һ�����嶯��" << endl;
	ParserItem pit(-1, "");
	//�����µ���ʱ����
	pit.temp = findTempV();
	string addr = "temp" + to_string(pit.temp);
	pit.addr = addr;
	//������Ԫʽ
	gen(nextquad++, "<", items[0].addr, items[2].addr, addr);

	//������ʱ����
	releaseTempV(items[0].temp);
	releaseTempV(items[2].temp);
	return pit;
}

ParserItem transP35(production p, vector<ParserItem> items)
{
	cout << "	������һ�����嶯��" << endl;
	ParserItem pit(-1, "");
	//�����µ���ʱ����
	pit.temp = findTempV();
	string addr = "temp" + to_string(pit.temp);
	pit.addr = addr;
	//������Ԫʽ
	gen(nextquad++, "<=", items[0].addr, items[2].addr, addr);

	//������ʱ����
	releaseTempV(items[0].temp);
	releaseTempV(items[2].temp);
	return pit;
}

ParserItem transP36(production p, vector<ParserItem> items)
{
	cout << "	������һ�����嶯��" << endl;
	ParserItem pit(-1, "");
	//�����µ���ʱ����
	pit.temp = findTempV();
	string addr = "temp" + to_string(pit.temp);
	pit.addr = addr;
	//������Ԫʽ
	gen(nextquad++, ">", items[0].addr, items[2].addr, addr);

	//������ʱ����
	releaseTempV(items[0].temp);
	releaseTempV(items[2].temp);
	return pit;
}

ParserItem transP37(production p, vector<ParserItem> items)
{
	cout << "	������һ�����嶯��" << endl;
	ParserItem pit(-1, "");
	//�����µ���ʱ����
	pit.temp = findTempV();
	string addr = "temp" + to_string(pit.temp);
	pit.addr = addr;
	//������Ԫʽ
	gen(nextquad++, ">=", items[0].addr, items[2].addr, addr);

	//������ʱ����
	releaseTempV(items[0].temp);
	releaseTempV(items[2].temp);
	return pit;
}

ParserItem transP38(production p, vector<ParserItem> items)
{
	cout << "	������һ�����嶯��" << endl;
	ParserItem pit(-1, "");
	pit.addr = items[0].addr;
	return pit;
}

ParserItem transP39(production p, vector<ParserItem> items)
{
	cout << "	������һ�����嶯��" << endl;
	ParserItem pit(-1, "");
	//�����µ���ʱ����
	pit.temp = findTempV();
	string addr = "temp" + to_string(pit.temp);
	pit.addr = addr;
	//������Ԫʽ
	gen(nextquad++, "+", items[0].addr, items[2].addr, addr);

	//������ʱ����
	releaseTempV(items[0].temp);
	releaseTempV(items[2].temp);
	return pit;
}

ParserItem transP40(production p, vector<ParserItem> items)
{
	cout << "	������һ�����嶯��" << endl;
	ParserItem pit(-1, "");
	//�����µ���ʱ����
	pit.temp = findTempV();
	string addr = "temp" + to_string(pit.temp);
	pit.addr = addr;
	//������Ԫʽ
	gen(nextquad++, "-", items[0].addr, items[2].addr, addr);

	//������ʱ����
	releaseTempV(items[0].temp);
	releaseTempV(items[2].temp);
	return pit;
}

ParserItem transP41(production p, vector<ParserItem> items)
{
	cout << "	������һ�����嶯��" << endl;
	ParserItem pit(-1, "");
	pit.addr = items[0].addr;
	return pit;
}

ParserItem transP42(production p, vector<ParserItem> items)
{
	cout << "	������һ�����嶯��" << endl;
	ParserItem pit(-1, "");
	//�����µ���ʱ����
	pit.temp = findTempV();
	string addr = "temp" + to_string(pit.temp);
	pit.addr = addr;
	//������Ԫʽ
	gen(nextquad++, "*", items[0].addr, items[2].addr, addr);

	//������ʱ����
	releaseTempV(items[0].temp);
	releaseTempV(items[2].temp);
	return pit;
}

ParserItem transP43(production p, vector<ParserItem> items)
{
	cout << "	������һ�����嶯��" << endl;
	ParserItem pit(-1, "");
	//�����µ���ʱ����
	pit.temp = findTempV();

	string addr = "temp" + to_string(pit.temp);
	pit.addr = addr;
	//������Ԫʽ
	gen(nextquad++, "/", items[0].addr, items[2].addr, addr);

	//������ʱ����
	releaseTempV(items[0].temp);
	releaseTempV(items[2].temp);
	return pit;
}

ParserItem transP44(production p, vector<ParserItem> items)
{
	cout << "	������һ�����嶯��" << endl;
	ParserItem pit(-1, "");
	pit.addr = items[0].addr;
	return pit;
}

ParserItem transP45(production p, vector<ParserItem> items)
{
	cout << "	������һ�����嶯��" << endl;
	ParserItem pit(-1, "");
	pit.temp = findTempV();

	string addr = "temp" + to_string(pit.temp);
	pit.addr = addr;
	gen(nextquad++, "-", items[1].addr, "", addr);
	return pit;
}

ParserItem transP46(production p, vector<ParserItem> items)
{
	cout << "	������һ�����嶯��" << endl;
	ParserItem pit(-1, "");
	pit.temp = findTempV();

	string addr = "temp" + to_string(pit.temp);
	pit.addr = addr;
	gen(nextquad++, "+", items[1].addr, "", addr);
	return pit;
}

ParserItem transP47(production p, vector<ParserItem> items)
{
	cout << "	������һ�����嶯��" << endl;
	ParserItem pit(-1, "");
	pit.addr = items[0].token->getValue();
	return pit;
}

ParserItem transP48(production p, vector<ParserItem> items)
{
	cout << "	������һ�����嶯��" << endl;
	ParserItem pit(-1, "");
	pit.addr = items[0].addr;
	return pit;
}

ParserItem transP49(production p, vector<ParserItem> items)
{
	cout << "	������һ�����嶯��" << endl;
	ParserItem pit(-1, "");
	pit.addr = items[1].addr;
	return pit;
}

ParserItem transP50(production p, vector<ParserItem> items)
{
	cout << "	������һ�����嶯��" << endl;
	ParserItem pit(-1, "");
	pit.instr = nextquad;
	return pit;
}

ParserItem transP51(production p, vector<ParserItem> items)
{
	cout << "	������һ�����嶯��" << endl;
	ParserItem pit(-1, "");
	pit.nextlist.push_back(nextquad);
	gen(nextquad,"GOTO", "", "", "");
	return pit;
}


//T->expression ��ʱexpression���������ɣ��ý��������ת
ParserItem transP52(production p, vector<ParserItem> items)
{
	cout << "	������һ�����嶯��" << endl;
	ParserItem pit(-1, "");
	
	pit.truelist.push_back(nextquad);
	gen(nextquad++,"BNE", items[0 ].addr, "0", "");

	pit.falselist.push_back(nextquad);
	gen(nextquad++, "GOTO", "","", "");


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
	trans[3] = transP3;
	trans[4] = transP4;
	trans[5] = transP5;
	trans[6] = transP6;
	trans[7] = transP7;
	trans[8] = transP8;
	trans[9] = transP9;
	trans[12] = transP12;
	trans[13] = transP13;
	trans[14] = transP14;
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
	trans[50] = transP50;
	trans[51] = transP51;
	trans[52] = transP52;
}

ParserItem SDT(production p, vector<ParserItem> items)
{
	auto t = trans[p.id];
	if (t != NULL)
		return t(p, items);
	else
		return ParserItem(-1, "");
}

//�﷨������
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
		Token* aptr = TokenList[Tokenindex];
		Token a = *TokenList[Tokenindex++];

		while (true)
		{
			ParserItem s = parseStack.top();
			//���Ҷ���

			if ( ACTIONtable[s.status].find(convertName(a.Name)) != ACTIONtable[s.status].end())
			{
				Actioncell act = ACTIONtable[s.status].at(convertName(a.Name));
				//�ƽ�����,��������һ����Vt
				if (act.op == "S")
				{
					parseStack.push(ParserItem(act.dest, convertName(a.Name),aptr));
					aptr = TokenList[Tokenindex];
					a = *TokenList[Tokenindex++];
					cout << act.op<< act.dest << endl;
					//�����Ÿı� ��ʶ������

				}
				//��Լ����
				else if (act.op == "R")
				{

					//��ȡ��Լ�õĲ���ʽ
					production p = wfdata.productions[act.dest];
					vector<ParserItem> tempItems;
					for (int i = 0; i < p.length; i++)
					{
						ParserItem topItem = parseStack.top();
						tempItems.push_back(topItem);
						parseStack.pop();
					}
					cout << p.toString() << endl;;
					//��ȡ��ǰջ��
					ParserItem nowTop = parseStack.top();
					//��������ԣ��и����ã�
					reverse(tempItems.begin(), tempItems.end());
					ParserItem newParserItem = SDT(p, tempItems);
					//����������󲿵�״̬�����
					newParserItem.status = GOTOtable[nowTop.status][p.left];
					newParserItem.symbol = p.left;
					//�������ջ
					parseStack.push(newParserItem);
					

				}
				else if (act.op == "ACC")
				{
					backpatch(parseStack.top().nextlist, nextquad);
					cout << "�﷨�������"<<endl;
					break;
				}
			}
			else
			{
				cout << "����" << endl;
				if (a.Name == 270)
				{
					cout << "�����declaration���" << endl;
				}
				break;
			}
		}
	}

};

void showFormula()
{
	ofstream outFirst(formulaPath);
	cout << "\n\n���������Ԫʽ����:\n";

	int index = 0;
	for (auto i : code)
	{
		string temp = "(" + i.op + " , " + i.src1 + " , " + i.src2 + " , " + i.dest + " )";
		outFirst <<index<< temp << endl;
		cout <<index<< temp << endl;;
		index++;
	}
}

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
	wfdata.outProductions("productionsId.txt");


	SLR mySLR;
	mySLR.buildstates();
	mySLR.showAllstates();
	cout << "\n\n\n����SLR������\n";
	mySLR.buildSLR();
	mySLR.showSLR();
	initTrans();
	Lexer lexer = Lexer();

	Parser parser(lexer.TokenList, mySLR);

	while (true)
	{
		//�ؽ�����
		env = NULL;
		code.clear();
		nextquad = 0;
		tempV.resize(0);

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

