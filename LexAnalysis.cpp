#include "LexAnalysis.h"

bool operator==(const FANode& a, const FANode& b) { return a.name == b.name; }

bool operator!=(const FANode& a, const FANode& b) { return a.name != b.name; }

bool operator<(const FANode& a, const FANode& b) { return a.name < b.name; }

//根据读入的一条产生式in，更新NFA
void FA::createNFA(string in) {
	string name;
	char path;
	string target;
	int len = in.length();
	if (len == 5) {
		name = in[0];
		path = in[3];
		target = in[4];
		FANode node1 = FANode(name);
		FANode node2 = FANode(target);
		this->states.insert(node1);
		this->states.insert(node2);
		if (name == "S") this->startSet.insert(node1);
		this->f[node1][path].insert(node2);
	}
	else {
		name = in[0];
		path = in[3];
		FANode node1 = FANode(name);
		this->states.insert(node1);
		this->f[node1][path] = setunion(this->f[node1][path], this->endSet);
	}
	if (path != 'E' && a.find(path) == string::npos) a += path;
}

//能够从T中的某个状态开始只通过epsilon转换到达的状态集合
set<FANode> FA::eClosure(set<FANode> T) {
	//将T的所有状态压入栈中
	stack<FANode> stk;
	set<FANode>::iterator it;
	for (it = T.begin(); it != T.end(); it++) stk.push(*it);

	//将eclosureT初始化为T
	set<FANode> eclosureT = T;

	while (!stk.empty()) {
		//栈顶元素出栈
		FANode t = stk.top();
		stk.pop();

		//对每个满足如下条件的u：从t出发有一个标号为epsilon的转换到达状态u(记作U)
		set<FANode> U = this->f[t]['E'];
		for (it = U.begin(); it != U.end(); it++) {
			//如果u不在eclosure中，将其加入并入栈
			if (eclosureT.find(*it) == eclosureT.end()) {
				eclosureT.insert(*it);
				stk.push(*it);
			}
		}
	}
	return eclosureT;
}

//能够从T中某个状态s出发通过标号为a的转换到达的状态的集合
set<FANode> FA::move(set<FANode> T, char a) {
	set<FANode>::iterator it;
	set<FANode> res;
	for (it = T.begin(); it != T.end(); it++) if (this->f[*it].find(a) != this->f[*it].end()) res = setunion(res, this->f[*it][a]);
	return res;
}

//用子集法将FA dfa构造为DFA
void FA::subset(FA& dfa) {
	//一开始eclosure(s0)是Dstates中的唯一状态，且未被标记
	queue<set<FANode> > Dstates0;//表示未标记
	queue<set<FANode> > Dstates1;//表示已标记
	set<FANode> ecs0 = this->eClosure(this->startSet);
	Dstates0.push(ecs0);

	//构造dfa的初始状态结点
	FANode dfastart = FANode("0");
	dfa.startSet.insert(dfastart);
	dfa.states.insert(dfastart);
	int cnt = 1;//用以表示结点名称
	queue<FANode> nodeque;//用来保存已构造的dfa结点
	nodeque.push(dfastart);

	//当Dstates中有一个未标记状态T
	while (!Dstates0.empty()) {
		//给T加上标记
		set<FANode> T = Dstates0.front();
		Dstates1.push(T);
		Dstates0.pop();
		FANode pre = nodeque.front(); //暂存由状态T构造的dfa结点
		nodeque.pop();

		//对每个输入符号
		int len = a.length();
		for (int i = 0; i <= len - 1; i++) {
			set<FANode> U = eClosure(move(T, a[i]));
			//若U不在Dstates中
			if (!find(Dstates0, U) && !find(Dstates1, U)) {
				//将U加入到Dstates中且不标记
				Dstates0.push(U);

				//同时由状态U构造dfa结点
				string name = to_string(cnt);
				FANode node = FANode(name);
				this->dfamap[U] = node;
				cnt++;
				nodeque.push(node);
				dfa.states.insert(node);

				//如果状态U包含终态结点，则将新构造的结点加入dfa的终态集
				set<FANode>::iterator it;
				set<FANode> e = this->endSet;
				for (it = e.begin(); it != e.end(); it++) {
					if (U.find(*it) != U.end()) {
						dfa.endSet.insert(node);
						break;
					}
				}
			}

			//更新dfa的转换函数
			if (!U.empty()) dfa.Dtran[pre][a[i]] = this->dfamap[U];
		}
	}
}

//将输入字符c转换为适合词法分析的符号
char ex(char c) {
	if (c >= 'A' && c <= 'Z') return tolower(c);
	return c;
}

//将两FANode集合a，b合并
set<FANode> setunion(set<FANode> a, set<FANode> b) {
	set<FANode> res = a;
	set<FANode>::iterator it;
	for (it = b.begin(); it != b.end(); it++) res.insert(*it);
	return res;
}

//判断两FANode集合a，b是否相等
bool equal(set<FANode> a, set<FANode> b) {
	if (a.size() != b.size()) return false;
	else {
		set<FANode>::iterator it1, it2;
		for (it1 = a.begin(), it2 = b.begin(); it1 != a.end(), it2 != b.end(); it1++, it2++) {
			FANode x = *it1;
			FANode y = *it2;
			if (x != y) return false;
		}
	}
	return true;
}

//判断某FANode集合u是否在队列q中
bool find(queue<set<FANode> > q, set<FANode> u) {
	queue<set<FANode>> tmp = q;
	while (!tmp.empty()) {
		set<FANode> cmp = tmp.front();
		if (equal(cmp, u)) return true;
		tmp.pop();
	}
	return false;
}

//判断字符串s的token类型
string judge(FANode node, string s) {
	if (isKey(s)) return s;
	if (isType(s)) return "Type";
	if (isOp(s)) return s;
	if (isBorder(s)) return s;
	if (node.name == "1") return "id";
	else return "Constant";
}