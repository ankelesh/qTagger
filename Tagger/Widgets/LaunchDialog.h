#pragma once
#include "settingsForm.h"
#include "LaunchInfo.h"
#include <qboxlayout.h>
#include <qlabel.h>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>

class LaunchDialog : public QWidget
	//	This class creates window with form to asc user about settings
{
	Q_OBJECT
private:
	LaunchInfo::options opts;		//	holds options
	QVBoxLayout* mainlayout;		//	layouts and buttons
	QLabel* header;
	settingsForm* setform;
	QHBoxLayout* butlay;
	QPushButton* ok;
	QPushButton* cancel;
	QRect scr;						//	holds size of the application window
public:
	LaunchInfo::options submit()
		//	This function wraps settings form to  fill winsize part of options
	{
		opts = setform->submit();
		opts.winsize.setHeight(scr.height() * 0.9);		//	future image will have 60% width of the screen 
		opts.winsize.setWidth(scr.width() * 0.6);
		return opts;
	};
	LaunchDialog(LaunchInfo::options& opt, QRect& rct);
public slots:
	void onOk();
signals:
	void done();
};