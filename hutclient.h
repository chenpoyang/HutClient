#ifndef HUTCLIENT_H
#define HUTCLIENT_H

#include <QMainWindow>
#include <QDockWidget>
#include <QLabel>
#include <QStatusBar>
#include "logindlg.h"
#include "controlpanel.h"
#include "datawidget.h"
#include "student.h"

namespace Ui {
    class HutClient;
}

class HutClient : public QMainWindow
{
    Q_OBJECT

public:
    explicit HutClient(QWidget *parent = 0);
    ~HutClient();
	void initMainWidget();
	void initMainAction();
	void initConnection();

public:
	ControlPanel *controlPanel;
	DataWidget *dataWidget;
	QDockWidget *dock;
	Student *stu;
	LoginDlg *login;

private slots:
	void callControlPanel();	/* ������������ */
	void launchStudent();	/* ����ѧ��������Ϣ���� */
	void setLabelStatus(QString &str);
	void showAuthorMsg();

private:
    Ui::HutClient *ui;
	QLabel *label;
	QAction *addTableRow, *subTableRow;
};

#endif // HUTCLIENT_H
