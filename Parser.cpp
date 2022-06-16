// 扫描源程序，转化为单词串
#include"lex.h"
#include"SLR.h"
#include "tools.h"
#include"parser.h"
#include<stack>
#include<functional>
using namespace std;
vector<string> sourcePath = { "sourceCode.txt","sourceCode1.txt","sourceCode2.txt","sourceCode3.txt" ,"sourceCode4.txt" ,"sourceCode5.txt" };
vector<string> tokenPath = { "Token.txt","Token1.txt","Token2.txt","Token3.txt" ,"Token4.txt" ,"Token5.txt" };
vector<formula> code;
vector<int> tempV;
string type;
int width;
int nextquad;
//找到一个可以用的临时变量单元
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
	if (i != -1)
		tempV[i] = 0;
}

//生成四元式
void gen(int nextquad, string op, string src1, string src2, string dest)
{
	code.push_back(formula(op, src1, src2, dest));
}

//合并两个list链：主要用于进行回填操作
list<int> merge(list<int>l1, list<int>l2)
{
	list<int> result;
	for (auto i : l1)
		result.push_back(i);
	for (auto i : l2)
		result.push_back(i);
	return result;
}

//回填四元式第四个操作数
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
	cout << "	调用了一个语义分析命令" << endl;
	ParserItem pit(-1, "");

	backpatch(items[0].nextlist, nextquad);
	return pit;
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

ParserItem transP3(production p, vector<ParserItem> items)
{
	cout << "	调用了一个语义分析命令" << endl;
	ParserItem pit(-1, "");

	backpatch(items[3].truelist, items[5].instr);
	backpatch(items[6].nextlist, items[1].instr);
	pit.nextlist = items[3].falselist;
	gen(nextquad++, "GOTO", "", "", to_string(items[1].instr));
	return pit;
}
ParserItem transP4(production p, vector<ParserItem> items)
{
	cout << "	调用了一个语义分析命令" << endl;
	ParserItem pit(-1, "");

	backpatch(items[2].truelist, items[4].instr);
	pit.nextlist = merge(items[2].falselist, items[5].nextlist);
	return pit;
}
ParserItem transP5(production p, vector<ParserItem> items)
{
	cout << "	调用了一个语义分析命令" << endl;
	ParserItem pit(-1, "");

	backpatch(items[2].truelist, items[4].instr);
	backpatch(items[2].falselist, items[8].instr);
	list<int> temp = merge(items[5].nextlist, items[6].nextlist);
	pit.nextlist = merge(temp, items[9].nextlist);

	return pit;
}


ParserItem transP6(production p, vector<ParserItem> items)
{
	cout << "	调用了一个语义分析命令" << endl;
	ParserItem pit(-1, "");

	pit.nextlist = items[0].nextlist;
	return pit;
}

ParserItem transP7(production p, vector<ParserItem> items)
{
	cout << "	调用了一个语义分析命令" << endl;
	ParserItem pit(-1, "");

	pit.nextlist = items[0].nextlist;
	return pit;
}

ParserItem transP8(production p, vector<ParserItem> items)
{
	cout << "	调用了一个语义分析命令" << endl;
	ParserItem pit(-1, "");


	pit.nextlist = items[0].nextlist;
	return pit;
}

ParserItem transP9(production p, vector<ParserItem> items)
{
	cout << "	调用了一个语义分析命令" << endl;
	ParserItem pit(-1, "");

	pit.nextlist = items[0].nextlist;

	return pit;
}


ParserItem transP12(production p, vector<ParserItem> items)
{
	cout << "	调用了一个语义分析命令" << endl;
	ParserItem pit(-1, "");

	pit.nextlist = items[2].nextlist;

	return pit;
}



//statements -> statements1 M statement   用M属性天statements1的next链,并传递statement的next链
ParserItem transP13(production p, vector<ParserItem> items)
{
	cout << "	调用了一个语义分析命令" << endl;
	ParserItem pit(-1, "");

	backpatch(items[0].nextlist, items[1].instr);
	pit.nextlist = items[2].nextlist;

	return pit;
}


ParserItem transP14(production p, vector<ParserItem> items)
{
	cout << "	调用了一个语义分析命令" << endl;
	ParserItem pit(-1, "");

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

//由于没有构造符号表所以这里模拟一下
ParserItem transP20(production p, vector<ParserItem> items)
{
	cout << "	调用了一个语义动作" << endl;
	ParserItem pit(-1, "");
	pit.addr = items[0].addr;
	return pit;
}

ParserItem transP21(production p, vector<ParserItem> items)
{
	cout << "	调用了一个语义动作" << endl;
	ParserItem pit(-1, "");
	pit.addr = items[0].token->getValue();

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

//M-># 记录下一条指令地址
ParserItem transP50(production p, vector<ParserItem> items)
{
	cout << "	调用了一个语义动作" << endl;
	ParserItem pit(-1, "");
	pit.instr = nextquad;
	return pit;
}

//N-># 记录N的next链，生成一条GOTO
ParserItem transP51(production p, vector<ParserItem> items)
{
	cout << "	调用了一个语义动作" << endl;
	ParserItem pit(-1, "");
	pit.nextlist.push_back(nextquad);
	gen(nextquad++, "GOTO", "", "", "");
	return pit;
}


//T->expression 此时expression代码已生成，用结果进行跳转
ParserItem transP52(production p, vector<ParserItem> items)
{
	cout << "	调用了一个语义动作" << endl;
	ParserItem pit(-1, "");

	pit.truelist.push_back(nextquad);
	gen(nextquad++, "BNE", items[0].addr, "0", "");

	pit.falselist.push_back(nextquad);
	gen(nextquad++, "GOTO", "", "", "");


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

//语法分析器
class Parser
{
public:
	vector <class Token*> TokenList;   //词法单元序列
	vector<unordered_map<string, int>> GOTOtable;
	vector <unordered_map<string, Actioncell>> ACTIONtable;

	string formulaPath = "formula.txt";

	Parser(vector<class Token*> T,SLR s)
	{
		TokenList = T;
		GOTOtable = s.GOTOtable;
		ACTIONtable = s.ACTIONtable;
	}
	void showFormula()
	{
		ofstream outFirst(formulaPath);
		cout << "\n\n部分语句四元式如下:\n";

		int index = 0;
		for (auto i : code)
		{
			string temp = "(" + i.op + " , " + i.src1 + " , " + i.src2 + " , " + i.dest + " )";
			outFirst << index << temp << endl;
			cout << index << temp << endl;;
			index++;
		}
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
					backpatch(parseStack.top().nextlist, nextquad);
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



int main()
{ 
	cout << endl; cout << endl; cout << endl; cout << endl;

	wfdata.outProductions("productionsId.txt");


	SLR mySLR;
	mySLR.buildstates();
	mySLR.showAllstates();
	cout << "\n\n\n构造SLR分析表\n";
	mySLR.buildSLR();
	mySLR.showSLR();
	
	Lexer lexer = Lexer();

	Parser parser(lexer.TokenList, mySLR);
	initTrans();
	while (true)
	{
		//重建环境:清空四元式列表，初始化指令序号，清空临时变量分配表
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
			parser.showFormula();
		}

	}

	return 0;
}

