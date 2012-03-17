#ifndef LOGINDLG_H
#define LOGINDLG_H

#include <QDialog>
#include <QMessageBox>
#include <QDebug>
#include <QtSql>
#include <QtGui>
#include <QMessageBox>
#include <QDebug>
#include <QSqlError>

namespace Ui {
    class LoginDlg;
}

class LoginDlg : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDlg(QWidget *parent = 0);
    ~LoginDlg();
	QString userName() const;
	QString password() const;
	QSqlError addConnection(const QString &driver, const QString &dbName, const QString &host,
				  const QString &user, const QString &passwd, int port = 3306);
	void initConnInfo();
	void initSignalAndSlot();
	QSqlDatabase linkDb(const QString &driver, const QString &dbName, const QString &host,
		const QString &user, const QString &passwd, int port = 3306);
	QString getDbUser() const;
	QString getDbPwd() const;
	QString getDbDriver() const;
	QString getDbName() const;
	QString getHost() const;
	int getHostPort() const;
	void setLinkMsg(const QStringList &tmpMsg);
	QStringList getLinkMsg();
	QString initSql();
	void initBackground();

public:
	QString usr, pwd, driver, dbName, host;
	int port;

private slots:
	void onOkBtnClicked();
	void onCancelBtnClicked()
	{
		reject();
	}

private:
    Ui::LoginDlg *ui;
	QStringList linkMsg;
};

#endif // LOGINDLG_H
