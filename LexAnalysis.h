#pragma once
#ifndef Lex
#define Lex
#include <string>
#include <map>
#include <unordered_set>
#include <iostream>
#include <fstream>
#include <stack>
#include <set>
#include <queue>
#include <iterator>
#include <iomanip>
using namespace std;

//�Զ���״̬�����
class FANode {
public:
	string name;//״̬�������
	FANode() {};
	FANode(string name) { this->name = name; }
};

bool operator==(const FANode& a, const FANode& b);

bool operator!=(const FANode& a, const FANode& b);

bool operator<(const FANode& a, const FANode& b);

//�Զ�����
class FA {
public:
	set<FANode> startSet;//��̬��
	set<FANode> endSet;//��̬��
	set<FANode> states;//״̬��
	map<FANode, map<char, set<FANode> > > f;//nfaת������
	map<FANode, map<char, FANode> > Dtran;//dfaת������
	string a;//������ż�
	map<set<FANode>, FANode> dfamap;//nfa״̬����dfa����ӳ��

	FA() {};

	FA(set<FANode> startSet, set<FANode> endSet, set<FANode> states, map<FANode, map<char, set<FANode> > > f) {
		this->startSet = startSet;
		this->endSet = endSet;
		this->states = states;
		this->f = f;
	}

	void createNFA(string in);//���ݶ����һ������ʽin������NFA

	set<FANode> eClosure(set<FANode> T);//�ܹ���T�е�ĳ��״̬��ʼֻͨ��epsilonת�������״̬����

	set<FANode> move(set<FANode> T, char a);//�ܹ���T��ĳ��״̬s����ͨ�����Ϊa��ת�������״̬�ļ���

	void subset(FA& dfa);//���Ӽ�����FA dfa����ΪDFA
};

bool isKey(string s);//�ж�s�Ƿ�Ϊ�ؼ���

bool isType(string s);//�ж�s�Ƿ�Ϊ���ͱ�ʶ��

bool isOp(string s);//�ж�s�Ƿ�Ϊ�����

bool isBorder(string s);//�ж�s�Ƿ�Ϊ���

char ex(char c);//�������ַ�cת��Ϊ�ʺϴʷ������ķ���

set<FANode> setunion(set<FANode> a, set<FANode> b);//����FANode����a��b�ϲ�

bool equal(set<FANode> a, set<FANode> b);//�ж���FANode����a��b�Ƿ����

bool find(queue<set<FANode> > q, set<FANode> u);//�ж�ĳFANode����u�Ƿ��ڶ���q��

string judge(FANode node, string s);//�ж��ַ���s��token����
#endif