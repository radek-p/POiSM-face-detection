#pragma once
#include "declarations.h"

#include <QMainWindow>
#include <memory>
#include <opencv2/core/core.hpp>

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	explicit MainWindow(std::weak_ptr<Director> parent);
	~MainWindow();

signals:
	void fileOpened(QString sFileName);
	void closeRequested();

public slots:
	void showOpenDialog();

private slots:
	void on_actionOpenImage_triggered();
	void on_actionExit_triggered();

private:
	std::weak_ptr<Director> m_pDirector;
	Ui::MainWindow *ui;
};
