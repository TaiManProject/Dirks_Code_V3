#define _ALLOW_ITERATOR_DEBUG_LEVEL_MISMATCH
#include "QSettingWindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    commonTool.log("Starting FaceMatch application.");
	QApplication a(argc, argv);
	QSettingWindow w;
	w.show();
	srand ( time(NULL) );
	return a.exec();
}
