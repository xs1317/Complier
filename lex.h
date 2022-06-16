#pragma once
#include<iostream>
#include<cstdio>
#include<fstream>
#include<string>
#include<unordered_map>
#include<list>
#include<fstream>
#include<vector>
#include<ctype.h>
#include "Token.h"

// 做词法分析处理的类
// 键：词素，值：Token对象
class Lexer
{
public:
	//创建符号表并初始化

	int line = 1;
	//存放下一个输入字符,要么为空要么指向下一个字符；为空时可以略过，自动读入下一个
	char peek = ' ';
	int index = -1;
	bool flag = true;
	string sourceCodePath = "sourceCode1.txt";
	string tokenOutPath = "Lex.txt";
	string input;
	vector <class Token*> TokenList;   //词法单元序列
	unordered_map<string, Word>Words = {};

	//将所有关键字和运算符加入Word
	Lexer();

	//初始化peek line等变量
	void init();

	//填Word表
	void reserve(Word w);

	//读入下一个字符
	void readch();

	//读入下一个字符并判断
	bool readch(char c);

	//扫描输入，返回下一个词法单元
	Token* scan();

	//输出词法单元序列
	void showTokenList();

	//获取文件输入
	void getInput();

	bool scanAll();
};
