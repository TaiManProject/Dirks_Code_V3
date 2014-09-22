#ifndef BORDERMAINWINDOW_H
#define BORDERMAINWINDOW_H

#include <iostream>
#include <string>

#include <QMainWindow>
#include <QFileDialog>
#include <QDir>
#include <QGraphicsPixmapItem>
#include <QStringList>
#include <QThread>
#include <QTimer>
#include <QKeyEvent>

#include <opencv/cv.h>

#include "ui_QBorderMainWindow.h"
#include "ClickableQLabel.h"
#include "FaceMatchingManager.h"
#include "FaceSynthesisManager.h"

typedef struct {
  int i;
  double d;
  double confidence;
} IntDouble;

struct intDouble_less {
    bool operator ()(IntDouble const& a, IntDouble const& b) const {
        if (a.d <= b.d) return true;
        if (a.d > b.d) return false;
        return false;
    }
};

class QBorderMainWindow : public QMainWindow {
	Q_OBJECT

public:
    std::vector<cv::Mat> wantedPersons;
    std::vector<cv::Mat> wantedPersonsSynthesized;
    std::vector<QString> wantedPersonFileNames;
    std::vector< std::vector< cv::Point2d > > wantedPersonsLandmarkPointsList;

    int matchedIndex;
    int partialMatchedIndex;

    cv::Mat inputPerson;
    QString inputPersonFileName;
    std::vector< cv::Point2d > inputPersonLandmarkPoints;

    std::vector<IntDouble> faceVerificationResult;

    std::vector<IntDouble> partialFaceVerificationResult;
    std::vector<IntDouble> sortedPartialFaceVerificationResult;

    std::vector<IntDouble> sortedFaceVerificationResult;
    std::vector<std::vector<double> > faceVerificationResultRaw;

    bool currentlyCapturing;

    Parameter parameter;

    QBorderMainWindow(Parameter parameter_p, QWidget *parent = 0, Qt::WindowFlags flags = 0);
    void keyPressEvent(QKeyEvent* e); //to capture an image from the camera
    //face alignment cannot be run in parallel :(
    QMutex faceAlignMutex;
    //opencv can not be called in parallel :(
    QMutex openCVMutex;

    FaceMatchingManager faceMatchingManager;
    FaceSynthesisManager faceSynthesisManager;

private slots:
    void updateCamera();
    void onClickedWantedPerson(ClickableQLabel* label);
    void onClickedInputPerson(ClickableQLabel* label);
    void onClickedMatchedPerson(ClickableQLabel* label);
    void onClickedCamera(ClickableQLabel* label);
    void onClickedInputPersonLabel(ClickableQLabel* label);
    void trainFaceSynthesizeCompleted();
    void faceSynthesizeCompleted();

private:
	Ui::BorderMainWindowClass ui;
    void trainFaceVerification(std::vector<cv::Mat> additionalImages, QMutex& openCVMutex);
    void updateUI();
    void loadWantedPersons();
    QStringList getWantedPersonQStringListFromFile(QString filePath);
	void startCamera();
    void stopCamera();
    cv::VideoCapture cameraVideoCapture;
    QTimer* cameraTimer;
    void performFaceVerification();
    void trainFaceSynthesize();
};

#endif // BORDERMAINWINDOW_H
