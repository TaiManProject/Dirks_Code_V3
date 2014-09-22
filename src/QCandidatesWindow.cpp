#include "QCandidatesWindow.h"
#include "commonTool.h"

const bool SAVE_TO_FILE = false;
const std::string SAVE_DIR = "E:/ITFFaceMatchingProject/Result/Patch/";

QImage Mat2QImage_WC(const cv::Mat_<double> &src) {
	//double scale = 255.0;
	double scale = 1.0;
	QImage dest(src.cols, src.rows, QImage::Format_ARGB32);

	for (int y = 0; y < src.rows; ++y) {
		const double *srcrow = src[y];
		QRgb *destrow = (QRgb*)dest.scanLine(y);
		for (int x = 0; x < src.cols; ++x) {
			unsigned int color = srcrow[x] * scale;
			destrow[x] = qRgba(color, color, color, 255);
		}
	}
	
	return dest;
}


/*
windowcandidates::windowcandidates(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}
*/

QCandidatesWindow::QCandidatesWindow(CandidatePatchSet* candidatePatches, cv::Mat inputPatch, QWidget *parent)
    : QWidget(parent) {
	ui.setupUi(this);
    std::string timestamp = getTimeStamp().toStdString();


	for ( int i = 0; i < candidatePatches->size(); i++){
		QCandidatePatchWidget* candidatePatchWidget= candidatePatches->getCandidatePatchWidget(i);
		ui.vlo_candidatePatch->addWidget(candidatePatchWidget);

		if (SAVE_TO_FILE){
			bool done1 = cv::imwrite( SAVE_DIR + timestamp + "_" + int2str(i) + "_source.jpg", candidatePatches->getSource(i));
			bool done2 = cv::imwrite( SAVE_DIR + timestamp + "_" + int2str(i) + "_target.jpg", candidatePatches->getTarget(i));
		}

		connect(candidatePatchWidget , SIGNAL(mouseOverSignals(int, patchIdentifier)), this, SLOT(mouseOverOnPatch(int, patchIdentifier)));
	}

	searchWin = candidatePatches->getSearchWin();

	sourceOriginalScene = new QGraphicsScene(0);
    sourceOriginalScene->addPixmap(QPixmap::fromImage(Mat2QImage_WC(inputPatch)));
    ui.grv_sourceOriginal->setScene(sourceOriginalScene);
	ui.grv_sourceOriginal->scale(4.0, 4.0);

	sourceCandidatePatchResultScene = new QGraphicsScene(0);
	sourceCandidatePatchResultScene->addPixmap(QPixmap::fromImage(Mat2QImage_WC(candidatePatches->getCombinedSource())));
	ui.grv_sourceCandidatePatchResult->setScene(sourceCandidatePatchResultScene);
	ui.grv_sourceCandidatePatchResult->scale(4.0, 4.0);

	targetCandidatePatchResultScene = new QGraphicsScene(0);
	targetCandidatePatchResultScene->addPixmap(QPixmap::fromImage(Mat2QImage_WC(candidatePatches->getCombinedTarget())));
	ui.grv_targetCandidatePatchResult->setScene(targetCandidatePatchResultScene);
	ui.grv_targetCandidatePatchResult->scale(4.0, 4.0);

	if (SAVE_TO_FILE){
		cv::imwrite( SAVE_DIR + timestamp + "_" + "sourceOriginal.jpg", inputPatch);
		cv::imwrite( SAVE_DIR + timestamp + "_" + "sourceCandidatePatchResult.jpg", candidatePatches->getCombinedSource());
		cv::imwrite( SAVE_DIR + timestamp + "_" + "targetCandidatePatchResult.jpg", candidatePatches->getCombinedTarget());
	}

	QString rmsString;
	rmsString.setNum(candidatePatches->RMSByPatch(inputPatch, _CANDIDATE_SOURCE_));
	ui.lab_rms->setText(rmsString);

	candidatePatchOrgImageScene = NULL;
	imagePatchRect = NULL;

}

QCandidatesWindow::~QCandidatesWindow()
{
	sourceOriginalScene->clear();
	delete sourceOriginalScene;
}

void QCandidatesWindow::mouseOverOnPatch(int type, patchIdentifier identifier){
	//ui.lab_candidatePatchImagePath->setText(imagePath);
	

	//QString imagePath;
	QString imagePos;
	QString imagePosTopLeftX;
	QString imagePosTopLeftY;

	//imagePath.setNum(identifier.numberOfImage);
	imagePosTopLeftX.setNum(identifier.colRange.start);
	imagePosTopLeftY.setNum(identifier.rowRange.start);
	imagePos = "(" + imagePosTopLeftX + ", " + imagePosTopLeftY + ")";
	ui.lab_candidatePatchPosition->setText(imagePos);

	QRect patchPos(QPoint(identifier.colRange.start, identifier.rowRange.start), QPoint(identifier.colRange.end, identifier.rowRange.end));

	emit changeImage(type, identifier.numberOfImage, patchPos);

}

void QCandidatesWindow::setImagePath(QString imagePath) {
	ui.lab_candidatePatchImagePath->setText(imagePath);
}

void QCandidatesWindow::setImagePatchPos(QRect patchPosRect) {
	if (imagePatchRect != NULL){
		//delete imagePatchRect;
	}
	imagePatchRect = candidatePatchOrgImageScene->addRect(patchPosRect, QPen(Qt::yellow), QBrush(Qt::transparent));
	imageSearchWinRect = candidatePatchOrgImageScene->addRect(searchWin, QPen(Qt::red), QBrush(Qt::transparent));

}

void QCandidatesWindow::setImage(cv::Mat image) {
	if (candidatePatchOrgImageScene != NULL){
		delete candidatePatchOrgImageScene;
	}
	candidatePatchOrgImageScene = new QGraphicsScene(0);
    candidatePatchOrgImageScene->addPixmap(QPixmap::fromImage(Mat2QImage(image)));
	ui.grv_candidatePatchImage->setScene(candidatePatchOrgImageScene);
}
