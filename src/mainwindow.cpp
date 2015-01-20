#include "mainwindow.h"
#include "director.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <iostream>
#include <QGraphicsView>
#include <QGraphicsScene>


MainWindow::MainWindow(std::weak_ptr<Director> parent) :
	QMainWindow(),
	m_pDirector(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	ui->preview->setDirector(parent);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::showOpenDialog()
{
	QFileDialog oDialog(this);
	oDialog.setFileMode(QFileDialog::ExistingFile);
	oDialog.setNameFilter(tr("Images (*.png *.bmp *.jpg)"));

	QStringList oFileNames;
	if (oDialog.exec())
	{
		oFileNames = oDialog.selectedFiles();
		std::cout << "Opened!" << oFileNames[0].toStdString() << "\nEmitting signal!" << std::endl;
		emit fileOpened(oFileNames.at(0));
	}
}

void MainWindow::on_actionOpenImage_triggered()
{
	showOpenDialog();
}

void MainWindow::on_actionExit_triggered()
{
	emit closeRequested();
}
