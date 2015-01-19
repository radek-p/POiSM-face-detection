#include "mainwindow.h"
#include "director.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QString>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QComboBox>
#include <iostream>


const std::map<FaceDetector::ImageType, QString> MainWindow::imageTypes {
//	{ FaceDetector::ImageType::SKIN_THRESHOLD, QString("Skin threshold") },
	{ FaceDetector::ImageType::ORIGINAL_IMAGE, QString("Oryginalny obraz") },
	{ FaceDetector::ImageType::SKIN_THRESHOLD, QString("Kolor twarzy") },
	{ FaceDetector::ImageType::HAIR_THRESHOLD, QString("Kolor wlosow") }
};

MainWindow::MainWindow(std::weak_ptr<Director> parent) :
	QMainWindow(),
	m_pDirector(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	setupTypeSelect();
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

void MainWindow::on_actionDetectFaces_triggered()
{
	emit detectRequested();
}

void MainWindow::setupTypeSelect()
{
	for (auto &pair : imageTypes)
	{
		ui->typeSelect->addItem(pair.second, QVariant::fromValue(pair.first));
	}
}

void MainWindow::on_typeSelect_currentIndexChanged(int index)
{
	std::cout << "on_typeSelect_currentIndexChanged(" << index << ")" << std::endl;

	emit typeChangeRequested(ui->typeSelect->itemData(index).value<FaceDetector::ImageType>());
}
