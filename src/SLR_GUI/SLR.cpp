#include "SLR.h"


void SLR::GetFirst()
{
	bool flag = true;  // first集合有无变化过
	while (flag)
	{
		flag = false;
		for (auto& key : nonterminal)  // 遍历所有非终结符
		{
			first[key];  // 初始化
			for (auto& rules : gramma[key])  // 遍历非终结符对应的所有规则
			{
				for (auto& rule : rules)
				{
					if (rule[0] >= 'A' && rule[0] <= 'Z')  // 非终结符
					{
						/* 把另一个非终结符的first集合加进来 */
						string nt;
						nt.append(1, rule[0]);
						if (nt == key)  // 避免自己加入自己的first集合
							break;
						for (auto& c : first[nt])
						{
							if (first[key].count(c) == 0)  // 不存在时才需要加入
							{
								first[key].insert(c);
								flag = true;
							}
						}
					}
					else // 非终结符: a, +, @, (, *
					{
						string r = ""; r.append(1, rule[0]);
						if (first[key].count(r) == 0)  // 不存在时才需要加入
						{
							first[key].insert(r);
							flag = true;
						}
					}
				}
			}
		}
	}
}

void SLR::GetFollow()
{
	bool flag = true;       // 修改为标志
	bool emptyFlag = true;  // 记录最后一个非终结符号是否可以为$
	follow[START].insert("@");

	while (flag)
	{
		flag = false;
		for (auto& key : nonterminal)  // 遍历所有非终结符
		{
			for (auto& rules : gramma[key])  // 遍历非终结符对应的所有规则
			{
				for (auto& rule : rules)
				{
					if (rule.length() == 1 && rule[0] >= 'A' && rule[0] <= 'Z')  // 单独处理独一份的情况
					{
						for (auto& f : follow[key])
							follow[rule].insert(f);
					}
					int current = rule.length() - 1;
					char last = rule[current--];
					if (current < 0) continue;  // 防止越界，如A->@
					char pred = rule[current];  // 超前看一个

					while ((last >= 'A' && last <= 'Z'))  // 遇到非终结符
					{
						string last_s; last_s.append(1, last);
						string pred_s; pred_s.append(1, pred);
						/* 若该文法规则的最后一项是非终结符号，把当前key的follow集合给它 */
						for (auto& f : follow[key])
						{
							if (follow[last_s].count(f) == 0)
							{
								follow[last_s].insert(f);
								flag = true;
							}
						}
						/* 若当前文法符号可以是空，则把当前key的follow集合给它的前一个非终结符号 */
						if (canBeEmpty[last_s] == true)
						{

							if (pred >= 'A' && pred <= 'Z')  // 如果是两个非终结符连着
							{
								// 把当前key的follow集合给前一个非终结符的follow集合
								for (auto& f : follow[key])
								{
									if (follow[pred_s].count(f) == 0)
									{
										follow[pred_s].insert(f);
										flag = true;
									}
								}
								// 把后一个非终结符的first集合给前一个非终结符的follow集合
								for (auto& f : first[last_s])
								{
									if (f == "@") continue;  // 不加入空符号
									if (follow[pred_s].count(f) == 0)
									{
										follow[pred_s].insert(f);
										flag = true;
									}
								}
							}
						}
						if (current == 0)
							break;
						last = rule[current--];
						pred = rule[current];
					}
					while ((last < 'A' || last > 'Z'))  // 遇到终结符
					{
						if (pred >= 'A' && pred <= 'Z')  // 超前看一个
						{
							string pred_s; pred_s.append(1, pred);
							string last_s; last_s.append(1, last);
							if (follow[pred_s].count(last_s) == 0)
							{
								follow[pred_s].insert(last_s);
								flag = true;
							}
							// 存在非终结符-终结符的情况马上跳出循环
							break;
						}
						// 否则继续回退
						if (current == 0)
							break;
						last = rule[current--];
						pred = rule[current];
					}
				}
			}
		}
	}
}

void SLR::GenerateDFA()
{
	/* 先初始化开始节点 */
	vector<Item> node{ Item(START, 0, 0, ITEMTYPE::MOVE) };
	DFA_nodes.push_back(node);
	for (int i = 0; i < DFA_nodes.size(); i++)  // 遍历所有DFA结点
	{
		forward[i] = map<string, int>();
		backward[i] = map<string, int>();
		Expand(i);  // 扩展该节点的文法
		for (int j = 0; j < DFA_nodes[i].size(); j++)  // 遍历结点的所有规则
		{
			auto& target = DFA_nodes[i][j];

			if (target.type == ITEMTYPE::MOVE)  // 移进项
			{
				string s = gramma[target.key][0][target.value_num];
				string subs = s.substr(target.index, 1);
				Item newItem(target.key, target.value_num, target.index + 1, ITEMTYPE::MOVE);  // 扫描后当前.的下标+1
				if (newItem.index >= s.size())
					newItem.type = ITEMTYPE::REDUCE;
				if (forward[i].count(subs) == 0)
				{
					// 该转换s还未出现
					int k = 0;
					for (k = 0; k < DFA_nodes.size(); k++)
					{
						auto& node = DFA_nodes[k];
						if (count(node.begin(), node.end(), newItem) > 0)  // 当前结点已存在
						{
							break;
						}
					}
					if (k < DFA_nodes.size())  // 当前项目结点已存在
					{
						forward[i][subs] = k;
					}
					else
					{
						DFA_nodes.push_back(vector<Item>{newItem});
						forward[i][subs] = DFA_nodes.size() - 1;  // 结点编号放在最后
					}
				}
				else
				{
					int k = forward[i][subs];
					auto& node = DFA_nodes[k];
					if (count(node.begin(), node.end(), newItem) == 0)  // 当前结点不存在
					{
						DFA_nodes[k].push_back(newItem);
					}
				}
			}
			else  // 归约
			{
				auto& followTarget = follow[target.key];
				for (auto& k : followTarget)
				{
					if (backward[i].find(k) != backward[i].end())
					{
						isSLRFlag = false;
						notSLRreason = "第" + to_string(i) + "个结点归约时存在冲突";
					}
					backward[i][k] = j;
				}
			}
		}
	}
	if (isSLRFlag)
	{
		for (int i = 0; i < DFA_nodes.size(); i++)
		{
			set<string> s1, s2, res;
			for (auto& f : forward[i])
				s1.insert(f.first);
			for (auto& b : backward[i])
				s2.insert(b.first);
			// 求交际，若移进和归约存在交集，说明存在移进归约冲突，则不是SLR文法
			set_intersection(s1.begin(), s1.end(), s2.begin(), s2.end(), inserter(res, res.begin()));
			if (res.size() > 0)
			{
				isSLRFlag = false;
				notSLRreason = "第" + to_string(i) + "个结点移进时存在冲突";
				break;
			}
		}
	}
	else
	{
	
	}
}

void SLR::Expand(int index)
{
	auto& node = DFA_nodes[index];
	for (int i = 0; i < node.size(); i++)
	{
		auto& target = node[i];
		if (target.type == ITEMTYPE::MOVE)
		{
			// 根据结点标记的符号找对应的语法加进来
			string expand_s = gramma[target.key][0][target.value_num];
			string expand_c; expand_c.append(1, expand_s[target.index]);
			if (nonterminal.count(expand_c) > 0)  // 非终结符才加进来做扩充
			{
				auto& rules = gramma[expand_c];
				for (int j = 0; j < rules[0].size(); j++)
				{
					Item newItem(expand_c, j, 0, ITEMTYPE::MOVE);
					if (rules[0][j] == "@")
					{
						newItem.type = ITEMTYPE::REDUCE;
					}
					if (count(node.begin() + i, node.end(), newItem) == 0)  // 避免重复添加
						node.push_back(newItem);
				}
			}
		}
	}
}

SLR::SLR()
{
	isFirstFlag = true;
	isSLRFlag = true;
	notSLRreason = "";
}

SLR::~SLR()
{
}

void SLR::Input(string input)
{
	string endFlag = "$";
	//string input;
	int count;
	string left;
	vector<vector<string>> rules;
	/*
	*   示例文法：
		E->TA
		A->+TA|@
		T->FB
		B->*FB|@
		F->(E)|i
		$
	*/

	if (input == endFlag)
		return;
	count = 0;
	vector<string> thisRule;  // 本次循环的规则
	string right;  // 保存右部
	for (int i = 0; i < input.length(); i++)
	{
		while (input[i] == ' ')
		{
			i++;
		}
		if (input[i] >= 'A' && input[i] <= 'Z')
		{
			string temp; temp.append(1, input[i]);
			nonterminal.insert(temp);
		}
		else
		{
			string temp; temp.append(1, input[i]);
			//if (temp == "@") temp = "$";
			if (temp != "|" && temp != "-")
				terminal.insert(temp);
		}

		if (i + 1 < input.length() && input[i] == '-' && input[i + 1] == '>')
		{
			// 前面的就是左部
			int index = i - 1;
			while (index >= 0 && input[index] == ' ')
				index--;
			left = input[index];
			if (isFirstFlag)  // 第一次输入，保存文法开始符号
			{
				START = left;
				isFirstFlag = false;
			}
			right.pop_back();  // 去掉刚才->左边的符号
			i++;
			continue;  // 跳过->
		}
		if (input[i] == '|')  // 说明遇到了或，右边的就是新规则
		{
			thisRule.push_back(right);
			right.clear();
			continue;
		}
		if (input[i] == '@')
		{
			right.push_back('@');
			canBeEmpty[left] = true;
		}
		else
		{
			right.push_back(input[i]);  // 保存右部
			if (canBeEmpty.count(left) == 0)  // 不存在时才需要初始化
				canBeEmpty[left] = false;
		}
	}

	thisRule.push_back(right);
	rules = gramma[left];
	rules.push_back(thisRule);
	gramma[left] = rules;
}

void SLR::Reset()
{
	isFirstFlag = true;
	isSLRFlag = true;
	notSLRreason.clear();
	START.clear();
	rules.clear();
	gramma.clear();
	canBeEmpty.clear();
	nonterminal.clear();
	terminal.clear();
	first.clear();
	follow.clear();
	DFA_nodes.clear();
	forward.clear();
	backward.clear();
}

void SLR::Analyze()
{
	GetFirst();
	GetFollow();
	GenerateDFA();
}

bool SLR::GetIsSLRFlag()
{
	return isSLRFlag;
}

unordered_map<string, vector<vector<string>>> SLR::GetGramma()
{
	return gramma;
}

string SLR::GetReason()
{
	return notSLRreason;
}

string SLR::GetSTART()
{
	return START;
}

vector<vector<Item>> SLR::GetDFA()
{
	return DFA_nodes;
}

unordered_map<string, set<string>> SLR::getFirst()
{
	return first;
}

unordered_map<string, set<string>> SLR::getFollow()
{
	return follow;
}

map<int, map<string, int>> SLR::GetForward()
{
	return forward;
}

map<int, map<string, int>> SLR::GetBackward()
{
	return backward;
}

unordered_set<string> SLR::GetNonterminal()
{
	return nonterminal;
}

set<string> SLR::GetTerminal()
{
	return terminal;
}
