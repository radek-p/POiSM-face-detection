#pragma once

#include "declarations.h"
#include "facedetector.h"

#include <QGraphicsView>
#include <memory>
#include <QGraphicsPixmapItem>


class ImagePreviewWidget : public QGraphicsView
{
	Q_OBJECT
public:
	ImagePreviewWidget(QWidget *parent = nullptr);

public:
	void setDirector(std::weak_ptr<Director> director);

public slots:
	void onImageUpdated();

protected:
	void fitInView(const QRectF &p_oRect, Qt::AspectRatioMode p_oAspectRatioMode);
	void resizeEvent(QResizeEvent *p_pEvent);

private:
	std::weak_ptr<Director> m_pDirector;
	QGraphicsPixmapItem *m_pPixmap;

	void resetScene();
	void drawFaces(std::shared_ptr<const FaceDetector::Result> p_pResult);
};
