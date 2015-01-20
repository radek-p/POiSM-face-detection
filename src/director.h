#pragma once
#include "declarations.h"
#include "facedetector.h"

#include <memory>
#include <QObject>
#include <opencv2/core/core.hpp>


class Director : public QObject, public virtual std::enable_shared_from_this<Director>
{
	Q_OBJECT
public:
	explicit Director(QObject *parent = 0);
	~Director();

	void showMainWindow();
	std::shared_ptr<const cv::Mat> image();
	std::shared_ptr<const FaceDetector::Result> getDetectionResult() const;

signals:
	void imageUpdated();

public slots:
	void onOpenFile(QString sFileName);
	void onClose();
	void onDetect();
	void onTypeChangeRequested(FaceDetector::ImageType p_eType);

private:
	std::weak_ptr<Director> weak_from_this();
	std::shared_ptr<MainWindow> m_pMainWindow;
	std::shared_ptr<cv::Mat> m_pOriginalImage;
	std::shared_ptr<FaceDetector::Result> m_pDetectionResult;
	std::shared_ptr<cv::Mat> m_pImage;
	std::unique_ptr<FaceDetector> m_pDetector;

	void detect();
};
