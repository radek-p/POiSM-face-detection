#include "director.h"

#include "mainwindow.h"

#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <QFile>

using namespace std;
using namespace cv;


Director::Director(QObject *parent) :
	QObject(parent)
{
	setObjectName("director");
}

Director::~Director()
{

}

void Director::onOpenFile(QString sFileName)
{
	std::cout << "onOpenFile()" << std::endl;
	QFile file(sFileName);
	if (!file.open(QIODevice::ReadOnly))
	{
		cerr << "File could not be opened." << std::endl;
		return;
	}

	QByteArray tmp = file.readAll();
	std::vector<char> data(tmp.begin(), tmp.end());

	m_pImage = std::shared_ptr<cv::Mat>(new cv::Mat(imdecode(data, 1)));

	emit imageUpdated();
}

void Director::onClose()
{
	m_pMainWindow.reset();
}

void Director::showMainWindow()
{
	if (!m_pMainWindow)
	{
		m_pMainWindow = shared_ptr<MainWindow>(new MainWindow(weak_from_this()));
		QObject::connect(m_pMainWindow.get(), SIGNAL(fileOpened(QString)), this, SLOT(onOpenFile(QString)));
		QObject::connect(m_pMainWindow.get(), SIGNAL(closeRequested()), this, SLOT(onClose()));
		m_pMainWindow->setWindowTitle(tr("Rozpoznawanie twarzy"));
	}

	m_pMainWindow->show();
}

std::shared_ptr<const Mat> Director::image()
{
	return m_pImage;
}

std::weak_ptr<Director> Director::weak_from_this()
{
	return std::weak_ptr<Director>(shared_from_this());
}

