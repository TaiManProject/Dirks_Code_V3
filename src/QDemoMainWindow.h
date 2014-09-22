#ifndef DEMOMAINWINDOW_H
#define DEMOMAINWINDOW_H

#include <iostream>
#include <string>

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>
#include <QGraphicsScene>
#include <QGridLayout>
#include <QPointF>
#include <QGraphicsView>
#include <QRect>
#include <QGraphicsRectItem>
#include <QDateTime>
#include <QTextStream>
#include <QDir>

#include <opencv/cv.h>

#include "ui_QDemoMainWindow.h"
#include "GeneralManager.h"
#include "QCandidatePatchWidget.h"
#include "QCandidatesWindow.h"

class QDemoMainWindow : public QMainWindow
{
	Q_OBJECT

public:
    QDemoMainWindow(Parameter parameter_p, QWidget *parent = 0, Qt::WindowFlags flags = 0);
	~QDemoMainWindow();
	
	QGraphicsScene *inputImageScene;
    QGraphicsScene *outputImageScene;
    QGraphicsScene *faceRecognitionScene;

	QGraphicsPixmapItem *inputImagePixmap;
	QGraphicsPixmapItem *outputImagePixmap;
	QGraphicsPixmapItem *faceRecogImagePixmap;

    QGraphicsRectItem *inputImagePatchRect;
    QGraphicsRectItem *outputImagePatchRect;

    //QVBoxLayout *candidatePatchsLayout;
    //QWidget * candidatePatchsWidget;

	QCandidatesWindow* candidatesWindow;

private slots:

    void on_but_chooseInputImage_clicked();
    void on_but_createSketch_clicked();


    void on_but_save_clicked();
    void on_but_captureImage_clicked();
	void on_but_faceRecogintion_clicked();

	void on_act_preferences_actived();
	void on_act_saveImg_actived();
	void on_act_saveImputImg_actived();

	void showFaceRecogResult(int rank);
	

public slots:
    void imageGraphicsViewClicked(QPointF scenePoint);
	void setSetting();
	void removeSettingWindows();
	void startCommandFile(QString commandFilePath);
	void changeCandidatePatchOrgImg(int type, int photoID, QRect patchPos);
	void updateUISlot();

private:
	Ui::MainWindowClass ui;

	QLabel* faceSynthesisDatabaseStateBarText;
	QLabel* faceRecognitionDatabaseStateBarText;
	QLabel* timeNeededText;
	QLabel* maxSearchText;

	GeneralManager* generalManager;
    void checkMode();
    void updateUI();
	Parameter* getPatameterFromCommand(QStringList commandLineItems);

	bool disconnect_lsw_faceRecogResult;

};

#endif // MAINWINDOW_H
