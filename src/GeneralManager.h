#ifndef GENERALMANAGER_H
#define GENERALMANAGER_H


#include <opencv/cv.h>
#include <QStringList>
#include <QProgressBar>
#include <QDir>
#include <QMouseEvent>
#include <QMessageBox>
#include <QLabel>
#include <QList>
#include <QTime>
#include <QFile>

#include <opencv/cxcore.h>
#include <opencv/cvaux.h>

#include <flann/flann.hpp>

#include "FaceAlignment.h"
#include "FaceSynthesisManager.h"
#include "FaceMatchingManager.h"
//#include "FaceRecognitionManager.h"

#include "commonTool.h"
#include "IntegralImage.h"
#include "IntegralImages.h"
#include "CandidatePatchSet.h"

/*! A Controller class */
class GeneralManager : public QObject
{
	Q_OBJECT
public:
	/**
	* A constructor of the controller.
	*/
    GeneralManager();

	//parameters
	Parameter parameter;

	int timeNeedForFindKNN;
	int timeNeedForMinEng;
	int timeNeedForFaceRecog;

    QImage outputQImage;
    QImage inputQImage;
    QImage faceRecognitionQImage;

	QString sketchPath;
	QString photoPath;
	QString faceRecogDBPath;
	QString faceRecogTrainedFile;
    
	int numberOfTrainedImage;

	cv::Mat inputImg;
	cv::Mat outputImg;
	cv::Mat faceRecogImg;
	//double faceRecogImgScore;

	bool showInputImg;
	bool showOutputImg;
	bool showFaceRecogImg;


	FaceSynthesisManager faceSynthesisManager;
    FaceMatchingManager faceMatchingManager;

	int numberOfFaceRecogDBImage;

	bool faceRecogResultChanged;
	QStringList faceRecogResultsFilePath;
	std::vector<double> faceRecogResultsScore;

	void displayFaceRecogResult(int rank);
	void clearFaceRecogResult();

    QString checkTrained();
	QString MWFStart();

	QStringList getFaceRecogResult();

	/**
	* a method that set the input image path that will be synthesis
	* @param inputImage_P a path of image that will be synthesis
	*/
    bool setInputImagePath(std::string inputImage_P);

	bool setInputSynthesisImage(cv::Mat inputImage);

	/**
	* a method that return the synthesised image in OpenCV format.
	* @return a synthesised image in cv::Mat format
	*/
	cv::Mat getOutputImage();
	
	/**
	* a method that return the input image in OpenCV format.
	* @return an input image in cv::Mat format
	*/
	cv::Mat getInputImage();
	
	/**
	* a method that save the synthesised image
	* @param saveImagePath a path to save the synthesised image.
	*/
    bool saveOutputImage(std::string saveImagePath);
    bool saveInputImage(std::string saveImagePath);

    void setMode(int mode_p);
    void setNewPatch(bool setNewPatch);
    CandidatePatchSet* getCandidatePatchSet(QPointF scenePoint);
    QPoint getPatchCoordinate(QPointF scenePoint);
    QRect getPatchRect(QPoint patchCoordinate);
	void updateAllParameter();
	cv::Mat getPatchFromInputImage(QRect rect);
	//cv::Mat getCanPatchOrgImg(int type, int index);
	QString getCanPatchOrgImgPath(int type, int index);



	///**
	//* a method that start the Face Recognition. Note that the FaceRecogTraining() should be executed and the input image path should be set.
	//*/
	void FaceRecog(cv::Mat inputImg);

    QString train(QMutex& mutex);
	QString trainFaceRecog();
	QString buildFaceRecogTrainedFile(QString faceRecogTrainedFilePath, QString faceRecogTrainedFile);
	QString trainFaceSynthesis();
    QString trainFaceMatching(std::vector<cv::Mat> additionalImages, QMutex& openCVMutex);

	QStringList wantedPersonList;
	QStringList wantedFaceRecogResultsFilePath;
	std::vector<double> wantedFaceRecogResultsScore;
	void setWantedPersonList(QStringList wantedPersonList_p);
	bool isWantedPerson(QString personFileName);
	bool isRecogListHaveWantedPerson();
	QStringList getWantedFaceRecogResult();
	void displayWantedFaceRecogResult(int rank);

    bool faceAlign(cv::Mat& inputImg, cv::Mat& alignedImg, std::vector<cv::Point2d>& landmarkPoints);

	//face matching
    //TODO: call it getMultiLevelDistance ...
    void faceMatchMulti(cv::Mat& target1, cv::Mat& target2, double &similarity, double &confidence);
    //void faceMatchSingle(cv::Mat& target1, cv::Mat& target2, double &similarity);
    double getDistance(cv::Mat& target1, cv::Mat& target2);
    double getNormalizedSimilarity(cv::Mat& target1, cv::Mat& target2);
    //double getConfidence(cv::Mat& img);


signals:
	void updateUI();
	void openCameraSignal();

public slots:
	void FSTrainingDoneSlot();
	void FaceRecogTrainingDoneSlot();

};

#endif // GENERALMANAGER_H
