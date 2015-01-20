#include "imagepreviewwidget.h"
#include "director.h"

#include <QGraphicsScene>

ImagePreviewWidget::ImagePreviewWidget(QWidget *parent) :
	QGraphicsView(parent)
{
	setScene(new QGraphicsScene());
	m_pPixmap = scene()->addPixmap(QPixmap(QSize(0, 0)));
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

ImagePreviewWidget::~ImagePreviewWidget()
{

}

void ImagePreviewWidget::setDirector(std::weak_ptr<Director> director)
{
	m_pDirector = director;
	QObject::connect(m_pDirector.lock().get(), SIGNAL(imageUpdated()), this, SLOT(onImageUpdated()));
}

void ImagePreviewWidget::drawFaceRectangles()
{
	scene()->addRect(10,10,110,110, QPen(), QBrush(Qt::red));
}

void ImagePreviewWidget::onImageUpdated()
{
	auto pDirector = m_pDirector.lock();
	if (!pDirector)
		return;

	auto mImage = pDirector->image();

	const uchar *qImageBuffer = (const uchar*)mImage->data;
	QImage img(qImageBuffer, mImage->cols, mImage->rows, mImage->step, QImage::Format_RGB888);
	QPixmap oPixmap = QPixmap::fromImage(img.rgbSwapped());
	m_pPixmap->setPixmap(oPixmap);

	setSceneRect(QRectF(QRect(QPoint(0, 0), oPixmap.size())));
	fitInView(sceneRect(), Qt::KeepAspectRatio);
}

void ImagePreviewWidget::fitInView(const QRectF &p_oRect, Qt::AspectRatioMode p_oAspectRatioMode)
{
	if (p_oRect.isNull())
		return;

	QRectF oUnity = matrix().mapRect(QRectF(0, 0, 1, 1));

	if (oUnity.isEmpty())
		return;

	scale(1 / oUnity.width(), 1 / oUnity.height());
	QRectF oViewRect = viewport()->rect();

	if (oViewRect.isEmpty())
		return;

	QRectF oSceneRect = matrix().mapRect(p_oRect);

	if (oSceneRect.isEmpty())
		return;

	qreal dXratio = oViewRect.width() / oSceneRect.width();
	qreal dYratio = oViewRect.height() / oSceneRect.height();

	switch (p_oAspectRatioMode)
	{
		case Qt::KeepAspectRatio:
			dXratio = dYratio = qMin(dXratio, dYratio);
			break;
		case Qt::KeepAspectRatioByExpanding:
			dXratio = dYratio = qMax(dXratio, dYratio);
			break;
		case Qt::IgnoreAspectRatio:
			break;
	}

	scale(dXratio, dYratio);
	centerOn(p_oRect.center());
}

void ImagePreviewWidget::resizeEvent(QResizeEvent *p_pEvent)
{
	fitInView(sceneRect(), Qt::KeepAspectRatio);
}

