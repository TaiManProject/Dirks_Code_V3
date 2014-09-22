#include "FaceMatchingSingle.h"

FaceMatchingSingle::FaceMatchingSingle(int rowStart, int rowEnd, int colStart, int colEnd, std::vector<cv::Mat>& orgImages, std::vector<int>& labels, int faceMatchingControlerID, QMutex& openCVMutex) {
    commonTool.log(QString("FaceMatchingController::FaceMatchingController. (rowStart, rowEnd, colStart, colEnd) = (%1, %2, %3, %4)").arg(rowStart).arg(rowEnd).arg(colStart).arg(colEnd));
    this->faceMatchingControlerID = faceMatchingControlerID;
	this->rowRange.start = rowStart;
	this->rowRange.end = rowEnd;
	this->colRange.start = colStart;
	this->colRange.end = colEnd;
    this->openCVMutex = &openCVMutex;
    openCVMutex.lock();
    for (int i = 0; i < (int)orgImages.size(); i++) {
        cv::Mat croppedImg = this->cropImg(orgImages.at(i));
        //
        cv::namedWindow( "faceMatchingTraining", CV_WINDOW_NORMAL ); //crash here ...
        cv::imshow( "faceMatchingTraining", croppedImg );
        //
        this->images.push_back(croppedImg);
        commonTool.log(QString("%1 %2").arg(croppedImg.cols).arg(croppedImg.rows));
        commonTool.log(QString("%1").arg(croppedImg.type()));

	}
    cv::destroyWindow("faceMatchingTraining");

	this->labels = labels;
	this->train();

    if (true) {
        maxDistance = 0;
        //commonTool.log("start getting max distance ...");
        for (int i=0;i<(int)images.size();i++) {
            cv::Mat projection1 = cv::subspaceProject(W, mean, images.at(i).reshape(1, 1));
            for (int j=i+1;j<(int)images.size();j++) {
                //commonTool.log(QString("%1 %2").arg(i).arg(j));
                cv::Mat projection2 = cv::subspaceProject(W, mean, images.at(j).reshape(1, 1));
                double distance = cv::norm(projection1, projection2);
                if (distance > maxDistance)
                    maxDistance = distance;
            }
        }
        //commonTool.log(QString("Stop getting max distance ... %1").arg(maxDistance));
    } else {
        //hardcode (not good ...) only OK for single level ...
        maxDistance = 10000;
    }
    commonTool.log(QString("maxDistance ---> %1").arg(maxDistance));
    openCVMutex.unlock();
}

void FaceMatchingSingle::train() {
    commonTool.log("FaceMatchingController::train().");
    commonTool.log(QString("number ## of image in training DB --> %1").arg(images.size()));
    images.size();
    this->model = cv::createFisherFaceRecognizer();
    //this->model = cv::createFisherFaceRecognizer(100);
	this->model->train(images, labels);
	this->W = model->getMat("eigenvectors");
	this->mean = model->getMat("mean");
}

int FaceMatchingSingle::getArea() {
	return rowRange.size() * colRange.size();
}

void FaceMatchingSingle::save(QString dirPath) {
    QString fullPath = dirPath + "/" + QString::number(this->faceMatchingControlerID);
    this->model->save(fullPath.toStdString());
}

void FaceMatchingSingle::load(QString dirPath) {
    QString fullPath = dirPath + "/" + QString::number(this->faceMatchingControlerID);
    this->model->load(fullPath.toStdString());
}

double FaceMatchingSingle::getDistance(cv::Mat& target1, cv::Mat& target2) {
    cv::Mat sample1 = cropImg(target1);
    cv::Mat sample2 = cropImg(target2);
    if (sample1.type() == CV_8UC3) {
        commonTool.log("error, why color?");
        cv::cvtColor(sample1, sample1, CV_BGR2GRAY);
    }
    if (sample2.type() == CV_8UC3) {
        commonTool.log("error, why color ?");
        cv::cvtColor(sample2, sample2, CV_BGR2GRAY);
    }
    if (sample1.type() != sample2.type()) {
        commonTool.log("error, diffrent image type of sample");
    }
    cv::Mat proj1 = cv::subspaceProject(W, mean, sample1.reshape(1, 1));
    cv::Mat proj2 = cv::subspaceProject(W, mean, sample2.reshape(1, 1));
    return cv::norm(proj1, proj2);
}

std::vector<double> FaceMatchingSingle::getNormalizedSimilarity(cv::Mat& target1, cv::Mat& target2) {
    cv::Mat sample1 = cropImg(target1);
    cv::Mat sample2 = cropImg(target2);
    if (sample1.type() == CV_8UC3) {
        cv::cvtColor(sample1, sample1, CV_BGR2GRAY);
    }
    if (sample2.type() == CV_8UC3) {
        cv::cvtColor(sample2, sample2, CV_BGR2GRAY);
    }
    cv::Mat proj1 = cv::subspaceProject(W, mean, sample1.reshape(1, 1));
    cv::Mat proj2 = cv::subspaceProject(W, mean, sample2.reshape(1, 1));
    double distance = cv::norm(proj1, proj2);
    double similarity = ((maxDistance - distance) / maxDistance) * 100;

    if (similarity < 0){
        similarity = 0.0;
    }
    if (similarity > 100.0){
        similarity = 100.0;
    }
    std::vector<double> similarities;
    similarities.push_back(similarity);
    return similarities;
}

cv::Mat FaceMatchingSingle::cropImg(cv::Mat img) {
    return img(rowRange, colRange).clone(); //is it necessary to clone here?
}
