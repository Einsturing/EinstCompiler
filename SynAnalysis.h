#pragma once
#ifndef Syn
#define Syn
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

//LR(1)项目类
class Item {
public:
	string left;//项目左侧非终结符
	queue<string> right;//项目右侧符号
	int pos;//当前点走过的符号数
	int len;//项目右侧符号数
	bool complete = 0;//用于求闭包时判断项目是否访问
	int index;//产生式标号
	unordered_set<string> lookup;//展望符集合

	Item() { pos = 0, len = 0; }

	string getNow();//得到该项目目前待输入符号

	void setLen();//设置右侧符号数量
};

bool operator==(const class Item& a, const class Item& b);

//用于hash的项目hash类
class Item_hash {
public:
	size_t operator()(const class Item& _i) const {
		string tmp = to_string(_i.index);
		return hash<string>()(tmp);
	}
};

//LR(1)项集类
class Items {
public:
	unordered_set<Item, Item_hash> items;//项目集合
	int index;//项目集标号
	int hash_id = 0;//哈希ID
	bool complete = 0;//用于求项目集族时判断项目集是否访问
};

bool operator==(const class Items& a, const class Items& b);

//用于hash的项目集hash类
class Items_hash {
public:
	size_t operator()(const class Items& _is) const {
		string tmp = to_string(_is.hash_id);
		return hash<string>()(tmp);
	}
};

//LR(1)项目集族类
class ItemsFamily {
public:
	unordered_set<Items, Items_hash> C;//项目集集合
};

unordered_set<string> ussUnion(unordered_set<string> a, unordered_set<string> b);//将两无序字符串集合a，b合并

bool haveEmptyProduc(string x);//判断某符号x有无空产生式

bool canEmpty(string x);//判断某符号x能否推导至空

unordered_set<string> first(string x);//求某符号x的FIRST集

unordered_set<string> first_beta_a(Item I);//求形如A->α·Xβ,a的项目I的FIRST(βa)

Items CLOSURE(Items I);//求项目集I的闭包

Items GOTO(Items I, string X);//项目集I遇到符号X的转换函数

void OUT(Item I);//将某项目I输出

void OUT(Items I);//将某项目集I输出

int canReduction(Items I, string a);//判断项目集I遇到符号a能否进行归约，并返回选用产生式标号

void outStack(stack<int> s);//自底向上将栈s输出

void outItem(Item i);//将某项目i以产生式形式输出

Item findItem(int index);//根据标号index寻找对应产生式
#endif