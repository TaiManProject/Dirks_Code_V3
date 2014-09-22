#include "QImageGraphicsView.h"

using namespace std;

QImageGraphicsView::QImageGraphicsView(QWidget *parent)
	: QGraphicsView(parent)
{
	QObject::connect(
		this->horizontalScrollBar(), SIGNAL(valueChanged(int)), 
		this, SLOT(horizontalScrollBarPosChangedSlot(int)));
	QObject::connect(
		this->verticalScrollBar(), SIGNAL(valueChanged(int)),
		this, SLOT(verticalScrollBarPosChangedSlot(int)));
}

QImageGraphicsView::~QImageGraphicsView()
{

}

void QImageGraphicsView::scaleView(qreal scaleFactor)
{
    qreal factor = matrix().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 1 || factor > 100)
        return;

    scale(scaleFactor, scaleFactor);
    emit scaleChanged(scaleFactor);
}

void QImageGraphicsView::wheelEvent(QWheelEvent *event)
{

    scaleView(pow((double)2, event->delta() / 240.0));
}

void QImageGraphicsView::setScale(qreal value){
    scale(value, value);
}

void QImageGraphicsView::mouseReleaseEvent(QMouseEvent *event){
    QPointF pointAfterScale(mapToScene(event->pos()));

    emit mouseClicked(pointAfterScale);
}

void QImageGraphicsView::horizontalScrollBarPosChangedSlot(int pos){
	emit horizontalScrollBarPosChangedSignal(pos);
}
void QImageGraphicsView::verticalScrollBarPosChangedSlot(int pos){
	emit verticalScrollBarPosChangedSignal(pos);
}

void QImageGraphicsView::setHorizontalScrollBarPos(int pos){
	this->horizontalScrollBar()->setValue(pos);
}

void QImageGraphicsView::setVerticalScrollBarPos(int pos){
	this->verticalScrollBar()->setValue(pos);
}