#include "mainwindow.h"
#include "director.h"

#include <QApplication>
#include <QFile>
#include <iostream>
#include <memory>

using namespace std;


int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	app.setApplicationName("Face recognition");
	shared_ptr<Director> pDirector = shared_ptr<Director>(new Director());
	QFile file(":/styles/styles.qss");
	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		app.setStyleSheet(file.readAll());
		file.close();
	}
	pDirector->showMainWindow();
	return app.exec();
}
