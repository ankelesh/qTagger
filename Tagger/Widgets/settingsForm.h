#pragma once

#include <qformlayout.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include "LaunchInfo.h"

class settingsForm : public QWidget
	//	This class coonstructs form for user input
{
	Q_OBJECT
private:
	QFormLayout* formlayout;
	QLineEdit* rpath;		//	input dir
	QLineEdit* opath;		//	output dir
	QHBoxLayout* rb;
	QRadioButton* comicmode;	//	radiobuttons to determine which mode will be used
	QRadioButton* stdmode;
	QCheckBox* d_auth;			//	checkboxes	for bool variables in options
	QCheckBox* cl_tg;
	QCheckBox* bck;
	QCheckBox* delor;
	QSpinBox* font_int;
	QSpinBox* font_tag;
public:
	settingsForm(QWidget* qw, LaunchInfo::options& opts);
	LaunchInfo::options submit();	//	collects info from form into options structure
public slots:
	void onChange();			//		changes opath to be synced with rpath
	void onEnter();				//		sends request to continue
signals:
	void done();				//		is emitted when user finishes filling the form
};