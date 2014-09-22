#ifndef CLUSTERVIDEOPROCESSINGTHREAD_H
#define CLUSTERVIDEOPROCESSINGTHREAD_H

#include <QThread>
#include "commonTool.h"
#include "GeneralManager.h"
#include "VideoData.h"

class VideoProcessingThread : public QThread {
};

class ClusterVideoProcessingThread : public VideoProcessingThread  {
    Q_OBJECT
public:
    ClusterVideoProcessingThread(QString videoPath,
                       int videoID,
                       QString applicationDataPath,
                       QMutex &faceAlignMutex,
                       QMutex &openCVMutex,
                       ClusterVideoData* data,
                       std::vector<cv::Mat>* inputSuspectPhotoList,
                       std::vector<std::vector<cv::Point2d> >* inputSuspectLandmarkPointsList,
                       FaceMatchingManager* facematchingManager
                       );

    int videoID;
    QString videoPath;
    ClusterVideoData* data;
    std::vector<cv::Mat>* inputSuspectPhotoList;
    std::vector<std::vector<cv::Point2d> >* inputSuspectLandmarkPointsList;
    FaceMatchingManager* faceMatchingManager;
private:
    void run();
    cv::Rect detectFace(cv::Mat& img);
    QString applicationDataPath;
    QMutex* faceAlignMutex;
    QMutex* openCVMutex;
    void performFaceMatchingForFrame(int videoID, int frameID, std::vector<cv::Mat>& faceListForThisFrame, std::vector<std::vector<cv::Point2d> >& landmarkListForThisFrame, std::vector<cv::Point2d>& centersOfLandmarks);

signals:
    void videoPreProcessUpdateProgess(int videoID, int progress);
    void videoPreProcessDone(int videoID);
};

class RawVideoProcessingThread : public VideoProcessingThread {
    Q_OBJECT
public:
    RawVideoProcessingThread(QString videoPath,
                       int videoID,
                       QString applicationDataPath,
                       QMutex &faceAlignMutex,
                       QMutex &openCVMutex,
                       RawVideoData* data,
                       std::vector<cv::Mat>* inputSuspectPhotoList,
                       std::vector<std::vector<cv::Point2d> >* inputSuspectLandmarkPointsList,
                       FaceMatchingManager* facematchingManager
                       );

    int videoID;
    QString videoPath;
    RawVideoData* data;
    std::vector<cv::Mat>* inputSuspectPhotoList;
    std::vector<std::vector<cv::Point2d> >* inputSuspectLandmarkPointsList;
    FaceMatchingManager* faceMatchingManager;
private:
    void run();
    cv::Rect detectFace(cv::Mat& img);
    QString applicationDataPath;
    QMutex* faceAlignMutex;
    QMutex* openCVMutex;
    void performFaceMatchingForFrame(int videoID, int frameID);

signals:
    void videoPreProcessUpdateProgess(int videoID, int progress);
    void videoPreProcessDone(int videoID);
};

#endif // CLUSTERVIDEOPROCESSINGTHREAD_H
