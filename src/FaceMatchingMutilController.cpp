#include "FaceMatchingMutilController.h"

FaceMatchingMuti::FaceMatchingMuti(std::vector<cv::Mat> imgs, std::vector<int> labels, QMutex& openCVMutex) {
	this->labels = labels;
    this->images = imgs;

	for (int i = 0; i < numberOfLevel; i++){
		int rowStep = (ORG_ROW_TO - ORG_ROW_FROM) / numberOfRowInController[i];
		int colStep = (ORG_COL_TO - ORG_COL_FROM) / numberOfColInController[i];
		int rowStart = ORG_ROW_FROM;
		int rowEnd = rowStart + rowStep;
		for (int j = 0; j < numberOfRowInController[i]; j++){ //row
			int colStart = ORG_COL_FROM;
			int colEnd = colStart + colStep;
			for (int k = 0; k < numberOfColInController[i]; k++){ //col
                FaceMatchingSingle faceMatchingController(rowStart, rowEnd, colStart, colEnd, images, labels, (i * 10) + (j * numberOfColInController[i]) + k, openCVMutex);
				faceMatchingControllers.push_back(faceMatchingController);
                //int size = faceMatchingControllers.size();

				colStart = colEnd;
				colEnd = colStart + colStep;
			}
			rowStart = rowEnd;
			rowEnd = rowStart + rowStep;
		}
	}
}

void FaceMatchingMuti::train() {
    for (int i = 0; i < (int)faceMatchingControllers.size(); i++){
        faceMatchingControllers.at(i).train();
	}
}

void FaceMatchingMuti::save(QString dirPath){
    for (int i = 0; i < (int)faceMatchingControllers.size(); i++){
        faceMatchingControllers.at(i).save(dirPath);
	}
}

void FaceMatchingMuti::load(QString dirPath){
    for (int i = 0; i < (int)faceMatchingControllers.size(); i++){
        faceMatchingControllers.at(i).load(dirPath);
	}
}

void FaceMatchingMuti::faceMatching(cv::Mat target1, cv::Mat target2, double &similarity, double &confidence) {
    //QList<double> similarityForAllControllers;
    //for (int i=0; i<(int)faceMatchingControllers.size(); i++) {
    //    //double similarity = faceMatchingControllers.at(i).faceMatching(target1, target2);
    //    faceMatchingControllers.at(i).getNormalizedSimilarity(target1, target2);
    //	similarityForAllControllers.push_back(similarity);
    //}
    //FaceMatchingSimilarityTree faceMatchingSimilarityTree(similarityForAllControllers, SIMILARITY_THRESHOLD);
    //faceMatchingSimilarityTree.computeSimilarityAndConfidence(similarity, confidence);
}

std::vector<double> FaceMatchingMuti::getAllNormalizedSimilarity(cv::Mat& target1, cv::Mat& target2) {
    std::vector<double> similarityForAllControllers;
    for (int i=0; i<(int)faceMatchingControllers.size(); i++) {
        //double similarity = faceMatchingControllers.at(i).faceMatching(target1, target2);
        FaceMatchingSingle singleMatcher = faceMatchingControllers.at(i);
        std::vector<double> similarity = singleMatcher.getNormalizedSimilarity(target1, target2);
        similarityForAllControllers.push_back(similarity.at(0));
        //commonTool.log(QString("similarity [%1, %2, %3, %4] ---> %5").arg(singleMatcher.rowRange.start).arg(singleMatcher.rowRange.end).arg(singleMatcher.colRange.start).arg(singleMatcher.colRange.end).arg(similarity.at(0)));
    }
    return similarityForAllControllers;
}

std::vector<double> FaceMatchingMuti::getNormalizedSimilarity(cv::Mat& target1, cv::Mat& target2) {
    //
    std::vector<double> similarityForAllControllers = getAllNormalizedSimilarity(target1, target2);
    //FaceMatchingSimilarityTree faceMatchingSimilarityTree(similarityForAllControllers, SIMILARITY_THRESHOLD);
    //faceMatchingSimilarityTree.computeSimilarityAndConfidence(similarity, confidence);
    //

    return similarityForAllControllers;
    //return faceMatchingControllers.at(0).getNormalizedSimilarity(target1, target2);
}

cv::Mat FaceMatchingMuti::cropImg(cv::Mat img) {
    return faceMatchingControllers.at(0).cropImg(img);
}


