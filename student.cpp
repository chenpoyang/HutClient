#include <QDebug>
#include "student.h"
#include "ui_student.h"

Student::Student(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Student)
{
	ui->setupUi(this);
	initMainWidget();
	initDefaultData();
	initConnection();
	this->setVisible(false);
}

void Student::initDefaultData()
{
	ui->sexComBo->addItem(tr("男"));
	ui->sexComBo->addItem(tr("女"));
	ui->graduateTypeCB->addItem("本科生");
	ui->graduateTypeCB->addItem("专科生");
	ui->schoolLE->setText(tr("湖南工业大学"));
	ui->semesterOne->setText(tr("0"));
	ui->semesterTwo->setText(tr("0"));
	ui->semesterTwo->setText(tr("0"));
	ui->semesterThree->setText(tr("0"));
	ui->semesterFour->setText(tr("0"));
	ui->semesterFive->setText(tr("0"));
	ui->semesterSix->setText(tr("0"));
	ui->semesterSeven->setText(tr("0"));
	ui->semesterEight->setText(tr("0"));
	ui->semesterNine->setText(tr("0"));
	ui->semesterTen->setText(tr("0"));
	ui->remarkTE->setText(tr("无"));
}

void Student::initConnection()
{
	connect(ui->confireAddStu, SIGNAL(clicked()),
			this, SLOT(getStuInfo()));
	connect(ui->cancelAddStu, SIGNAL(clicked()),
			this, SLOT(close()));
}

/* 获取学生个人信息 */
void Student::getStuInfo()
{
	int len;
	QRegExp reg("^\\d+$");

	certificateNum = ui->certifiedNumLE->text();
	stuId = ui->stuIdLE->text();
	name = ui->stuNameLE->text();
	sex = ui->sexComBo->currentText();
	ethnic = ui->ethnicLE->text();
	birthDate = ui->birthDateLE->text();
	birthPlace = ui->birthPlaceLE->text();
	politiStu = ui->politicalStuLE->text();
	school = ui->schoolLE->text();
	college = ui->collegeLE->text();
	major = ui->majorLE->text();
	entryDate = ui->entryTimeLE->text();
	graduateType = ui->graduateTypeCB->currentText();
	remark = ui->remarkTE->toPlainText();

	len = 0;
	if (reg.exactMatch(ui->semesterOne->text()))
	{
		mark[len++] = ui->semesterOne->text().toInt();
	}

	if (reg.exactMatch(ui->semesterTwo->text()))
	{
		mark[len++] = ui->semesterTwo->text().toInt();
	}

	if (reg.exactMatch(ui->semesterThree->text()))
	{
		mark[len++] = ui->semesterThree->text().toInt();
	}

	if (reg.exactMatch(ui->semesterFour->text()))
	{
		mark[len++] = ui->semesterFour->text().toInt();
	}

	if (reg.exactMatch(ui->semesterFive->text()))
	{
		mark[len++] = ui->semesterFive->text().toInt();
	}

	if (reg.exactMatch(ui->semesterSix->text()))
	{
		mark[len++] = ui->semesterSix->text().toInt();
	}

	if (reg.exactMatch(ui->semesterSeven->text()))
	{
		mark[len++] = ui->semesterSeven->text().toInt();
	}

	if (reg.exactMatch(ui->semesterEight->text()))
	{
		mark[len++] = ui->semesterEight->text().toInt();
	}

	if (reg.exactMatch(ui->semesterNine->text()))
	{
		mark[len++] = ui->semesterNine->text().toInt();
	}

	if (reg.exactMatch(ui->semesterTen->text()))
	{
		mark[len++] = ui->semesterTen->text().toInt();
	}

	if (len != STU_MARK_CNT)
	{
		QMessageBox::warning(this, tr("警告:"),
			tr("所填数据不合法, 请检查更正!"), QMessageBox::Yes, QMessageBox::No);
		return;
	}
	qDebug() << len;

	insertData();
}

/* 向数据库插入数据 */
void Student::insertData()
{
	QStringList strList;
	QString str, strLabel;
	QMessageBox info;
	int i, len, sum;

	strList << certificateNum;
	strList << stuId;
	strList << name;
	strList << sex;
	strList << ethnic;
	strList << birthDate;
	strList << birthPlace;
	strList << politiStu;
	strList << school;
	strList << college;
	strList << major;
	strList << entryDate;
	strList << graduateType;
	strList << ui->semesterOne->text();
	strList << ui->semesterTwo->text();
	strList << ui->semesterThree->text();
	strList << ui->semesterFour->text();
	strList << ui->semesterFive->text();
	strList << ui->semesterSix->text();
	strList << ui->semesterSeven->text();
	strList << ui->semesterEight->text();
	strList << ui->semesterNine->text();
	strList << ui->semesterTen->text();

	len = STU_MARK_CNT;
	sum = 0;
	for (i = 0; i < len; ++i)
	{
		sum += mark[i];
	}

	str = "";
	while (sum)
	{
		i = sum % 10;
		sum /= 10;
		str = (i + '0') + str;
	}
	if (str == "")
	{
		str += '0';
	}
	strList << str;
	strList << ui->remarkTE->toPlainText();
	strList << "否";

	info.setText(tr("是否确认添加学生信息?"));
	info.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	info.setWindowTitle(tr("提示:"));
	info.setWindowIcon(QIcon(":/images/hutIcon.ico"));
	if (info.exec() != QMessageBox::Yes)
	{
		this->close();
		this->hide();
	}
	else
	{
		emit submitStu(strList);	/* 发送一条学生记录 */
		strLabel = "学生 ";
		strLabel += name;
		strLabel += " 已添加到表格中!";
		emit this->sendTextForLabel(strLabel);	/* 设置主控Label */
		qDebug() << this->isHidden();
		clearData();	/* 清除上一条记录 */
		qDebug() << "已发送一条信息到DataWidget!";
	}
}

void Student::initMainWidget()
{
	resize(630, 460);
	setMaximumSize(630, 460);
	setMinimumSize(630, 460);
	setWindowTitle(tr("添加学生"));
	setWindowIcon(QIcon(":/images/hutIcon.ico"));
	setVisible(true);
}

void Student::clearData()
{
	ui->certifiedNumLE->setText(tr(""));
	ui->stuIdLE->setText(tr(""));
	ui->stuNameLE->setText(tr(""));
	ui->ethnicLE->setText(tr(""));
	ui->birthDateLE->setText(tr(""));
	ui->birthPlaceLE->setText(tr(""));
	ui->politicalStuLE->setText(tr(""));
	ui->collegeLE->setText(tr(""));
	ui->majorLE->setText(tr(""));
	ui->entryTimeLE->setText(tr(""));
	ui->remarkTE->setText(tr("无"));
	ui->semesterOne->setText(tr("0"));
	ui->semesterTwo->setText(tr("0"));
	ui->semesterThree->setText(tr("0"));
	ui->semesterFour->setText(tr("0"));
	ui->semesterFive->setText(tr("0"));
	ui->semesterSix->setText(tr("0"));
	ui->semesterSeven->setText(tr("0"));
	ui->semesterEight->setText(tr("0"));
	ui->semesterNine->setText(tr("0"));
	ui->semesterTen->setText(tr("0"));
}

void Student::keyPressEvent(QKeyEvent *event)
{
	int keyCode = event->key();

	if (keyCode == Qt::Key_Enter)
	{
		getStuInfo();
	}
}

Student::~Student()
{
    delete ui;
}
