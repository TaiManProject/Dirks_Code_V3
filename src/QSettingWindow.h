#ifndef WINDOWSETTING_H
#define WINDOWSETTING_H

#include <QWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QMainWindow>
#include "ui_QSettingWindow.h"

#include "commonTool.h"
#include "QDemoMainWindow.h"
#include "QBorderMainWindow.h"
#include "SurveillanceVideoPrototype.h"

class QSettingWindow : public QWidget {
	Q_OBJECT

public:
    QSettingWindow(QWidget *parent = 0, Qt::WindowFlags flags = 0);

	void checkMode();
	void checkNewPatch();

	void updateUI();

	Parameter getAllParamter();
	void setAllsetting(Parameter parameter);

	QMainWindow* mainWindow;

private slots:
    //void on_rab_p2s_clicked();
    //void on_rab_s2p_clicked();
    //void on_chb_newPatch_clicked();
    void on_but_dataPath_clicked();
    void on_okButton_clicked();
    //void on_but_commandFile_clicked();
    //void foundKNNModeChanged();
    //void on_but_faceRecogDBTraining_clicked();
    //void on_but_faceRecogDBOutputFile_clicked();
    //void on_but_startBuildTrainedFile_clicked();

private:
	Ui::windowSetting ui;
    //bool initialized;
	Parameter param;
    QString currentDataPath;

signals:
	void commandFileClicked(QString commandFilePath);
};

#endif // WINDOWSETTING_H
