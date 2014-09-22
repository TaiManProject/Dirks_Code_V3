#ifndef WINDOWCANDIDATES_H
#define WINDOWCANDIDATES_H


#include "CandidatePatchSet.h"
#include "ui_QCandidatesWindow.h"


#include <QWidget>
#include <QRect>

#include <opencv/cv.h>
#include <opencv/highgui.h>

class QCandidatesWindow : public QWidget
{
	Q_OBJECT

public slots:
	void mouseOverOnPatch(int type, patchIdentifier identifier);

signals:
	void changeImage(int type, int photoID, QRect patchPos);

public:
	void setImagePath(QString imagePath);
	void setImagePatchPos(QRect patchPosRect);
	void setImage(cv::Mat image);
	QCandidatesWindow(CandidatePatchSet* candidatePatches, cv::Mat inputPatch,  QWidget *parent = 0);
	~QCandidatesWindow();

private:

	QGraphicsScene *sourceOriginalScene;
	QGraphicsScene *sourceCandidatePatchResultScene;
	QGraphicsScene *targetCandidatePatchResultScene;
	QGraphicsScene *candidatePatchOrgImageScene;
	QGraphicsRectItem *imagePatchRect;
	QGraphicsRectItem *imageSearchWinRect;

	QRect searchWin;

	Ui::windowcandidates ui;
};

#endif // WINDOWCANDIDATES_H
