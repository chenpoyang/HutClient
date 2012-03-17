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

/* ����ָ��ѧԺ�µ�רҵ */
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
		qDebug() << "û�����ݿ⽨������!";
	}
	db.close();
	db.removeDatabase(QSqlDatabase::defaultConnection);
}

/* ��ѯ */
void DataWidget::getQueryData(const QStringList &tmpList)
{
	QSqlDatabase db = linkDb(driver, dbName, host, user, passwd, port);
	QSqlQuery que;
	QString college, major, orderRu, stuName, stuId, strMsg, strTmp;
	bool orderByStuId;
	int row, col, i;
	qDebug() << "DataWidget���ڲ�ѯ��..." << tmpList;

	row = 0;
	col = TABLE_FIELD;
	this->setTableRow(row);

	college = tmpList.at(0);
	major = tmpList.at(1);

	/* ������� */
	if (tmpList.at(2).toStdString() == "֤����")
	{
		orderRu = "certificateNum";
		qDebug() << "��֤��������";
		orderByStuId = false;
	}
	else
	{
		orderRu = "stuId";
		qDebug() << "��ѧ�������";
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
		strTmp = "�Ѳ�ѯ�� " + strTmp;
		strTmp += " ����¼!";
		emit this->setTextForMain(strTmp);
		qDebug() << strTmp;
	}
	else
	{
		QMessageBox::warning(this,
			tr("����"), tr("�޷��������ݿ�!"), QMessageBox::Yes, QMessageBox::No);
	}
	db.close();
	db.removeDatabase(QSqlDatabase::defaultConnection);
}

/* ��ȡ������֤���� */
void DataWidget::getBatchCerData(const QStringList &tmpList)
{
	qDebug() << "DataWidget����������֤��...";

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
		tips.setText(tr("�����������֤!"));
		db.close();
		db.removeDatabase(QSqlDatabase::defaultConnection);
		emit this->sendLabelStatus(tr("������֤�����!"));
	}
	else
	{
		tips.setText(tr("������֤����, ��ȷ��MySQL������!"));
	}
	tips.setWindowTitle(tr("��֤���:"));
	tips.setWindowIcon(QIcon(":/images/hutIcon.ico"));
	tips.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	tips.exec();
}

/* �ѵ�ǰ�������д�����ݿ� */
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

					strMsg = tr("����������ݵ�");
					strMsg += strCurRow;
					strMsg += "��, ��";
					strCurCol.setNum(j + 1, 10);
					strMsg += strCurCol;
					strMsg += "�в��Ϸ�, ϵͳ��ֹͣ����, ����!";
					QMessageBox::warning(this, tr("����!"),
						strMsg, QMessageBox::Yes, QMessageBox::No);
					db.rollback();
					return;
				}
			}
			else if (j == 25)
			{
				str = model->item(i, j)->text();
				if (str == "��" || str == "1")
				{
					//que.bindValue(j, 1); ���ݿ������
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

	strMsg = "��";
	strTmp.setNum(sum);
	strMsg += strTmp;
	strMsg += "����¼��д�����ݿ�";

	tips.setText(strMsg);
	tips.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	tips.setWindowTitle(tr("���:"));
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

/* ��ȡ�������ݿ����Ϣ */
QStringList DataWidget::getLinkDbMsg()
{
	return linkMsg;
}

/* �����������ݿ����Ϣ */
void DataWidget::setLinkDbMsg(const QStringList &tmpMsg)
{
	linkMsg = tmpMsg;

	driver = linkMsg.at(0);
	dbName = linkMsg.at(1);
	host = linkMsg.at(2);
	user = linkMsg.at(3);
	passwd = linkMsg.at(4);
	port = linkMsg.at(5).toInt();
	qDebug() << "�ѷֽ��������ݿ����Ϣ:";
	qDebug() << driver << dbName << host << user << passwd << port;
}

void DataWidget::getLinkMsg(QStringList &strList)
{
	linkMsg = strList;
}

/* �������ݿ� */
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
		QMessageBox::warning(this, tr("����"),
			"����MySql����, ��ȷ��MySql���������û�����������!", QMessageBox::Ok);
		if (db.isOpen())
		{
			db.close();
		}
	}

	return db;
}

/* �ӱ����е����ļ� */
void DataWidget::importData()
{
	/* �ļ��Ի��� */
	QString filename, strLine, str;
	QStringList strList;
	QRegExp reg("\\s+");
	QMessageBox tips;
	int i, len, index, sum;

	filename = QFileDialog::getOpenFileName(this,
		tr("�����ļ�"), "./", tr("Text Documents (*.txt)"));

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

		str = "�ѵ��� ";
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
		str = str + " ����¼!";

		tips.setText(tr("�Ƿ񽫵�ǰ���е����ݵ������ݿ�?"));
		tips.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		tips.setWindowTitle(tr("��ʾ:"));
		tips.setWindowIcon(QIcon(":/images/hutIcon.ico"));
		if (tips.exec() == QMessageBox::Yes)
		{
			saveCurrentRecord();	/* ����¼д�����ݿ� */
			emit sendLabelStatus(str);
		}
	}
}

/* �����ļ������� */
void DataWidget::exportData()
{
	QSqlDatabase db;
	QString filename;
	QString str, sql;
	int i, j, col = TABLE_FIELD, len;
	QMessageBox tips;
	QRegExp reg("*.xls*");

	/* ��ʾ�� */
	tips.setText(tr("�ѳɹ������ݵ���!"));
	tips.setWindowTitle(tr("��ʾ:"));
	tips.setWindowIcon(QIcon(":/images/hutIcon.ico"));
	tips.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

	filename = QFileDialog::getSaveFileName(this,
		tr("����"), "./", tr("Text Documents (*.txt);;Excel ������ (*.xls)"));

	len = filename.length();
	if (filename.at(len - 1).toAscii() == 's' ||
		filename.at(len - 1).toAscii() == 'x')
	{
		qDebug() << "����excel";
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
		emit this->sendLabelStatus(tr("�����ѵ���!"));
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
			emit this->sendLabelStatus(tr("�����ѵ���!"));
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

	emit this->sendLabelStatus(tr("�ռ�¼�����, ����д!"));
}

void DataWidget::subStuRecord()
{
	QMessageBox tips;
	QString str, sql, strTmp;
	QSqlDatabase db;

	tips.setWindowTitle(tr("ɾ��:"));
	tips.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	tips.setWindowIcon(QIcon(":/images/hutIcon.ico"));
	if (tableRow > 0)
	{
		strTmp = model->item(tableRow - 1, 0)->text();
		str = "�Ƿ�ɾ��֤����Ϊ:";
		str += strTmp;
		str += "��һ��¼?";
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
				tips.setText(tr("ɾ���ɹ�!"));
				emit sendLabelStatus(tr("��ɾ������һ����¼!"));
			}
			else
			{
				tips.setText(tr("ɾ��ʧ��!"));
			}
			tips.exec();
		}
	}
}

/* ��ʼ���� */
void DataWidget::initField()
{
	QString str = tr("֤���� ѧ�� ���� �Ա� ���� �������� ������ ������ò ѧУ ѧԺ רҵ ��ѧʱ�� ��ҵ������ ѧ��һ ѧ�ڶ� ѧ���� ѧ���� ѧ���� ѧ���� ѧ���� ѧ�ڰ� ѧ�ھ� ѧ��ʮ �ܷ� ��ע ��֤");
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

/* �����ݲ��뵽����� */
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

	qDebug() << "���յ�����LoginDlg�������ݿ����Ϣ!";
	qDebug() << linkMsg;
}
