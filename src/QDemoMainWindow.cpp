#include "QDemoMainWindow.h"

using namespace std;
const int NUM_QUERY_FRAME = 2;

const int FRAME_WIDTH = 960;
const int FRAME_HEIGHT = 720;

const QString CAPTURED_IMAGE_DIR = "capturedImage";
const QString CAPTURED_IMAGE_EXT = ".png";


QDemoMainWindow::QDemoMainWindow(Parameter parameter_p, QWidget *parent, Qt::WindowFlags flags)
	: QMainWindow(parent, flags)
{

    generalManager = new GeneralManager();
	generalManager->parameter = parameter_p;
    QMutex openCVMutex;
    generalManager->train(openCVMutex);

    ui.setupUi(this);


    inputImageScene = new QGraphicsScene(0);
    outputImageScene = new QGraphicsScene(0);
    faceRecognitionScene = new QGraphicsScene(0);

	ui.grv_inputImage->setScene(inputImageScene);
    ui.grv_outputImage->setScene(outputImageScene);
    ui.grv_outputFaceRecognition->setScene(faceRecognitionScene);


	inputImagePixmap = NULL;
    outputImagePixmap = NULL;
    faceRecogImagePixmap = NULL;


    inputImagePatchRect = NULL;
    outputImagePatchRect = NULL;

	candidatesWindow = NULL;
	
	//maxSearchWin = new QRect();
	//maxSearchWinOrgPatchRect = new QRect();

//	candidatePatchsLayout = NULL;
//    candidatePatchsWidget = NULL;

    QObject::connect(                
				ui.grv_inputImage, SIGNAL(scaleChanged(qreal)),
                ui.grv_outputImage, SLOT(setScale(qreal)));
    QObject::connect(
                ui.grv_outputImage, SIGNAL(scaleChanged(qreal)),
                ui.grv_inputImage, SLOT(setScale(qreal)));
    QObject::connect(
                ui.grv_inputImage, SIGNAL(mouseClicked(QPointF )),
                this, SLOT(imageGraphicsViewClicked(QPointF )));
    QObject::connect(
                ui.grv_outputImage, SIGNAL(mouseClicked(QPointF )),
                this, SLOT(imageGraphicsViewClicked(QPointF )));
	QObject::connect(
                ui.grv_inputImage, SIGNAL(horizontalScrollBarPosChangedSignal(int)),
                ui.grv_outputImage, SLOT(setHorizontalScrollBarPos(int)));
	QObject::connect(
                ui.grv_inputImage, SIGNAL(verticalScrollBarPosChangedSignal(int)),
                ui.grv_outputImage, SLOT(setVerticalScrollBarPos(int)));
    QObject::connect(
                ui.grv_outputImage, SIGNAL(horizontalScrollBarPosChangedSignal(int)),
                ui.grv_inputImage, SLOT(setHorizontalScrollBarPos(int)));
	QObject::connect(
                ui.grv_outputImage, SIGNAL(verticalScrollBarPosChangedSignal(int)),
                ui.grv_inputImage, SLOT(setVerticalScrollBarPos(int)));



	QObject::connect(
				ui.but_FaceRecognition, SIGNAL( clicked() ),
                this, SLOT(on_but_faceRecogintion_clicked()));

	QObject::connect(
				ui.act_preferences, SIGNAL( triggered() ),
                this, SLOT(on_act_preferences_actived()));

	QObject::connect(
				ui.act_saveImg, SIGNAL( triggered() ),
                this, SLOT(on_act_saveImg_actived()));

	QObject::connect(
				ui.act_saveInputImg, SIGNAL( triggered() ),
                this, SLOT(on_act_saveImputImg_actived()));

	QObject::connect(
				ui.lsw_recognitionResult, SIGNAL(currentRowChanged (int)),
				this, SLOT(showFaceRecogResult(int)));

	QObject::connect(generalManager, SIGNAL(updateUI()), this, SLOT(updateUISlot()));



    //New the stateBarText
	faceRecognitionDatabaseStateBarText = new QLabel();
	faceSynthesisDatabaseStateBarText = new QLabel();

	//New the timeNeededText
	timeNeededText = new QLabel();

	//New the maxSearchText
	maxSearchText = new QLabel();

    updateUI();
	on_act_preferences_actived();
	

	
}

QDemoMainWindow::~QDemoMainWindow()
{

}


void QDemoMainWindow::on_but_chooseInputImage_clicked()
{
    QString inputImagePath = QFileDialog::getOpenFileName( this, "Input Image",
                                                   "G:/13-14 Sem_2/Project/CUFS/1_testing/photos", "Images (*.png *.bmp *.jpg)",
                                                   0, 0 );

    if (inputImagePath.isNull()){
		return;
	}

	bool readable = generalManager->setInputImagePath(inputImagePath.toStdString());
	if (!readable){
		QMessageBox msgBox;
		msgBox.setText("ERROR on reading image");
		msgBox.exec();

		return;
	}

    updateUI();
}

void QDemoMainWindow::on_but_createSketch_clicked()
{


    QString errorString;
    errorString = generalManager->MWFStart();


    if (errorString.isEmpty()){

        updateUI();
    } else{
        QMessageBox::critical(this, "Error", errorString, QMessageBox::Ok, QMessageBox::Ok);
    }

}



void QDemoMainWindow::updateUI(){
    
    //ui.lab_photoPath->setText(generalManager->photoPath);
    //ui.lab_sketchPath->setText(generalManager->sketchPath);


    //grv_inputImage
    inputImagePatchRect = NULL;
	QList<QGraphicsItem*> items = inputImageScene->items();
	while (!items.empty())
	{
		inputImageScene->removeItem(items.first());
		delete items.first();
		items.removeFirst();
	}
	if(generalManager->showInputImg){
		
        inputImagePixmap = new QGraphicsPixmapItem(QPixmap::fromImage(generalManager->inputQImage));
        inputImageScene->addItem(inputImagePixmap);
        ui.grv_inputImage->show();
    }
       
	//grv_outputImage
	//outputImageScene->clear();
	outputImagePatchRect = NULL;
	items = outputImageScene->items();
	while (!items.empty())
	{
		outputImageScene->removeItem(items.first());
		delete items.first();
		items.removeFirst();
	}
	if(generalManager->showOutputImg){
		
        outputImagePixmap = new QGraphicsPixmapItem(QPixmap::fromImage(generalManager->outputQImage));
		outputImageScene->addItem(outputImagePixmap);
        ui.grv_outputImage->show();
    }

    //grv_outputFaceRecognition
    //faceRecognitionScene->clear();
    //ui.grv_outputFaceRecognition->setScene(NULL);
	items = faceRecognitionScene->items();
	while (!items.empty())
	{
		faceRecognitionScene->removeItem(items.first());
		delete items.first();
		items.removeFirst();
	}
	if(generalManager->showFaceRecogImg){
		
        faceRecogImagePixmap = new QGraphicsPixmapItem(QPixmap::fromImage(generalManager->faceRecognitionQImage));
		faceRecognitionScene->addItem(faceRecogImagePixmap);
        ui.grv_outputFaceRecognition->show();
    }



    //lab_trainedData
	int trainingDataSize = generalManager->numberOfTrainedImage;
    if (trainingDataSize == 0){
		faceSynthesisDatabaseStateBarText->setText("Face Synthesis Database loading.");
    } else if (trainingDataSize > 0){
		faceSynthesisDatabaseStateBarText->setText("Face Synthesis Database size: " + QString::number(trainingDataSize));
    }
	statusBar()->addWidget(faceSynthesisDatabaseStateBarText);


	int faceRecogDataSize = generalManager->numberOfFaceRecogDBImage;
    if (faceRecogDataSize == 0){
		faceRecognitionDatabaseStateBarText->setText("Face Recognition Database loading.");
    } else if (faceRecogDataSize > 0){
		faceRecognitionDatabaseStateBarText->setText("Face Recognition Database size: " + QString::number(faceRecogDataSize));
    }
	statusBar()->addWidget(faceRecognitionDatabaseStateBarText);

	//synthesis button
	if (trainingDataSize == 0){
		ui.but_createSketch->setEnabled(false);
    } else if (trainingDataSize > 0){
		ui.but_createSketch->setEnabled(true);
	}

	//face recognition button
    if (faceRecogDataSize == 0){
		ui.but_FaceRecognition->setEnabled(false);
    } else if (faceRecogDataSize > 0){
		ui.but_FaceRecognition->setEnabled(true);
    }
	

	//time for sym
	int timeNeedForKNN = generalManager->timeNeedForFindKNN;
	int timeNeedForMinEng = generalManager->timeNeedForMinEng;
	int timeNeedForFaceRecog = generalManager->timeNeedForFaceRecog;
	QString timeNeedQString = "";

	if (timeNeedForKNN != -1 && timeNeedForMinEng != -1){
		timeNeedQString += "Find candidate patches: " + QString::number(timeNeedForKNN) + "ms ";
		timeNeedQString += "/ Compute the weight: " + QString::number(timeNeedForMinEng) + "ms ";
	}
	if (timeNeedForFaceRecog != -1){
		timeNeedQString += "/ Face Recognition: " + QString::number(timeNeedForFaceRecog) + "ms ";
	}

	if (timeNeedQString.size() > 0){
		timeNeededText->setText(timeNeedQString);
		statusBar()->addWidget(timeNeededText);
	}

	//ui.lsw_recognitionResult
	if (generalManager->faceRecogResultChanged){
		disconnect_lsw_faceRecogResult = true;
		while (ui.lsw_recognitionResult->count() > 0){
			QListWidgetItem * item = ui.lsw_recognitionResult->item(0);
			if (item != NULL){
				delete item;
			}
		}
		disconnect_lsw_faceRecogResult = false;
		ui.lsw_recognitionResult->addItems(generalManager->getFaceRecogResult());
		generalManager->faceRecogResultChanged = false;
	}

	
}






void QDemoMainWindow::on_but_save_clicked()
{
	
    QString savePath = QFileDialog::getSaveFileName(NULL, "Save","./output.jpg", tr("Images (*.png *.jpg *.bmp)") );
    bool saveResult;
    if (savePath.isNull() == false){
       // saveResult = generalManager->outputImage.save(savePath);
		saveResult = generalManager->saveOutputImage(savePath.toStdString());

        if (saveResult == false){
            QMessageBox::critical(this, "Error", "Cannot save the file", QMessageBox::Ok, QMessageBox::Ok);
        }
    }
    updateUI();
	

}



void QDemoMainWindow::imageGraphicsViewClicked(QPointF scenePoint){
    // no need to show anything if no input or output images
    if (generalManager->inputQImage.isNull() || generalManager->outputQImage.isNull()){
        //cout << "Please select an image or do synthesizing first" << endl;
        return;
    }

    //remove all previous item
	/*
	if (candidatePatchsLayout != NULL){
		QLayoutItem* item;
		while ( ( item = candidatePatchsLayout ->takeAt( 0 ) ) != NULL )
		{
			delete item->widget();
			delete item;
		}
	}
	*/
    if (inputImagePatchRect != NULL){
        inputImageScene->removeItem(inputImagePatchRect);
        delete inputImagePatchRect;
    }
    if (outputImagePatchRect != NULL){
        outputImageScene->removeItem(outputImagePatchRect);
        delete outputImagePatchRect;
    }


    // add a rect to the both image
    QPoint patchCoordinate = generalManager->getPatchCoordinate(scenePoint);
    QRect patchRect = generalManager->getPatchRect(patchCoordinate);

	//get the patch from input image
	cv::Mat imagePatch = generalManager->getPatchFromInputImage(patchRect);

    inputImagePatchRect = inputImageScene->addRect(patchRect, QPen(Qt::yellow), QBrush(Qt::transparent));
    outputImagePatchRect = outputImageScene->addRect(patchRect, QPen(Qt::yellow), QBrush(Qt::transparent));;

	/*
	if (candidatePatchsLayout != NULL){
		delete candidatePatchsLayout;
		candidatePatchsLayout = NULL;
	}
	
	if (candidatePatchsWidget != NULL){
		delete candidatePatchsWidget;
		candidatePatchsWidget = NULL;
	}
	*/
	//candidatePatchsLayout = new QVBoxLayout();
    //candidatePatchsWidget = new QWidget();

    //candidatePatchsWidget->setLayout(candidatePatchsLayout);

    CandidatePatchSet* result = generalManager->getCandidatePatchSet(scenePoint);

	if (candidatesWindow != NULL){
		delete candidatesWindow;
		candidatesWindow = NULL;
	}



	//candidatesWindow = new windowcandidates(candidatePatchsWidget,Mat2QImage(imagePatch), Mat2QImage(combinedPatchSourceImage), Mat2QImage(combinedPatchTargetImage),RMSValue);
	candidatesWindow = new QCandidatesWindow(result, imagePatch);
	//candidatesWindow->setGeometry(this->geometry().x() + this->width() /*- candidatesWindow->width()*/, this->geometry().y() , 0, 0);
	candidatesWindow->show();
	connect(candidatesWindow, SIGNAL(changeImage(int, int, QRect)), this, SLOT(changeCandidatePatchOrgImg(int, int, QRect)));

    //computer which patch

    //call the Candidate Patch Scroll Area and give the
	
}


void QDemoMainWindow::on_but_captureImage_clicked()
{

	QMessageBox msgBox;
    msgBox.setText("Press 'c' to capture an image, press 'q' to quit the capture process.");
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();

    CvCapture *capture = 0;
    IplImage  *frame = 0;
	IplImage  *tempframe = 0;
    int       key = 0;

	//whiteFrame
	IplImage  *whiteFrame = 0;
	whiteFrame = cvCreateImage(cvSize(100, 100), IPL_DEPTH_8U, 1);
	for ( int i = 0; i < 10000; i++){
		whiteFrame->imageData[i] = 255;
	}

	//create dir
	QDir capturedImageDir(generalManager->parameter.dataPath);
	if (!capturedImageDir.cd(CAPTURED_IMAGE_DIR)){
		if (!capturedImageDir.mkdir(CAPTURED_IMAGE_DIR)){
			return;
		}
		if (!capturedImageDir.cd(CAPTURED_IMAGE_DIR)){
			return;
		}
	}

    // initialize camera 
    capture = cvCaptureFromCAM(0);

	cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH, 99999 );
	cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT, 99999 );

    int frameWidth = cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH);
    int frameHeight = cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT);

    // always check 
    if ( !capture ) {
        fprintf( stderr, "Cannot open initialize webcam!\n" );
        return;
    }

    // create a window for the video 
    cvNamedWindow("camera window", CV_WINDOW_NORMAL );
	cvSetWindowProperty("camera window", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
	cvSetWindowProperty("camera window", CV_WND_PROP_ASPECTRATIO, CV_WINDOW_KEEPRATIO);

    while( key != 'q') {
        // get a frame 
        frame = cvQueryFrame( capture );

        // always check 
        if( !frame ) break;

        cvFlip(frame, NULL,  1);

        if (key == 'c') {

			for (int i = 0; i < NUM_QUERY_FRAME; i++){
				frame = cvQueryFrame( capture );
			}

			// display a full white
			cvShowImage("camera window", whiteFrame);
			cvWaitKey(500);

            //capture the current frame
			for (int i = 0; i < NUM_QUERY_FRAME; i++){
				frame = cvQueryFrame( capture );
			}

            break;
        }

        cvShowImage("camera window", frame);

        // exit if user press 'q' 
        key = cvWaitKey( 1 );
		
    }

	QString filePath = capturedImageDir.absoluteFilePath(getTimeStamp()) + CAPTURED_IMAGE_EXT;


    QByteArray latinArray = filePath.toLatin1();
    //cvSaveImage(filePath.toAscii().data(), frame);
    cvSaveImage(latinArray.constData(), frame);

    // free memory 
    cvDestroyWindow( "camera window" );
    cvReleaseCapture( &capture );

	

	generalManager->setInputImagePath(filePath.toStdString());
	updateUI();
	
	
}

void QDemoMainWindow::on_act_saveImg_actived(){
	QString savePath = QFileDialog::getSaveFileName(NULL, "Save","./output.jpg", tr("Images (*.png *.xpm *.jpg)") );
    bool saveResult;
    if (savePath.isNull() == false){
		saveResult = generalManager->saveOutputImage(savePath.toStdString());

        if (saveResult == false){
            QMessageBox::critical(this, "Error", "Cannot save the file", QMessageBox::Ok, QMessageBox::Ok);
        }
    }
}


void QDemoMainWindow::on_act_saveImputImg_actived(){
	QString savePath = QFileDialog::getSaveFileName(NULL, "Save","./output.jpg", tr("Images (*.png *.xpm *.jpg)") );
    bool saveResult;
    if (savePath.isNull() == false){
		saveResult = generalManager->saveInputImage(savePath.toStdString());
        if (saveResult == false){
            QMessageBox::critical(this, "Error", "Cannot save the file", QMessageBox::Ok, QMessageBox::Ok);
        }
    }

}

void QDemoMainWindow::on_act_preferences_actived(){
//	setting = new QSettingWindow(generalManager->parameter);
	
	//QObject::connect(
	//			setting, SIGNAL(okClicked()),
 //               this, SLOT(setSetting()));

	//QObject::connect(
	//			setting, SIGNAL(cancelClicked()),
 //               this, SLOT(removeSettingWindows()));

	//QObject::connect(
	//			setting, SIGNAL(commandFileClicked(QString)),
	//			this, SLOT(startCommandFile(QString)));

 //   setting->show();
}

void QDemoMainWindow::setSetting(){
	/*
	generalManager->parameter = setting->getAllParamter();

	if (!generalManager->parameter.initialized){
		QString errorString = "";
		errorString += generalManager->train();


		if (errorString.size() == 0){
			this->setVisible(true);
			updateUI();
			generalManager->parameter.initialized = true;
			removeSettingWindows();
		} else {
			QMessageBox::critical(this, "Error", errorString, QMessageBox::Ok, QMessageBox::Ok);
		}
	} else {
		removeSettingWindows();
	}*/
}

void QDemoMainWindow::removeSettingWindows(){
	//if (generalManager->parameter.initialized){
	//	setting->close();
	//	delete setting;
	//} else {
	//	exit(0);
	//}
}

void QDemoMainWindow::on_but_faceRecogintion_clicked(){

	generalManager->FaceRecog(generalManager->getOutputImage());
	generalManager->displayFaceRecogResult(0);
	updateUI();

	//std::vector <string> fileNames;
	//std::vector <double> scores;

	//generalManager->getFaceFacogResults(fileNames, scores);
	//if (!fileNames.empty()){
	//	QString msgBoxText;
	//	for ( int i = 0; i < fileNames.size(); i++){
	//		msgBoxText = msgBoxText + QString::fromStdString(fileNames.at(i)) + "\t" + QString::number(scores.at(i)) + "\n";
	//	}
	//	QMessageBox msgBox;
	//	msgBox.setText(msgBoxText);
	//	msgBox.exec();
	//}

}

void QDemoMainWindow::changeCandidatePatchOrgImg(int type, int photoID, QRect patchPos){
	
	QString orgImgPath = generalManager->getCanPatchOrgImgPath(type, photoID);
	cv::Mat orgImg = cv::imread(orgImgPath.toStdString());

	candidatesWindow->setImagePath(orgImgPath);
	candidatesWindow->setImage(orgImg);
	candidatesWindow->setImagePatchPos(patchPos);
}

void QDemoMainWindow::startCommandFile(QString commandFilePath){
	//QFile commandFile(commandFilePath);

	//if (commandFile.open(QIODevice::ReadOnly|QFile::Truncate) ){
	//	mainwindowController* commandFileController;

	//	QTextStream commandStream(&commandFile);
	//	QString aCommandLine = commandStream.readLine();
	//	

	//	do {
	//		commandFileController = new mainwindowController();

	//		QStringList commandLineItems;
	//		commandLineItems = aCommandLine.split(",");

	//		int numberOfTestImage = commandLineItems.at(1).toInt();
	//		QString outputPath = commandLineItems.at(3);
	//		QDir outputDir(outputPath);
	//		if (!outputDir.exists()){
	//			continue;
	//		}

	//		Parameter* paramFromCommand = getPatameterFromCommand(commandLineItems);
	//		if (paramFromCommand == NULL){
	//			continue;
	//		}
	//		commandFilegeneralManager->parameter = *paramFromCommand;

	//		

	//		commandFilegeneralManager->MWFTraining();
	//		commandFilegeneralManager->FaceRecogTraining();
	//		
	//		for (int i = 0; i < numberOfTestImage; i++){
	//			QString aInputImageLine = commandStream.readLine();
	//			QStringList inputImageLineItems;
	//			inputImageLineItems = aInputImageLine.split(",");

	//			//set input file name
	//			commandFilegeneralManager->setInputImagePath(inputImageLineItems.at(1).toStdString());
	//			
	//			//start MWF
	//			commandFilegeneralManager->MWFStart();

	//			//get output file name
	//			QString outputFileName = "";
	//			if (inputImageLineItems.size() >= 3){
	//				outputFileName = inputImageLineItems.at(2) + "_";
	//			}
	//			outputFileName = outputFileName + getTimeStamp() + ".jpg";

	//			//get output file full path
	//			QString outputFileFullPath;
	//			outputFileFullPath = outputDir.absoluteFilePath(outputFileName);

	//			//Save outputImage
	//			commandFilegeneralManager->saveOutputImage(outputFileFullPath.toStdString());
	//		}
	//		
	//		/*
	//		QMessageBox msgBox;
	//		msgBox.setText(aCommandLine);
	//		msgBox.exec();
	//		*/
	//		delete commandFileController;
	//		delete paramFromCommand;
	//		aCommandLine = commandStream.readLine(); //read next line

	//	} while (!aCommandLine.isNull());  

	//}

}

Parameter* QDemoMainWindow::getPatameterFromCommand(QStringList commandLineItems){
	//Parameter* paramFromCommand = new Parameter;
	//paramFromCommand->dataPath = commandLineItems.at(2);
	//paramFromCommand->sketchPath = paramFromCommand->dataPath + "/Sketch";
	//paramFromCommand->photoPath = paramFromCommand->dataPath + "/Photo";
	//paramFromCommand->databasePath = paramFromCommand->dataPath + "/FaceRecog";
	//
	////MWF Mode
	//if (commandLineItems.at(4).toUpper() == "P2S"){
	//	paramFromCommand->mode = _CONTROLLER_P2S_;
	//} else if (commandLineItems.at(4).toUpper() == "S2P"){
	//	paramFromCommand->mode = _CONTROLLER_S2P_;
	//} else {
	//	return NULL;
	//}

	////Add six patchs
	//if (commandLineItems.at(5).toUpper() == "Y"){
	//	paramFromCommand->newPatch = true;
	//} else if (commandLineItems.at(5).toUpper() == "N"){
	//	paramFromCommand->newPatch = false;
	//} else {
	//	return NULL;
	//}

	////Number of Good Can
	//paramFromCommand->numOfGoodCan = commandLineItems.at(6).toInt();

	////Number of Random Can
	//paramFromCommand->numOfRandomCan = commandLineItems.at(7).toInt();

	////KNN Mode
	//if (commandLineItems.at(8).toUpper() == "KD-TREE"){
	//	paramFromCommand->foundKNNMode = _KNN_KDTREE_;
	//} else if (commandLineItems.at(8).toUpper() == "BRUTE FORCE"){
	//	paramFromCommand->foundKNNMode = _KNN_BRUTEFORCE_;
	//} else {
	//	return NULL;
	//}

	////Number of KD-Tree
	//paramFromCommand->numberOfKDTree = commandLineItems.at(9).toInt();

	////Number of Search
	//paramFromCommand->numberOfLeafChecked = commandLineItems.at(10).toInt();

	////Mode of KD-Tree
	//if (commandLineItems.at(11).toUpper() == "SINGLE"){
	//	paramFromCommand->modeOfKDTree = _KDTREE_SINGLE_;
	//} else if (commandLineItems.at(11).toUpper() == "MULTI"){
	//	paramFromCommand->modeOfKDTree  = _KDTREE_MULTI_;
	//} else {
	//	return NULL;
	//}

	////sampling rate
	//paramFromCommand->samplingRate = commandLineItems.at(12).toInt();

	////add dim
	//if (commandLineItems.at(13).toUpper() == "Y"){
	//	paramFromCommand->additionDim = true;
	//} else if (commandLineItems.at(13).toUpper() == "N"){
	//	paramFromCommand->additionDim = false;
	//} else {
	//	return NULL;
	//}

	////add weight
	//paramFromCommand->additionDimWeight = commandLineItems.at(14).toInt(); 

	//paramFromCommand->imageCol = 200;
	//paramFromCommand->imageRol = 250;

	return NULL;
}

void QDemoMainWindow::showFaceRecogResult(int rank ){
	if (!disconnect_lsw_faceRecogResult){
		if (ui.lsw_recognitionResult->count() > 0){
			generalManager->displayFaceRecogResult(rank);
			updateUI();
		}
	}
}

void QDemoMainWindow::updateUISlot(){
	updateUI();
}
