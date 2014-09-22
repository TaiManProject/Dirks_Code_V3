#ifndef QCAMERAWIDGET_H
#define QCAMERAWIDGET_H

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "commonTool.h"

const QString TEX_START_CAPTURE = "Press 'C' to capture";
const int CAMERA_CAPTURE_COUNT_DOWN_TIME = 3;

class QCameraWidget : public QWidget {
	Q_OBJECT

signals:
	void capturedImage();

public:
    QCameraWidget(int camareIndex, QMutex& openCVMutex, QMutex& faceAlignMutex, std::string dataPath);
	~QCameraWidget();

	void stopCamera();
	void startCamera();
	
	void updateCameraImage();
	cv::Mat getCurrentCvImage();

	void keyPressEvent(QKeyEvent* e);

	
private slots:
	void cameraTimeEvent();
	void cameraCaptureCountDownEvent();
private:
	//UI
    QGraphicsScene* cameraScene;
    QGraphicsView* cameraView;
    QGraphicsPixmapItem* cameraImagePixmap;
    QGridLayout* layout;
    QLabel* camereLabel;
	void updateCameraLabel();
	
	//opencv Camera
	cv::VideoCapture camera;
	int cameraDeviceID;

    QTimer* cameraTimer;

	//Camera Image
    cv::Mat grayCameraCvImg;
    QImage cameraQImg;

	//camera capture
    QTimer* cameraCaptureTimer;
	int cameraCaptureCountdown;
	cv::Mat capturedCvImg;

    QMutex* openCVMutex;
    QMutex* faceAlignMutex;
    std::string dataPath;

};

#endif // WIDGETCANDIDATEPATCH_H
