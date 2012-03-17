#include "logindlg.h"
#include "ui_logindlg.h"

LoginDlg::LoginDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDlg)
{
    ui->setupUi(this);
	this->setWindowTitle(tr("大学生素质拓展管理系统"));
	this->setWindowIcon(QIcon(":/images/hutIcon.ico"));
	initSignalAndSlot();
	initBackground();
}

void LoginDlg::initBackground()
{
	QPalette pal = this->palette();
	QString filename = "./login.jpg";
	QPixmap pixmap(filename);
	pal.setBrush(QPalette::Window,QBrush(pixmap));
	setPalette(pal);
}

QString LoginDlg::initSql()
{
	QString sql;

	sql = "CREATE DATABASE IF NOT EXISTS `hut_stu_mge`;";
	sql += "CREATE  TABLE IF NOT EXISTS `hut_stu_mge`.`student` (";
	sql += "  `certificateNum` VARCHAR(20) NOT NULL ,";
	sql += "  `stuId` VARCHAR(15) NOT NULL ,";
	sql += "  `name` VARCHAR(20) NOT NULL ,";
	sql += "  `sex` VARCHAR(10) NOT NULL ,";
	sql += "  `ethnic` VARCHAR(20) NOT NULL ,";
	sql += "  `birthdate` VARCHAR(30) NOT NULL ,";
	sql += "  `birthplace` VARCHAR(150) NOT NULL ,";
	sql += "  `politicalStus` VARCHAR(45) NOT NULL ,";
	sql += "  `school` VARCHAR(45) NOT NULL ,";
	sql += "  `college` VARCHAR(45) NOT NULL ,";
	sql += "  `major` VARCHAR(45) NOT NULL ,";
	sql += "  `entryTime` VARCHAR(45) NOT NULL ,";
	sql += "  `graduateStus` VARCHAR(45) NOT NULL ,";
	sql += "  `semesterOne` INT NOT NULL DEFAULT 0 ,";
	sql += "  `semesterTwo` INT NOT NULL DEFAULT 0 ,";
	sql += "  `semesterThree` INT NOT NULL DEFAULT 0 ,";
	sql += "  `semesterFour` INT NOT NULL DEFAULT 0 ,";
	sql += "  `semesterFive` INT NOT NULL DEFAULT 0 ,";
	sql += "  `semesterSix` INT NOT NULL DEFAULT 0 ,";
	sql += "  `semesterSeven` INT NOT NULL DEFAULT 0 ,";
	sql += "  `semesterEight` INT NOT NULL DEFAULT 0 ,";
	sql += "  `semesterNine` INT NOT NULL DEFAULT 0 ,";
	sql += "  `semesterTen` INT NOT NULL DEFAULT 0 ,";
	sql += "  `totalMark` INT NOT NULL DEFAULT 0 ,";
	sql += "  `remark` VARCHAR(1000) NULL ,";
	sql += "  `certified` INT NOT NULL DEFAULT 0 ,";
	sql += "  PRIMARY KEY (`certificateNum`, `stuId`) ,";
	sql += "  UNIQUE INDEX `certificateNum_UNIQUE` (`certificateNum` ASC) ,";
	sql += "  INDEX `college` (`college` ASC) ,";
	sql += "  UNIQUE INDEX `stuId_UNIQUE` (`stuId` ASC) )";
	sql += "ENGINE = InnoDB ";
	sql += "DEFAULT CHARACTER SET = utf8 ";
	sql += "COLLATE = utf8_general_ci;";

	QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
	db.setHostName(this->getHost());
	db.setPort(this->getHostPort());
	QSqlQuery que(sql, db);
	if (db.open(this->usr, this->pwd))
	{
		if (!que.exec())
		{
			QMessageBox::warning(this, "警告!",
				tr("初始化数库失败, 系统将退出!"), QMessageBox::Yes, QMessageBox::No);
			exit(-1);
		}
	}
	qDebug() << sql;
	return sql;
}

/* 获取连接数据库的信息 */
QStringList LoginDlg::getLinkMsg()
{
	return linkMsg;
}

/* 设置连接数据库的信息 */
void LoginDlg::setLinkMsg(const QStringList &tmpMsg)
{
	linkMsg = tmpMsg;
}

QSqlDatabase LoginDlg::linkDb(const QString &driver, const QString &dbName,
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

/* 初始化信号与槽 */
void LoginDlg::initSignalAndSlot()
{
	connect(ui->okBtn, SIGNAL(clicked()),
			this, SLOT(onOkBtnClicked()));
	connect(ui->cancelBtn, SIGNAL(clicked()),
			this, SLOT(onCancelBtnClicked()));
}

/* 登陆验证, 判断是否可以连接数据库 */
void LoginDlg::onOkBtnClicked()
{
	QSqlError err;
	QSqlDatabase db;
	QStringList strList;
	QString str;
	int tmpPort, index;
	usr = userName();
	pwd = password();

	initConnInfo();	/* 初始化MySql主机和数据库 */
	initSql();	/* 初始数据库 */

	err = addConnection(driver, dbName, host, usr, pwd, port);
	db = linkDb(driver, dbName, host, usr, pwd, port);
	if (err.type() == QSqlError::NoError)
	{
		/* 将数库连接的信息传给DataWidget */
		strList << getDbDriver();
		strList << getDbName();
		strList << getHost();
		strList << getDbUser();
		strList << getDbPwd();

		/* 将端口转换成字符串 */
		str = "";
		tmpPort = getHostPort();
		qDebug() << "端口:" << tmpPort;
		if (tmpPort == 0)
		{
			str = str + '0';
		}
		while (tmpPort)
		{
			index = tmpPort % 10;
			tmpPort /= 10;
			str = (index + '0') + str;
		}

		strList << str;
		this->setLinkMsg(strList);	/* 设置连接数据库的信息 */
		qDebug() << "link msg has been set!" << strList;

		this->accept();	/* 连接信息正确 */
	}
	else
	{
		this->reject();	/* 连接信息错误 */
	}

	db.close();
}

/* 初始化连接信息 */
void LoginDlg::initConnInfo()
{	
	driver = "QMYSQL";
	dbName = "hut_stu_mge";
	host = "127.0.0.1";
	port = 3306;
}

/* 获取用户输入的用户名 */
QString LoginDlg::userName() const
{
	return ui->useLineEdit->text();
}

/* 获取用户输入的密码 */
QString LoginDlg::password() const
{
	return ui->pwdLineEdit->text();
}

/* 连接数据库 */
QSqlError LoginDlg::addConnection(const QString &driver, const QString &dbName,
		const QString &host, const QString &user, const QString &passwd, int port)
{
	static int conCnt = 0;
	QSqlError err;

	QSqlDatabase db = QSqlDatabase::addDatabase(driver, QString("conn%1").arg(++conCnt));
	db.setHostName(host);
	db.setDatabaseName(dbName);
	db.setPort(port);
	if (!db.open(user, passwd) || user != "root")
	{
		err = db.lastError();
		QSqlDatabase::removeDatabase(QString("conn%1").arg(conCnt));
	}

	return err;
}

QString LoginDlg::getDbDriver() const
{
	return driver;
}

QString LoginDlg::getHost() const
{
	return host;
}

int LoginDlg::getHostPort() const
{
	return port;
}

QString LoginDlg::getDbName() const
{
	return dbName;
}

QString LoginDlg::getDbUser() const
{
	return usr;
}

QString LoginDlg::getDbPwd() const
{
	return pwd;
}

LoginDlg::~LoginDlg()
{
    delete ui;
	qDebug() << "没登陆成功, LoginDlg内存被释放";
}
