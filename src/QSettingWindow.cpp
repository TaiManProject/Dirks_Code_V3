#include "QSettingWindow.h"

QSettingWindow::QSettingWindow(QWidget *parent, Qt::WindowFlags flags ) : QWidget(parent, flags) {
    commonTool.log("Opening Setting Window.");
	ui.setupUi(this);

    //init parameter
	param.initialized = false;
	param.mode = _CONTROLLER_S2P_;
    param.inputImagePath = "C:/Users/Mosquito/Desktop/f1-001-01.jpg";
    currentDataPath = dataPath;
    param.dataPath = currentDataPath;
	param.numberOfKDTree = 4;
	param.modeOfKDTree = _KDTREE_SINGLE_;
	param.numberOfLeafChecked = 1024;
	param.additionDim = true;
	param.additionDimWeight = 99;
	param.foundKNNMode = _KNN_KDTREE_;
	param.samplingRate = 2;
	param.newPatch = false;
	param.numOfGoodCan = 10;
	param.numOfRandomCan = 0;
    param.SearchWidthUpper = 5;       // 5 pixels for upper, lower, left and right search range for each window
    param.SearchWidthLower = 8;
    param.patchSizeUpper = 20;
    param.patchSizeLower = 10;
    param.OverlapSizeUpper = 5;
    param.OverlapSizeLower = 5;
	param.imageCol = 200;
	param.imageRol = 250;

    updateUI();
	this->adjustSize();
}


void QSettingWindow::on_but_dataPath_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Select Data Path"),
                                                    currentDataPath,
													QFileDialog::ShowDirsOnly
													| QFileDialog::DontResolveSymlinks);
	if (!photoPath.isNull()){
        currentDataPath = path;
		updateUI();
	}
    
}

void QSettingWindow::updateUI(){
    ui.lab_dataPath->setText(currentDataPath);
}


void QSettingWindow::on_okButton_clicked() {
    if (ui.rab_appBorder->isChecked()){
        commonTool.log("Opening Border Control Prototype (AKA Prototype #2).");
		mainWindow = new QBorderMainWindow(param);
	} else if (ui.rab_appVideo->isChecked()){
        commonTool.log("Opening Surveillance Video Prototype (AKA Prototype #3).");
		mainWindow = new SurveillanceVideoPrototype(param);
	}
    mainWindow->show();
    this->setVisible(false);
}
