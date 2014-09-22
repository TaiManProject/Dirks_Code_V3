#include "VideoProcessingThread.h"

ClusterVideoProcessingThread::ClusterVideoProcessingThread(QString videoPath,
                                       int videoID,
                                       QString applicationDataPath,
                                       QMutex &faceAlignMutex,
                                       QMutex &openCVMutex,
                                       ClusterVideoData* data,
                                       std::vector<cv::Mat>* inputSuspectPhotoList,
                                       std::vector<std::vector<cv::Point2d> >* inputSuspectLandmarkPointsList,
                                       FaceMatchingManager* faceMatchingManager
                                       ) {
    this->videoPath = videoPath;
    this->videoID = videoID;
    this->applicationDataPath = applicationDataPath;
    this->faceAlignMutex = &faceAlignMutex;
    this->openCVMutex = &openCVMutex;
    this->data = data;
    this->inputSuspectPhotoList = inputSuspectPhotoList;
    this->inputSuspectLandmarkPointsList = inputSuspectLandmarkPointsList;
    this->faceMatchingManager = faceMatchingManager;
}



void ClusterVideoProcessingThread::performFaceMatchingForFrame(int videoID, int frameID, std::vector<cv::Mat>& faceListForThisFrame, std::vector<std::vector<cv::Point2d> >& landmarkListForThisFrame, std::vector<cv::Point2d>& centersOfLandmarks) {

    //do it for each suspect ...
    for (int suspectID = 0;suspectID < (int)inputSuspectPhotoList->size();suspectID++) {

        for (int i=0;i<(int)centersOfLandmarks.size();i++) {
            if (centersOfLandmarks.size() > 0) {
                commonTool.log(QString("(faceID=%1) Face (frameID=%2)").arg(i).arg(frameID));
            } else {
                commonTool.log(QString("(faceID=%1) NoFace (frameID=%2)").arg(i).arg(frameID));
            }
        }

        for (int faceID=0;faceID<(int)centersOfLandmarks.size();faceID++) {
            if (data->getOpenClustersWithinBounds(videoID, suspectID, centersOfLandmarks.at(faceID), 10.0).size()==0) {
                //no open cluster within bounds

                cv::Point2d location = centersOfLandmarks.at(faceID);
                data->addCluster(videoID, suspectID, frameID, location);

                commonTool.log(QString("Start a cluster"));
            }

            //now the faceID has a face within its bounds!
            std::vector<Cluster*> clustersWithinBounds = data->getOpenClustersWithinBounds(videoID, suspectID, centersOfLandmarks.at(faceID), 10.0);

            //int numberOfClustersWithinBounds = clustersWithinBounds.size();

            Cluster* c = clustersWithinBounds.at(0); //Todo: take the one that is the best match!

            //do matching
            //Cluster& c = data->clusters.at(videoID).at(suspectID).back();
            std::vector<cv::Point2d> suspectLandmark = inputSuspectLandmarkPointsList->at(suspectID);
            cv::Mat currentSuspect = this->inputSuspectPhotoList->at(suspectID);
            std::vector<cv::Point2d> videoLandmark = landmarkListForThisFrame.at(faceID);
            double sumOfLandmarkDistances = 10000000;
            if (videoLandmark.size() == suspectLandmark.size() && suspectLandmark.size() > 0) {
                sumOfLandmarkDistances = 0.0;
                for (int currentPoint = 0; currentPoint < (int)suspectLandmark.size(); currentPoint++) {
                    sumOfLandmarkDistances += cv::norm(suspectLandmark.at(currentPoint) - videoLandmark.at(currentPoint)); //hardcode the first face
                }
            }
            c->locationList.push_back(centersOfLandmarks.at(faceID));
            double photoDistance = 10000000;
            double photoSimilarity = 0.0;
            cv::Mat currentVideoFace;
            if (faceListForThisFrame.size()>0)
                currentVideoFace = faceListForThisFrame.at(faceID);
            if (!currentVideoFace.empty()) {
                photoDistance = faceMatchingManager->getDistance(currentSuspect, currentVideoFace);
                photoSimilarity = faceMatchingManager->getNormalizedSimilarity(currentSuspect, currentVideoFace).at(0);
            }
            c->photoDistance.push_back(photoDistance);
            c->photoSimilarity.push_back(photoSimilarity);
            c->sumOfLandmarkDistances.push_back(sumOfLandmarkDistances);
            c->stopFrameID = -2; //means it has been edited.
            //finish the matching

        }

        //close open clusters
        data->closeAllOpenClusters(videoID, suspectID, frameID-1);

        //open marked clusters
        std::vector<Cluster*> markedClusters = data->getMarkedClusters(videoID, suspectID);
        for (int i=0;i<(int)markedClusters.size();i++) {
            Cluster* c = markedClusters.at(i);
            c->stopFrameID = -1;
        }

        /*
        if (centersOfLandmarks.size() > 0 &&//there is a face in this frame
           data->getOpenClusters(videoID, suspectID).size()==0) { //no open cluster

            //create and add a cluster for videoID, suspectID, frameID
            Cluster c;
            c.suspectID = suspectID;
            c.startFrameID = frameID;
            c.stopFrameID = -1;

            data->clusterIDs.push_back(data->clusters.at(videoID).at(suspectID).size());
            data->videoIDs.push_back(videoID);
            data->suspectIDs.push_back(suspectID);
            //must be post the above!
            data->clusters.at(videoID).at(suspectID).push_back(c);

            commonTool.log(QString("Start a cluster"));
        } else if (centersOfLandmarks.size() == 0 //there is no face
                   && data->getOpenClusters(videoID, suspectID).size()>0) { //a cluster is currently open
            commonTool.log(QString("Stop a cluster"));
            data->clusters.at(videoID).at(suspectID).back().stopFrameID = frameID-1;
        }

        if (centersOfLandmarks.size() > 0) { //there is a face, we can assume an open cluster exist at the end.
            Cluster& c = data->clusters.at(videoID).at(suspectID).back();
            std::vector<cv::Point2d> suspectLandmark = inputSuspectLandmarkPointsList->at(suspectID);
            cv::Mat currentSuspect = this->inputSuspectPhotoList->at(suspectID);
            std::vector<cv::Point2d> videoLandmark = landmarkListForThisFrame.at(faceID);

            double sumOfLandmarkDistances = 10000000;
            if (videoLandmark.size() == suspectLandmark.size() && suspectLandmark.size() > 0) {
                sumOfLandmarkDistances = 0.0;
                for (int currentPoint = 0; currentPoint < (int)suspectLandmark.size(); currentPoint++) {
                    sumOfLandmarkDistances += cv::norm(suspectLandmark.at(currentPoint) - videoLandmark.at(currentPoint)); //hardcode the first face
                }
            }

            c.locationList.push_back(centersOfLandmarks.at(faceID));
            double photoDistance = 10000000;
            double photoSimilarity = 0.0;
            cv::Mat currentVideoFace;
            if (faceListForThisFrame.size()>0)
                currentVideoFace = faceListForThisFrame.at(faceID);
            if (!currentVideoFace.empty()) {
                photoDistance = faceMatchingManager->getDistance(currentSuspect, currentVideoFace);
                photoSimilarity = faceMatchingManager->getNormalizedSimilarity(currentSuspect, currentVideoFace);
            }
            c.photoDistance.push_back(photoDistance);
            c.photoSimilarity.push_back(photoSimilarity);
            c.sumOfLandmarkDistances.push_back(sumOfLandmarkDistances);
        }

        */

    }
}


void ClusterVideoProcessingThread::run() {
    openCVMutex->lock();
    cv::VideoCapture video;
    video.open(this->videoPath.toStdString());
    openCVMutex->unlock();

    cv::Mat frame;
    int numberOfFramesInVideo = (int)video.get(CV_CAP_PROP_FRAME_COUNT);

    int frameID = 0; // the current frame number that is processed
    FaceAlignment fa(applicationDataPath.toStdString(), *faceAlignMutex);
    int maxFacesPerFrame = 0;

    //data->videoIDs.push_back(videoID);
    data->clusters.push_back(std::vector< std::vector<Cluster*> >());

    for (int i=0;i<(int)inputSuspectPhotoList->size();i++) {
         //add a new cluster list for each suspect
        data->clusters.at(videoID).push_back(std::vector<Cluster*>());
    }

    openCVMutex->lock();
    while (video.read(frame)) {
        //commonTool.log(QString("frameID --> %1").arg(frameID));
        openCVMutex->unlock();
        std::vector<cv::Mat> faceListForThisFrame;
        std::vector<std::vector<cv::Point2d> > landmarkListForThisFrame;

        cv::Mat grayFrame;
        cv::cvtColor(frame, grayFrame, CV_RGB2GRAY);

        emit videoPreProcessUpdateProgess(this->videoID, frameID);

        //show just the input frame
        openCVMutex->lock();
        cv::imshow(QString("video %1").arg(videoID).toStdString(), grayFrame );
        cv::moveWindow(QString("video %1").arg(videoID).toStdString(), 0, 0);
        openCVMutex->unlock();

        std::vector<cv::Point2d> centersOfLandmarks;
        fa.alignStasm4(grayFrame, faceListForThisFrame, landmarkListForThisFrame, centersOfLandmarks);
        //
        //commonTool.log(QString("centersOfLandmarks.size() --> %1").arg(centersOfLandmarks.size()));
        //commonTool.log(QString("landmarkListForThisFrame.size() --> %1").arg(landmarkListForThisFrame.size()));
        //
        int y = 0;
        //visualize the detected faces+landmark
        for (int i=0;i<(int)faceListForThisFrame.size();i++) {
                commonTool.showImageAndLandMarks((QString("video ID %1 face ID %2").arg(videoID).arg(i)),faceListForThisFrame.at(i), landmarkListForThisFrame.at(i), CV_RGB(255,0,0), *openCVMutex);
                openCVMutex->lock();
                cv::moveWindow((QString("video ID %1 face ID %2").arg(videoID).arg(i)).toStdString(), 1000, y);
                openCVMutex->unlock();
                y+=300;
        }
        //set all the other to black
        for (int i=faceListForThisFrame.size();i<maxFacesPerFrame;i++) {
            cv::Mat black = cv::Mat::zeros(cv::Size(200, 250), CV_8UC1); // a black image
            openCVMutex->lock();
            cv::imshow((QString("video ID %1 face ID %2").arg(videoID).arg(i)).toStdString(), black );
            cv::moveWindow((QString("video ID %1 face ID %2").arg(videoID).arg(i)).toStdString(), 1000, y);
            openCVMutex->unlock();
            y+=300;
        }

        if (maxFacesPerFrame < (int)faceListForThisFrame.size()) {
            maxFacesPerFrame = faceListForThisFrame.size();
        }

        //data->videoFaces.at(videoID).push_back();
        //data->videoLandmarkPointsList.at(videoID).push_back();
//
        performFaceMatchingForFrame(videoID, frameID, faceListForThisFrame, landmarkListForThisFrame, centersOfLandmarks);
//
        openCVMutex->lock();
        frameID = (int)video.get(CV_CAP_PROP_POS_FRAMES);
        openCVMutex->unlock();

        openCVMutex->lock();
    }

    for (int suspectID = 0;suspectID < (int)inputSuspectPhotoList->size();suspectID++) {
        //close the last cluster if it is still open

        //open marked clusters
        std::vector<Cluster*> openClusters = data->getOpenClusters(videoID, suspectID);
        for (int i=0;i<(int)openClusters.size();i++) {
            Cluster* c = openClusters.at(i);
            c->stopFrameID = frameID-1;
        }

        //calc the averages
        for (int i=0;i<(int)data->clusters.at(videoID).at(suspectID).size();i++) {
            Cluster* c = data->clusters.at(videoID).at(suspectID).at(i);
            c->avgPhotoDistance = 0.0;
            c->avgPhotoSimilarity = 0.0;
            c->avgLandmarkDistances = 0.0;
            for (int j=0;j<(int)c->photoDistance.size();j++) {
                c->avgPhotoDistance += c->photoDistance.at(j);
                c->avgPhotoSimilarity += c->photoSimilarity.at(j);
                c->avgLandmarkDistances += c->sumOfLandmarkDistances.at(j);
            }
            c->avgPhotoDistance /= c->photoDistance.size();
            c->avgPhotoSimilarity /= c->photoDistance.size();
            c->avgLandmarkDistances /= c->photoDistance.size();
        }

        //not yet working, enable later ...
        data->removeSmallClusters(videoID, suspectID, 10);

    }


    openCVMutex->unlock();
    frameID = numberOfFramesInVideo;
    openCVMutex->lock();
    cv::destroyAllWindows();
    video.release();
    openCVMutex->unlock();

    emit videoPreProcessUpdateProgess(this->videoID, frameID);
    emit videoPreProcessDone(this->videoID);
}

RawVideoProcessingThread::RawVideoProcessingThread(QString videoPath,
                                       int videoID,
                                       QString applicationDataPath,
                                       QMutex &faceAlignMutex,
                                       QMutex &openCVMutex,
                                       RawVideoData* data,
                                       std::vector<cv::Mat>* inputSuspectPhotoList,
                                       std::vector<std::vector<cv::Point2d> >* inputSuspectLandmarkPointsList,
                                       FaceMatchingManager* faceMatchingManager
                                       ) {
    this->videoPath = videoPath;
    this->videoID = videoID;
    this->applicationDataPath = applicationDataPath;
    this->faceAlignMutex = &faceAlignMutex;
    this->openCVMutex = &openCVMutex;
    this->data = data;
    this->inputSuspectPhotoList = inputSuspectPhotoList;
    this->inputSuspectLandmarkPointsList = inputSuspectLandmarkPointsList;
    this->faceMatchingManager = faceMatchingManager;
}


void RawVideoProcessingThread::performFaceMatchingForFrame(int videoID, int frameID) {
    std::vector< std::vector<cv::Point2d> >* currentLandmarksForThisFrame = &data->videoLandmarkPointsList.at(videoID).at(frameID);
    data->sumOfLandmarkDistances.at(videoID).push_back(std::vector <std::vector<double> >());
    data->photoDistance.at(videoID).push_back(std::vector <std::vector<double> >());
    data->photoSimilarity.at(videoID).push_back(std::vector <std::vector<double> >());

    for (int faceID = 0;faceID<(int)currentLandmarksForThisFrame->size();faceID++) { //iterate over all faces in the current frame
        std::vector<cv::Point2d> videoLandmark = currentLandmarksForThisFrame->at(faceID);

        data->sumOfLandmarkDistances.at(videoID).at(frameID).push_back(std::vector<double>());
        data->photoDistance.at(videoID).at(frameID).push_back(std::vector<double>());
        data->photoSimilarity.at(videoID).at(frameID).push_back(std::vector<double>());

        for (int suspectID = 0; suspectID < (int)inputSuspectPhotoList->size(); suspectID++) { //iterate over all suspects
            std::vector<cv::Point2d> suspectLandmark = inputSuspectLandmarkPointsList->at(suspectID);
            cv::Mat currentSuspect = this->inputSuspectPhotoList->at(suspectID);

            double sumOfDistances = 10000000;
            if (videoLandmark.size() == suspectLandmark.size() && suspectLandmark.size() > 0) {
                sumOfDistances = 0.0;
                for (int currentPoint = 0; currentPoint < (int)suspectLandmark.size(); currentPoint++) {
                     sumOfDistances += cv::norm(suspectLandmark.at(currentPoint) - videoLandmark.at(currentPoint)); //hardcode the first face
                }
            }

            cv::Mat currentVideoFace;
            if (data->videoFaces.at(videoID).at(frameID).size()>0)
                currentVideoFace = data->videoFaces.at(videoID).at(frameID).at(faceID);

            //texture

            double distance = 10000000;
            double normalizedSimilarity = 0.0;

            //We don't use multilevel in the video prototype
            //double similarityMultiLevel = 0;
            //double confidenceMultiLevel = 0;
            //if (enableMultiLevelMatching && !currentVideoFace.empty()) {
            //    faceMatchingManager->faceMatchMulti(currentSuspect, currentVideoFace, similarityMultiLevel, confidenceMultiLevel);
            //}

            if (!currentVideoFace.empty()) {
                distance = faceMatchingManager->getDistance(currentSuspect, currentVideoFace);
                normalizedSimilarity = faceMatchingManager->getNormalizedSimilarity(currentSuspect, currentVideoFace).at(0);
            }

            data->suspectIDs.push_back(suspectID);
            data->videoIDs.push_back(videoID);
            data->frameIDs.push_back(frameID);
            //commonTool.log(QString("processing frameID ---> %1").arg(frameID)); //
            data->faceIDs.push_back(faceID);
            data->sumOfLandmarkDistances.at(videoID).at(frameID).at(faceID).push_back(sumOfDistances);
            data->photoDistance.at(videoID).at(frameID).at(faceID).push_back(distance);
            data->photoSimilarity.at(videoID).at(frameID).at(faceID).push_back(normalizedSimilarity);
        }
    }
}


void RawVideoProcessingThread::run() {
    openCVMutex->lock();
    cv::VideoCapture video;
    video.open(this->videoPath.toStdString());
    openCVMutex->unlock();

    cv::Mat frame;
    int numberOfFramesInVideo = (int)video.get(CV_CAP_PROP_FRAME_COUNT);

    int frameID = 0; // the current frame number that is processed
    FaceAlignment fa(applicationDataPath.toStdString(), *faceAlignMutex);
    int maxFacesPerFrame = 0;

    ///
    data->sumOfLandmarkDistances.push_back(std::vector<std::vector< std::vector<double> > >());
    data->photoDistance.push_back(std::vector< std::vector<std::vector<double> > >());
    data->photoSimilarity.push_back(std::vector< std::vector<std::vector<double> > >());
    ///

    while (video.read(frame)) {
        //commonTool.log(QString("frameID --> %1").arg(frameID));

        std::vector<cv::Mat> faceListForThisFrame;
        std::vector<std::vector<cv::Point2d> > landmarkListForThisFrame;

        cv::Mat grayFrame;
        cv::cvtColor(frame, grayFrame, CV_RGB2GRAY);

        emit videoPreProcessUpdateProgess(this->videoID, frameID);

        //show just the input frame
        openCVMutex->lock();
        cv::imshow(QString("video %1").arg(videoID).toStdString(), grayFrame );
        cv::moveWindow(QString("video %1").arg(videoID).toStdString(), 0, 0);
        openCVMutex->unlock();

        std::vector<cv::Point2d> centersOfLandmarks; //not used
        fa.alignStasm4(grayFrame, faceListForThisFrame, landmarkListForThisFrame, centersOfLandmarks);

        int y = 0;
        //visualize the detected faces+landmark
        for (int i=0;i<(int)faceListForThisFrame.size();i++) {
                commonTool.showImageAndLandMarks((QString("video ID %1 face ID %2").arg(videoID).arg(i)),faceListForThisFrame.at(i), landmarkListForThisFrame.at(i), CV_RGB(255,0,0), *openCVMutex);
                openCVMutex->lock();
                cv::moveWindow((QString("video ID %1 face ID %2").arg(videoID).arg(i)).toStdString(), 1000, y);
                openCVMutex->unlock();
                y+=300;
        }
        //set all the other to black
        for (int i=faceListForThisFrame.size();i<maxFacesPerFrame;i++) {
            cv::Mat black = cv::Mat::zeros(cv::Size(200, 250), CV_8UC1); // a black image
            openCVMutex->lock();
            cv::imshow((QString("video ID %1 face ID %2").arg(videoID).arg(i)).toStdString(), black );
            cv::moveWindow((QString("video ID %1 face ID %2").arg(videoID).arg(i)).toStdString(), 1000, y);
            openCVMutex->unlock();
            y+=300;
        }

        if (maxFacesPerFrame < (int)faceListForThisFrame.size()) {
            maxFacesPerFrame = faceListForThisFrame.size();
        }

        data->videoFaces.at(videoID).push_back(faceListForThisFrame);
        data->videoLandmarkPointsList.at(videoID).push_back(landmarkListForThisFrame);
//
        performFaceMatchingForFrame(videoID, frameID);
//
        openCVMutex->lock();
        frameID = (int)video.get(CV_CAP_PROP_POS_FRAMES);
        openCVMutex->unlock();
    }

    frameID = numberOfFramesInVideo;
    openCVMutex->lock();
    cv::destroyAllWindows();
    video.release();
    openCVMutex->unlock();

    emit videoPreProcessUpdateProgess(this->videoID, frameID);
    emit videoPreProcessDone(this->videoID);
}
