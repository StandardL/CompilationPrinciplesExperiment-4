#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <algorithm>

using namespace std;

enum class ITEMTYPE
{
	MOVE, // 移进
	REDUCE, // 规约
};

struct Item
{
	/*
	* Item的结构如下：
	* E -> E + T
	* key => E
	* value_num => which rule
	* index => point position
	* type => MOVE or REDUCE
	*/
	string key;
	int value_num;
	int index;
	ITEMTYPE type;
	Item(string k = "", int vn = 0, int id = 0, ITEMTYPE it = ITEMTYPE::MOVE)
	{
		key = k;
		value_num = vn;
		index = id;
		type = it;
	}
	bool operator==(const Item& item) const
	{
		return key == item.key && value_num == item.value_num && index == item.index && type == item.type;
	}
};

class SLR
{
private:
	bool isFirstFlag;
	bool isSLRFlag;
	string START;  // 文法开始符号
	string notSLRreason;  // 不是SLR文法的原因

	vector<vector<string>> rules;
	unordered_map<string, vector<vector<string>>> gramma;  // 文法
	map<string, bool> canBeEmpty;  // 文法可以为空的非终结符
	unordered_set<string> nonterminal;  // 非终结符集合
	set<string> terminal;  // 终结符集合
	unordered_map<string, set<string>> first;  // first集合
	unordered_map<string, set<string>> follow;  // follow集合

	vector<vector<Item>> DFA_nodes;  // LR(0)DFA节点
	map<int, map<string, int>> forward;
	map<int, map<string, int>> backward;

	void GetFirst();
	void GetFollow();
	void GenerateDFA();
	void Expand(int index);
public:
	SLR();
	~SLR();

	void Input(string input);
	void Reset();
	void Analyze();
	bool GetIsSLRFlag();
	unordered_map<string, vector<vector<string>>> GetGramma();
	string GetReason();
	string GetSTART();
	vector<vector<Item>> GetDFA();
	unordered_map<string, set<string>> getFirst();
	unordered_map<string, set<string>> getFollow();
	map<int, map<string, int>> GetForward();
	map<int, map<string, int>> GetBackward();
	unordered_set<string> GetNonterminal();
	set<string> GetTerminal();
};

