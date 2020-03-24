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

//LR(1)��Ŀ��
class Item {
public:
	string left;//��Ŀ�����ս��
	queue<string> right;//��Ŀ�Ҳ����
	int pos;//��ǰ���߹��ķ�����
	int len;//��Ŀ�Ҳ������
	bool complete = 0;//������հ�ʱ�ж���Ŀ�Ƿ����
	int index;//����ʽ���
	unordered_set<string> lookup;//չ��������

	Item() { pos = 0, len = 0; }

	string getNow();//�õ�����ĿĿǰ���������

	void setLen();//�����Ҳ��������
};

bool operator==(const class Item& a, const class Item& b);

//����hash����Ŀhash��
class Item_hash {
public:
	size_t operator()(const class Item& _i) const {
		string tmp = to_string(_i.index);
		return hash<string>()(tmp);
	}
};

//LR(1)���
class Items {
public:
	unordered_set<Item, Item_hash> items;//��Ŀ����
	int index;//��Ŀ�����
	int hash_id = 0;//��ϣID
	bool complete = 0;//��������Ŀ����ʱ�ж���Ŀ���Ƿ����
};

bool operator==(const class Items& a, const class Items& b);

//����hash����Ŀ��hash��
class Items_hash {
public:
	size_t operator()(const class Items& _is) const {
		string tmp = to_string(_is.hash_id);
		return hash<string>()(tmp);
	}
};

//LR(1)��Ŀ������
class ItemsFamily {
public:
	unordered_set<Items, Items_hash> C;//��Ŀ������
};

unordered_set<string> ussUnion(unordered_set<string> a, unordered_set<string> b);//���������ַ�������a��b�ϲ�

bool haveEmptyProduc(string x);//�ж�ĳ����x���޿ղ���ʽ

bool canEmpty(string x);//�ж�ĳ����x�ܷ��Ƶ�����

unordered_set<string> first(string x);//��ĳ����x��FIRST��

unordered_set<string> first_beta_a(Item I);//������A->����X��,a����ĿI��FIRST(��a)

Items CLOSURE(Items I);//����Ŀ��I�ıհ�

Items GOTO(Items I, string X);//��Ŀ��I��������X��ת������

void OUT(Item I);//��ĳ��ĿI���

void OUT(Items I);//��ĳ��Ŀ��I���

int canReduction(Items I, string a);//�ж���Ŀ��I��������a�ܷ���й�Լ��������ѡ�ò���ʽ���

void outStack(stack<int> s);//�Ե����Ͻ�ջs���

void outItem(Item i);//��ĳ��Ŀi�Բ���ʽ��ʽ���

Item findItem(int index);//���ݱ��indexѰ�Ҷ�Ӧ����ʽ
#endif