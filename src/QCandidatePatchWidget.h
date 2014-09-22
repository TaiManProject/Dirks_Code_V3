#ifndef WIDGETCANDIDATEPATCH_H
#define WIDGETCANDIDATEPATCH_H

#include <QWidget>
#include "ui_QCandidatePatchWidget.h"
#include "commonTool.h"

class QCandidatePatchWidget : public QWidget
{
	Q_OBJECT

public slots:
	void mouseOver(int type);

signals:
	void mouseOverSignals(int type, patchIdentifier identifier);

public:
	QCandidatePatchWidget(QWidget *parent = 0);
	~QCandidatePatchWidget();

	void setCandidatePatch(QImage sourcePatchImage, QImage tagertPatchImage, double weight, patchIdentifier identifier, int id);
	bool isGoodCan();
	bool isRandomCan();
	bool isAddCan();

    QGraphicsScene *sourceImageScene;
    QGraphicsScene *targetImageScene;

private:
	int patchID;
	patchIdentifier identifier;
	Ui::wig_CandidatePatch ui;


};

#endif // WIDGETCANDIDATEPATCH_H
