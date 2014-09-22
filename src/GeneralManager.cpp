#include "GeneralManager.h"
#include <QObject>
#include <QMessageBox>
#include <iostream>

using namespace std;

GeneralManager::GeneralManager(){

	faceRecogResultChanged = false;

	showInputImg = false;
	showOutputImg = false;
	showFaceRecogImg = false;

	parameter.initialized = false;

	//init parameter
	parameter.mode = _CONTROLLER_S2P_;

    //TODO: do I need the path here?
    parameter.inputImagePath = "C:/Users/Mosquito/Desktop/f1-001-01.jpg";
    parameter.dataPath = "G:/13-14 Sem_2/Project/CUFS/1_testing";
	
	parameter.numberOfKDTree = 4;
	parameter.modeOfKDTree = _KDTREE_SINGLE_;
	parameter.numberOfLeafChecked = 1024;
	parameter.additionDim = true;
	parameter.additionDimWeight = 99;

	//parameter.foundKNNMode = _KNN_BRUTEFORCE_;
	//parameter.samplingRate = 1;

	parameter.foundKNNMode = _KNN_KDTREE_;
	parameter.samplingRate = 2;

	parameter.newPatch = false;

    //parameter.RealNumOfCan = 10;          // select 10 candidates from the training dataset
	parameter.numOfGoodCan = 10;
	parameter.numOfRandomCan = 0;
    parameter.SearchWidthUpper = 5;       // 5 pixels for upper, lower, left and right search range for each window
    parameter.SearchWidthLower = 8;
    parameter.patchSizeUpper = 20;
    parameter.patchSizeLower = 10;
    parameter.OverlapSizeUpper = 5;
    parameter.OverlapSizeLower = 5;
	parameter.imageCol = 200;
	parameter.imageRol = 250;

	//init the time to -1, which mean not sym
	timeNeedForFindKNN = -1;
	timeNeedForMinEng = -1;
	timeNeedForFaceRecog = -1;

	QObject::connect(&faceSynthesisManager, SIGNAL(FSTrainingDone()), this, SLOT(FSTrainingDoneSlot()));
	//QObject::connect(&faceRecognitionManager, SIGNAL(faceRecogTrainingDone()), this, SLOT(FaceRecogTrainingDoneSlot()));
	
	numberOfTrainedImage = 0;
	numberOfFaceRecogDBImage = 0;

}

QString GeneralManager::train(QMutex& openCVMutex) {

   /* commonTool.log("Performing Training (synthesize + matching).");

	QString errorString;

    if (true) {
        sketchPath = parameter.dataPath + "/" + SKETCH_DIR;
        photoPath = parameter.dataPath + "/" + PHOTO_DIR;
        //faceRecogTrainedFile = parameter.dataPath + "/" + FACE_RECOG_TRAINED_FILE_NAME;

        faceSynthesisManager.setParameter(parameter);
        errorString += faceSynthesisManager.setPath(photoPath, sketchPath);
        if (parameter.mode == _CONTROLLER_P2S_){
            faceSynthesisManager.setDirectionPhotoToSketch();
        } else if (parameter.mode == _CONTROLLER_S2P_){
            faceSynthesisManager.setDirectionSketchToPhoto();
        }
    }

     faceSynthesisManager.train();

    faceRecogDBPath = parameter.dataPath + "/" + QString::fromStdString(FaceMatchingManager::trainingFileName);
    faceMatchingManager.trainMulti(faceRecogDBPath, openCVMutex);

    commonTool.log(QString("Training for face synthesize data + face recognition completed. Error string = %1.").arg(errorString));

    return errorString;*/
    ///////////////////
    QString errorString;
    return errorString;
    ///////////////////
}


QString GeneralManager::trainFaceSynthesis() {
    //TODO: read from file ...

    commonTool.log("GeneralManager::trainFaceSynthesis().");

	QString errorString;

	sketchPath = parameter.dataPath + "/" + SKETCH_DIR;
	photoPath = parameter.dataPath + "/" + PHOTO_DIR;	

	faceSynthesisManager.setParameter(parameter);
	errorString += faceSynthesisManager.setPath(photoPath, sketchPath);
	if (parameter.mode == _CONTROLLER_P2S_){
		faceSynthesisManager.setDirectionPhotoToSketch();
	} else if (parameter.mode == _CONTROLLER_S2P_){
		faceSynthesisManager.setDirectionSketchToPhoto();
	}
    faceSynthesisManager.train();

	return errorString;
}

QString GeneralManager::trainFaceRecog(){
	QString errorString;

    commonTool.log("GeneralManager::trainFaceRecog().");

	//faceRecogDBPath = parameter.dataPath + "/" + FACE_RECOG_DB_DIR;
	//faceRecogTrainedFile = parameter.dataPath + "/" + FACE_RECOG_TRAINED_FILE_NAME;

	//errorString += faceRecognitionManager.loadTrainedDatabase(faceRecogDBPath, faceRecogTrainedFile);

	return errorString;
}

QString GeneralManager::buildFaceRecogTrainedFile(QString faceRecogTrainedFilePath, QString faceRecogTrainedFile){
	QString errorString;

	//errorString += faceRecognitionManager.buildTrainedFile(faceRecogTrainedFilePath, faceRecogTrainedFile);

	return errorString;
}

void GeneralManager::FSTrainingDoneSlot() {

    commonTool.log("GeneralManager::FSTrainingDoneSlot().");

	numberOfTrainedImage = faceSynthesisManager.getNumberOfTrainedImage();
	emit updateUI();
}

void GeneralManager::FaceRecogTrainingDoneSlot(){

    commonTool.log("GeneralManager::FaceRecogTrainingDoneSlot().");

	/*cout << "FaceRecogTrainingDoneSlot at GM" << endl;
	numberOfFaceRecogDBImage = faceRecognitionManager.getNumberOfTrainedImage();
	emit openCameraSignal();
	emit updateUI();*/
}

//TODO: remove this in the future ...
bool GeneralManager::faceAlign(cv::Mat& inputImg, cv::Mat& alignedImg, std::vector<cv::Point2d>& landmarkPoints) {
    QMutex mutex; //TODO: don't create a new mutex here, get it from the single one in existence ...
    FaceAlignment fa(parameter.dataPath.toStdString(), mutex);
    cv::Point2d centersOfLandmarks; //not used
    return fa.alignStasm4(inputImg, alignedImg, landmarkPoints, centersOfLandmarks);
}

bool GeneralManager::setInputImagePath(std::string inputImage_P){
    commonTool.log("GeneralManager::setInputImagePath");
	QString inputImageQStr(inputImage_P.c_str());
    parameter.inputImagePath = inputImageQStr;

	cv::Mat selectedImg = cv::imread(inputImage_P);
	if (selectedImg.data == NULL){
		return false;
	}

    std::vector<cv::Point2d> extractedLandMarks;

    cv::Mat alignedImg;
    bool success = faceAlign(selectedImg, alignedImg, extractedLandMarks);
    inputImg = alignedImg;

	inputQImage = Mat2QImage(inputImg);

	showInputImg = true;
	showOutputImg = false;
	showFaceRecogImg = false;

	clearFaceRecogResult();
	
    return true;
}

bool GeneralManager::setInputSynthesisImage(cv::Mat alignedImg) {
    //Dirk edit: I disabled this due to redundand code!!!
    //cv::Mat alignedImg = faceAlign(inputImage);

	this->inputImg = alignedImg;

	return true;
}


QString GeneralManager::MWFStart() {

    commonTool.log("Performing face synthesize.");

	faceSynthesisManager.synthesize(inputImg);

	outputImg = faceSynthesisManager.getSysthesizedImg();
    outputQImage = Mat2QImage(outputImg);

	timeNeedForFindKNN = faceSynthesisManager.getTimeNeedForFindKNNInMS();
	timeNeedForMinEng = faceSynthesisManager.getTimeNeedForMinEngInMS();

	showOutputImg = true;

    commonTool.log("Face synthesize completed.");

    return "";
}

void GeneralManager::setMode(int mode_p){
    parameter.mode = mode_p;
}

void GeneralManager::setNewPatch(bool setNewPatch){ // use six new pathc or not
    parameter.newPatch = setNewPatch;
}

cv::Mat GeneralManager::getOutputImage(){
	return outputImg;
}

cv::Mat GeneralManager::getInputImage(){
	return inputImg;
}

bool GeneralManager::saveOutputImage(std::string saveImagePath){
    return cv::imwrite(saveImagePath, outputImg);
}

bool GeneralManager::saveInputImage(std::string saveImagePath){
    //TODO fix this
    //return faceAlignmentManager.saveImg(saveImagePath);
    ///////////
    return true;
    ///////////
}

QPoint GeneralManager::getPatchCoordinate(QPointF scenePoint){
    QPoint patchCoordinate;

	int patchSizeMinusOverlap = parameter.patchSizeLower - parameter.OverlapSizeLower;

    patchCoordinate.setX(scenePoint.rx()/patchSizeMinusOverlap);
    patchCoordinate.setY(scenePoint.ry()/patchSizeMinusOverlap);

	int PatchStepLower = parameter.patchSizeLower - parameter.OverlapSizeLower;

	int NumWidLower = (parameter.imageCol- parameter.patchSizeLower)/PatchStepLower + 1;
	int NumHeiLower = (parameter.imageRol- parameter.patchSizeLower)/PatchStepLower + 1;

    //compute too Right and too Bottom case
    cout << "too Right? " << patchCoordinate.rx() << "/\n";// << Sketch->NumWidLower - 1 << endl;
    if (patchCoordinate.rx() > NumWidLower - 1){
        //cout << "At too Right" << endl;
        patchCoordinate.setX(NumWidLower - 1);
    }
    cout << "too Bottom? " << patchCoordinate.ry() << "/\n";// << Sketch->NumHeiLower - 1 << endl;
    if (patchCoordinate.ry() > NumHeiLower - 1){
        //cout << "At too Bottom" << endl;
        patchCoordinate.setY(NumHeiLower - 1);
    }

    return patchCoordinate;

}

CandidatePatchSet* GeneralManager::getCandidatePatchSet(QPointF scenePoint){
    QPoint patchCoordinate = getPatchCoordinate(scenePoint);
	return faceSynthesisManager.getCandidatePatchSet(patchCoordinate);
}

QRect GeneralManager::getPatchRect(QPoint patchCoordinate){
	int patchSizeMinusOverlap = parameter.patchSizeLower - parameter.OverlapSizeLower;
    QPoint topLeft;
    QPoint bottomRight;

    topLeft.setX(patchCoordinate.rx() * patchSizeMinusOverlap);
    topLeft.setY(patchCoordinate.ry() * patchSizeMinusOverlap);

    bottomRight.setX(parameter.patchSizeLower + topLeft.rx());
    bottomRight.setY(parameter.patchSizeLower + topLeft.ry());

    QRect returnRect(topLeft, bottomRight);
    return returnRect;
}

//why is thi commented? ...
//jackys old method?
void GeneralManager::FaceRecog(cv::Mat inputImage){
	//if (inputImage.type() == CV_8UC3){
	//	cv::cvtColor(inputImage, inputImage, CV_BGR2GRAY);
	//}
	//clearFaceRecogResult();

	//faceRecognitionManager.recognize(inputImage);
	//showFaceRecogImg = true;
	//faceRecogResultChanged = true;

	//faceRecognitionManager.getRecognizedResults(faceRecogResultsFilePath, faceRecogResultsScore);
	//
	//for (int i = 0; i < faceRecogResultsFilePath.size(); i++){
	//	if (wantedPersonList.contains(faceRecogResultsFilePath.at(i))){
	//		wantedFaceRecogResultsFilePath.push_back(faceRecogResultsFilePath.at(i));
	//		wantedFaceRecogResultsScore.push_back(faceRecogResultsScore.at(i));
	//	}
	//}

	//timeNeedForFaceRecog = faceRecognitionManager.getTimeNeedForFaceRecogInMS();
}

void GeneralManager::displayFaceRecogResult(int rank){
	faceRecogImg = cv::imread((faceRecogDBPath + "/" + faceRecogResultsFilePath.at(rank)).toStdString());
	faceRecognitionQImage = Mat2QImage(faceRecogImg);
	//faceRecogImgScore = faceRecogResultsScore.at(rank);
}

void GeneralManager::displayWantedFaceRecogResult(int rank){
	faceRecogImg = cv::imread((faceRecogDBPath + "/" + FACE_RECOG_DB_ORG_SKETCH_FOLDER_NAME + "/" + wantedFaceRecogResultsFilePath.at(rank)).toStdString());
	if (!faceRecogImg.data){
		faceRecogImg = cv::imread((faceRecogDBPath + "/" + wantedFaceRecogResultsFilePath.at(rank)).toStdString());
	}
	faceRecognitionQImage = Mat2QImage(faceRecogImg);
	//faceRecogImgScore = faceRecogResultsScore.at(rank);
}

void GeneralManager::clearFaceRecogResult(){
	faceRecogResultChanged = true;

	faceRecogResultsFilePath.clear();
	faceRecogResultsScore.clear();
	wantedFaceRecogResultsFilePath.clear();
	wantedFaceRecogResultsScore.clear();
}

QStringList GeneralManager::getFaceRecogResult(){
	QStringList faceRecogResults;
	for (int i = 0; i < faceRecogResultsFilePath.size(); i++){
		faceRecogResults.push_back(faceRecogResultsFilePath.at(i) + "\t" + QString::number(faceRecogResultsScore.at(i)));
	}
	return faceRecogResults;
}

cv::Mat GeneralManager::getPatchFromInputImage(QRect rect){
	cv::Mat patch = faceSynthesisManager.getPatchFromInputImage(rect);
	return patch;
}

QString GeneralManager::getCanPatchOrgImgPath(int type, int index){
	return faceSynthesisManager.getCanPatchOrgImgPath(type, index);;
}


void GeneralManager::setWantedPersonList(QStringList wantedPersonList_p){
	wantedPersonList = wantedPersonList_p;

}

bool GeneralManager::isWantedPerson(QString personFileName){
	return wantedPersonList.contains(personFileName);
}

bool GeneralManager::isRecogListHaveWantedPerson(){
	for (int i = 0; i < faceRecogResultsFilePath.size(); i++){
		if (wantedPersonList.contains(faceRecogResultsFilePath.at(i))){
			return true;
		}
	}
	return false; 
}

QStringList GeneralManager::getWantedFaceRecogResult(){
	QStringList wantedRecogList;
	for (int i = 0; i < wantedFaceRecogResultsFilePath.size(); i++){
		wantedRecogList.push_back(wantedFaceRecogResultsFilePath.at(i) + "\t" + QString::number(wantedFaceRecogResultsScore.at(i)));
	}
	return wantedRecogList;
}

void GeneralManager::faceMatchMulti(cv::Mat& target1, cv::Mat& target2, double &similarity, double &confidence) {
    //faceMatchingManager.faceMatchMulti(target1, target2, similarity, confidence);
}

//void GeneralManager::faceMatchSingle(cv::Mat& target1, cv::Mat& target2, double &similarity) {
//    faceMatchingManager.faceMatchSingle(target1, target2, similarity);
//}

double GeneralManager::getDistance(cv::Mat& target1, cv::Mat& target2) {
    return faceMatchingManager.getDistance(target1, target2);
}

double GeneralManager::getNormalizedSimilarity(cv::Mat& target1, cv::Mat& target2) {
    //return faceMatchingManager.getNormalizedSimilarity(target1, target2);
    return 0.0;
}

//double GeneralManager::getConfidence(cv::Mat& img) {
//    return faceMatchingManager.getConfidence(img);
//}
