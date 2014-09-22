#include "QVideoProgressWindow.h"


QVideoProgressWindow::QVideoProgressWindow(void)
{
}

QVideoProgressWindow::QVideoProgressWindow(int numberOfInputSuspect, int numberOfVideos, QWidget *parent, Qt::WindowFlags flags )
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);
	ui.prb_faceRecognition->setValue(0);
	ui.prb_suspectFaceAlignment->setValue(0);
	ui.prb_videoFaceDetection->setValue(0);

	this->numberOfInputSuspect = numberOfInputSuspect;
	this->numberOfVideos = numberOfVideos;

	this->progressForVidoes = new int[this->numberOfVideos];
	for ( int i = 0; i < this->numberOfVideos; i++){
		this->progressForVidoes[i] = 0;
	}

	ui.prb_suspectFaceAlignment->setMaximum(this->numberOfInputSuspect);

	ui.prb_videoFaceDetection->setMaximum(this->numberOfVideos * 100);
}


QVideoProgressWindow::~QVideoProgressWindow(void)
{
	delete [] this->progressForVidoes;
}
//
//void QVideoProgressWindow::setTotalNumberOfFaceInVideos(QList<int> numberOfFaceInVideos){
//	this->numberOfFaceInVideos = numberOfFaceInVideos;
//	int sumOfFace = 0;
//	for (int i = 0; i < numberOfFaceInVideos.size(); i++){
//		sumOfFace += numberOfFaceInVideos.at(i);
//	}
//	ui.prb_videoFaceAlignment->setMaximum(sumOfFace);
//
//	int faceRecognitionMax = this->numberOfInputSuspect * sumOfFace;
//	ui.prb_faceRecognition->setMaximum(faceRecognitionMax);
//}
//
//void QVideoProgressWindow::setSuspectFaceAlignmentProgess(int suspectNumber){
//	ui.prb_suspectFaceAlignment->setValue(suspectNumber);
//}
//
////also update face alignment in video
//void QVideoProgressWindow::setVideoFaceDetectionProgess(int videoID, int progress){
//	this->progressForVidoes[videoID] = progress;
//
//	int sumOfProgress = 0;
//	for ( int i = 0; i < this->numberOfVideos; i++){
//		sumOfProgress += this->progressForVidoes[i];
//	}
//
//	ui.prb_videoFaceDetection->setValue(sumOfProgress);
//
//}
//
//
//void QVideoProgressWindow::setFaceRecognitionProgess(int resultNumber){
//	ui.prb_faceRecognition->setValue(resultNumber);
//}
void QVideoProgressWindow::updateProgress(int type, int current, int max){
	if (type == 0){
		ui.prb_videoFaceDetection->setMaximum(max);
		ui.prb_videoFaceDetection->setValue(current);
	} else if (type == 1){
		ui.prb_faceRecognition->setMaximum(max);
		ui.prb_faceRecognition->setValue(current);
	}
}


void QVideoProgressWindow::allDone(){
	this->close();
	
}
