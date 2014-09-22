#include "QBorderMainWindow.h"
#include "FaceMatchingManager.h"
#include "ClickableQLabel.h"
#include "FaceAlignment.h"

QBorderMainWindow::QBorderMainWindow(Parameter parameter, QWidget *parent, Qt::WindowFlags flags):QMainWindow(parent, flags) {
    this->parameter = parameter;

    faceMatchingManager.enableMultiLevel = true;

    //training for face verification
    //add suspects to training data?
    if (false) {
        //TODO:
        //trainFaceMatching(inputSuspecPhotoList, openCVMutex);
    } else {
        std::vector<cv::Mat> x;
        trainFaceVerification(x, openCVMutex);
    }
  //  commonTool.log("#1");
    //training for face synthesize
    trainFaceSynthesize();

  //  commonTool.log("#2");

    ui.setupUi(this);
    connect(ui.inputPerson, &ClickableQLabel::clicked, this, &QBorderMainWindow::onClickedInputPerson);
    connect(ui.matchedPerson, &ClickableQLabel::clicked, this, &QBorderMainWindow::onClickedMatchedPerson);
    connect(ui.camera, &ClickableQLabel::clicked, this, &QBorderMainWindow::onClickedCamera);
    connect(ui.inputPersonLabel, &ClickableQLabel::clicked, this, &QBorderMainWindow::onClickedInputPersonLabel);

    connect(&faceSynthesisManager, &FaceSynthesisManager::FSTrainingDone, this, &QBorderMainWindow::trainFaceSynthesizeCompleted);
    connect(&faceSynthesisManager, &FaceSynthesisManager::FSDone, this, &QBorderMainWindow::faceSynthesizeCompleted);

    matchedIndex = -1;
    partialMatchedIndex = -1;

 //   commonTool.log("#3");
    startCamera();
 //   commonTool.log("#4");
    loadWantedPersons();
 //   commonTool.log("#5");
    updateUI();
 //   commonTool.log("#6");
}



QStringList QBorderMainWindow::getWantedPersonQStringListFromFile(QString filePath) {
    QStringList wantedPersonList;
    commonTool.log(QString("Importing wanted persons from file %1").arg(filePath));
    QFile wantedPersonFile(filePath);
    if (wantedPersonFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        while (!wantedPersonFile.atEnd()) {
            QString name = wantedPersonFile.readLine().trimmed();
            wantedPersonList.append(name);
        }
        wantedPersonFile.close();
    }
    commonTool.log(QString("Imported %1 wanted persons.").arg(wantedPersonList.size()));
    return wantedPersonList;
}

void QBorderMainWindow::loadWantedPersons() {
    QStringList wantedPersonList = getWantedPersonQStringListFromFile(parameter.dataPath + "completeAR/wanted.txt");
    QString wantedPersonFolder = parameter.dataPath + "completeAR/";
    FaceAlignment fa(parameter.dataPath.toStdString(), faceAlignMutex);
    for(int i = 0; i < wantedPersonList.size(); i++) {
        cv::Mat grayWantedPerson;
        {
            cv::Mat wantedPerson = cv::imread((wantedPersonFolder+wantedPersonList.at(i)).toStdString());
            cv::cvtColor(wantedPerson, grayWantedPerson,CV_RGB2GRAY);
        }
//        cv::Mat grayWantedPerson = cv::imread((wantedPersonFolder+wantedPersonList.at(i)).toStdString(),CV_LOAD_IMAGE_GRAYSCALE);
        cv::Mat alignedFace;

//        cv::imwrite(parameter.dataPath.toStdString() + "a.bmp", grayWantedPerson);

        std::vector<cv::Point2d> landmarkPoints;
        cv::Point2d centerOfLandmarks; //not used
        fa.alignStasm4(grayWantedPerson, alignedFace, landmarkPoints, centerOfLandmarks);

        if (alignedFace.empty()) {
            commonTool.log("aligned face is empty ...");
            if (grayWantedPerson.cols == 200 && grayWantedPerson.rows == 250){
                alignedFace = grayWantedPerson;
            } else {
                commonTool.log("Error. Cannot align face");
                continue;
            }
        }
        wantedPersonsLandmarkPointsList.push_back(landmarkPoints);
        wantedPersons.push_back(alignedFace); 
        wantedPersonFileNames.push_back(wantedPersonList.at(i));
        cv::Mat emptySketch;
        wantedPersonsSynthesized.push_back(emptySketch);
      }

    //

    QStringList wantedPersonSketchList = getWantedPersonQStringListFromFile(parameter.dataPath + "completeAR/wantedSketch.txt");

    for(int i = 0; i < wantedPersonSketchList.size(); i++) {
        cv::Mat grayWantedPerson;
        {
            cv::Mat wantedPerson = cv::imread((wantedPersonFolder+wantedPersonSketchList.at(i)).toStdString());
            cv::cvtColor(wantedPerson, grayWantedPerson,CV_RGB2GRAY);

        }
        cv::Mat alignedSketch;
        std::vector<cv::Point2d> landmarkPoints;
        cv::Point2d centerOfLandmarks; //not used
        fa.alignStasm4(grayWantedPerson, alignedSketch, landmarkPoints, centerOfLandmarks);

        if (alignedSketch.empty()) {
            commonTool.log("aligned face is empty ...");
            if (grayWantedPerson.cols == 200 && grayWantedPerson.rows == 250){
                alignedSketch = grayWantedPerson;
            } else {
                commonTool.log("Error. Cannot align face");
                continue;
            }
        }

        wantedPersonsLandmarkPointsList.push_back(landmarkPoints);
        wantedPersons.push_back(alignedSketch);
        wantedPersonFileNames.push_back(wantedPersonSketchList.at(i));
        wantedPersonsSynthesized.push_back(alignedSketch);
    }
}

void QBorderMainWindow::onClickedInputPersonLabel(ClickableQLabel* label) {
    QString selectedFile = QFileDialog::getOpenFileName(this,
                                                        tr("Select the image file"), dataPath+"completeAR/",
                                                        tr("Image Files (*.png *.jpg *.bmp)"));
    if (selectedFile.size() != 0) {
        cv::Mat selectedGrayImg;
        {
            cv::Mat selectedImg = cv::imread(selectedFile.toStdString());
            cv::cvtColor(selectedImg, selectedGrayImg,CV_RGB2GRAY);
        }
        cv::Mat alignedFace;
        std::vector<cv::Point2d> landmarkPoints;
        FaceAlignment fa(parameter.dataPath.toStdString(), faceAlignMutex);
        cv::Point2d centerOfLandmarks; //not used
        fa.alignStasm4(selectedGrayImg, alignedFace, landmarkPoints, centerOfLandmarks);
        if (alignedFace.empty()){
            commonTool.log("Error importing input person ...");
        }
        inputPerson = alignedFace;
        QFileInfo f(selectedFile);
        inputPersonFileName = f.fileName();
        inputPersonLandmarkPoints = landmarkPoints;
        performFaceVerification();
    }
}

void QBorderMainWindow::onClickedCamera(ClickableQLabel* label) {
    if (currentlyCapturing) {
        stopCamera();
        ui.camera->setText("click to start capture");
    } else {
        startCamera();
    }
}

void QBorderMainWindow::onClickedMatchedPerson(ClickableQLabel* label) {
    openCVMutex.lock();
    cv::destroyWindow("Matched Person");
    openCVMutex.unlock();

    if (matchedIndex != -1) {
        cv::Mat matchedPerson = wantedPersons.at(matchedIndex);
        std::vector< cv::Point2d > matchedPersonLandmarkPoints = wantedPersonsLandmarkPointsList.at(matchedIndex);
        commonTool.showImageAndLandMarks(QString("Matched Person"), matchedPerson, matchedPersonLandmarkPoints, CV_RGB(255,0,0), openCVMutex);
    }

    openCVMutex.lock();
    cv::moveWindow("Matched Person", 400, 0);
    openCVMutex.unlock();
}

void QBorderMainWindow::onClickedInputPerson(ClickableQLabel* label) {
    openCVMutex.lock();
    cv::destroyWindow("Input Person");
    openCVMutex.unlock();

    commonTool.showImageAndLandMarks(QString("Input Person"), inputPerson, inputPersonLandmarkPoints, CV_RGB(255,0,0), openCVMutex);

    openCVMutex.lock();
    cv::moveWindow("Input Person", 200, 0);
    openCVMutex.unlock();
}

void QBorderMainWindow::onClickedWantedPerson(ClickableQLabel* label) {

    openCVMutex.lock();
    cv::destroyWindow("Wanted Person");
    cv::destroyWindow("Synthesized Wanted Person");
    cv::destroyWindow("Input Person");
    cv::destroyWindow("croppedInputPerson");
    cv::destroyWindow("croppedWantedPerson");
    cv::destroyWindow("scoreImageScale1");
    cv::destroyWindow("scoreImageScale2");
    cv::destroyWindow("scoreImageScale3");
    cv::destroyWindow("scoreImageScale4");
    openCVMutex.unlock();

    //commonTool.log(QString("ui.wantedPersons->indexOf(label) --> %1").arg(ui.wantedPersons->indexOf(label)));




    int i = ui.wantedPersons->indexOf(label)/2;
    commonTool.log(QString("i --> %1").arg(i));
    //if (sortedFaceVerificationResult.size()>0) {
    if (matchedIndex!=-1) {
        //a non partial match is above the treshold
        i = sortedFaceVerificationResult.at(sortedFaceVerificationResult.size()-1-i).i;
    } else if (partialMatchedIndex != -1) {
        // a non partial match is not above the threshold but partial is
        i = sortedPartialFaceVerificationResult.at(sortedPartialFaceVerificationResult.size()-1-i).i;
    }
    commonTool.log(QString("i --> %1").arg(i));

    //display the wanted person
    cv::Mat croppedWantedPerson = faceMatchingManager.cropImg(wantedPersons.at(i));
    commonTool.showImageAndLandMarks(QString("Wanted Person"), wantedPersons.at(i), wantedPersonsLandmarkPointsList.at(i), CV_RGB(255,0,0), openCVMutex);
    openCVMutex.lock();
    cv::moveWindow("Wanted Person", 0, 0);
    openCVMutex.unlock();

    //display the synthesized sketch if it is available!
    if (!wantedPersonsSynthesized.at(i).empty()) {
        croppedWantedPerson = faceMatchingManager.cropImg(wantedPersonsSynthesized.at(i));
        commonTool.showImageAndLandMarks(QString("Synthesized Wanted Person"), wantedPersonsSynthesized.at(i), wantedPersonsLandmarkPointsList.at(i), CV_RGB(255,0,0), openCVMutex);
    }
    openCVMutex.lock();
    cv::moveWindow("Synthesized Wanted Person", 0, 300);
    openCVMutex.unlock();

    //display the input person
    //display the cropped img that are used in face verification
    cv::Mat croppedInputPerson;
    if (!inputPerson.empty()) {
        commonTool.showImageAndLandMarks(QString("Input Person"), inputPerson, inputPersonLandmarkPoints, CV_RGB(255,0,0), openCVMutex);
        openCVMutex.lock();
        cv::moveWindow("Input Person", 200, 0);
        openCVMutex.unlock();

        croppedInputPerson = faceMatchingManager.cropImg(inputPerson);
        openCVMutex.lock();
        cv::namedWindow( "croppedInputPerson", CV_WINDOW_NORMAL );
        cv::imshow( "croppedInputPerson", croppedInputPerson );
        cv::moveWindow("croppedInputPerson", 600, 0);

        cv::namedWindow( "croppedWantedPerson", CV_WINDOW_NORMAL );
        cv::imshow( "croppedWantedPerson", croppedWantedPerson );
        cv::moveWindow("croppedWantedPerson", 800, 0);
        openCVMutex.unlock();

    }

    //visualize the match scores!
    //
    //x -> column
    //y -> row
    cv::Mat scoreImageScale1;
    cv::Mat scoreImageScale2;
    cv::Mat scoreImageScale3;
    cv::Mat scoreImageScale4;
    cv::cvtColor(wantedPersons.at(i), scoreImageScale1, CV_GRAY2BGR);
    cv::cvtColor(wantedPersons.at(i), scoreImageScale2, CV_GRAY2BGR);
    cv::cvtColor(wantedPersons.at(i), scoreImageScale3, CV_GRAY2BGR);
    cv::cvtColor(wantedPersons.at(i), scoreImageScale4, CV_GRAY2BGR);

    commonTool.log(QString("Matching score for subject %1 -->").arg(i));
    if (faceMatchingManager.enableMultiLevel && sortedFaceVerificationResult.size()>0) {
        FaceMatchingSingle singleMatcher = faceMatchingManager.controllerMulti->faceMatchingControllers.at(0);
        cv::putText(scoreImageScale1, QString("%1").arg((int)faceVerificationResultRaw.at(i).at(0)).toStdString() ,cv::Point( singleMatcher.colRange.start, singleMatcher.rowRange.end), cv::FONT_HERSHEY_PLAIN, 2.0, cv::Scalar( 0, 0, 255 ));
        cv::rectangle(scoreImageScale1, cv::Point( singleMatcher.colRange.start, singleMatcher.rowRange.start), cv::Point( singleMatcher.colRange.end, singleMatcher.rowRange.end), cv::Scalar( 0, 0, 255 ));
        commonTool.log(QString("similarity [%1, %2, %3, %4] ---> %5").arg(singleMatcher.rowRange.start).arg(singleMatcher.rowRange.end).arg(singleMatcher.colRange.start).arg(singleMatcher.colRange.end).arg(faceVerificationResultRaw.at(i).at(0)));


        for (int ii=1;ii<3;ii++) {
            FaceMatchingSingle singleMatcher = faceMatchingManager.controllerMulti->faceMatchingControllers.at(ii);
            cv::putText(scoreImageScale2, QString("%1").arg((int)faceVerificationResultRaw.at(i).at(ii)).toStdString() ,cv::Point( singleMatcher.colRange.start, singleMatcher.rowRange.end), cv::FONT_HERSHEY_PLAIN, 2.0, cv::Scalar( 0, 0, 255 ));
            cv::rectangle(scoreImageScale2, cv::Point( singleMatcher.colRange.start, singleMatcher.rowRange.start), cv::Point( singleMatcher.colRange.end, singleMatcher.rowRange.end), cv::Scalar( 0, 0, 255 ));
            commonTool.log(QString("similarity [%1, %2, %3, %4] ---> %5").arg(singleMatcher.rowRange.start).arg(singleMatcher.rowRange.end).arg(singleMatcher.colRange.start).arg(singleMatcher.colRange.end).arg(faceVerificationResultRaw.at(i).at(ii)));
        }

        for (int ii=3;ii<7;ii++) {
            FaceMatchingSingle singleMatcher = faceMatchingManager.controllerMulti->faceMatchingControllers.at(ii);
            cv::putText(scoreImageScale3, QString("%1").arg((int)faceVerificationResultRaw.at(i).at(ii)).toStdString() ,cv::Point( singleMatcher.colRange.start, singleMatcher.rowRange.end), cv::FONT_HERSHEY_PLAIN, 2.0, cv::Scalar( 0, 0, 255 ));
            cv::rectangle(scoreImageScale3, cv::Point( singleMatcher.colRange.start, singleMatcher.rowRange.start), cv::Point( singleMatcher.colRange.end, singleMatcher.rowRange.end), cv::Scalar( 0, 0, 255 ));
            commonTool.log(QString("similarity [%1, %2, %3, %4] ---> %5").arg(singleMatcher.rowRange.start).arg(singleMatcher.rowRange.end).arg(singleMatcher.colRange.start).arg(singleMatcher.colRange.end).arg(faceVerificationResultRaw.at(i).at(ii)));
        }

        for (int ii=7;ii<(int)faceVerificationResultRaw.at(i).size();ii++) {
            FaceMatchingSingle singleMatcher = faceMatchingManager.controllerMulti->faceMatchingControllers.at(ii);
            cv::putText(scoreImageScale4, QString("%1").arg((int)faceVerificationResultRaw.at(i).at(ii)).toStdString() ,cv::Point( singleMatcher.colRange.start, singleMatcher.rowRange.end), cv::FONT_HERSHEY_PLAIN, 2.0, cv::Scalar( 0, 0, 255 ));
            cv::rectangle(scoreImageScale4, cv::Point( singleMatcher.colRange.start, singleMatcher.rowRange.start), cv::Point( singleMatcher.colRange.end, singleMatcher.rowRange.end), cv::Scalar( 0, 0, 255 ));
            commonTool.log(QString("similarity [%1, %2, %3, %4] ---> %5").arg(singleMatcher.rowRange.start).arg(singleMatcher.rowRange.end).arg(singleMatcher.colRange.start).arg(singleMatcher.colRange.end).arg(faceVerificationResultRaw.at(i).at(ii)));
        }
    }

    if (sortedFaceVerificationResult.size()>0) {
        commonTool.log(QString("<-- Matching score for subject %1").arg(i));
        cv::namedWindow( "scoreImageScale1", CV_WINDOW_NORMAL );
        cv::imshow( "scoreImageScale1",  scoreImageScale1);
        cv::moveWindow("scoreImageScale1", 400, 200);

        cv::namedWindow( "scoreImageScale2", CV_WINDOW_NORMAL );
        cv::imshow( "scoreImageScale2",  scoreImageScale2);
        cv::moveWindow("scoreImageScale2", 600, 200);

        cv::namedWindow( "scoreImageScale3", CV_WINDOW_NORMAL );
        cv::imshow( "scoreImageScale3",  scoreImageScale3);
        cv::moveWindow("scoreImageScale3", 800, 200);

        cv::namedWindow( "scoreImageScale4", CV_WINDOW_NORMAL );
        cv::imshow( "scoreImageScale4",  scoreImageScale4);
        cv::moveWindow("scoreImageScale4", 1000, 200);
    }
}

void QBorderMainWindow::updateUI() {
    commonTool.log("updateUI");
    ui.lab_result->setStyleSheet("");
    ui.lab_result->setText("");
    if (matchedIndex!=-1) {
        //found a full match above the threshold
        ui.lab_result->setStyleSheet("background-color:red; color : black;");
        ui.lab_result->setText("Potential Match Found");
    } else if (partialMatchedIndex!=-1) {
        //Did not find a full match above the threshold, but did find a partial match
        ui.lab_result->setStyleSheet("background-color:yellow; color : black;");
        ui.lab_result->setText("Partial Match Found");
    } else {
        //Did not find a full match or a partial match above the threshold
        //ui.lab_result->setStyleSheet("background-color:rgba(0, 0, 0, 0); color : black;");
        ui.lab_result->setStyleSheet("background-color:green; color : black;");
        ui.lab_result->setText("No Match Found");
    }

    //update the wanted persons
    QLayoutItem *child;
    while ((child = ui.wantedPersons->takeAt(0)) != 0)  {
        delete child;
    }

    //face verification has already been performed, therefore sort the result ...
    if (matchedIndex!=-1) {
        //found a non partial match above the threshold
        for (int i=(int)wantedPersons.size()-1;i>=0;i--) {
            ClickableQLabel* label = new ClickableQLabel();
            connect(label, &ClickableQLabel::clicked, this, &QBorderMainWindow::onClickedWantedPerson);
            label->setPixmap(QPixmap::fromImage(Mat2QImage(wantedPersons.at(sortedFaceVerificationResult.at(i).i))));
            label->setAutoFillBackground(true);
            label->setToolTip(wantedPersonFileNames.at(sortedFaceVerificationResult.at(i).i));
            ui.wantedPersons->addWidget(label);
            QLabel* l = new QLabel();
            l->setStyleSheet("qproperty-alignment: AlignCenter;");
            l->setText(QString("Subject No.: %2 \nSimilarity Score: *%1\nPart. Similarity Score: %3\nPart. Confidence: %4\n ").arg(faceVerificationResult.at(sortedFaceVerificationResult.at(i).i).d).arg(sortedFaceVerificationResult.at(i).i).arg(partialFaceVerificationResult.at(sortedFaceVerificationResult.at(i).i).d).arg(partialFaceVerificationResult.at(sortedFaceVerificationResult.at(i).i).confidence));
            l->setAutoFillBackground(true); //important
            ui.wantedPersons->addWidget(l);
        }
    } else if (partialMatchedIndex!=-1) {
        for (int i=(int)wantedPersons.size()-1;i>=0;i--) {
            ClickableQLabel* label = new ClickableQLabel();
            connect(label, &ClickableQLabel::clicked, this, &QBorderMainWindow::onClickedWantedPerson);
            label->setPixmap(QPixmap::fromImage(Mat2QImage(wantedPersons.at(sortedPartialFaceVerificationResult.at(i).i))));
            label->setAutoFillBackground(true);
            label->setToolTip(wantedPersonFileNames.at(sortedPartialFaceVerificationResult.at(i).i));
            ui.wantedPersons->addWidget(label);
            QLabel* l = new QLabel();
            l->setStyleSheet("qproperty-alignment: AlignCenter;");
            l->setText(QString("Subject No.: %2 \nSimilarity Score: %1\nPart. Similarity Score: *%3\nPart. Confidence: %4\n").arg(faceVerificationResult.at(sortedPartialFaceVerificationResult.at(i).i).d).arg(sortedPartialFaceVerificationResult.at(i).i).arg(partialFaceVerificationResult.at(sortedPartialFaceVerificationResult.at(i).i).d).arg(partialFaceVerificationResult.at(sortedPartialFaceVerificationResult.at(i).i).confidence));
            l->setAutoFillBackground(true); //important
            ui.wantedPersons->addWidget(l);
        }
    } else {
        for (int i=0;i<(int)wantedPersons.size();i++) {
            ClickableQLabel* label = new ClickableQLabel();
            connect(label, &ClickableQLabel::clicked, this, &QBorderMainWindow::onClickedWantedPerson);
            label->setPixmap(QPixmap::fromImage(Mat2QImage(wantedPersons.at(i))));
            label->setAutoFillBackground(true);
            label->setToolTip(wantedPersonFileNames.at(i));
            ui.wantedPersons->addWidget(label);
            QLabel* l = new QLabel();
            l->setStyleSheet("qproperty-alignment: AlignCenter;");
            l->setText(QString("Subject No.: *%1 \nSimilarity Score: n/a\nPart. Similarity Score: n/a\nPart. Confidence: n/a\n ").arg(i));
            l->setAutoFillBackground(true); //important
            ui.wantedPersons->addWidget(l);
        }
    }


    //update the input person
    if (!inputPerson.empty()) {
        ui.inputPerson->setPixmap(QPixmap::fromImage(Mat2QImage(inputPerson)));
        ui.inputPerson->setToolTip(inputPersonFileName);
    }
    //update the matched person
    if(matchedIndex!=-1) {
        cv::Mat matchedPerson = wantedPersons.at(matchedIndex);
        ui.matchedPerson->setPixmap(QPixmap::fromImage(Mat2QImage(matchedPerson)));
        ui.matchedPersonLabel->setText(QString("Subject No.: %2\nSimilarity Score: %1").arg(faceVerificationResult.at(matchedIndex).d).arg(matchedIndex));
    } else if (partialMatchedIndex!=-1) {
        cv::Mat matchedPerson = wantedPersons.at(partialMatchedIndex);
        ui.matchedPerson->setPixmap(QPixmap::fromImage(Mat2QImage(matchedPerson)));
        ui.matchedPersonLabel->setText(QString("Subject No.: %2\nPart. Similarity Score: %1\n Part. Confidence: %3\n").arg(partialFaceVerificationResult.at(partialMatchedIndex).d).arg(partialMatchedIndex).arg(partialFaceVerificationResult.at(partialMatchedIndex).confidence));
    } else {
        ui.matchedPerson->setAlignment(Qt::AlignCenter);
        ui.matchedPerson->setText("n/a");
        ui.matchedPersonLabel->setText(QString(""));
    }
}

void QBorderMainWindow::startCamera() {

    // initialize camera
    commonTool.log(QString("Initialize Camera ..."));
    cameraVideoCapture = cv::VideoCapture(0); //cameraID = 0;

    if(!cameraVideoCapture.isOpened()){ // check if we succeeded
        QMessageBox msgBox;
        msgBox.setText("Camera cannot be initialize. Please check and try again!");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
        return;
    }

    cameraTimer = new QTimer();
    connect(cameraTimer, SIGNAL(timeout()), this, SLOT(updateCamera()));
    cameraTimer->start(100);
    currentlyCapturing = true;
}

void QBorderMainWindow::stopCamera() {
    currentlyCapturing = false;
    cameraTimer->stop();
    cameraVideoCapture.release();
}

void QBorderMainWindow::updateCamera() {
    openCVMutex.lock();
    cv::Mat cameraImgRGB;
    cv::Mat cameraImgGray;
    cameraVideoCapture >> cameraImgRGB; // get a new frame from camera
    cv::cvtColor(cameraImgRGB, cameraImgGray, CV_RGB2GRAY);
    cv::flip(cameraImgGray, cameraImgGray, 1);
    cv::Mat cameraImgGraySmall(cameraImgGray.size().height/2, cameraImgGray.size().width/2, CV_8UC1);
    cv::resize(cameraImgGray, cameraImgGraySmall,cameraImgGraySmall.size());
    ui.camera->setPixmap(QPixmap::fromImage(Mat2QImage(cameraImgGraySmall)));
    openCVMutex.unlock();
}

void QBorderMainWindow::keyPressEvent(QKeyEvent* e) {
    commonTool.log("Key Pressed!");
    if (e->key() == 'c' || e->key() == 'C' ) {
        cv::Mat cameraImgRGB;
        cv::Mat cameraImgGray;
        openCVMutex.lock();
        cameraVideoCapture >> cameraImgRGB; // get a new frame from camera
        openCVMutex.unlock();
        cv::cvtColor(cameraImgRGB, cameraImgGray, CV_RGB2GRAY);
        cv::flip(cameraImgGray, cameraImgGray, 1);
        FaceAlignment fa(parameter.dataPath.toStdString(), faceAlignMutex);
        std::vector<std::vector<cv::Point2d> > detectedLandmarks;
        std::vector<cv::Point2d> centers;
        fa.detectLandmarks(cameraImgGray, detectedLandmarks, centers);
        std::vector<cv::Point2d> landmarks;
        if (detectedLandmarks.size() > 0) {
            landmarks = detectedLandmarks.at(0);
        } else {
            commonTool.log("No landmarks found");
        }
        commonTool.showImageAndLandMarks(QString("Captured Snapshot"), cameraImgGray, landmarks, CV_RGB(255, 0, 0), openCVMutex);
        int key = cv::waitKey(0);
        commonTool.log(QString("%1").arg(key));
        if (key==32) { //space
            //keep this image

            std::vector<cv::Point2d> landmarkPoints;
            cv::Point2d centerOfLandmarks; //not used
            cv::Mat alignedFace;
            fa.alignStasm4(cameraImgGray, alignedFace, landmarkPoints, centerOfLandmarks);

            if (alignedFace.empty()) {
                commonTool.log("aligned face is empty ...");
                commonTool.log("Error. Cannot align face");
            }
            inputPersonLandmarkPoints = landmarkPoints;
            inputPerson = alignedFace;

            commonTool.log("Keep this image");
        } else {
            //don't want this image
            commonTool.log("Dont keep this image");

        }
        cv::destroyWindow("Captured Snapshot");
        if (!inputPerson.empty()) {
            performFaceVerification();
        }
        updateUI();
    }
}

void QBorderMainWindow::trainFaceVerification(std::vector<cv::Mat> additionalImages, QMutex& openCVMutex) {
    QString faceRecogDBPath = dataPath + "/" + QString::fromStdString(FaceMatchingManager::trainingDirectory);
    faceMatchingManager.train(faceRecogDBPath, additionalImages, openCVMutex);
}

void QBorderMainWindow::trainFaceSynthesize() {
    commonTool.log("QBorderMainWindow::trainFaceSynthesis()");
    QString sketchPath = parameter.dataPath + "/" + SKETCH_DIR;
    QString photoPath = parameter.dataPath + "/" + PHOTO_DIR;
    faceSynthesisManager.setParameter(parameter);
    faceSynthesisManager.setPath(photoPath, sketchPath);
    faceSynthesisManager.setDirectionSketchToPhoto();
    faceSynthesisManager.train();
}

void QBorderMainWindow::faceSynthesizeCompleted() {
    int ii=0;
    for (int i=0;i<(int)wantedPersonsSynthesized.size();i++) {
        if (!wantedPersonsSynthesized.at(i).empty()) {
            cv::Mat img = faceSynthesisManager.synthesizedImgs.at(ii++);
            wantedPersonsSynthesized.at(i) = img;
        }
    }
    performFaceVerification();
}

void QBorderMainWindow::trainFaceSynthesizeCompleted() {
    commonTool.log("QBorderMainWindow::trainFaceSynthesizeCompleted()");
    //synthesize the face now
    std::vector<cv::Mat> sketches;
    for (int i=0;i<(int)wantedPersonsSynthesized.size();i++) {
        if (!wantedPersonsSynthesized.at(i).empty()) {
            sketches.push_back(wantedPersonsSynthesized.at(i));
        }
    }
    faceSynthesisManager.synthesize(sketches);
}

void QBorderMainWindow::performFaceVerification() {
    //verify each input face with the wantedPersons.
    commonTool.log("Performing face verification");
    matchedIndex = -1;
    partialMatchedIndex = -1;
    if (!inputPerson.empty()) {
        faceVerificationResult.clear();
        partialFaceVerificationResult.clear();
        sortedPartialFaceVerificationResult.clear();
        sortedFaceVerificationResult.clear();
        faceVerificationResultRaw.clear();
        for (int i=0;i<(int)wantedPersons.size();i++) {
            std::vector<double> similarities;
            if (!wantedPersonsSynthesized.at(i).empty()) {
                similarities = faceMatchingManager.getNormalizedSimilarity(wantedPersonsSynthesized.at(i), inputPerson);
                commonTool.log(QString("%1 -> %2*").arg(i).arg(similarities.at(0)));
            } else {
                similarities = faceMatchingManager.getNormalizedSimilarity(wantedPersons.at(i), inputPerson);
                commonTool.log(QString("%1 -> %2").arg(i).arg(similarities.at(0)));
            }
            //
            commonTool.log(QString("numberOfSimilarityScores = %1").arg(similarities.size()));
            //
            if (faceMatchingManager.enableMultiLevel) {
                for (int i=0;i<(int)similarities.size();i++) {
                    FaceMatchingSingle singleMatcher = faceMatchingManager.controllerMulti->faceMatchingControllers.at(i);
                    commonTool.log(QString("similarity [%1, %2, %3, %4] ---> %5").arg(singleMatcher.rowRange.start).arg(singleMatcher.rowRange.end).arg(singleMatcher.colRange.start).arg(singleMatcher.colRange.end).arg(similarities.at(i)));
                }
            }
            //
            IntDouble full;
            full.i = i;
            full.confidence = 100.0;
            full.d = similarities.at(0);
            faceVerificationResult.push_back(full);
            faceVerificationResultRaw.push_back(similarities);
            //partial score ...
            //
            IntDouble partial;
            partial.i = i;
            partial.confidence = 0.0;
            partial.d = 0.0;
            int c=0;
            if (similarities.at(0) > faceMatchingManager.alarmThreshold) {
                c++;
                partial.confidence += 100.0;
                partial.d += similarities.at(0);
            }
            if (c==0) {
                if (similarities.at(1) > faceMatchingManager.alarmThreshold) {
                    c++;
                    partial.confidence += 50.0;
                    partial.d += similarities.at(1);
                }
                if (similarities.at(2) > faceMatchingManager.alarmThreshold) {
                    c++;
                    partial.confidence += 50.0;
                    partial.d += similarities.at(2);
                } if (c==0) {
                    if (similarities.at(3) > faceMatchingManager.alarmThreshold) {
                        c++;
                        partial.confidence += 25.0;
                        partial.d += similarities.at(3);
                    }
                    if (similarities.at(4) > faceMatchingManager.alarmThreshold) {
                        c++;
                        partial.confidence += 25.0;
                        partial.d += similarities.at(4);
                    }
                    if (similarities.at(5) > faceMatchingManager.alarmThreshold) {
                        c++;
                        partial.confidence += 25.0;
                        partial.d += similarities.at(5);
                    }
                    if (similarities.at(6) > faceMatchingManager.alarmThreshold) {
                        c++;
                        partial.confidence += 25.0;
                        partial.d += similarities.at(6);
                    }
                    if (c==0) {
                        if (similarities.at(7) > faceMatchingManager.alarmThreshold) {
                            c++;
                            partial.confidence += 100.0/8.0;
                            partial.d += similarities.at(7);
                        }
                        if (similarities.at(8) > faceMatchingManager.alarmThreshold) {
                            c++;
                            partial.confidence += 100.0/8.0;
                            partial.d += similarities.at(8);
                        }
                        if (similarities.at(9) > faceMatchingManager.alarmThreshold) {
                            c++;
                            partial.confidence += 100.0/8.0;
                            partial.d += similarities.at(9);
                        }
                        if (similarities.at(10) > faceMatchingManager.alarmThreshold) {
                            c++;
                            partial.confidence += 100.0/8.0;
                            partial.d += similarities.at(10);
                        }
                        if (similarities.at(11) > faceMatchingManager.alarmThreshold) {
                            c++;
                            partial.confidence += 100.0/8.0;
                            partial.d += similarities.at(11);
                        }
                        if (similarities.at(12) > faceMatchingManager.alarmThreshold) {
                            c++;
                            partial.confidence += 100.0/8.0;
                            partial.d += similarities.at(12);
                        }
                        if (similarities.at(13) > faceMatchingManager.alarmThreshold) {
                            c++;
                            partial.confidence += 100.0/8.0;
                            partial.d += similarities.at(13);
                        }
                        if (similarities.at(14) > faceMatchingManager.alarmThreshold) {
                            c++;
                            partial.confidence += 100.0/8.0;
                            partial.d += similarities.at(14);
                        }
                        if (c==0) {
                            //no more levels
                        } else {
                            partial.d /= c;
                        }
                    } else {
                        partial.d /= c;
                    }
                } else {
                    partial.d /= c;
                }
            }
            partialFaceVerificationResult.push_back(partial);
            //
        }

        //sort
        sortedFaceVerificationResult = faceVerificationResult;
        std::sort(sortedFaceVerificationResult.begin(), sortedFaceVerificationResult.end(), intDouble_less());

        sortedPartialFaceVerificationResult = partialFaceVerificationResult;
        std::sort(sortedPartialFaceVerificationResult.begin(), sortedPartialFaceVerificationResult.end(), intDouble_less());

        //get the highest Score! (necessary?)
        matchedIndex = sortedFaceVerificationResult.back().i;
        if (faceVerificationResult.at(matchedIndex).d < faceMatchingManager.alarmThreshold)
            matchedIndex = -1;

        partialMatchedIndex = sortedPartialFaceVerificationResult.back().i;
        if (partialFaceVerificationResult.at(partialMatchedIndex).d < faceMatchingManager.alarmThreshold)
            partialMatchedIndex = -1;

        commonTool.log(QString("matchedIndex --> %1").arg(matchedIndex));
        commonTool.log(QString("partialMatchedIndex --> %1").arg(partialMatchedIndex));

    }
    updateUI();
}
