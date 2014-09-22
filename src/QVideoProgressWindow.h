#pragma once

#include <QMainWindow>
#include <QTableView>
#include <QFileDialog>

#include "ui_QVideoProgessWindow.h"

#include "commonTool.h"


class QVideoProgressWindow : public QMainWindow
{
	Q_OBJECT

public:
	QVideoProgressWindow(void);
    QVideoProgressWindow(int numberOfInputSuspect, int numberOfVideos, QWidget *parent = 0, Qt::WindowFlags flags = 0);
	~QVideoProgressWindow(void);


public slots:
	//void setTotalNumberOfFaceInVideos(QList<int> numberOfFaceInVideos);
	//void setSuspectFaceAlignmentProgess(int suspectNumber);
	//void setVideoFaceDetectionProgess(int videoID, int progress);
	//void setVideoFaceAlignmentProgess(int videoID, int currentFaceNumber, int totalFaceNumber);
	//void setFaceRecognitionProgess(int resultNumber);
	void updateProgress(int type, int current, int max);
	void allDone();

private:
	//UI
	Ui::VideoProgress ui;

	//for current video process
	int *progressForVidoes;

	//for max value of progress bar
	int numberOfInputSuspect;
	int numberOfVideos;
	QList<int> numberOfFaceInVideos;
	
private slots:
	
};

