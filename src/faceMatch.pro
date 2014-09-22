TEMPLATE = app
TARGET = faceMatch
INCLUDEPATH += .

CONFIG += qt
#CONFIG += console
QT += widgets

#LIBS += -L/opt/local/lib
#LIBS += -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_objdetect -lopencv_contrib
#LIBS += -lstasm4 #need to compile by yourself
#LIBS += -lgsl -lgslcblas -lm #available via macports
#INCLUDEPATH += "/opt/local/include" "/opt/local/include/opencv"

################
#LIBS += D:\OpenCV\opencv\build\x86\vc10\lib
#LIBS += D:\OpenCV\opencv\build\x86\vc10\staticlib
LIBS += D:\OpenCV\opencv\build\x86\vc10\lib\opencv_calib3d248.lib
LIBS += D:\OpenCV\opencv\build\x86\vc10\lib\opencv_contrib248.lib
LIBS += D:\OpenCV\opencv\build\x86\vc10\lib\opencv_core248.lib
LIBS += D:\OpenCV\opencv\build\x86\vc10\lib\opencv_features2d248.lib
LIBS += D:\OpenCV\opencv\build\x86\vc10\lib\opencv_flann248.lib
LIBS += D:\OpenCV\opencv\build\x86\vc10\lib\opencv_gpu248.lib
LIBS += D:\OpenCV\opencv\build\x86\vc10\lib\opencv_highgui248.lib
LIBS += D:\OpenCV\opencv\build\x86\vc10\lib\opencv_imgproc248.lib
LIBS += D:\OpenCV\opencv\build\x86\vc10\lib\opencv_legacy248.lib
LIBS += D:\OpenCV\opencv\build\x86\vc10\lib\opencv_ml248.lib
LIBS += D:\OpenCV\opencv\build\x86\vc10\lib\opencv_nonfree248.lib
LIBS += D:\OpenCV\opencv\build\x86\vc10\lib\opencv_objdetect248.lib
#LIBS += C:\Users\Mosquito\Desktop\stasm4.1.0\stasm4.1.0\build\
LIBS += C:\Users\Mosquito\Desktop\stasm.lib
#LIBS += C:\Users\Mosquito\Desktop\stasm4.1.0\vc10\Debug\stasm_lib.obj
#LIBS += C:\Users\Mosquito\Desktop\stasm4.1.0\vc10\Debug\startshape.obj


INCLUDEPATH += D:\OpenCV\opencv\build\include
INCLUDEPATH += D:\OpenCV\opencv\build\include\opencv
# INCLUDEPATH += D:\OpenCV\opencv\build\include\opencv2
INCLUDEPATH += C:\Users\Mosquito\Desktop\hugin-2013.0.0\src\foreign\flann
INCLUDEPATH += C:\Users\Mosquito\Desktop\hugin-2013.0.0\src\foreign
INCLUDEPATH += C:\Users\Mosquito\Desktop\stasm4.1.0\stasm4.1.0\stasm4
INCLUDEPATH += C:\Users\Mosquito\Desktop\stasm4.1.0\stasm4.1.0\

# Input
HEADERS += \
           CandidatePatchSet.h \
           commonTool.h \
           FaceMatchingManager.h \
           FaceMatchingMutilController.h \
           FaceMatchingSimilarityTree.h \
           FaceSynthesisManager.h \
           FSTrainingThread.h \
           GeneralManager.h \
           IntegralImage.h \
           IntegralImages.h \
           QBorderMainWindow.h \
           QCameraWidget.h \
           QCandidatePatchGraphicsView.h \
           QCandidatePatchScrollArea.h \
           QCandidatePatchWidget.h \
           QCandidatesWindow.h \
           QDemoMainWindow.h \
           QImageGraphicsView.h \
           QInputSuspectTableModel.h \
           QInputVideoTableModel.h \
           QSettingWindow.h \
           QVideoProgressWindow.h \
           SearchWin.h \
           Synthesis.h \
           UsingQP.h \
    FaceAlignment.h \
    FaceMatchingSingle.h \
    SurveillanceVideoPrototype.h \
    VideoData.h \
    VideoDataProxyModel.h \
    VideoDataTableModel.h \
    VideoProcessingThread.h \
    ClickableQLabel.h \
    stasm4/asm.h \
    stasm4/atface.h \
    stasm4/basedesc.h \
    stasm4/classicdesc.h \
    stasm4/convshape.h \
    stasm4/err.h \
    stasm4/eyedet.h \
    stasm4/eyedist.h \
    stasm4/faceroi.h \
    stasm4/hat.h \
    stasm4/hatdesc.h \
    stasm4/landmarks.h \
    stasm4/landtab_muct77.h \
    stasm4/misc.h \
    stasm4/pinstart.h \
    stasm4/print.h \
    stasm4/shape17.h \
    stasm4/shapehacks.h \
    stasm4/shapemod.h \
    stasm4/startshape.h \
    stasm4/stasm.h \
    stasm4/stasm_landmarks.h \
    stasm4/stasm_lib.h \
    stasm4/stasm_lib_ext.h
FORMS += QBorderMainWindow.ui \
         QCameraWidget.ui \
         QCandidatePatchWidget.ui \
         QCandidatesWindow.ui \
         QDemoMainWindow.ui \
         QSettingWindow.ui \
         QVideoMainWindow.ui \
         QVideoProgessWindow.ui \
         widgetcandidatepatch.ui
SOURCES += \
           CandidatePatchSet.cpp \
           commonTool.cpp \
           FaceMatchingManager.cpp \
           FaceMatchingMutilController.cpp \
           FaceMatchingSimilarityTree.cpp \
           FaceSynthesisManager.cpp \
           FSTrainingThread.cpp \
           GeneralManager.cpp \
           IntegralImage.cpp \
           IntegralImages.cpp \
           main.cpp \
           QBorderMainWindow.cpp \
           QCameraWidget.cpp \
           QCandidatePatchGraphicsView.cpp \
           QCandidatePatchScrollArea.cpp \
           QCandidatePatchWidget.cpp \
           QCandidatesWindow.cpp \
           QDemoMainWindow.cpp \
           QImageGraphicsView.cpp \
           QInputSuspectTableModel.cpp \
           QInputVideoTableModel.cpp \
           QSettingWindow.cpp \
           QVideoProgressWindow.cpp \
           SearchWin.cpp \
           Synthesis.cpp \
           UsingQP.cpp \
    FaceAlignment.cpp \
    FaceMatchingSingle.cpp \
    SurveillanceVideoPrototype.cpp \
    VideoDataProxyModel.cpp \
    VideoDataTableModel.cpp \
    VideoProcessingThread.cpp \
    stasm4/asm.cpp \
    stasm4/classicdesc.cpp \
    stasm4/convshape.cpp \
    stasm4/err.cpp \
    stasm4/eyedet.cpp \
    stasm4/eyedist.cpp \
    stasm4/faceroi.cpp \
    stasm4/hat.cpp \
    stasm4/hatdesc.cpp \
    stasm4/landmarks.cpp \
    stasm4/misc.cpp \
    stasm4/pinstart.cpp \
    stasm4/print.cpp \
    stasm4/shape17.cpp \
    stasm4/shapehacks.cpp \
    stasm4/shapemod.cpp \
    stasm4/startshape.cpp \
    stasm4/stasm.cpp \
    stasm4/stasm_lib.cpp
RESOURCES += mainwindow.qrc
