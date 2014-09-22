#ifndef QCANDIDATEPATCHGRAPHICSVIEW_H
#define QCANDIDATEPATCHGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QPropertyAnimation>

#include "commonTool.h"



class QCandidatePatchGraphicsView : public QGraphicsView
{
	Q_OBJECT

public:
	QCandidatePatchGraphicsView(QWidget *parent);
	~QCandidatePatchGraphicsView();

	bool isSource();
	bool isTarget();

	void setSource();
	void setTarget();

	

private:
	int type; //sourse or target

	void enterEvent(QEvent *e);
    void leaveEvent(QEvent *e);

signals:
	void mouseEnter(int type);
};

#endif // QCANDIDATEPATCHGRAPHICSVIEW_H
