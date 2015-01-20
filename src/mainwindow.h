#pragma once
#include "declarations.h"
#include "facedetector.h"

#include <QMainWindow>
#include <QMetaType>
#include <opencv2/core/core.hpp>
#include <memory>
#include <map>

namespace Ui {
class MainWindow;
}

Q_ENUMS(FaceDetector::ImageType)
Q_DECLARE_METATYPE(FaceDetector::ImageType)void on_typeSelect_currentIndexChanged(int index);


class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(std::weak_ptr<Director> parent);
	~MainWindow();

signals:
	void fileOpened(QString sFileName);
	void closeRequested();
	void detectRequested();
	void typeChangeRequested(FaceDetector::ImageType);

public slots:
	void showOpenDialog();

private slots:
	void on_actionOpenImage_triggered();
	void on_actionExit_triggered();
	void on_typeSelect_currentIndexChanged(int index);

	void on_actionDetectFaces_triggered();

private:
	std::weak_ptr<Director> m_pDirector;
	Ui::MainWindow *ui;

	void setupTypeSelect();

	static const std::map<FaceDetector::ImageType, QString> imageTypes;
};
