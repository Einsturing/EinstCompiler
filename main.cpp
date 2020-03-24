#include "LexAnalysis.h"
#include "SynAnalysis.h"

//�ؼ���
set<string> key = { "do", "if", "return", "break", "else", "false", "for", "while", "true" };

//����
set<string> type = { "int", "float", "double", "char", "bool", "complex", "short", "long", "string", "void" };

//�����
set<string> op = { "+", "+=", "-", "-=", "*", "*=", "/", "/=", "%", "%=", "<", "<=", "=", "==", ">", ">=", "&", "&&", "|", "||", "!", "!=", "++", "--" };

//���
set<string> border = { ",", ";", "(", ")", "{", "}", "[", "]" };

unordered_set<string> gSymbols;//�ķ�����
unordered_set<string> VT;//�ս��
unordered_set<string> VN;//���ս��
map<int, map<string, string> > Action;//ACTION��
map<int, map<string, string> > Goto;//GOTO��
unordered_set<Item, Item_hash> CFG;//�������޹��ķ�

//�ж�s�Ƿ�Ϊ�ؼ���
bool isKey(string s) {
	if (key.count(s) == 1) return true;
	return false;
}

//�ж�s�Ƿ�Ϊ���ͱ�ʶ��
bool isType(string s) {
	if (type.count(s) == 1) return true;
	return false;
}

//�ж�s�Ƿ�Ϊ�����
bool isOp(string s) {
	if (op.count(s) == 1) return true;
	return false;
}

//�ж�s�Ƿ�Ϊ���
bool isBorder(string s) {
	if (border.count(s) == 1) return true;
	return false;
}

//�ж�ĳ����x���޿ղ���ʽ
bool haveEmptyProduc(string x) {
	unordered_set<Item, Item_hash>::iterator itcfg;
	for (itcfg = CFG.begin(); itcfg != CFG.end(); itcfg++) {
		Item produc = *itcfg;
		if (produc.left == x) {
			queue<string> tmp = produc.right;
			string s = tmp.front();
			if (s == "$") return true;
		}
	}
	return false;
}

//�ж�ĳ����x�ܷ��Ƶ�����
bool canEmpty(string x) {
	if (VT.count(x) != 0 || x == "$") return false;
	if (haveEmptyProduc(x)) return true;
	else {
		unordered_set<Item, Item_hash>::iterator itcfg;
		for (itcfg = CFG.begin(); itcfg != CFG.end(); itcfg++) {
			Item produc = *itcfg;
			if (produc.left == x) {
				queue<string> tmp = produc.right;
				while (!tmp.empty()) {
					string s = tmp.front();
					if (!haveEmptyProduc(s)) return false;
				}
			}
		}
	}
	return true;
}

//��ĳ����x��FIRST��
unordered_set<string> first(string x) {
	unordered_set<string> rtn;
	if (VT.count(x) != 0) {
		rtn.insert(x);
		return rtn;
	}
	else if (x != "$") {
		unordered_set<Item, Item_hash>::iterator itcfg;
		for (itcfg = CFG.begin(); itcfg != CFG.end(); itcfg++) {
			Item produc = *itcfg;
			if (produc.left == x) {
				queue<string> tmp = produc.right;
				string s = tmp.front();
				tmp.pop();
				rtn = ussUnion(rtn, first(s));
				while (!tmp.empty() && canEmpty(s)) {
					s = tmp.front();
					rtn = ussUnion(rtn, first(s));
					tmp.pop();
				}
			}
		}
		return rtn;
	}
	return rtn;
}

//����Ŀ��I�ıհ�
Items CLOSURE(Items I) {
	Items rtn = I;
	bool flag;
	while (1) {
		flag = 0;
		unordered_set<Item, Item_hash>::iterator it;
		for (it = rtn.items.begin(); it != rtn.items.end(); it++) {
			Item item = *it;
			if (!(*it).complete) {
				flag = 1;
				if (item.len != item.pos) {
					string nowSymbol = item.getNow();
					if (VN.count(nowSymbol) != 0) {
						unordered_set<Item, Item_hash>::iterator itcfg;
						for (itcfg = CFG.begin(); itcfg != CFG.end(); itcfg++) {
							Item produc = *itcfg;
							if (produc.left == nowSymbol) {
								unordered_set<string> newlookup = first_beta_a(item);
								produc.lookup = newlookup;
								if (rtn.items.count(produc) == 0) rtn.items.insert(produc);
							}
						}
					}
				}
				item.complete = 1;
				rtn.items.erase(it);
				rtn.items.insert(item);
				break;
			}
		}
		if (flag == 0) break;
	}
	return rtn;
}

//���ݱ��indexѰ�Ҷ�Ӧ����ʽ
Item findItem(int index) {
	unordered_set<Item, Item_hash>::iterator it;
	for (it = CFG.begin(); it != CFG.end(); it++) {
		Item tmp = *it;
		if (tmp.index == index) return tmp;
	}
}

//���дʷ�����
void LexAnalyse() {
	//���������ķ�
	string in;
	ifstream ifile_reg;
	ifile_reg.open("reg.txt", ios::in);

	//���ݶ������ʽ����NFA
	FA fa = FA();
	FANode nfaend = FANode("@");//��̬��"@"��ʾ
	fa.endSet.insert(nfaend);
	while (ifile_reg) {
		ifile_reg >> in;
		fa.createNFA(in);
	}

	//NFAת��ΪDFA
	FA dfa = FA();
	fa.subset(dfa);

	//����Դ���룬���з���
	ifstream ifile_code;
	ifile_code.open("code.txt", ios::in);
	string path;
	int cnt = 0;
	FANode start = *dfa.startSet.begin();

	//���token����
	ofstream ofile_token;
	ofile_token.open("token.txt", ios::out);
	while (getline(ifile_code, in)) {
		in = in.substr(in.find_first_not_of(" "));
		cnt++;
		FANode state = start;
		for (int i = 0; i < in.length(); i++) {
			char c = ex(in[i]);
			if (c == ' ') {
				if (dfa.endSet.count(state) == 0) cout << "�����к�:" << cnt << " ���:���Ϸ�����" << " token����:" << path << endl;
				else {
					cout << "�����к�:" << cnt << " ���:" << judge(state, path) << " token����:" << path << endl;
					ofile_token << judge(state, path) << endl;
				}
				state = start;
				path = "";
				continue;
			}
			if (dfa.endSet.count(state) == 1) {
				if (dfa.Dtran[state].find(c) != dfa.Dtran[state].end()) {
					state = dfa.Dtran[state][c];
					path += in[i];
				}
				else {
					cout << "�����к�:" << cnt << " ���:" << judge(state, path) << " token����:" << path << endl;
					ofile_token << judge(state, path) << endl;
					state = start;
					path = "";
					i--;
				}
				continue;
			}
			path += in[i];
			if (dfa.Dtran[state].find(c) == dfa.Dtran[state].end()) {
				cout << "�����к�:" << cnt << " ���:���Ϸ�����" << " token����:" << path << endl;
				state = start;
				path = "";
			}
			else state = dfa.Dtran[state][c];
		}
		if (dfa.endSet.count(state) == 0) cout << "�����к�:" << cnt << " ���:���Ϸ�����" << " token����:" << path << endl;
		else {
			cout << "�����к�:" << cnt << " ���:" << judge(state, path) << " token����:" << path << endl;
			ofile_token << judge(state, path) << endl;
		}
		state = start;
		path = "";
	}
	ofile_token << "#" << endl;
}

//�����﷨����
void SynAnalyse() {
	//����token����
	string in;
	ifstream ifile_token;
	ifile_token.open("token.txt");

	//�����������޹��ķ�
	ifstream ifile_cfg;
	ifile_cfg.open("cfg.txt");
	gSymbols.insert("#");

	int cnt = 1;
	while (getline(ifile_cfg, in)) {
		Item item;
		string left = "";
		int i = 0;
		while (in[i] != ' ') {
			left += in[i];
			i++;
		}
		VN.insert(left);
		gSymbols.insert(left);
		item.left = left;
		i += 3;
		string right = "";
		for (i; i < in.length();) {
			if (in[i] == ' ') {
				i++;
				while (in[i] != ' ' && i < in.length()) {
					right += in[i];
					i++;
				}
				if (right.find('<') != string::npos && right.find('>') != string::npos) VN.insert(right);
				else if (right != "$") VT.insert(right);
				if (right != "$") gSymbols.insert(right);
				item.right.push(right);
				right = "";
			}
		}
		item.setLen();
		item.index = cnt++;
		CFG.insert(item);
	}

	Item startItem;
	startItem.left = "<����>";
	startItem.right.push("<��������>");
	startItem.lookup.insert("#");
	startItem.index = 1;
	startItem.setLen();
	Items I0;
	I0.index = 0;
	I0.items.insert(startItem);
	Items cloI0 = CLOSURE(I0);
	ItemsFamily lr1;
	lr1.C.insert(cloI0);
	bool flag;
	cnt = 1;
	OUT(cloI0);
	while (1) {
		flag = 0;
		unordered_set<Items, Items_hash>::iterator it;
		for (it = lr1.C.begin(); it != lr1.C.end(); it++) {
			if (!(*it).complete) {
				flag = 1;
				Items I = *it;
				unordered_set<string>::iterator it1;
				for (it1 = gSymbols.begin(); it1 != gSymbols.end(); it1++) {
					string X = *it1;
					Items gotoIX = GOTO(I, X);
					if (gotoIX.items.size() != 0 && lr1.C.count(gotoIX) == 0) {
						gotoIX.index = cnt++;
						lr1.C.insert(gotoIX);
					}
					string yiru = "s";
					string guiyue = "r";
					if (lr1.C.find(gotoIX) != lr1.C.end()) {
						unordered_set<Items, Items_hash>::iterator it2 = lr1.C.find(gotoIX);
						cout << "goto(" << I.index << "," << X << ") " << endl;
						OUT(*it2);
						if (VT.count(X) != 0) Action[I.index][X] = yiru + to_string((*it2).index);
						else Goto[I.index][X] = to_string((*it2).index);
					}
					else if (VT.count(X) != 0) {
						int reduction = canReduction(I, X);
						if (reduction != 0) Action[I.index][X] = guiyue + to_string(reduction);
					}
					else if (X == "#") {
						int reduction = canReduction(I, X);
						if (reduction == 1) Action[I.index][X] = "acc";
						else if (reduction != 0) Action[I.index][X] = guiyue + to_string(reduction);
					}
				}
				I.complete = 1;
				lr1.C.erase(it);
				lr1.C.insert(I);
				break;
			}
		}
		if (flag == 0) break;
	}

	int step = 1;
	stack<int> stateStack;
	stack<string> symbolStack;
	stateStack.push(0);
	bool readNext = 1;
	while (1) {
		if (readNext) ifile_token >> in;
		cout << "Setp:" << step++ << " ";
		cout << "Stack:";
		outStack(stateStack);
		cout << "Input:" << in << " ";
		cout << "Action:";
		if (VT.count(in) != 0 || in == "#") {
			int nowstate = stateStack.top();
			string act = Action[nowstate][in];
			if (act == "acc") {
				cout << "����" << endl;
				cout << "yes" << endl;
				return;
			}
			else if (act[0] == 's') {
				string num = act.substr(1);
				cout << "����״̬s" << num << endl;
				stateStack.push(stoi(num));
				symbolStack.push(in);
				readNext = 1;
			}
			else if (act[0] == 'r') {
				string num = act.substr(1);
				cout << "����";
				Item item = findItem(stoi(num));
				outItem(item);
				cout << "���й�Լ" << endl;
				int cnt = item.len;
				while (cnt--) {
					symbolStack.pop();
					if (item.right.front() != "$") stateStack.pop();
				}
				symbolStack.push(item.left);
				int newstate = stoi(Goto[stateStack.top()][symbolStack.top()]);
				stateStack.push(newstate);
				readNext = 0;
			}
			else {
				cout << "��������:" << in << endl;
				cout << "No" << endl;
				return;
			}
		}
		else {
			cout << "��������:" << in << endl;
			cout << "No" << endl;
			return;
		}
	}
}

int main() {
	LexAnalyse();
	SynAnalyse();
	return 0;
}