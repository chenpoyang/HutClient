#include "datawidget.h"
#include "ui_datawidget.h"
#include <QDebug>

DataWidget::DataWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataWidget)
{
    ui->setupUi(this);
	tableRow = 0;
	tableCol = TABLE_FIELD;

	model = new QStandardItemModel(this);
	model->setRowCount(tableRow);
	model->setColumnCount(tableCol);
	ui->tableView->horizontalHeader()->setMovable(true);
	ui->tableView->setModel(model);
	//ui->tableView->verticalHeader()->hide();
	ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	initField();
	initConnection();
}

/* 更新指定学院下的专业 */
void DataWidget::updateClassQuery(QString college)
{
	QStringList strList;
	QSqlDatabase db;
	QString sql;

	db = linkDb(driver, dbName, host, user, passwd);
	sql = "select distinct major from student where college = '";
	sql += college;
	sql += "'";
	QSqlQuery que(sql, db);
	que.bindValue(0, college);
	if (que.exec())
	{
		while (que.next())
		{
			strList << que.value(0).toString();
		}
		emit sendClassSearch(strList);
		qDebug() << strList;
	}
	else
	{
		qDebug() << "没有数据库建立连接!";
	}
	db.close();
	db.removeDatabase(QSqlDatabase::defaultConnection);
}

/* 查询 */
void DataWidget::getQueryData(const QStringList &tmpList)
{
	QSqlDatabase db = linkDb(driver, dbName, host, user, passwd, port);
	QSqlQuery que;
	QString college, major, orderRu, stuName, stuId, strMsg, strTmp;
	bool orderByStuId;
	int row, col, i;
	qDebug() << "DataWidget正在查询中..." << tmpList;

	row = 0;
	col = TABLE_FIELD;
	this->setTableRow(row);

	college = tmpList.at(0);
	major = tmpList.at(1);

	/* 排序规则 */
	if (tmpList.at(2).toStdString() == "证书编号")
	{
		orderRu = "certificateNum";
		qDebug() << "按证书编号排序";
		orderByStuId = false;
	}
	else
	{
		orderRu = "stuId";
		qDebug() << "按学号排序号";
		orderByStuId = false;
	}

	stuName = tmpList.at(3);
	stuId = tmpList.at(4);

	if (stuName.length() <= 0 && stuId.length() > 0)
	{
		if (orderByStuId)
		{
			que.prepare(tr("select * from student where stuId = ? order by stuId asc"));
		}
		else
		{
			que.prepare(tr("select * from student where stuId = ? order by certificateNum asc"));
		}
		que.bindValue(0, stuId);
	}
	else if (stuName.length() > 0 && stuId.length() <= 0)
	{
		if (orderByStuId)
		{
			que.prepare(tr("select * from student where name = ? order by stuId asc"));
		}
		else
		{
			que.prepare(tr("select * from student where name = ? order by certificateNum asc"));
		}
		que.bindValue(0, stuName);
	}
	else if (stuName.length() > 0 && stuId.length() > 0)
	{
		que.prepare(tr("select * from student where name = ? and stuId = ?"));
		que.bindValue(0, stuName);
		que.bindValue(1, stuId);
	}
	else
	{
		if (orderByStuId)
		{
			que.prepare(tr("select * from student where college = ? and major = ? order by stuId asc"));
		}
		else
		{
			que.prepare(tr("select * from student where college = ? and major = ? order by certificateNum asc"));
		}
		que.bindValue(0, college);
		que.bindValue(1, major);
	}
	if (que.exec())
	{
		while (que.next())
		{
			this->setTableRow(row + 1);
			for (i = 0; i < col; ++i)
			{
				strMsg = que.value(i).toString();
				QStandardItem *item = new QStandardItem(strMsg);
				model->setItem(row, i, item);
			}
			++row;
		}

		strTmp.setNum(row);
		strTmp = "已查询到 " + strTmp;
		strTmp += " 条记录!";
		emit this->setTextForMain(strTmp);
		qDebug() << strTmp;
	}
	else
	{
		QMessageBox::warning(this,
			tr("警告"), tr("无法连接数据库!"), QMessageBox::Yes, QMessageBox::No);
	}
	db.close();
	db.removeDatabase(QSqlDatabase::defaultConnection);
}

/* 获取批量认证条件 */
void DataWidget::getBatchCerData(const QStringList &tmpList)
{
	qDebug() << "DataWidget正在批量认证中...";

	QString graType, sql;
	int limit;
	QSqlDatabase db;
	QMessageBox tips;

	graType = tmpList.at(0);
	limit = tmpList.at(1).toInt();

	db = linkDb(driver, dbName, host, user, passwd);
	QSqlQuery que("", db);
	sql = "";
	que.prepare(tr("update student set certified = 1 where totalMark >= ?; update student set certified = 0 where totalMark < ?"));
	que.bindValue(0, limit);
	que.bindValue(1, limit);
	if (que.exec())
	{
		db.close();
		db.removeDatabase(QSqlDatabase::defaultConnection);
		tips.setText(tr("已完成批量认证!"));
		db.close();
		db.removeDatabase(QSqlDatabase::defaultConnection);
		emit this->sendLabelStatus(tr("批量认证已完成!"));
	}
	else
	{
		tips.setText(tr("批量认证出错, 请确保MySQL已启动!"));
	}
	tips.setWindowTitle(tr("认证结果:"));
	tips.setWindowIcon(QIcon(":/images/hutIcon.ico"));
	tips.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	tips.exec();
}

/* 把当前表格数据写入数据库 */
void DataWidget::saveCurrentRecord()
{
	int i, j, row, col, sum;
	QSqlDatabase db;
	QString str, strCurRow, strCurCol, strMsg, strTmp;
	QRegExp reg("^\\d+$");
	QMessageBox tips;

	row = this->getTableRow();
	col = this->getTableCol();
	db = linkDb(driver, dbName, host, user, passwd);
	QSqlQuery que("", db);
	sum = 0;
	for (i = 0; i < row; ++i)
	{
		que.prepare(tr("insert into student values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"));
		for (j = 0; j < col; ++j)
		{
			if ((j >= 13 && j <= 23))
			{
				str = model->item(i, j)->text();
				if (reg.exactMatch(str))
				{
					que.bindValue(j, str.toInt());
				}
				else
				{
					strCurRow.setNum(i + 1, 10);
					qDebug() << strCurRow;

					strMsg = tr("所导入的数据第");
					strMsg += strCurRow;
					strMsg += "行, 第";
					strCurCol.setNum(j + 1, 10);
					strMsg += strCurCol;
					strMsg += "列不合法, 系统将停止导入, 请检查!";
					QMessageBox::warning(this, tr("警告!"),
						strMsg, QMessageBox::Yes, QMessageBox::No);
					db.rollback();
					return;
				}
			}
			else if (j == 25)
			{
				str = model->item(i, j)->text();
				if (str == "是" || str == "1")
				{
					//que.bindValue(j, 1); 数据库主软件
					que.bindValue(j, 0);
				}
				else
				{
					que.bindValue(j, 0);
				}
			}
			else
			{
				que.bindValue(j, model->item(i, j)->text());
			}
		}
		if (que.exec())
		{
			++sum;
		}
	}

	strMsg = "有";
	strTmp.setNum(sum);
	strMsg += strTmp;
	strMsg += "条记录被写入数据库";

	tips.setText(strMsg);
	tips.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	tips.setWindowTitle(tr("结果:"));
	tips.setWindowIcon(QIcon(":/images/hutIcon.ico"));
	tips.exec();
	this->setTableRow(0);
	this->setTableCol(col);

	que.exec(tr("select distinct college from student"));
	QStringList colStrList;
	colStrList.clear();
	while (que.next())
	{
		colStrList << que.value(0).toString();
	}
	emit sendCollegeSearch(colStrList);
	qDebug() << colStrList << "emit sendCollegeSearch(colStrList)";

	db.close();
	db.removeDatabase(QSqlDatabase::defaultConnection);

}

/* 获取连接数据库的信息 */
QStringList DataWidget::getLinkDbMsg()
{
	return linkMsg;
}

/* 设置连接数据库的信息 */
void DataWidget::setLinkDbMsg(const QStringList &tmpMsg)
{
	linkMsg = tmpMsg;

	driver = linkMsg.at(0);
	dbName = linkMsg.at(1);
	host = linkMsg.at(2);
	user = linkMsg.at(3);
	passwd = linkMsg.at(4);
	port = linkMsg.at(5).toInt();
	qDebug() << "已分解连接数据库的信息:";
	qDebug() << driver << dbName << host << user << passwd << port;
}

void DataWidget::getLinkMsg(QStringList &strList)
{
	linkMsg = strList;
}

/* 连接数据库 */
QSqlDatabase DataWidget::linkDb(const QString &driver, const QString &dbName,
			const QString &host, const QString &user, const QString &passwd, int port)
{
	QSqlError err;
	QSqlDatabase db = QSqlDatabase::addDatabase(driver);
	db.setHostName(host);
	db.setDatabaseName(dbName);
	db.setPort(port);
	if (!db.open(user, passwd) || user != tr("root"))
	{
		err = db.lastError();
		QMessageBox::warning(this, tr("警告"),
			"连接MySql出错, 请确保MySql已启动且用户名和密无误!", QMessageBox::Ok);
		if (db.isOpen())
		{
			db.close();
		}
	}

	return db;
}

/* 从本地中导入文件 */
void DataWidget::importData()
{
	/* 文件对话框 */
	QString filename, strLine, str;
	QStringList strList;
	QRegExp reg("\\s+");
	QMessageBox tips;
	int i, len, index, sum;

	filename = QFileDialog::getOpenFileName(this,
		tr("导入文件"), "./", tr("Text Documents (*.txt)"));

	QFile file(filename);
	if (file.open(QIODevice::ReadOnly))
	{
		QTextStream in(&file);
		while ((strLine = in.readLine()), strLine.length() > 0)
		{
			strList = strLine.split(reg);
			qDebug() << strList;
			len = strList.length();
			for (i = 0; i < len && i < TABLE_FIELD; ++i)
			{
				QStandardItem *item = new QStandardItem(strList.at(i));
				model->setItem(getTableRow(), i, item);
			}
			this->setTableRow(getTableRow() + 1);
		}

		str = "已导入 ";
		sum = getTableRow();
		if (sum == 0)
		{
			str = str + '0';
		}
		while (sum)
		{
			index = sum % 10;
			sum /= 10;
			str = (index + '0') + str;
		}
		str = str + " 条记录!";

		tips.setText(tr("是否将当前表中的数据导入数据库?"));
		tips.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		tips.setWindowTitle(tr("提示:"));
		tips.setWindowIcon(QIcon(":/images/hutIcon.ico"));
		if (tips.exec() == QMessageBox::Yes)
		{
			saveCurrentRecord();	/* 将记录写入数据库 */
			emit sendLabelStatus(str);
		}
	}
}

/* 导出文件到本地 */
void DataWidget::exportData()
{
	QSqlDatabase db;
	QString filename;
	QString str, sql;
	int i, j, col = TABLE_FIELD, len;
	QMessageBox tips;
	QRegExp reg("*.xls*");

	/* 提示框 */
	tips.setText(tr("已成功将数据导出!"));
	tips.setWindowTitle(tr("提示:"));
	tips.setWindowIcon(QIcon(":/images/hutIcon.ico"));
	tips.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

	filename = QFileDialog::getSaveFileName(this,
		tr("导出"), "./", tr("Text Documents (*.txt);;Excel 工作簿 (*.xls)"));

	len = filename.length();
	if (filename.at(len - 1).toAscii() == 's' ||
		filename.at(len - 1).toAscii() == 'x')
	{
		qDebug() << "导出excel";
		sql = "select * from student order by stuId asc into outfile '";
		sql += filename;
		sql += "'";
		db = linkDb(driver, dbName, host, user, passwd, port);
		QSqlQuery que(sql, db);
		qDebug() << sql;
		if (que.exec())
		{
			tips.exec();
		}
		db.close();
		db.removeDatabase(QSqlDatabase::defaultConnection);
		emit this->sendLabelStatus(tr("数据已导出!"));
		return;
	}

	sql = tr("select * from student order by stuId asc");
	QFile file(filename);

	if (file.open(QIODevice::WriteOnly))
	{
		QTextStream  out(&file);
		db = linkDb(driver, dbName, host, user, passwd, port);
		sql = "select * from student";
		QSqlQuery que(sql, db);
		if (que.exec())
		{
			while (que.next())
			{
				str = "";
				for (i = 0; i < col; ++i)
				{
					str += que.value(i).toString();
					if (i + 1 == col)
					{
						str += '\r';
						str += '\n';
					}
					else
					{
						str += '	';
					}
				}
				out << str;
			}
			emit this->sendLabelStatus(tr("数据已导出!"));
		}
		tips.exec();

		db.close();
		db.removeDatabase(QSqlDatabase::defaultConnection);
	}
}

void DataWidget::addStuRecord()
{
	int i, len;
	QStandardItem *item = NULL;

	len = TABLE_FIELD;
	for (i = 0; i < len; ++i)
	{
		if ((i >= 13 && i <= 23) || i == 25)
		{
			item = new QStandardItem(tr("0"));
		}
		else
		{
			item = new QStandardItem(tr(""));
		}
		model->setItem(tableRow, i, item);
	}
	model->setRowCount(++tableRow);

	emit this->sendLabelStatus(tr("空记录已添加, 请填写!"));
}

void DataWidget::subStuRecord()
{
	QMessageBox tips;
	QString str, sql, strTmp;
	QSqlDatabase db;

	tips.setWindowTitle(tr("删除:"));
	tips.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	tips.setWindowIcon(QIcon(":/images/hutIcon.ico"));
	if (tableRow > 0)
	{
		strTmp = model->item(tableRow - 1, 0)->text();
		str = "是否删除证书编号为:";
		str += strTmp;
		str += "这一记录?";
		tips.setText(str);
		if (tips.exec() == QMessageBox::Yes)
		{
			db = linkDb(driver, dbName, host, user, passwd, port);
			sql = tr("delete from student where certificateNum = '");
			sql += strTmp;
			sql += "'";
			QSqlQuery que(sql, db);
			if (que.exec(sql))
			{
				model->removeRow(tableRow - 1);
				model->setRowCount(--tableRow);
				tips.setText(tr("删除成功!"));
				emit sendLabelStatus(tr("已删表格最后一条记录!"));
			}
			else
			{
				tips.setText(tr("删除失败!"));
			}
			tips.exec();
		}
	}
}

/* 初始化栏 */
void DataWidget::initField()
{
	QString str = tr("证书编号 学号 姓名 性别 民族 出生年月 出生地 政治面貌 学校 学院 专业 入学时间 毕业生类型 学期一 学期二 学期三 学期四 学期五 学期六 学期七 学期八 学期九 学期十 总分 备注 认证");
	QStringList strList;
	int i, len;

	strList = str.split(QRegExp("\\s+"));
	len = strList.length();
	for (i = 0; i < len; ++i)
	{
		model->setHeaderData(i, Qt::Horizontal, strList.at(i));
	}
}

void DataWidget::initConnection()
{
	connect(this->model, SIGNAL(itemChanged(QStandardItem*)), model, SLOT(submit()));
}

void DataWidget::setTableRow(const int &_row)
{
	tableRow = _row;
	model->setRowCount(tableRow);
}

void DataWidget::setTableCol(const int &_col)
{
	tableCol = _col;
}

int DataWidget::getTableRow()
{
	return tableRow;
}

int DataWidget::getTableCol()
{
	return tableCol;
}

/* 将数据插入到表格中 */
void DataWidget::insertIntoTable(QStringList &strList)
{
	int i, len;
	QString str;

	len = strList.length();
	for (i = 0; i < len && i < TABLE_FIELD; ++i)
	{
		str = strList.at(i);
		QStandardItem *item = new QStandardItem(str);
		model->setItem(this->getTableRow(), i, item);;
	}

	this->setTableRow(this->getTableRow() + 1);
}

DataWidget::~DataWidget()
{
    delete ui;

	qDebug() << "已收到来自LoginDlg连接数据库的信息!";
	qDebug() << linkMsg;
}
