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
	/* 工具栏的Action响应 */
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
	/* 添加新学生时, 显示学生个人信息添加的窗口 */
	connect(controlPanel, SIGNAL(onAddStuClicked()),
			stu, SLOT(show()));

	/* 学生个人信息窗口提交学生信息, 数据窗口部件接收 */
	connect(stu, SIGNAL(submitStu(QStringList&)),
			dataWidget, SLOT(insertIntoTable(QStringList&)));

	/* 表格的增减 */
	connect(this->addTableRow, SIGNAL(triggered()),
			dataWidget, SLOT(addStuRecord()));
	connect(this->subTableRow, SIGNAL(triggered()),
			dataWidget, SLOT(subStuRecord()));

	/* 导入导出 */
	connect(this->controlPanel, SIGNAL(importDataToTable()),
			dataWidget, SLOT(importData()));
	connect(this->controlPanel, SIGNAL(exportDataToLocal()),
			dataWidget, SLOT(exportData()));

	/* 控制面板发送保存信号, DataWidget把数据写入数据库 */
	connect(this->controlPanel, SIGNAL(readyToSave()),
			dataWidget, SLOT(saveCurrentRecord()));

	/* 查询与响应 */
	connect(controlPanel, SIGNAL(sendQueryData(const QStringList&)),
			dataWidget, SLOT(getQueryData(const QStringList&)));

	/* 批量认证与响应 */
	connect(controlPanel, SIGNAL(sendBatchCertificate(const QStringList&)),
			dataWidget, SLOT(getBatchCerData(const QStringList&)));

	/* 更新查找条件 */
	connect(dataWidget, SIGNAL(sendCollegeSearch(QStringList&)),
			controlPanel, SLOT(setCollegeComboBox(QStringList&)));

	/* 更新指定学院下的专业 */
	connect(controlPanel, SIGNAL(sendCollege(QString)),
			dataWidget, SLOT(updateClassQuery(QString)));
	connect(dataWidget, SIGNAL(sendClassSearch(QStringList&)),
			controlPanel, SLOT(setClassComboBox(QStringList&)));

	/* 更新Label */
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
	addTableRow = ui->toolBar->addAction(tr("添加表项"));
	addTableRow->setShortcut(QKeySequence("CTRL++"));
	addTableRow->setToolTip(tr("CTRL +"));
	subTableRow = ui->toolBar->addAction(tr("删除表项"));
	subTableRow->setShortcut(QKeySequence(tr("CTRL+-")));
	subTableRow->setToolTip(tr("CTRL -"));
}

void HutClient::initMainWidget()
{
	QStatusBar *bar;
	QStringList linkDbMsg;
	int status;

	/* 初始化登陆界面 */
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

	/* 初始化中央部件 */
	dataWidget = new DataWidget(this);
	setCentralWidget(dataWidget);

	if (status == QDialog::Accepted)
	{
		linkDbMsg = login->getLinkMsg();
		dataWidget->setLinkDbMsg(linkDbMsg);
		qDebug() << "已设置!";
		qDebug() << dataWidget->getLinkDbMsg();
	}

	/* 初始化控制面板 */
	dock = new QDockWidget(tr("控制面板"), this);
	controlPanel = new ControlPanel(dock);
	dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	dock->setWidget(controlPanel);
	addDockWidget(Qt::LeftDockWidgetArea, dock);

	/* 窗口顶部属性 */
	setWindowIcon(QIcon(":/images/hutIcon.ico"));
	setWindowTitle(tr("大学生素质拓展管理系统"));

	/* QLabel */
	bar = statusBar();
	label = new QLabel(bar);
	label->setMinimumSize(200, 20);
	label->setFrameShadow(QFrame::Sunken);
	label->setFrameShape(QFrame::WinPanel);
	label->setText(tr("登陆成功!"));
	bar->addWidget(label);

	/* 学生个人信息输入窗口 */
	stu = new Student(this);
	stu->setVisible(false);
}

/* 控制面板的显隐 */
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
