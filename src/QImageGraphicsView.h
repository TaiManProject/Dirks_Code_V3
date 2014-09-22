#ifndef IMAGEGRAPHICSVIEW_H
#define IMAGEGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QWheelEvent>
#include <QWidget>
#include <QEvent>
#include <QScrollBar>
#include <QMessageBox>
#include <math.h>
#include <iostream>

class QImageGraphicsView : public QGraphicsView
{
	Q_OBJECT

public:
	QImageGraphicsView(QWidget *parent);
	~QImageGraphicsView();

protected:
    void wheelEvent(QWheelEvent *event);
    void scaleView(qreal scaleFactor);
    void mouseReleaseEvent(QMouseEvent *event);

public slots:
    void setScale(qreal value);
	void setHorizontalScrollBarPos(int pos);
	void setVerticalScrollBarPos(int pos);
	void horizontalScrollBarPosChangedSlot(int pos);
	void verticalScrollBarPosChangedSlot(int pos);

signals:
    void scaleChanged(qreal newValue);
    void mouseClicked(QPointF scenePoint);
	void horizontalScrollBarPosChangedSignal(int pos);
	void verticalScrollBarPosChangedSignal(int pos);
	
	
};

#endif // IMAGEGRAPHICSVIEW_H
