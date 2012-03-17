#include <QtGui/QApplication>
#include <QTextCodec>
#include "hutclient.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
	Q_INIT_RESOURCE(Image);
	a.setWindowIcon(QIcon(":/resourse/hutIcon.ico"));
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GBK"));
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GBK"));
	QTextCodec::setCodecForTr(QTextCodec::codecForName("GBK"));
	HutClient client;
	client.show();

    return app.exec();
}
