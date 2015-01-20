#include "director.h"

#include "mainwindow.h"
#include "facedetector.h"
#include "facedetectorconfig.h"

#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <QFile>

using namespace std;
using namespace cv;


Director::Director(QObject *parent) :
	QObject(parent),
	m_pDetector(new FaceDetector(std::unique_ptr<FaceDetectorConfig>(new FaceDetectorConfig())))
{
	setObjectName("director");

	// TODO Remove
	onOpenFile(QString("/home/radek/Obrazy/photobook/100_2840.JPG"));
}

Director::~Director()
{

}

void Director::onOpenFile(QString sFileName)
{
	std::cout << "onOpenFile(\"" << sFileName.toStdString() << "\")" << std::endl;
	QFile file(sFileName);
	if (!file.open(QIODevice::ReadOnly))
	{
		cerr << "File could not be opened." << std::endl;
		return;
	}

	QByteArray tmp = file.readAll();
	std::vector<char> data(tmp.begin(), tmp.end());

	m_pOriginalImage = m_pImage = std::shared_ptr<cv::Mat>(new cv::Mat(imdecode(data, 1)));
	m_pDetectionResult.reset();

	emit imageUpdated();
}

void Director::onClose()
{
	m_pMainWindow.reset();
}

void Director::onDetect()
{
	detect();
}

void Director::onTypeChangeRequested(FaceDetector::ImageType p_eType)
{
	typedef FaceDetector::ImageType T;


	if (!m_pDetectionResult)
		p_eType = T::ORIGINAL_IMAGE;
	else if (m_pDetectionResult->m_vPartialResults.find(p_eType) == m_pDetectionResult->m_vPartialResults.end())
		p_eType = T::ORIGINAL_IMAGE;

	std::cout << "onTypeChangeRequested();" << std::endl;
	if (p_eType == T::ORIGINAL_IMAGE)
	{
		m_pImage = m_pOriginalImage;
		std::cout << "ORIGINAL IMAGE" << std::endl;
	}
	else
	{
		m_pImage = m_pDetectionResult->m_vPartialResults[p_eType];
		std::cout << "OTHER IMAGE" << std::endl;
	}

	emit imageUpdated();
}

void Director::showMainWindow()
{
	if (!m_pMainWindow)
	{
		m_pMainWindow = shared_ptr<MainWindow>(new MainWindow(weak_from_this()));
		QObject::connect(m_pMainWindow.get(), SIGNAL(fileOpened(QString)), this, SLOT(onOpenFile(QString)));
		QObject::connect(m_pMainWindow.get(), SIGNAL(closeRequested()), this, SLOT(onClose()));
		QObject::connect(m_pMainWindow.get(), SIGNAL(detectRequested()), this, SLOT(onDetect()));
		QObject::connect(m_pMainWindow.get(), SIGNAL(typeChangeRequested(FaceDetector::ImageType)), this, SLOT(onTypeChangeRequested(FaceDetector::ImageType)));
		m_pMainWindow->setWindowTitle(tr("Rozpoznawanie twarzy"));
	}

	m_pMainWindow->show();
}

std::shared_ptr<const Mat> Director::image()
{
	return m_pImage;
}

std::shared_ptr<const FaceDetector::Result> Director::getDetectionResult() const
{
	return m_pDetectionResult;
}

std::weak_ptr<Director> Director::weak_from_this()
{
	return std::weak_ptr<Director>(shared_from_this());
}

void Director::detect()
{
	if (!m_pDetector)
		return;

	m_pDetectionResult = m_pDetector->detect(m_pImage);

	emit imageUpdated();
}

