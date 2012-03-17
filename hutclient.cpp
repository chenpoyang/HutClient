#include "hutclient.h"
#include "ui_hutclient.h"

HutClient::HutClient(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HutClient)
{
    ui->setupUi(this);
	initMainWidget();
	initMainAction();
	initConnection();
}

void HutClient::initConnection()
{
	/* ��������Action��Ӧ */
	connect(ui->actImp, SIGNAL(triggered()),
			controlPanel, SLOT(sendImportSignal()));
	connect(ui->actExp, SIGNAL(triggered()),
			controlPanel, SLOT(sendExportSignal()));
	connect(ui->actExit, SIGNAL(triggered()),
			this, SLOT(close()));
	connect(ui->actSearch, SIGNAL(triggered()),
			controlPanel, SLOT(launchQuery()));
	connect(ui->actAbout, SIGNAL(triggered()),
			this, SLOT(showAuthorMsg()));

	connect(ui->actPanel, SIGNAL(triggered()),
			this, SLOT(callControlPanel()));
	/* �����ѧ��ʱ, ��ʾѧ��������Ϣ��ӵĴ��� */
	connect(controlPanel, SIGNAL(onAddStuClicked()),
			stu, SLOT(show()));

	/* ѧ��������Ϣ�����ύѧ����Ϣ, ���ݴ��ڲ������� */
	connect(stu, SIGNAL(submitStu(QStringList&)),
			dataWidget, SLOT(insertIntoTable(QStringList&)));

	/* �������� */
	connect(this->addTableRow, SIGNAL(triggered()),
			dataWidget, SLOT(addStuRecord()));
	connect(this->subTableRow, SIGNAL(triggered()),
			dataWidget, SLOT(subStuRecord()));

	/* ���뵼�� */
	connect(this->controlPanel, SIGNAL(importDataToTable()),
			dataWidget, SLOT(importData()));
	connect(this->controlPanel, SIGNAL(exportDataToLocal()),
			dataWidget, SLOT(exportData()));

	/* ������巢�ͱ����ź�, DataWidget������д�����ݿ� */
	connect(this->controlPanel, SIGNAL(readyToSave()),
			dataWidget, SLOT(saveCurrentRecord()));

	/* ��ѯ����Ӧ */
	connect(controlPanel, SIGNAL(sendQueryData(const QStringList&)),
			dataWidget, SLOT(getQueryData(const QStringList&)));

	/* ������֤����Ӧ */
	connect(controlPanel, SIGNAL(sendBatchCertificate(const QStringList&)),
			dataWidget, SLOT(getBatchCerData(const QStringList&)));

	/* ���²������� */
	connect(dataWidget, SIGNAL(sendCollegeSearch(QStringList&)),
			controlPanel, SLOT(setCollegeComboBox(QStringList&)));

	/* ����ָ��ѧԺ�µ�רҵ */
	connect(controlPanel, SIGNAL(sendCollege(QString)),
			dataWidget, SLOT(updateClassQuery(QString)));
	connect(dataWidget, SIGNAL(sendClassSearch(QStringList&)),
			controlPanel, SLOT(setClassComboBox(QStringList&)));

	/* ����Label */
	connect(stu, SIGNAL(sendTextForLabel(QString&)),
			this, SLOT(setLabelStatus(QString&)));
	connect(dataWidget, SIGNAL(sendLabelStatus(QString&)),
			this, SLOT(setLabelStatus(QString&)));
	connect(dataWidget, SIGNAL(setTextForMain(QString&)),
			this, SLOT(setLabelStatus(QString&)));
	connect(controlPanel, SIGNAL(sendTextForLabel(QString&)),
			this, SLOT(setLabelStatus(QString&)));
}

void HutClient::setLabelStatus(QString &str)
{
	label->setText(str);
}

void HutClient::launchStudent()
{
	if (stu == NULL)
	{
		stu = new Student(this);
		stu->show();
	}
	else
	{
		stu->show();
	}
}

void HutClient::showAuthorMsg()
{
	QMessageBox tip;
	QString msg = "Based on Qt 4.7.4 (32bit)\nBuilt on Nov. 15 2011 at 17:10:00\nCopyright @ www.icedy.com.All right reserved.\n";
	msg += "Technical Support:young.cpy@gmail.com";

	tip.setText(tr("About HutClient"));
	tip.setText(msg);
	tip.setStandardButtons(QMessageBox::Close);
	tip.exec();
}

void HutClient::initMainAction()
{
	ui->toolBar->addAction(ui->actImp);
	ui->toolBar->addAction(ui->actExp);
	ui->toolBar->addAction(ui->actPanel);
	ui->toolBar->addAction(ui->actSearch);
	addTableRow = ui->toolBar->addAction(tr("��ӱ���"));
	addTableRow->setShortcut(QKeySequence("CTRL++"));
	addTableRow->setToolTip(tr("CTRL +"));
	subTableRow = ui->toolBar->addAction(tr("ɾ������"));
	subTableRow->setShortcut(QKeySequence(tr("CTRL+-")));
	subTableRow->setToolTip(tr("CTRL -"));
}

void HutClient::initMainWidget()
{
	QStatusBar *bar;
	QStringList linkDbMsg;
	int status;

	/* ��ʼ����½���� */
	login = new LoginDlg(this);
	if ((status = login->exec()) != QDialog::Accepted)
	{
		if (login != NULL)
		{
			delete login;
			login = NULL;
		}
		exit(0);
	}

	/* ��ʼ�����벿�� */
	dataWidget = new DataWidget(this);
	setCentralWidget(dataWidget);

	if (status == QDialog::Accepted)
	{
		linkDbMsg = login->getLinkMsg();
		dataWidget->setLinkDbMsg(linkDbMsg);
		qDebug() << "������!";
		qDebug() << dataWidget->getLinkDbMsg();
	}

	/* ��ʼ��������� */
	dock = new QDockWidget(tr("�������"), this);
	controlPanel = new ControlPanel(dock);
	dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	dock->setWidget(controlPanel);
	addDockWidget(Qt::LeftDockWidgetArea, dock);

	/* ���ڶ������� */
	setWindowIcon(QIcon(":/images/hutIcon.ico"));
	setWindowTitle(tr("��ѧ��������չ����ϵͳ"));

	/* QLabel */
	bar = statusBar();
	label = new QLabel(bar);
	label->setMinimumSize(200, 20);
	label->setFrameShadow(QFrame::Sunken);
	label->setFrameShape(QFrame::WinPanel);
	label->setText(tr("��½�ɹ�!"));
	bar->addWidget(label);

	/* ѧ��������Ϣ���봰�� */
	stu = new Student(this);
	stu->setVisible(false);
}

/* ������������ */
void HutClient::callControlPanel()
{
	if (dock->isHidden())
	{
		dock->show();
	}
	else
	{
		dock->hide();
	}
}

HutClient::~HutClient()
{
	qDebug() << "main widget was released!";
    delete ui;
	if (label != NULL)
	{
		delete label;
		label = NULL;
	}

	if (controlPanel != NULL)
	{
		delete controlPanel;
		controlPanel = NULL;
	}

	if(dataWidget != NULL)
	{
		delete dataWidget;
		dataWidget = NULL;
	}

	if (dock != NULL)
	{
		delete dock;
		dock = NULL;
	}

	if (stu != NULL)
	{
		delete stu;
		stu = NULL;
	}

	if (login != NULL)
	{
		delete login;
		login = NULL;
	}
}
