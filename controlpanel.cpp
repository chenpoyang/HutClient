#include "controlpanel.h"
#include "ui_controlpanel.h"
#define MIN_CERTIFICATE 0
#define MAX_CERTIFICATE 1000

ControlPanel::ControlPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ControlPanel)
{
	ui->setupUi(this);
	initConnection();
	ui->addStuBtn->setShortcut(QKeySequence(tr("ALT+A")));
	ui->addStuBtn->setToolTip(tr("ALT+A"));
	ui->certifiedSpinBox->setRange(MIN_CERTIFICATE, MAX_CERTIFICATE);
	ui->graduateTypeCmb->addItem(tr("本科生"));
	ui->graduateTypeCmb->addItem(tr("专科生"));
	ui->sortRuComboBox->addItem(tr("证书编号"));
	ui->batchCertifiedBtn->setEnabled(false);
}

/* 设置查询条件 */
void ControlPanel::setCollege(QString str)
{
	this->college = str;
}

void ControlPanel::setMajor(QString str)
{
	this->major = str;
}

void ControlPanel::setOrderRu(QString str)
{
	this->orderRu = str;
}

void ControlPanel::setStuName(QString str)
{
	this->stuName = str;
}

void ControlPanel::setStuId(QString str)
{
	this->stuId = str;
}

/* 发送学院信号 */
void ControlPanel::launchSendCollege(QString college)
{
	emit sendCollege(college);
}

/* 更新查询条件 */
void ControlPanel::setCollegeComboBox(QStringList &strList)
{
	ui->collegeComboBox->clear();
	ui->collegeComboBox->addItems(strList);
}

/* 同上 */
void ControlPanel::setClassComboBox(QStringList &strList)
{
	ui->classComboBox->clear();
	ui->classComboBox->addItems(strList);
}

/* 发送批量认证信号 */
void ControlPanel::launchBatchCertificate()
{
	QStringList strList;
	QString str;
	int sum, index;

	strList << ui->graduateTypeCmb->currentText();
	sum = ui->certifiedSpinBox->value();
	str = "";
	if (sum == 0)
	{
		str = '0' + str;
	}
	while (sum)
	{
		index = sum % 10;
		sum /= 10;
		str = (index + '0') + str;
	}
	strList << str;

	qDebug() << "sendBatchCertificate(strList)";
	emit sendBatchCertificate(strList);
}

/* 发送查询信号 */
void ControlPanel::launchQuery()
{
	QStringList strList;

	strList << college;
	strList << major;
	strList << orderRu;
	strList << stuName;
	strList << stuId;

	qDebug() << "sendQueryData(strList)";
	emit sendQueryData(strList);
}

/* 发送保存信号 */
void ControlPanel::launchReady()
{
	emit readyToSave();	/* 由DataWidget接收, 并处理 */
}

void ControlPanel::initConnection()
{
	/* 添加学生个人信息的对话框 */
	connect(ui->addStuBtn, SIGNAL(clicked()),
			this, SLOT(launchAddStu()));
	/* 导入 */
	connect(ui->impBtn, SIGNAL(clicked()),
			this, SLOT(sendImportSignal()));
	/* 导出 */
	connect(ui->expBtn, SIGNAL(clicked()),
			this, SLOT(sendExportSignal()));
	/* 保存 */
	connect(ui->saveBtn, SIGNAL(clicked()),
			this, SLOT(launchReady()));

	/* 查询 */
	connect(ui->queryBtn, SIGNAL(clicked()),
			this, SLOT(launchQuery()));

	/* 认证 */
	connect(ui->batchCertifiedBtn, SIGNAL(clicked()),
			this, SLOT(launchBatchCertificate()));

	/* 当学院改变时, 更改专业 */
	connect(ui->collegeComboBox, SIGNAL(currentIndexChanged(QString)),
			this, SLOT(launchSendCollege(QString)));

	/* 设置查询条件 */
	connect(ui->collegeComboBox, SIGNAL(currentIndexChanged(QString)),
			this, SLOT(setCollege(QString)));
	connect(ui->classComboBox, SIGNAL(currentIndexChanged(QString)),
			this, SLOT(setMajor(QString)));
	connect(ui->sortRuComboBox, SIGNAL(currentIndexChanged(QString)),
			this, SLOT(setOrderRu(QString)));
	connect(ui->stuNameLineEdit, SIGNAL(textChanged(QString)),
			this, SLOT(setStuName(QString)));
	connect(ui->stuIdLineEdit, SIGNAL(textChanged(QString)),
			this, SLOT(setStuId(QString)));
}

void ControlPanel::sendImportSignal()
{
	emit this->importDataToTable();
}

void ControlPanel::sendExportSignal()
{
	emit this->exportDataToLocal();
}

void ControlPanel::launchAddStu()
{
	emit onAddStuClicked();
}

void ControlPanel::keyPressEvent(QKeyEvent *event)
{
	qDebug() << event->key();
}

ControlPanel::~ControlPanel()
{
    delete ui;
}
