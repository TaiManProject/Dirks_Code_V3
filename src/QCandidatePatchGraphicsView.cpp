#include "QCandidatePatchGraphicsView.h"

QCandidatePatchGraphicsView::QCandidatePatchGraphicsView(QWidget *parent)
	: QGraphicsView(parent)
{

}

QCandidatePatchGraphicsView::~QCandidatePatchGraphicsView()
{

}

void QCandidatePatchGraphicsView::leaveEvent(QEvent * e)
{
          //hide the popup_menu
	//win->close();
}

void QCandidatePatchGraphicsView::enterEvent(QEvent * e)
{
	emit mouseEnter(type);

}

bool QCandidatePatchGraphicsView::isSource(){
	if (type == _CPGV_SOURCE_){
		return true;
	}
	else {
		return false;
	}
}

bool QCandidatePatchGraphicsView::isTarget(){
	if (type == _CPGV_TARGET_){
		return true;
	}
	else {
		return false;
	}
}

void QCandidatePatchGraphicsView::setSource(){
	type = _CPGV_SOURCE_;
}
void QCandidatePatchGraphicsView::setTarget(){
	type = _CPGV_TARGET_;
}

