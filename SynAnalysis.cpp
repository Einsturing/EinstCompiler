#include "SynAnalysis.h"

//�õ�����ĿĿǰ���������
string Item::getNow() {
	queue<string> tmp = this->right;
	int cnt = this->pos;
	while (cnt > 0) {
		tmp.pop();
		cnt--;
	}
	return tmp.front();
}

//�����Ҳ��������
void Item::setLen() {
	len = 0;
	queue<string> tmp = right;
	if (tmp.front() == "$") return;
	while (!tmp.empty()) {
		len++;
		tmp.pop();
	}
}

bool operator==(const class Item& a, const class Item& b) { return (a.left == b.left && a.right == b.right && a.pos == b.pos && a.len == b.len && a.lookup == b.lookup); }

bool operator==(const class Items& a, const class Items& b) { return (a.items == b.items); }

//���������ַ�������a��b�ϲ�
unordered_set<string> ussUnion(unordered_set<string> a, unordered_set<string> b) {
	unordered_set<string> res = a;
	unordered_set<string>::iterator it;
	for (it = b.begin(); it != b.end(); it++) res.insert(*it);
	return res;
}

//������A->����X��,a����ĿI��FIRST(��a)
unordered_set<string> first_beta_a(Item I) {
	unordered_set<string> rtn;
	int cnt = I.pos;
	queue<string> tmp = I.right;
	while (cnt > 0) {
		tmp.pop();
		cnt--;
	}
	tmp.pop();
	if (tmp.empty()) {
		rtn = ussUnion(rtn, I.lookup);
		return rtn;
	}
	string s = tmp.front();
	rtn = ussUnion(rtn, first(s));
	while (canEmpty(s)) {
		tmp.pop();
		if (!tmp.empty()) {
			s = tmp.front();
			rtn = ussUnion(rtn, first(s));
		}
		else {
			rtn = ussUnion(rtn, I.lookup);
			return rtn;
		}
	}
	return rtn;
}

//��Ŀ��I��������X��ת������
Items GOTO(Items I, string X) {
	Items J;
	unordered_set<Item, Item_hash>::iterator it;
	for (it = I.items.begin(); it != I.items.end(); it++) {
		Item item = *it;
		if (item.pos != item.len) {
			if (item.getNow() == X) {
				Item newitem;
				newitem.left = item.left;
				newitem.lookup = item.lookup;
				newitem.pos = item.pos + 1;
				newitem.right = item.right;
				newitem.index = item.index;
				newitem.setLen();
				J.items.insert(newitem);
			}
		}
	}
	return CLOSURE(J);
}

//��ĳ��ĿI���
void OUT(Item I) {
	cout << "[" << I.left << " -> ";
	int cnt = I.pos;
	queue<string> tmp = I.right;
	while (cnt > 0) {
		string s = tmp.front();
		cout << s;
		tmp.pop();
		cnt--;
	}
	cout << "��";
	while (!tmp.empty()) {
		string s = tmp.front();
		cout << s;
		tmp.pop();
	}
	cout << ", ";
	unordered_set<string> lkup = I.lookup;
	unordered_set<string>::iterator it;
	for (it = lkup.begin(); it != lkup.end(); it++) cout << *it << "/";
	cout << "\b";
	cout << "]; ";
}

//��ĳ��Ŀ��I���
void OUT(Items I) {
	cout << "State: " << I.index << " ";
	cout << "Closure: " << "{";
	unordered_set<Item, Item_hash>::iterator it;
	for (it = I.items.begin(); it != I.items.end(); it++) OUT(*it);
	cout << "}" << endl;
	cout << endl;
}

//�ж���Ŀ��I��������a�ܷ���й�Լ��������ѡ�ò���ʽ���
int canReduction(Items I, string a) {
	unordered_set<Item, Items_hash>::iterator it;
	for (it = I.items.begin(); it != I.items.end(); it++) {
		Item item = *it;
		if (item.pos == item.len) if (item.lookup.count(a) != 0) return item.index;
	}
	return 0;
}

//�Ե����Ͻ�ջs���
void outStack(stack<int> s) {
	stack<int> tmps = s;
	stack<int> tmp;
	while (!tmps.empty()) {
		tmp.push(tmps.top());
		tmps.pop();
	}
	while (!tmp.empty()) {
		cout << tmp.top() << " ";
		tmp.pop();
	}
}

//��ĳ��Ŀi�Բ���ʽ��ʽ���
void outItem(Item i) {
	cout << i.left << " ->";
	queue<string> tmp = i.right;
	while (!tmp.empty()) {
		cout << " " << tmp.front();
		tmp.pop();
	}
}