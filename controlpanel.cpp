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
	ui->graduateTypeCmb->addItem(tr("������"));
	ui->graduateTypeCmb->addItem(tr("ר����"));
	ui->sortRuComboBox->addItem(tr("֤����"));
	ui->batchCertifiedBtn->setEnabled(false);
}

/* ���ò�ѯ���� */
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

/* ����ѧԺ�ź� */
void ControlPanel::launchSendCollege(QString college)
{
	emit sendCollege(college);
}

/* ���²�ѯ���� */
void ControlPanel::setCollegeComboBox(QStringList &strList)
{
	ui->collegeComboBox->clear();
	ui->collegeComboBox->addItems(strList);
}

/* ͬ�� */
void ControlPanel::setClassComboBox(QStringList &strList)
{
	ui->classComboBox->clear();
	ui->classComboBox->addItems(strList);
}

/* ����������֤�ź� */
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

/* ���Ͳ�ѯ�ź� */
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

/* ���ͱ����ź� */
void ControlPanel::launchReady()
{
	emit readyToSave();	/* ��DataWidget����, ������ */
}

void ControlPanel::initConnection()
{
	/* ���ѧ��������Ϣ�ĶԻ��� */
	connect(ui->addStuBtn, SIGNAL(clicked()),
			this, SLOT(launchAddStu()));
	/* ���� */
	connect(ui->impBtn, SIGNAL(clicked()),
			this, SLOT(sendImportSignal()));
	/* ���� */
	connect(ui->expBtn, SIGNAL(clicked()),
			this, SLOT(sendExportSignal()));
	/* ���� */
	connect(ui->saveBtn, SIGNAL(clicked()),
			this, SLOT(launchReady()));

	/* ��ѯ */
	connect(ui->queryBtn, SIGNAL(clicked()),
			this, SLOT(launchQuery()));

	/* ��֤ */
	connect(ui->batchCertifiedBtn, SIGNAL(clicked()),
			this, SLOT(launchBatchCertificate()));

	/* ��ѧԺ�ı�ʱ, ����רҵ */
	connect(ui->collegeComboBox, SIGNAL(currentIndexChanged(QString)),
			this, SLOT(launchSendCollege(QString)));

	/* ���ò�ѯ���� */
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
