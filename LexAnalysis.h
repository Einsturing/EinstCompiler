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

//自动机状态结点类
class FANode {
public:
	string name;//状态结点名称
	FANode() {};
	FANode(string name) { this->name = name; }
};

bool operator==(const FANode& a, const FANode& b);

bool operator!=(const FANode& a, const FANode& b);

bool operator<(const FANode& a, const FANode& b);

//自动机类
class FA {
public:
	set<FANode> startSet;//初态集
	set<FANode> endSet;//终态集
	set<FANode> states;//状态集
	map<FANode, map<char, set<FANode> > > f;//nfa转换函数
	map<FANode, map<char, FANode> > Dtran;//dfa转换函数
	string a;//输入符号集
	map<set<FANode>, FANode> dfamap;//nfa状态集与dfa结点的映射

	FA() {};

	FA(set<FANode> startSet, set<FANode> endSet, set<FANode> states, map<FANode, map<char, set<FANode> > > f) {
		this->startSet = startSet;
		this->endSet = endSet;
		this->states = states;
		this->f = f;
	}

	void createNFA(string in);//根据读入的一条产生式in，更新NFA

	set<FANode> eClosure(set<FANode> T);//能够从T中的某个状态开始只通过epsilon转换到达的状态集合

	set<FANode> move(set<FANode> T, char a);//能够从T中某个状态s出发通过标号为a的转换到达的状态的集合

	void subset(FA& dfa);//用子集法将FA dfa构造为DFA
};

bool isKey(string s);//判断s是否为关键字

bool isType(string s);//判断s是否为类型标识符

bool isOp(string s);//判断s是否为运算符

bool isBorder(string s);//判断s是否为界符

char ex(char c);//将输入字符c转换为适合词法分析的符号

set<FANode> setunion(set<FANode> a, set<FANode> b);//将两FANode集合a，b合并

bool equal(set<FANode> a, set<FANode> b);//判断两FANode集合a，b是否相等

bool find(queue<set<FANode> > q, set<FANode> u);//判断某FANode集合u是否在队列q中

string judge(FANode node, string s);//判断字符串s的token类型
#endif