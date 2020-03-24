#include "LexAnalysis.h"
#include "SynAnalysis.h"

//关键字
set<string> key = { "do", "if", "return", "break", "else", "false", "for", "while", "true" };

//类型
set<string> type = { "int", "float", "double", "char", "bool", "complex", "short", "long", "string", "void" };

//运算符
set<string> op = { "+", "+=", "-", "-=", "*", "*=", "/", "/=", "%", "%=", "<", "<=", "=", "==", ">", ">=", "&", "&&", "|", "||", "!", "!=", "++", "--" };

//界符
set<string> border = { ",", ";", "(", ")", "{", "}", "[", "]" };

unordered_set<string> gSymbols;//文法符号
unordered_set<string> VT;//终结符
unordered_set<string> VN;//非终结符
map<int, map<string, string> > Action;//ACTION表
map<int, map<string, string> > Goto;//GOTO表
unordered_set<Item, Item_hash> CFG;//上下文无关文法

//判断s是否为关键字
bool isKey(string s) {
	if (key.count(s) == 1) return true;
	return false;
}

//判断s是否为类型标识符
bool isType(string s) {
	if (type.count(s) == 1) return true;
	return false;
}

//判断s是否为运算符
bool isOp(string s) {
	if (op.count(s) == 1) return true;
	return false;
}

//判断s是否为界符
bool isBorder(string s) {
	if (border.count(s) == 1) return true;
	return false;
}

//判断某符号x有无空产生式
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

//判断某符号x能否推导至空
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

//求某符号x的FIRST集
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

//求项目集I的闭包
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

//根据标号index寻找对应产生式
Item findItem(int index) {
	unordered_set<Item, Item_hash>::iterator it;
	for (it = CFG.begin(); it != CFG.end(); it++) {
		Item tmp = *it;
		if (tmp.index == index) return tmp;
	}
}

//进行词法分析
void LexAnalyse() {
	//读入正规文法
	string in;
	ifstream ifile_reg;
	ifile_reg.open("reg.txt", ios::in);

	//根据读入产生式构造NFA
	FA fa = FA();
	FANode nfaend = FANode("@");//终态用"@"表示
	fa.endSet.insert(nfaend);
	while (ifile_reg) {
		ifile_reg >> in;
		fa.createNFA(in);
	}

	//NFA转换为DFA
	FA dfa = FA();
	fa.subset(dfa);

	//读入源代码，进行分析
	ifstream ifile_code;
	ifile_code.open("code.txt", ios::in);
	string path;
	int cnt = 0;
	FANode start = *dfa.startSet.begin();

	//输出token序列
	ofstream ofile_token;
	ofile_token.open("token.txt", ios::out);
	while (getline(ifile_code, in)) {
		in = in.substr(in.find_first_not_of(" "));
		cnt++;
		FANode state = start;
		for (int i = 0; i < in.length(); i++) {
			char c = ex(in[i]);
			if (c == ' ') {
				if (dfa.endSet.count(state) == 0) cout << "所在行号:" << cnt << " 类别:不合法输入" << " token内容:" << path << endl;
				else {
					cout << "所在行号:" << cnt << " 类别:" << judge(state, path) << " token内容:" << path << endl;
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
					cout << "所在行号:" << cnt << " 类别:" << judge(state, path) << " token内容:" << path << endl;
					ofile_token << judge(state, path) << endl;
					state = start;
					path = "";
					i--;
				}
				continue;
			}
			path += in[i];
			if (dfa.Dtran[state].find(c) == dfa.Dtran[state].end()) {
				cout << "所在行号:" << cnt << " 类别:不合法输入" << " token内容:" << path << endl;
				state = start;
				path = "";
			}
			else state = dfa.Dtran[state][c];
		}
		if (dfa.endSet.count(state) == 0) cout << "所在行号:" << cnt << " 类别:不合法输入" << " token内容:" << path << endl;
		else {
			cout << "所在行号:" << cnt << " 类别:" << judge(state, path) << " token内容:" << path << endl;
			ofile_token << judge(state, path) << endl;
		}
		state = start;
		path = "";
	}
	ofile_token << "#" << endl;
}

//进行语法分析
void SynAnalyse() {
	//读入token序列
	string in;
	ifstream ifile_token;
	ifile_token.open("token.txt");

	//读入上下文无关文法
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
	startItem.left = "<代码>";
	startItem.right.push("<函数定义>");
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
				cout << "接受" << endl;
				cout << "yes" << endl;
				return;
			}
			else if (act[0] == 's') {
				string num = act.substr(1);
				cout << "移入状态s" << num << endl;
				stateStack.push(stoi(num));
				symbolStack.push(in);
				readNext = 1;
			}
			else if (act[0] == 'r') {
				string num = act.substr(1);
				cout << "根据";
				Item item = findItem(stoi(num));
				outItem(item);
				cout << "进行归约" << endl;
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
				cout << "错误输入:" << in << endl;
				cout << "No" << endl;
				return;
			}
		}
		else {
			cout << "错误输入:" << in << endl;
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