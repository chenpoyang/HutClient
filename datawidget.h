#ifndef DATAWIDGET_H
#define DATAWIDGET_H

#include <QWidget>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFileDialog>
#include <QTextStream>
#include <QFile>
#include <QRegExp>
#include <QIcon>
#include "student.h"
#define TABLE_FIELD 26

namespace Ui {
    class DataWidget;
}

class DataWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DataWidget(QWidget *parent = 0);
    ~DataWidget();
	void initField();
	void initConnection();
	void setTableRow(const int &_row);
	void setTableCol(const int &_col);
	int getTableRow();
	int getTableCol();
	QSqlDatabase linkDb(const QString &driver, const QString &dbName, const QString &host,
		const QString &user, const QString &passwd, int port = 3306);
	void setLinkDbMsg(const QStringList &tmpMsg);
	QStringList getLinkDbMsg();

	/* 数据库信息部分 */
	QString getDriver() { return driver; }
	QString getdbName() { return dbName; }
	QString getHost() { return host; }
	QString getUser() { return user; }
	QString getPwd() { return passwd; }
	int getPort() { return port; }

signals:
	void sendLabelStatus(QString &stu);
	void sendCollegeSearch(QStringList &strList);
	void sendClassSearch(QStringList &strList);
	void setTextForMain(QString &str);

private slots:
	void addStuRecord();
	void subStuRecord();
	void insertIntoTable(QStringList &strList);
	void importData();
	void exportData();
	void getLinkMsg(QStringList &strList);
	void saveCurrentRecord();
	void getQueryData(const QStringList &tmpList);
	void getBatchCerData(const QStringList &tmpList);
	void updateClassQuery(QString college);

public:
	QStandardItemModel *model;	/* 表格显示模型 */
	int tableRow, tableCol;

private:
	Ui::DataWidget *ui;
	QStringList linkMsg;
	QString driver, dbName, host, user, passwd;
	int port;
};

#endif // DATAWIDGET_H
