#include "LexAnalysis.h"

bool operator==(const FANode& a, const FANode& b) { return a.name == b.name; }

bool operator!=(const FANode& a, const FANode& b) { return a.name != b.name; }

bool operator<(const FANode& a, const FANode& b) { return a.name < b.name; }

//���ݶ����һ������ʽin������NFA
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

//�ܹ���T�е�ĳ��״̬��ʼֻͨ��epsilonת�������״̬����
set<FANode> FA::eClosure(set<FANode> T) {
	//��T������״̬ѹ��ջ��
	stack<FANode> stk;
	set<FANode>::iterator it;
	for (it = T.begin(); it != T.end(); it++) stk.push(*it);

	//��eclosureT��ʼ��ΪT
	set<FANode> eclosureT = T;

	while (!stk.empty()) {
		//ջ��Ԫ�س�ջ
		FANode t = stk.top();
		stk.pop();

		//��ÿ����������������u����t������һ�����Ϊepsilon��ת������״̬u(����U)
		set<FANode> U = this->f[t]['E'];
		for (it = U.begin(); it != U.end(); it++) {
			//���u����eclosure�У�������벢��ջ
			if (eclosureT.find(*it) == eclosureT.end()) {
				eclosureT.insert(*it);
				stk.push(*it);
			}
		}
	}
	return eclosureT;
}

//�ܹ���T��ĳ��״̬s����ͨ�����Ϊa��ת�������״̬�ļ���
set<FANode> FA::move(set<FANode> T, char a) {
	set<FANode>::iterator it;
	set<FANode> res;
	for (it = T.begin(); it != T.end(); it++) if (this->f[*it].find(a) != this->f[*it].end()) res = setunion(res, this->f[*it][a]);
	return res;
}

//���Ӽ�����FA dfa����ΪDFA
void FA::subset(FA& dfa) {
	//һ��ʼeclosure(s0)��Dstates�е�Ψһ״̬����δ�����
	queue<set<FANode> > Dstates0;//��ʾδ���
	queue<set<FANode> > Dstates1;//��ʾ�ѱ��
	set<FANode> ecs0 = this->eClosure(this->startSet);
	Dstates0.push(ecs0);

	//����dfa�ĳ�ʼ״̬���
	FANode dfastart = FANode("0");
	dfa.startSet.insert(dfastart);
	dfa.states.insert(dfastart);
	int cnt = 1;//���Ա�ʾ�������
	queue<FANode> nodeque;//���������ѹ����dfa���
	nodeque.push(dfastart);

	//��Dstates����һ��δ���״̬T
	while (!Dstates0.empty()) {
		//��T���ϱ��
		set<FANode> T = Dstates0.front();
		Dstates1.push(T);
		Dstates0.pop();
		FANode pre = nodeque.front(); //�ݴ���״̬T�����dfa���
		nodeque.pop();

		//��ÿ���������
		int len = a.length();
		for (int i = 0; i <= len - 1; i++) {
			set<FANode> U = eClosure(move(T, a[i]));
			//��U����Dstates��
			if (!find(Dstates0, U) && !find(Dstates1, U)) {
				//��U���뵽Dstates���Ҳ����
				Dstates0.push(U);

				//ͬʱ��״̬U����dfa���
				string name = to_string(cnt);
				FANode node = FANode(name);
				this->dfamap[U] = node;
				cnt++;
				nodeque.push(node);
				dfa.states.insert(node);

				//���״̬U������̬��㣬���¹���Ľ�����dfa����̬��
				set<FANode>::iterator it;
				set<FANode> e = this->endSet;
				for (it = e.begin(); it != e.end(); it++) {
					if (U.find(*it) != U.end()) {
						dfa.endSet.insert(node);
						break;
					}
				}
			}

			//����dfa��ת������
			if (!U.empty()) dfa.Dtran[pre][a[i]] = this->dfamap[U];
		}
	}
}

//�������ַ�cת��Ϊ�ʺϴʷ������ķ���
char ex(char c) {
	if (c >= 'A' && c <= 'Z') return tolower(c);
	return c;
}

//����FANode����a��b�ϲ�
set<FANode> setunion(set<FANode> a, set<FANode> b) {
	set<FANode> res = a;
	set<FANode>::iterator it;
	for (it = b.begin(); it != b.end(); it++) res.insert(*it);
	return res;
}

//�ж���FANode����a��b�Ƿ����
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

//�ж�ĳFANode����u�Ƿ��ڶ���q��
bool find(queue<set<FANode> > q, set<FANode> u) {
	queue<set<FANode>> tmp = q;
	while (!tmp.empty()) {
		set<FANode> cmp = tmp.front();
		if (equal(cmp, u)) return true;
		tmp.pop();
	}
	return false;
}

//�ж��ַ���s��token����
string judge(FANode node, string s) {
	if (isKey(s)) return s;
	if (isType(s)) return "Type";
	if (isOp(s)) return s;
	if (isBorder(s)) return s;
	if (node.name == "1") return "id";
	else return "Constant";
}