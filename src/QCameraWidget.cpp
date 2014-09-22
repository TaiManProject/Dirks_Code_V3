#include "QCameraWidget.h"
#include "FaceAlignment.h"

QCameraWidget::QCameraWidget( int cameraIndex, QMutex& openCVMutex, QMutex& faceAlignMutex, std::string dataPath) {
    commonTool.log(QString("Opening Camera Window for camera with index %1.").arg(cameraIndex));

    this->openCVMutex = &openCVMutex;
    this->faceAlignMutex = &faceAlignMutex;
    this->dataPath = dataPath;

	cameraScene = NULL;
	cameraView = NULL;
	cameraImagePixmap = NULL;
	layout = NULL;

	camereLabel = new QLabel();
	camereLabel->setAlignment(Qt::AlignCenter);
	camereLabel->setFont(QFont("Arial", 22, QFont::Bold));
	camereLabel->setText(TEX_START_CAPTURE);

	cameraTimer = new QTimer();
	connect(cameraTimer, SIGNAL(timeout()), this, SLOT(cameraTimeEvent()));

	cameraCaptureTimer = new QTimer();
	connect(cameraCaptureTimer, SIGNAL(timeout()), this, SLOT(cameraCaptureCountDownEvent()));
	cameraCaptureCountdown = -1;

    cameraDeviceID = cameraIndex;

	layout = new QGridLayout(this);

	cameraScene = new QGraphicsScene();
	cameraView = new QGraphicsView();
	cameraView->setScene(cameraScene);

	layout->addWidget(cameraView, 0, 0);
	layout->addWidget(camereLabel, 1, 0);

	

	QMessageBox msgBox;
    // initialize camera 
    camera = cv::VideoCapture(cameraDeviceID);

    if(!camera.isOpened()){ // check if we succeeded
		msgBox.setText("camera cannot initialize, please check and try again");
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.setStandardButtons(QMessageBox::Ok);
		msgBox.exec();
		return;
	}
	//get one frame for setting the size of window


    commonTool.log(QString("hahaha1"));

    cv::Mat cameraCvImg;
    camera >> cameraCvImg; // get a new frame from camera
    cv::cvtColor(cameraCvImg, grayCameraCvImg, CV_RGB2GRAY);
    commonTool.log(QString("hahaha2"));
    cv::flip(grayCameraCvImg, grayCameraCvImg, 1);
    cameraView->setMinimumSize(grayCameraCvImg.cols+10, grayCameraCvImg.rows+10);

	updateCameraLabel();
}


QCameraWidget::~QCameraWidget(){

	if (cameraImagePixmap != NULL){
		delete cameraImagePixmap;
	}
	//QGraphicsPixmapItem* cameraImagePixmap;

	if (cameraScene != NULL){
		delete cameraScene;
	}
	//QGraphicsScene* cameraScene;

	if (cameraView != NULL){
		delete cameraView;
	}
	//QGraphicsView* cameraView;


	if (layout != NULL){
		delete layout;
	}
	//QGridLayout* layout;

	if (camereLabel != NULL){
		delete camereLabel;
	}
	//QLabel* camereLabel;

	if (camera.isOpened()){
		camera.release();
	}
	//cv::VideoCapture camera;

	if (cameraTimer != NULL){
		delete cameraTimer;
	}
	//QTimer* cameraTimer;

    //cameraCvImg.release();

	if (cameraCaptureTimer != NULL){
		delete cameraCaptureTimer;
	}
    //QTimer* cameraCaptureTimer;

}


void QCameraWidget::stopCamera(){
	if (cameraTimer->timerId() != -1){
		cameraTimer->stop();
	}

	if (camera.isOpened()){
		camera.release();
	}

	//cameraDeviceID++;
	//startCamera();

    commonTool.log("Closing Camera Widget.");
}

void QCameraWidget::startCamera() {
    cameraTimer->start(50);
}

void QCameraWidget::cameraTimeEvent(){
	updateCameraImage();
    cameraQImg = Mat2QImage(grayCameraCvImg);

	if (cameraImagePixmap != NULL){
		delete cameraImagePixmap;
	}
    cameraImagePixmap = new QGraphicsPixmapItem(QPixmap::fromImage(cameraQImg));

	cameraScene->addItem(cameraImagePixmap);
	cameraView->show();
}

void QCameraWidget::updateCameraImage(){
    cv::Mat cameraCvImg;
    camera >> cameraCvImg; // get a new frame from camera
    cv::cvtColor(cameraCvImg, grayCameraCvImg, CV_RGB2GRAY);
    cv::flip(grayCameraCvImg, grayCameraCvImg, 1);
}

void QCameraWidget::keyPressEvent(QKeyEvent* e){
	if (e->key() == 'c' || e->key() == 'C' ){
		//startedFirstTime = true;
		//startCapture = true;
	}
	cameraCaptureTimer->start(1000);
	cameraCaptureCountdown = CAMERA_CAPTURE_COUNT_DOWN_TIME;
	updateCameraLabel();
}

void QCameraWidget::updateCameraLabel(){
	camereLabel->setText(TEX_START_CAPTURE);
	if (cameraCaptureCountdown != -1){
		camereLabel->setText(QString::number(cameraCaptureCountdown));
	}
}

void QCameraWidget::cameraCaptureCountDownEvent(){
	if (cameraCaptureCountdown > 0){
		cameraCaptureCountdown--;
		updateCameraLabel();
	} else {
		cameraCaptureTimer->stop();



        capturedCvImg = grayCameraCvImg.clone();

        FaceAlignment fa(dataPath, *faceAlignMutex);

        std::vector<std::vector<cv::Point2d> > detectedLandmarks;
        std::vector<cv::Point2d> centers;
        fa.detectLandmarks(capturedCvImg, detectedLandmarks, centers);

        std::vector<cv::Point2d> landmarks;
        if (detectedLandmarks.size() > 0) {
            landmarks = detectedLandmarks.at(0);
        }

        commonTool.showImageAndLandMarks(QString("Captured Snapshot"), capturedCvImg, landmarks, CV_RGB(255, 0, 0), *openCVMutex);


        //fa.alignStasm4(selectedGraySketch, alignedGraySketch, landmarkPointsSketch);

        //TODO visualize the extrected landmarks
        //cv::namedWindow( "Captured Snapshot", CV_WINDOW_NORMAL );
        //cv::imshow( "Captured Snapshot", capturedCvImg );
        int key = cv::waitKey(0);

        commonTool.log(QString("%1").arg(key));

        //todo read key
        if (key==32) { //space
            //keep this image
            stopCamera();
            this->close();
            cv::destroyWindow("Captured Snapshot");
            emit capturedImage();
        } else {
            //don't want this image
            cv::destroyWindow("Captured Snapshot");
        }



	}
}

cv::Mat QCameraWidget::getCurrentCvImage(){
	return capturedCvImg;
}
