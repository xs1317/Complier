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

//��Word��
void Lexer::reserve(Word w)
{
	Words.emplace(w.lexeme, w);
}

//��ȡ�ļ�����
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

//������һ���ַ�
void Lexer::readch()
{
	index++;
	peek = input[index];
}

//������һ���ַ����ж�
bool Lexer::readch(char c)
{
	readch();

	if (peek != c)
		return false;	//������ˣ�ʶ����һ�����ػ�ͣ���ڵ�ǰλ��
	peek = ' ';		//����Ϊ�գ��´�ɨ���Զ�����һ���ַ�
	return true;
}

//ɨ�����룬������һ���ʷ���Ԫ
Token* Lexer::scan()
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
				index -= 2;
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
				} while (isdigit(peek) || peek >= 'A' && peek <= 'F');
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
			t = new Token('|');
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

//ɨ�����дʷ���Ԫ
bool Lexer::scanAll()
{
	bool flag = true;
	//д��lexer.scan() ʵ������ÿ�λ��һ���ʷ���Ԫ
	while (flag)
	{
		Token* temp = scan();
		if (temp != NULL)
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

//��ʾ�ʷ���Ԫ����
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
			cout << "<δʶ�����>";
			outFile << "δʶ�����";
		}
	}
	cout << endl;
}
