#include "QCandidatePatchWidget.h"

QCandidatePatchWidget::QCandidatePatchWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	sourceImageScene = new QGraphicsScene(0);
    targetImageScene = new QGraphicsScene(0);
}

QCandidatePatchWidget::~QCandidatePatchWidget()
{

}

void QCandidatePatchWidget::setCandidatePatch(QImage sourcePatchImage, QImage tagertPatchImage, double weight, patchIdentifier identifier_p, int id){
	patchID = id;
	identifier = identifier_p;

    QString weightString;
	weight = weight * 100; // change to %
    weightString.setNum(weight, 'f', 1);

	QString numberOfImageString;
	numberOfImageString.setNum(identifier.numberOfImage);

	QString posString;
	QString colDiff, rowDiff;
	QString orgCol, orgRow;
	QString rowIndex;

	rowDiff.setNum(identifier.rowRange.start - identifier.orgRowRange.start);
	colDiff.setNum(identifier.colRange.start - identifier.orgColRange.start);
	orgCol.setNum(identifier.orgColRange.start);
	orgRow.setNum(identifier.orgRowRange.start);

	posString = "Row Diff:" + rowDiff + " \nCol Diff:" + colDiff + "\nRow Index:" + rowIndex;

    sourceImageScene->clear();
    sourceImageScene->addPixmap(QPixmap::fromImage(sourcePatchImage));
    ui.grv_sourceCandidatePatch->setScene(sourceImageScene);
	ui.grv_sourceCandidatePatch->setSource();

    targetImageScene->clear();
    targetImageScene->addPixmap(QPixmap::fromImage(tagertPatchImage));
    ui.grv_targetCandidatePatch->setScene(targetImageScene);
    ui.grv_targetCandidatePatch->setTarget();


    ui.grv_sourceCandidatePatch->scale(4.0, 4.0);
    ui.grv_targetCandidatePatch->scale(4.0, 4.0);	    
	ui.lab_candidatePatchWeight->setText(weightString + "%");
	ui.lab_candidatePatchWeight_2->setText(weightString + "%");

	//for next step
	//ui.lab_fromPos->setText(posString);
	//ui.lab_fromImage->setText(numberOfImageString);
	QString idString;
	idString.setNum(id);
	if (identifier.patchType == _CAN_RANDOM_){
		ui.lab_candidateID->setText("Candidate " + idString + "*: ");
	} else if (identifier.patchType == _CAN_GOOD_){
		ui.lab_candidateID->setText("Candidate " + idString + "#: ");
	} else if (identifier.patchType == _CAN_ADD_){
		ui.lab_candidateID->setText("Candidate " + idString + "+: ");
	}

	connect(ui.grv_sourceCandidatePatch, SIGNAL(mouseEnter(int)), this, SLOT(mouseOver(int)));
	connect(ui.grv_targetCandidatePatch, SIGNAL(mouseEnter(int)), this, SLOT(mouseOver(int)));
	
}

void QCandidatePatchWidget::mouseOver(int type){
	if (!isAddCan()){
		emit mouseOverSignals(type, identifier);
	}
}


bool QCandidatePatchWidget::isGoodCan(){
	if (identifier.patchType == _CAN_GOOD_){
		return true;
	}
	return false;
}

bool QCandidatePatchWidget::isRandomCan(){
	if (identifier.patchType == _CAN_RANDOM_){
		return true;
	}
	return false;
}

bool QCandidatePatchWidget::isAddCan(){
	if (identifier.patchType == _CAN_ADD_){
		return true;
	}
	return false;
}