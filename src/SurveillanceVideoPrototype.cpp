#include "SurveillanceVideoPrototype.h"

SurveillanceVideoPrototype::SurveillanceVideoPrototype(void) { }

SurveillanceVideoPrototype::SurveillanceVideoPrototype(Parameter parameter_p, QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags) {
	qsrand(QTime::currentTime().msec());
    uVideoID = 0;
    faceMatchingManager.enableMultiLevel = false;
    //
    useRaw = false;
    //
    addSuspectEnabled = true;
    addVideoEnabled = false;
    if (useRaw) {
        data = new RawVideoData();
    } else {
        data = new ClusterVideoData();
    }
	//GM
	parameter_p.mode = _CONTROLLER_S2P_;
    gm = new GeneralManager();
    gm->parameter = parameter_p;
    //add suspects to training data?
    if (false) {
        trainFaceMatching(inputSuspecPhotoList, openCVMutex);
    } else {
        std::vector<cv::Mat> x;
        trainFaceMatching(x, openCVMutex);
    }

    gm->trainFaceSynthesis();

	connect(gm, SIGNAL(updateUI()), this, SLOT(updateUI()));

	//UI
	ui.setupUi(this);
	faceSysTrainingDoenLabel = new QLabel();
	ui.stb_mainWindow->addWidget(faceSysTrainingDoenLabel);
	ui.sli_faceRecognitionConfidence->setValue(int(FACE_RECOG_RESULT_THRESHOLD * 100));

    inputSuspectTableModel = new QInputSuspectTableModel(0, inputSuspectSketchList, inputSuspecPhotoList);
	ui.tlv_inputSuspect->setModel(inputSuspectTableModel);
	ui.tlv_inputSuspect->resizeColumnsToContents();

	//input video table
    filesListTableListModel = new QInputVideoTableModel(0, videoFileNames, middleColumnVideoIDs, totalFramesList);
	ui.tlv_inputVideo->setModel(filesListTableListModel);
	ui.tlv_inputVideo->resizeColumnsToContents();
	
	//face recognition result table
    if (useRaw) {
        faceRecognitionTableModel = new RawVideoDataTableModel(0, (static_cast<RawVideoData*>(data)));
        faceRecognitionProxyModel = new RawVideoDataProxyModel(FACE_RECOG_RESULT_THRESHOLD);
    } else {
        faceRecognitionTableModel = new ClusterVideoDataTableModel(0, (static_cast<ClusterVideoData*>(data)));
        faceRecognitionProxyModel = new ClusterVideoDataProxyModel(FACE_RECOG_RESULT_THRESHOLD);
    }


    //std::vector <std::vector< std::vector<double> > > sumOfLandmarkDistancesInSketch;
    //std::vector <std::vector< std::vector<double> > > sumOfLandmarkDistancesInPhoto;


    faceRecognitionProxyModel->setSourceModel(faceRecognitionTableModel);

    ui.tlv_faceRecognitionResult->setModel(faceRecognitionProxyModel);
	ui.tlv_faceRecognitionResult->resizeColumnsToContents();
    connect(
        ui.tlv_faceRecognitionResult->selectionModel(),
        SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
        this,
        SLOT(faceRecognitionResultSelectionChangedSlot(const QItemSelection &, const QItemSelection &))
    );
	
	//UI connect slider and table model + label
    connect(ui.sli_faceRecognitionConfidence, SIGNAL(valueChanged(int)), faceRecognitionProxyModel, SLOT(setThreshold(int)));
    connect(ui.sli_faceRecognitionConfidence, SIGNAL(valueChanged(int)), this, SLOT(updateUI()));

	updateUI();
}

//add sketch
//executed when the select sketch button is clicked
void SurveillanceVideoPrototype::on_addSketchButton_clicked() {

    QStringList selectedFiles = QFileDialog::getOpenFileNames(this,
        tr("Select the sketch files"), sketchPath,
        tr("Image Files (*.png *.jpg *.bmp)"));

    QString errorFileQString;
    for (int i = 0; i < selectedFiles.size(); i++) {
        commonTool.log(QString("Selected Sketch file: %1.").arg((selectedFiles.at(i))));

        cv::Mat selectedGraySketch;
        {
            cv::Mat selectedSketch = cv::imread(selectedFiles.at(i).toStdString());
            cv::cvtColor(selectedSketch, selectedGraySketch,CV_RGB2GRAY);
        }

        cv::Mat alignedGraySketch;
        std::vector<cv::Point2d> landmarkPointsSketch;
        //bool success = gm->faceAlign(selectedSketch, alignedSketch, landmarkPointsSketch);

        FaceAlignment fa(gm->parameter.dataPath.toStdString(), faceAlignMutex);
        cv::Point2d centerOfLandmarks; //not used
        fa.alignStasm4(selectedGraySketch, alignedGraySketch, landmarkPointsSketch, centerOfLandmarks);


        cv::Mat alignedSketch;
        cv::cvtColor(alignedGraySketch, alignedSketch,CV_GRAY2RGB);

        if (alignedGraySketch.empty()){
            if (selectedGraySketch.cols == 200 && selectedGraySketch.rows == 250){
                alignedGraySketch = selectedGraySketch;
            } else {
                errorFileQString += selectedFiles.at(i) + "\n";
                continue;
            }
        }

        inputSuspectSketchList.push_back(alignedSketch);
        inputSuspectLandmarkPointsList.push_back(landmarkPointsSketch);

        gm->setInputSynthesisImage(alignedSketch);
        //TODO: Message: Please wait while syntheiszing the face ... (new thread?)
        gm->MWFStart();

        cv::Mat synthesizedPhoto = gm->getOutputImage();
        cv::Mat alignedSynthesizedPhoto;
        std::vector<cv::Point2d> landmarkPointsPhoto;
        fa.alignStasm4(synthesizedPhoto, alignedSynthesizedPhoto, landmarkPointsPhoto, centerOfLandmarks);
        inputSuspecPhotoList.push_back(alignedSynthesizedPhoto);
        //inputSuspectPhotoLandmarkPointsList.push_back(landmarkPointsPhoto);
        addVideoEnabled = true;
    }
    if (!errorFileQString.isEmpty()){
        QMessageBox::warning(0, "Select Photo", "The face of following file(s) cannot be located. Please try again.\n" + errorFileQString, QMessageBox::Ok);
    }
    inputSuspectTableModel->updateTable();
    updateUI();
}

void SurveillanceVideoPrototype::on_addSnapshotButton_clicked() {
    cameraWidget = new QCameraWidget(0, openCVMutex, faceAlignMutex, gm->parameter.dataPath.toStdString());
    connect(cameraWidget, SIGNAL(capturedImage()), this, SLOT(getCapturedImage()));
    cameraWidget->show();
    cameraWidget->startCamera();
}

//add Photo
void SurveillanceVideoPrototype::on_addPhotoButton_clicked() {
    QStringList selectedFiles = QFileDialog::getOpenFileNames(this,
        tr("Select the image files"), photoPath,
        tr("Image Files (*.png *.jpg *.bmp)"));

    QString errorFileQString;

    for (int i = 0; i < selectedFiles.size(); i++) {
        cv::Mat selectedGrayImg;
        {
            cv::Mat selectedImg = cv::imread(selectedFiles.at(i).toStdString());
            cv::cvtColor(selectedImg, selectedGrayImg,CV_RGB2GRAY);
        }

        cv::Mat alignedFace;
        std::vector<cv::Point2d> landmarkPoints;
        //gm->faceAlign(selectedGrayImg, alignedFace, landmarkPoints);
        FaceAlignment fa(gm->parameter.dataPath.toStdString(), faceAlignMutex);
        cv::Point2d centerOfLandmarks; //not used
        fa.alignStasm4(selectedGrayImg, alignedFace, landmarkPoints, centerOfLandmarks);

        if (alignedFace.empty()){
            if (selectedGrayImg.cols == 200 && selectedGrayImg.rows == 250){
                alignedFace = selectedGrayImg;
            } else {
                errorFileQString += selectedFiles.at(i) + "\n";
                continue;
            }
        }

        inputSuspectSketchList.push_back(cv::Mat());
        inputSuspecPhotoList.push_back(alignedFace);
        inputSuspectLandmarkPointsList.push_back(landmarkPoints);
        addVideoEnabled = true;
    }

    if (!errorFileQString.isEmpty()){
        QMessageBox::warning(0, "Select Photo", "The face of following file(s) cannot be located. Please try again.\n" + errorFileQString, QMessageBox::Ok);
    }

    inputSuspectTableModel->updateTable();
    updateUI();
    //faceRecognitionCrossCheck();
}

void SurveillanceVideoPrototype::on_addVideoButton_clicked() {

	QStringList selectedVideosList = QFileDialog::getOpenFileNames(this, 
        tr("Select Input Video Files"), videoPath,
        tr("Video Files(*.avi *.mov)"));

    if (useRaw) {
        RawVideoData* rawData = static_cast<RawVideoData*>(data);
        for (int i = 0; i < selectedVideosList.size(); i++) {
            commonTool.log(QString("Selected video file: %1").arg((selectedVideosList.at(i))));
            rawData->videoFaces.push_back(std::vector<std::vector< cv::Mat > >());
            rawData->videoLandmarkPointsList.push_back(std::vector<std::vector< std::vector< cv::Point2d > > >());
            progressList.push_back(0);
            videoFileNames.push_back(selectedVideosList.at(i));
            middleColumnVideoIDs.push_back(uVideoID);

            //get the number of frames
            cv::VideoCapture video;
            video.open(selectedVideosList.at(i).toStdString());
            int numberOfFrames = (int)video.get(CV_CAP_PROP_FRAME_COUNT);
            totalFramesList.push_back(numberOfFrames);
            video.release();

            //create a new thread to process the video
            RawVideoProcessingThread* thread = new RawVideoProcessingThread(
                        selectedVideosList.at(i),
                        uVideoID++,
                        gm->parameter.dataPath,
                        faceAlignMutex,
                        openCVMutex,
                        rawData,
                        &inputSuspecPhotoList,
                        &inputSuspectLandmarkPointsList,
                        &faceMatchingManager);
            connect(thread, SIGNAL(videoPreProcessDone(int)), this, SLOT(processVideosDoneSlot(int)));
            connect(thread, SIGNAL(videoPreProcessUpdateProgess(int, int)), this, SLOT(processVideosUpdateProgessSlot(int, int))); //there is a no such signal error message!

            processVideoThreads.push_back(thread);
        }
    } else {
        ClusterVideoData* clusterData = static_cast<ClusterVideoData*>(data);
        for (int i = 0; i < selectedVideosList.size(); i++) {
            commonTool.log(QString("Selected video file: %1").arg((selectedVideosList.at(i))));

            progressList.push_back(0);
            videoFileNames.push_back(selectedVideosList.at(i));
            middleColumnVideoIDs.push_back(uVideoID);

            //get the number of frames
            cv::VideoCapture video;
            video.open(selectedVideosList.at(i).toStdString());
            int numberOfFrames = (int)video.get(CV_CAP_PROP_FRAME_COUNT);
            totalFramesList.push_back(numberOfFrames);
            video.release();

            //create a new thread to process the video
            ClusterVideoProcessingThread* thread = new ClusterVideoProcessingThread(
                        selectedVideosList.at(i),
                        uVideoID++,
                        gm->parameter.dataPath,
                        faceAlignMutex,
                        openCVMutex,
                        clusterData,
                        &inputSuspecPhotoList,
                        &inputSuspectLandmarkPointsList,
                        &faceMatchingManager);
            connect(thread, SIGNAL(videoPreProcessDone(int)), this, SLOT(processVideosDoneSlot(int)));
            connect(thread, SIGNAL(videoPreProcessUpdateProgess(int, int)), this, SLOT(processVideosUpdateProgessSlot(int, int))); //there is a no such signal error message!

            processVideoThreads.push_back(thread);
        }
    }


    //start the first one
    if (processVideoThreads.size() > 0) {
        processVideoThreads.at(0)->start();
        addSuspectEnabled = false;
        addVideoEnabled = false;
        updateUI();
    }


	filesListTableListModel->updateTable();
	ui.tlv_inputVideo->resizeColumnsToContents();
}

void SurveillanceVideoPrototype::processVideosUpdateProgessSlot(int videoID, int progress) {
    progressList.at(videoID) = progress;
    filesListTableListModel->updateTable(videoID, progressList);
    faceRecognitionTableModel->updateTable();
}

void SurveillanceVideoPrototype::processVideosDoneSlot(int videoID) {

    //data.videoFaces.at(videoID) = processVideoThreads.at(videoID)->faces;
    //data.videoLandmarkPointsList.at(videoID) = this->processVideoThreads.at(videoID)->landmarks;
    //totalFramesList.at(videoID) = progressList.at(videoID);

    VideoProcessingThread* thread = processVideoThreads.at(videoID);
    delete thread;
    processVideoThreads.at(videoID) = NULL;

    filesListTableListModel->updateTable(videoID, progressList);
    faceRecognitionTableModel->updateTable();

    for (int i=0;i<(int)processVideoThreads.size();i++) {
        if (processVideoThreads.at(i)!=NULL) {
            processVideoThreads.at(i)->start();
            break;
        }
    }

    //ui.tlv_inputVideo->resizeColumnsToContents();
}

void SurveillanceVideoPrototype::getCapturedImage(){
	cv::Mat capturedImage = cameraWidget->getCurrentCvImage();

    //TODO:save the captured images somewhere?
	//to save the captured image
	//cv::imwrite("E:/image.jpg", capturedImage);

	delete cameraWidget;

    cv::Mat alignedFace;
    std::vector<cv::Point2d> landmarkPoints;
    //gm->faceAlign(capturedImage, alignedFace, landmarkPoints);

    FaceAlignment fa(gm->parameter.dataPath.toStdString(), openCVMutex);
    cv::Point2d centerOfLandmarks; //not used
    fa.alignStasm4(capturedImage, alignedFace, landmarkPoints, centerOfLandmarks);


	if (alignedFace.empty()){
		QMessageBox::warning(0, "Take Photo", "Cannot locate the face. Please try again.", QMessageBox::Ok);
		return;
	}

    inputSuspectSketchList.push_back(cv::Mat());
    inputSuspecPhotoList.push_back(alignedFace);

    inputSuspectLandmarkPointsList.push_back(landmarkPoints);

    addVideoEnabled = true;
	inputSuspectTableModel->updateTable();
    updateUI();
}

void SurveillanceVideoPrototype::updateUI() {
    //commonTool.log("QVideoMainWindow::updateUI()");
    if (gm->numberOfTrainedImage > 0){
        //done training
        faceSysTrainingDoenLabel->setText(FACE_SYS_TRAINING_DONE);
        if (addSuspectEnabled) {
            ui.addSketchButton->setDisabled(false);
        }

        ui.addSketchButton->setToolTip("Add a sketch of a suspect");
    } else {
        //doing training
        faceSysTrainingDoenLabel->setText(FACE_SYS_TRAINING_ONGOING);
        ui.addSketchButton->setDisabled(true);
        ui.addSketchButton->setToolTip(FACE_SYS_TRAINING_ONGOING);
    }

    ui.addPhotoButton->setEnabled(addSuspectEnabled);
    ui.addSnapshotButton->setEnabled(addSuspectEnabled);
    ui.addVideoButton->setEnabled(addVideoEnabled);

    //the slider number
    ui.lab_faceRecognitionConfidence->setText(QString::number(ui.sli_faceRecognitionConfidence->value()) + "%");

    ui.tlv_inputSuspect->resizeColumnsToContents();
    ui.tlv_inputVideo->resizeColumnsToContents();
    ui.tlv_faceRecognitionResult->resizeColumnsToContents();
}

void SurveillanceVideoPrototype::playVideo(QString videoPath, int startFrame, int endFrame, int videoID, int row) {
    cv::VideoCapture video(videoPath.toStdString());
    int fps = (int)video.get(CV_CAP_PROP_FPS);
    cv::Mat frame;

    int maxFacesPerFrame = 0;
    int frameIDClusterCounter = 0;
    int frameID = 0;
    while(video.read(frame)) {

        cv::Mat grayFrame;
        cv::cvtColor(frame, grayFrame, CV_RGB2GRAY);


        if (frameID < startFrame) {
            frameID++;
            continue;
        } else if (frameID > endFrame) {
            break;
        }

        //I moved this after the if ...
        //openCVMutex.lock();
        //cv::imshow(QString("video %1").arg(videoID).toStdString(), grayFrame);
        //cv::moveWindow((QString("video %1").arg(videoID)).toStdString(), 0, 0);
        //cv::waitKey(1000/fps);
        //openCVMutex.unlock();

        if (useRaw) {

            openCVMutex.lock();
            cv::imshow(QString("video %1").arg(videoID).toStdString(), grayFrame);
            cv::moveWindow((QString("video %1").arg(videoID)).toStdString(), 0, 0);
            cv::waitKey(1000/fps);
            openCVMutex.unlock();

            RawVideoData* rawData = static_cast<RawVideoData*>(data);
            int y = 0;
            if (frameID >= (int)rawData->videoFaces.at(videoID).size()) { //we have reached the end of the face vector
                break;
            }

            for (int i=0;i<(int)rawData->videoFaces.at(videoID).at(frameID).size();i++) {
                cv::Mat face = rawData->videoFaces.at(videoID).at(frameID).at(i);
                std::vector< cv::Point2d > landmark = rawData->videoLandmarkPointsList.at(videoID).at(frameID).at(i);
                commonTool.showImageAndLandMarks(QString("video ID %1 face ID %2").arg(videoID).arg(i), face, landmark, CV_RGB(0,255,0), openCVMutex);
                openCVMutex.lock();
                cv::moveWindow((QString("video ID %1 face ID %2").arg(videoID).arg(i)).toStdString(), 1000, y);
                openCVMutex.unlock();
                y+=300;
            }
            //set all the other to black
            for (int i = rawData->videoFaces.at(videoID).at(frameID).size();i<maxFacesPerFrame;i++) {
                cv::Mat black = cv::Mat::zeros(cv::Size(200, 250), CV_8UC1); // a black image
                openCVMutex.lock();
                cv::imshow((QString("video ID %1 face ID %2").arg(videoID).arg(i)).toStdString(), black );
                cv::moveWindow((QString("video ID %1 face ID %2").arg(videoID).arg(i)).toStdString(), 1000, y);
                openCVMutex.unlock();
                y+=300;
            }

            if ((int)rawData->videoFaces.at(videoID).at(frameID).size() > maxFacesPerFrame) {
                maxFacesPerFrame = rawData->videoFaces.at(videoID).at(frameID).size();
            }
        } else {
            //cluster, not raw

            if (row == -1) {
                //just play the movie without the center points...
                openCVMutex.lock();
                cv::imshow(QString("video %1").arg(videoID).toStdString(), grayFrame);
                cv::moveWindow((QString("video %1").arg(videoID)).toStdString(), 0, 0);
                cv::waitKey(1000/fps);
                openCVMutex.unlock();

            } else {
                ClusterVideoData* clusterData = static_cast<ClusterVideoData*>(data);
                Cluster* c = clusterData->clusterLocations.at(row);
                if (frameID >= c->startFrameID && frameID <= c->stopFrameID) {
                    cv::Point2d center = c->locationList.at(frameIDClusterCounter);
                    std::vector<cv::Point2d> dummyList;
                    dummyList.push_back(center);
                    commonTool.showImageAndLandMarks(QString("video %1").arg(videoID), grayFrame, dummyList, CV_RGB(1 ,1 ,1), openCVMutex);
                    openCVMutex.lock();
                    cv::moveWindow((QString("video %1").arg(videoID)).toStdString(), 0, 0);
                    cv::waitKey(1000/fps);
                    openCVMutex.unlock();
                    frameIDClusterCounter++;
                }
            }


        }
        frameID = (int)video.get(CV_CAP_PROP_POS_FRAMES);
    }
    //automatically destroy the video windows after it has reached the end
    commonTool.log("While loop has completed ..");
    openCVMutex.lock();
    cv::destroyAllWindows();
    video.release();
    openCVMutex.unlock();
}

void SurveillanceVideoPrototype::on_tlv_inputVideo_doubleClicked(const QModelIndex & index ) {
	int row = index.row();
	QModelIndex videoIDIndex = ui.tlv_inputVideo->model()->index(row, 0);
	int videoID = ui.tlv_inputVideo->model()->data(videoIDIndex).toInt();
    //commonTool.log(QString("Clicked on a video with index %1.").arg(videoID));

    QString videoPath = videoFileNames.at(videoID);

    int startFrame = 0;
    int endFrame = 0;

    openCVMutex.lock();
    cv::VideoCapture video(videoPath.toStdString());
    endFrame = (int)video.get(CV_CAP_PROP_FRAME_COUNT);
    video.release();
    openCVMutex.unlock();

    playVideo(videoPath, startFrame, endFrame, videoID, -1);
}

void SurveillanceVideoPrototype::on_tlv_inputSuspect_doubleClicked(const QModelIndex & index ){

    openCVMutex.lock();
    cv::destroyWindow("Suspect Sketch");
    cv::destroyWindow("Suspect Photo");
    openCVMutex.unlock();

    int row = index.row();
    QModelIndex suspectIDIndex = ui.tlv_inputSuspect->model()->index(row, 0);

    int suspectID = ui.tlv_inputSuspect->model()->data(suspectIDIndex).toInt();

    //commonTool.log(QString("Clicked on a suspect with index %1.").arg(suspectID));

    //visualize the landmarks
    cv::Mat sketch;
    if (suspectID < (int)this->inputSuspectSketchList.size())
        sketch = this->inputSuspectSketchList.at(suspectID);
    cv::Mat photo;
    if (suspectID < (int)this->inputSuspecPhotoList.size())
        photo = this->inputSuspecPhotoList.at(suspectID);

    std::vector<cv::Point2d> featurePoints;
    if (suspectID < (int)this->inputSuspectLandmarkPointsList.size())
        featurePoints = this->inputSuspectLandmarkPointsList.at(suspectID);

    if (!sketch.empty()) {
        commonTool.showImageAndLandMarks(QString("Suspect Sketch"), sketch, featurePoints, CV_RGB(255,0,0), openCVMutex);
        openCVMutex.lock();
        cv::moveWindow("Suspect Sketch", 0, 0);
        openCVMutex.unlock();
    }
    if (!photo.empty()) {
        commonTool.showImageAndLandMarks(QString("Suspect Photo"), photo, featurePoints, CV_RGB(0,255,0), openCVMutex);
        openCVMutex.lock();
        cv::moveWindow("Suspect Photo", 200, 0);
        openCVMutex.unlock();
    }
}

void SurveillanceVideoPrototype::on_tlv_faceRecognitionResult_doubleClicked(const QModelIndex & index) {
    if (useRaw) {
        int row = index.row();
        openCVMutex.lock();
        cv::destroyWindow("croppedFaceFromVideo");
        cv::destroyWindow("croppedPhotoOfSuspect");
        cv::destroyWindow("Video vs. Suspect");
        cv::destroyWindow("Suspect Photo");
        cv::destroyWindow("Suspect sketch");
        cv::destroyWindow("Video Frame");
        openCVMutex.unlock();

        QModelIndex resultModelIndex = ui.tlv_faceRecognitionResult->model()->index(row, 0);
        int resultID = ui.tlv_faceRecognitionResult->model()->data(resultModelIndex).toInt();

        RawVideoData* rawData = static_cast<RawVideoData*>(data);
        int videoID = rawData->videoIDs.at(resultID);
        int suspectID = rawData->suspectIDs.at(resultID);
        int frameID = rawData->frameIDs.at(resultID);
        int faceID = rawData->faceIDs.at(resultID);

        //commonTool.log(QString("selected videoID = %1").arg(videoID));
        //commonTool.log(QString("selected suspectID = %1").arg(suspectID));
        //commonTool.log(QString("selected resultID = %1").arg(indexID));

        cv::Mat sketchOfSuspect = inputSuspectSketchList.at(suspectID); //if no sketch, this doesnt work?
        cv::Mat photoOfSuspect = inputSuspecPhotoList.at(suspectID);
        std::vector<cv::Point2d> landmarkOfSuspect = inputSuspectLandmarkPointsList.at(suspectID);

        if (!sketchOfSuspect.empty()) {
            commonTool.showImageAndLandMarks(QString("Suspect Sketch"), sketchOfSuspect, landmarkOfSuspect, CV_RGB(255,0,0), openCVMutex);
            openCVMutex.lock();
            cv::moveWindow("Suspect Sketch", 0, 0);
            openCVMutex.unlock();
        }
        if (!photoOfSuspect.empty()) {
            commonTool.showImageAndLandMarks(QString("Suspect Photo"), photoOfSuspect, landmarkOfSuspect, CV_RGB(255,0,0), openCVMutex);
            openCVMutex.lock();
            cv::moveWindow("Suspect Photo", 200, 0);
            openCVMutex.unlock();
        }

        cv::Mat faceFromVideo = rawData->videoFaces.at(videoID).at(frameID).at(faceID);
        if (!faceFromVideo.empty()) {
            std::vector<cv::Point2d> landmarkOfVideoFrame = rawData->videoLandmarkPointsList.at(videoID).at(frameID).at(faceID);
            commonTool.showImageAndLandMarks(QString("Video Frame"), faceFromVideo, landmarkOfVideoFrame, CV_RGB(0,255,0), openCVMutex);
            openCVMutex.lock();
            cv::moveWindow("Video Frame", 400, 0);
            openCVMutex.unlock();
            cv::Mat x = cv::Mat::zeros(cv::Size(faceFromVideo.cols, faceFromVideo.rows), CV_8UC3); // a black image
            commonTool.showImageAndLandMarks(QString("Video vs. Suspect"), x, landmarkOfSuspect, landmarkOfVideoFrame, CV_RGB(255,0,0), CV_RGB(0,255,0), openCVMutex);
            openCVMutex.lock();
            cv::moveWindow("Video vs. Suspect", 600, 0);
            openCVMutex.unlock();
            cv::Mat croppedFaceFromVideo = faceMatchingManager.controllerSingle->cropImg(faceFromVideo);
            cv::Mat croppedPhotoOfSuspect = faceMatchingManager.controllerSingle->cropImg(photoOfSuspect);
            openCVMutex.lock();
            cv::namedWindow( "croppedPhotoOfSuspect", CV_WINDOW_NORMAL );
            cv::imshow( "croppedPhotoOfSuspect", croppedPhotoOfSuspect );
            cv::moveWindow("croppedPhotoOfSuspect", 800, 0);

            cv::namedWindow( "croppedFaceFromVideo", CV_WINDOW_NORMAL );
            cv::imshow( "croppedFaceFromVideo", croppedFaceFromVideo );
            cv::moveWindow("croppedFaceFromVideo", 1000, 0);
            openCVMutex.unlock();
        }
    } else {
        /*
        int row = index.row();
        QModelIndex resultModelIndex = ui.tlv_faceRecognitionResult->model()->index(row, 0);
        int resultID = ui.tlv_faceRecognitionResult->model()->data(resultModelIndex).toInt();

        ClusterVideoData* clusterData = static_cast<ClusterVideoData*>(data);
        int videoID = clusterData->videoIDs.at(resultID);
        int frameID = clusterData->frameIDs.at(resultID);
        int faceID = clusterData->faceIDs.at(resultID);
        int suspectID = clusterData->suspectIDs.at(resultID);

        commonTool.log(QString("%1 %2 %3 %4").arg(videoID).arg(frameID).arg(faceID).arg(suspectID));

        cv::Point2d location = clusterData->location.at(videoID).at(frameID).at(faceID);
        commonTool.log(QString("location --> %1 %2").arg(location.x).arg(location.y));
        */

        int row = index.row();
        QModelIndex resultModelIndex = ui.tlv_faceRecognitionResult->model()->index(row, 0);
        int resultID = ui.tlv_faceRecognitionResult->model()->data(resultModelIndex).toInt();

        ClusterVideoData* clusterData = static_cast<ClusterVideoData*>(data);
        int videoID = clusterData->videoIDs.at(resultID);
        Cluster* c = clusterData->clusterLocations.at(resultID);
        int startFrame = c->startFrameID;
        int endFrame = c->stopFrameID;

        QString videoPath = videoFileNames.at(videoID);

        playVideo(videoPath, startFrame, endFrame, videoID, resultID);
    }
}

void SurveillanceVideoPrototype::faceRecognitionResultSelectionChangedSlot(const QItemSelection & selected, const QItemSelection & deselected){
	if (selected.size() > 0){
        if (useRaw) {
            int selectedRow = selected.indexes().at(0).row();
            QModelIndex suspectIDIndex = ui.tlv_faceRecognitionResult->model()->index(selectedRow, 4);
            QModelIndex videoIDIndex = ui.tlv_faceRecognitionResult->model()->index(selectedRow, 1);
            int suspectID = ui.tlv_faceRecognitionResult->model()->data(suspectIDIndex).toInt();
            int videoID = ui.tlv_faceRecognitionResult->model()->data(videoIDIndex).toInt();
            QModelIndex suspectIndex = ui.tlv_inputSuspect->model()->index(suspectID, 0);
            QModelIndex videoIndex = ui.tlv_inputVideo->model()->index(videoID, 0);
            ui.tlv_inputSuspect->setCurrentIndex(suspectIndex);
            ui.tlv_inputVideo->setCurrentIndex(videoIndex);
        } else {
            int selectedRow = selected.indexes().at(0).row();
            QModelIndex suspectIDIndex = ui.tlv_faceRecognitionResult->model()->index(selectedRow, 4);
            QModelIndex videoIDIndex = ui.tlv_faceRecognitionResult->model()->index(selectedRow, 1);
            int suspectID = ui.tlv_faceRecognitionResult->model()->data(suspectIDIndex).toInt();
            int videoID = ui.tlv_faceRecognitionResult->model()->data(videoIDIndex).toInt();
            QModelIndex suspectIndex = ui.tlv_inputSuspect->model()->index(suspectID, 0);
            QModelIndex videoIndex = ui.tlv_inputVideo->model()->index(videoID, 0);
            ui.tlv_inputSuspect->setCurrentIndex(suspectIndex);
            ui.tlv_inputVideo->setCurrentIndex(videoIndex);
        }
	} else {
		ui.tlv_inputSuspect->clearSelection();
		ui.tlv_inputVideo->clearSelection();
	}
}

void SurveillanceVideoPrototype::trainFaceMatching(std::vector<cv::Mat> additionalImages, QMutex& openCVMutex) {
    QString faceRecogDBPath = dataPath + "/" + QString::fromStdString(FaceMatchingManager::trainingDirectory);
     faceMatchingManager.train(faceRecogDBPath, additionalImages, openCVMutex);
}
