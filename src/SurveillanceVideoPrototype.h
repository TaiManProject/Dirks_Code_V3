#pragma once

#include <QMainWindow>
#include <QTableView>
#include <QFileDialog>

#include "ui_QVideoMainWindow.h"

#include "commonTool.h"

#include "QInputVideoTableModel.h"
#include "QInputSuspectTableModel.h"
#include "QVideoProgressWindow.h"
#include "QCameraWidget.h"

#include "VideoDataTableModel.h"
#include "VideoDataProxyModel.h"
#include "VideoProcessingThread.h"

#include "GeneralManager.h"

const QString FACE_SYS_TRAINING_DONE = "Face synthesize tree build!";
const QString FACE_SYS_TRAINING_ONGOING = "Building face synthesize tree ...";
const double FACE_RECOG_RESULT_THRESHOLD = 0;

class SurveillanceVideoPrototype : public QMainWindow {
	Q_OBJECT
public:
    SurveillanceVideoPrototype(void);
    SurveillanceVideoPrototype(Parameter parameter_p, QWidget *parent = 0, Qt::WindowFlags flags = 0);

public slots:
	void getCapturedImage();
	void updateUI();
    void processVideosDoneSlot(int videoID);
    void processVideosUpdateProgessSlot(int videoID, int progress);
    void faceRecognitionResultSelectionChangedSlot(const QItemSelection& selected, const QItemSelection& deselected);

private:

    //data for left column:
    //for each suspect a sketch
    std::vector<cv::Mat> inputSuspectSketchList;
    //for each suspect a photo
    std::vector<cv::Mat> inputSuspecPhotoList;
    //for each suspect a list of landmakrs (extracted from sketch of photo)
    std::vector< std::vector< cv::Point2d > > inputSuspectLandmarkPointsList;

    int uVideoID; //to create a unique video ID
    std::vector<QString> videoFileNames;

    std::vector<int> middleColumnVideoIDs;
    //to display the current progress for each video
    std::vector<int> progressList;
    //to display the total number of frames for a video
    std::vector<int> totalFramesList;

    bool useRaw;
    VideoData* data;
    VideoDataTableModel* faceRecognitionTableModel; //todo: rename
    VideoDataProxyModel* faceRecognitionProxyModel; //todo: rename
    std::vector<VideoProcessingThread*> processVideoThreads;

    //face alignment cannot be run in parallel :(
    QMutex faceAlignMutex;
    //opencv can not be called in parallel :(
    QMutex openCVMutex;

    FaceMatchingManager faceMatchingManager;

	//UI
	Ui::VideoMainWindowClass ui;
	QLabel* faceSysTrainingDoenLabel;

	QInputVideoTableModel* filesListTableListModel;
	QInputSuspectTableModel* inputSuspectTableModel;

	QCameraWidget* cameraWidget;

    //TODO: remove it
    GeneralManager* gm;

    void playVideo(QString videoPath, int startFrame, int endFrame, int videoID, int clusterID);

    void trainFaceMatching(std::vector<cv::Mat> additionalImages, QMutex& openCVMutex);

    bool addSuspectEnabled;
    bool addVideoEnabled;

private slots:

    void on_addSnapshotButton_clicked();
    void on_addPhotoButton_clicked();
    void on_addSketchButton_clicked();
    void on_addVideoButton_clicked();
	void on_tlv_inputVideo_doubleClicked(const QModelIndex & index);
    void on_tlv_inputSuspect_doubleClicked(const QModelIndex & index);
	void on_tlv_faceRecognitionResult_doubleClicked(const QModelIndex & index);

};


