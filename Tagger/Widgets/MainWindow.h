#pragma once
#include <qformlayout.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qspinbox.h>
#include "LaunchDialog.h"
#include "Tagger.h"



class mainWindow :public QObject
	//	This class controls launchDialog window and creates Tagger window
{
	Q_OBJECT
public:
	struct full_start_set
		//	This structure holds everything used in program
	{
		LaunchInfo::options opts;		//	options to make conveyor
		All_tag_storage tags;			//	all tags
		QList<Tag> authors;				//	all authors
		void erase() {
			//	method to erase all, used for saving memory
			tags.fandom.clear();
			tags.name.clear();
			tags.chtype.clear();
			authors.clear();
		}
	};
private:
	QWidget root;		//	core widget, parent of the Tagger
	full_start_set start_set;	//	Settings and tags
	LaunchDialog* ld;		//	launch dialog and tagger windows
	Tagger* t;
	int glc;				//	loaded value of global tag counter
public:
	mainWindow::full_start_set loadAll(QString fname);	//	Loads from SAVE_FILENAME program data
	mainWindow(QRect& geom) :root()
	{
		glc = 0;				//	by default, constructs empty start_set and fills it with defaults
		start_set = loadAll("appdata.txt");	//	then attempts to load data

		ld = new LaunchDialog(start_set.opts, geom);		//	presets dialog with loaded info
		QObject::connect(ld, &LaunchDialog::done, this, &mainWindow::onLDconfirm);
		ld->show();
		t = nullptr;
	}
	void dumpAll(QString fname);	//	saves program data
	~mainWindow()
		//	attempts to save data if the application was shut down
	{
		if (t != nullptr)
			dumpAll(SAVE_FILENAME);
	}
public slots:
	void onLDconfirm();
	void save_called() { dumpAll(SAVE_FILENAME); }
};