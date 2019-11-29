#include "Widgets/MainWindow.h"
#include <QApplication>
#include <qscreen.h>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QRect rct = app.primaryScreen()->availableGeometry();
	mainWindow main(rct);
	return app.exec();
}
