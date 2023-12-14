#include "SLR_GUI.h"

SLR_GUI::SLR_GUI(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	/* 隐藏非SLR原因的框 */
	ui.lblNotSLR->hide();
	ui.textReason->hide();
	/* 锁定编辑 */
	ui.textReason->setReadOnly(true);

	/* slots */
	connect(ui.btnOpen, SIGNAL(clicked()), this, SLOT(btnOpenFileClicked()));
	connect(ui.btnSave, SIGNAL(clicked()), this, SLOT(btnSaveFileClicked()));
	connect(ui.btnAnalyse, SIGNAL(clicked()), this, SLOT(btnAnalyzeClicked()));
}

SLR_GUI::~SLR_GUI()
{}

void SLR_GUI::btnOpenFileClicked()
{
	/* Using QFileDialog to open file */
	QString fileName = QFileDialog::getOpenFileName(this, tr("打开文件"), "", tr("文法规则 (*.txt);;所有文件 (*)"));
	if (fileName.isEmpty())
	{
		/* Show warning */
		QMessageBox::warning(NULL, QString("提示"), QString("已取消读取！"));
		return;
	}
	/* Show contents in TextEdit */
	/* Read file */
	QFile file(fileName);
	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QTextStream in(&file);
		QString fileContent = in.readAll();

		ui.inputEdit->setPlainText(fileContent);
		file.close();
	}
	else
	{
		QString message = "打开文件" + fileName + "失败！";
		QMessageBox::warning(NULL, QString("错误"), message);
	}
}

void SLR_GUI::btnSaveFileClicked()
{
	/* Using QFileDialog to save file */
	QString fileName = QFileDialog::getSaveFileName(this, tr("保存文件"), "", tr("文本文件 (*.txt);;所有文件 (*)"));
	if (fileName.isEmpty())
	{
		/* Show warning */
		QMessageBox::warning(NULL, QString("提示"), QString("已取消保存！"));
		return;
	}
	/* Write file */
	QFile file(fileName);
	if (file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QTextStream out(&file);
		out << ui.inputEdit->toPlainText();
		file.close();
	}
	else
	{
		QString message = "保存文件" + fileName + "失败！";
		QMessageBox::warning(NULL, QString("错误"), message);
	}
}

void SLR_GUI::btnAnalyzeClicked()
{
	/* Judge whether is empty or not */
	QString input = ui.inputEdit->toPlainText();
	if (input.isEmpty())
	{
		QMessageBox::warning(NULL, QString("错误"), QString("输入为空！"));
		return;
	}
	/* Convert input to string */
	string inputStr = input.toStdString();
	/* Divide by '\n' */
	vector<string> rules;
	stringstream ss(inputStr);
	string rule;
	while (getline(ss, rule, '\n'))
	{
		rules.push_back(rule);
	}

	/* Analyze */
	slr.Reset();
	for (auto& rule : rules)
	{
		slr.Input(rule);
	}
	slr.Analyze();
	/* Show result */
	if (slr.GetIsSLRFlag())
	{
		ui.lblNotSLR->hide();
		ui.textReason->hide();
	}
	else
	{
		ui.lblNotSLR->show();
		ui.textReason->show();
		ui.textReason->setPlainText(QString::fromStdString(slr.GetReason()));
	}
	int cols = 2;

	/* TableWidget Initialize */
	// 禁止编辑
	ui.tableFirst->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui.tableFollow->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui.tableDFA->setEditTriggers(QAbstractItemView::NoEditTriggers);

	/* Show First */
	auto first = slr.getFirst();
	ui.tableFirst->clear();
	int firstRow = first.size();
	ui.tableFirst->setRowCount(firstRow);
	ui.tableFirst->setColumnCount(cols);
	auto qsl = QStringList() << "非终结符" << "First集合";
	ui.tableFirst->setHorizontalHeaderLabels(qsl);
	// 添加数据
	int r = 0;
	for (auto& key : first)
	{
		ui.tableFirst->setItem(r, 0, new QTableWidgetItem(QString::fromStdString(key.first)));
		auto& value = key.second;
		string valueStr = "";
		for (auto& v : value)
		{
			valueStr += v + " ";
		}
		ui.tableFirst->setItem(r, 1, new QTableWidgetItem(QString::fromStdString(valueStr)));
		r++;
	}

	/* Show Follow */
	auto follow = slr.getFollow();
	ui.tableFollow->clear();
	int followRow = follow.size();
	ui.tableFollow->setRowCount(followRow);
	ui.tableFollow->setColumnCount(cols);
	qsl = QStringList() << "非终结符" << "Follow集合";
	ui.tableFollow->setHorizontalHeaderLabels(qsl);
	// 添加数据
	r = 0;
	for (auto& key : follow)
	{
		ui.tableFollow->setItem(r, 0, new QTableWidgetItem(QString::fromStdString(key.first)));
		auto& value = key.second;
		string valueStr = "";
		for (auto& v : value)
		{
			valueStr += v + " ";
		}
		ui.tableFollow->setItem(r, 1, new QTableWidgetItem(QString::fromStdString(valueStr)));
		r++;
	}

	/* Show DFA */
	auto dfa = slr.GetDFA();
	auto gramma = slr.GetGramma();
	ui.tableDFA->clear();
	int dfaRow = dfa.size();
	int dfaCol = 2;
	ui.tableDFA->setRowCount(dfaRow);
	ui.tableDFA->setColumnCount(dfaCol);
	qsl = QStringList() << "状态" << "项目集";
	ui.tableDFA->setHorizontalHeaderLabels(qsl);
	// 添加数据
	for (int i = 0; i < dfaRow; i++)
	{
		ui.tableDFA->setItem(i, 0, new QTableWidgetItem(QString::number(i)));
		string valueStr = "";
		for (auto& item : dfa[i])
		{
			valueStr += item.key + " -> ";
			for (int j = 0; j < gramma[item.key][0][item.value_num].size(); j++)
			{
				if (j == item.index)
				{
					valueStr += "· ";
				}
				valueStr.append(1, gramma[item.key][0][item.value_num][j]);
			}
			if (item.index == gramma[item.key][0][item.value_num].size())
			{
				valueStr += "· ";
			}
			valueStr += "\n";
		}
		// 删去最后的换行符
		valueStr.pop_back();
		ui.tableDFA->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(valueStr)));
	}

	/* Show SLR */
	auto forward = slr.GetForward();
	auto backward = slr.GetBackward();
	ui.tableSLR->clear();
	int slrRow = dfaRow;
	auto slrterminal = slr.GetTerminal();
	auto slrnonterminal = slr.GetNonterminal();
	slrnonterminal.erase(slr.GetSTART());
	int slrCol = slrterminal.size() + slrnonterminal.size() + 1;
	ui.tableSLR->setRowCount(slrRow);
	ui.tableSLR->setColumnCount(slrCol);
	// 添加表头
	qsl = QStringList() << "状态";
	int index = 1;
	map<string, int> indexMap;
	for (auto& t : slrterminal)
	{
		qsl << QString::fromStdString(t);
		indexMap[t] = index;
		index++;
	}
	for (auto& nt : slrnonterminal)
	{
		qsl << QString::fromStdString(nt);
		indexMap[nt] = index;
		index++;
	}
	ui.tableSLR->setHorizontalHeaderLabels(qsl);
	// 添加数据
	for (int i = 0; i < slrRow; i++)
	{
		ui.tableSLR->setItem(i, 0, new QTableWidgetItem(ui.tableDFA->item(i, 1)->text()));
		// 加终结符的
		string valueStr = "";
		for (auto& t : slrterminal)
		{
			if (forward[i].find(t) != forward[i].end())
			{
				valueStr = "S" + to_string(forward[i][t]);
				ui.tableSLR->setItem(i, indexMap[t], new QTableWidgetItem(QString::fromStdString(valueStr)));
			}
			else if (backward[i].find(t) != backward[i].end())
			{
				auto ruleID = backward[i][t];  // 使用第几条规则进行归约，从0开始
				auto& ruleItem = dfa[i][ruleID];  // 所有规则
				auto& rule = gramma[ruleItem.key][0][ruleItem.value_num];
				valueStr = "r(" + ruleItem.key + " -> " + rule + ")";
				ui.tableSLR->setItem(i, indexMap[t], new QTableWidgetItem(QString::fromStdString(valueStr)));
			}
		}
		/*if (j <= slrterminal.size())
		{
			auto& t = slrterminal[j - 1];
			if (forward[i].find(t) != forward[i].end())
			{
				valueStr = "S" + to_string(forward[i][t]);
			}
			else if (backward[i].find(t) != backward[i].end())
			{
				valueStr = "r" + to_string(backward[i][t]);
			}
		}
		else
		{
			auto& nt = slrnonterminal[j - slrterminal.size() - 1];
			if (forward[i].find(nt) != forward[i].end())
			{
				valueStr = to_string(forward[i][nt]);
			}
		}*/
		// 加非终结符的
		valueStr.clear();
		for (auto& nt : slrnonterminal)
		{
			if (forward[i].find(nt) != forward[i].end())
			{
				valueStr = to_string(forward[i][nt]);
				ui.tableSLR->setItem(i, indexMap[nt], new QTableWidgetItem(QString::fromStdString(valueStr)));
				break;
			}
		}
	}

	/* Adjusted Column Width and Row Height */
	ui.tableFirst->resizeColumnsToContents();
	ui.tableFollow->resizeColumnsToContents();
	//ui.tableDFA->resizeColumnsToContents();
	ui.tableDFA->resizeRowsToContents();
	//ui.tableSLR->resizeColumnsToContents();
	ui.tableSLR->resizeRowsToContents();
}
