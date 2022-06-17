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

// ���ʷ������������
// �������أ�ֵ��Token����
class Lexer
{
public:
	//�������ű���ʼ��

	int line = 1;
	//�����һ�������ַ�,ҪôΪ��Ҫôָ����һ���ַ���Ϊ��ʱ�����Թ����Զ�������һ��
	char peek = ' ';
	int index = -1;
	bool flag = true;
	string sourceCodePath = "sourceCode1.txt";
	string tokenOutPath = "Lex.txt";
	string input;
	vector <class Token*> TokenList;   //�ʷ���Ԫ����
	unordered_map<string, Word>Words = {};

	//�����йؼ��ֺ����������Word
	Lexer();

	//��ʼ��peek line�ȱ���
	void init();

	//��Word��
	void reserve(Word w);

	//������һ���ַ�
	void readch();

	//������һ���ַ����ж�
	bool readch(char c);

	//ɨ�����룬������һ���ʷ���Ԫ
	Token* scan();

	//����ʷ���Ԫ����
	void showTokenList();

	//��ȡ�ļ�����
	void getInput();

	bool scanAll();
};
