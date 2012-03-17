#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

#include <QWidget>
#include <QKeyEvent>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include "datawidget.h"

namespace Ui {
    class ControlPanel;
}

class ControlPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ControlPanel(QWidget *parent = 0);
    ~ControlPanel();
	void initConnection();

protected:
	virtual void keyPressEvent(QKeyEvent *event);

signals:
	void onAddStuClicked();
	void importDataToTable();
	void exportDataToLocal();
	void readyToSave();
	void sendQueryData(const QStringList &tmpList);
	void sendBatchCertificate(const QStringList &tmpList);
	void sendCollege(QString college);
	void sendTextForLabel(QString &str);

private slots:
	void launchAddStu();
	void sendImportSignal();
	void sendExportSignal();
	void launchReady();
	void launchQuery();
	void launchBatchCertificate();
	void setCollegeComboBox(QStringList &strList);
	void setClassComboBox(QStringList &strList);
	void launchSendCollege(QString college);
	void setCollege(QString str);
	void setMajor(QString str);
	void setOrderRu(QString str);
	void setStuName(QString str);
	void setStuId(QString str);

private:
    Ui::ControlPanel *ui;
	QString college, major, orderRu, stuName, stuId;
};

#endif // CONTROLPANEL_H
