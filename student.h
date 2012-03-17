#ifndef STUDENT_H
#define STUDENT_H

#include <QDialog>
#include <QWidget>
#include <QSizePolicy>
#include <QMessageBox>
#include <QRegExp>
#include <QDebug>
#include <QIcon>
#include <QKeyEvent>
#define STU_MARK_CNT 10

namespace Ui {
    class Student;
}

class Student : public QDialog
{
    Q_OBJECT

public:
	explicit Student(QWidget *parent = 0);
    ~Student();
	void initMainWidget();
	void initDefaultData();
	void initConnection();
	void insertData();
	void clearData();

protected:
	virtual void keyPressEvent(QKeyEvent *event);

signals:
	void submitStu(QStringList &strList);
	void sendTextForLabel(QString &str);

private slots:
	void getStuInfo();

private:
    Ui::Student *ui;
	QString certificateNum, stuId, name, sex, ethnic, birthDate, birthPlace;
	QString politiStu, school, college, major, entryDate, graduateType, remark;
	int mark[STU_MARK_CNT];

};

#endif // STUDENT_H
