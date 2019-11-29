#pragma once
#include "Engines/Conveyor.h"
#include "AuthorTabWidget.h"
#include <qformlayout.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <Qt3DInput/qmouseevent.h>
#include "LaunchInfo.h"
#include "Engines/TagEngine.h"
#include "LabelClickable.h"
#define SAVE_FILENAME "appdata.txt"
static const float VERSION = 1.06;
static const float LATEST_SUPPORTED_SAVE = 1.06;
static const int   INTERFACE_FONT_SIZE = 13;
static const int   TAGS_FONT_SIZE = 15;

/*
		This file contains main gui classes and some support structures. Also defines constants used for saving.
				Constants :		VERSION				-	is written to save files to be sure that save file is suitable
								LATEST_SUPPORT_SAVE	-	defines which version was the first using save format
								SAVE_FILENAME		-	defines name of the save file
								DEFAULTS			-	settings used by defaults, when no save file exists
								INTERFACE_FONT_SIZE	-	size of interface fonts, used in buttons
								TAGS_FONT_SIZE		-	size of font used in lists holding tags
				 namespace:		LaunchInfo			-	defines options structure holding info about user settings
				 structures:	options				-	holds filenames and boolean versions used to prime conveyor
				 enums:			ConveyorMode		-	holds 2 possible modes of conveyor
				 classes:		settingsForm		-	child of QWIdget, creates form for user input
								launchDialog		-	child of QWidget, creates window and buttons for user dialog
						signals:	done			-	is sent when user confirms options
								LabelClickable		-	child of QLabel, is a simple wrapper to override the mouseEvent slots
						signals:	
									clicked(bool)	-	is emitted whenever label is clicked, bool determines the side, true=left
								Tagger				-	main widget, which holds image, tag lists, buttons etc.
						slots:	author_selected(Tag)-	operates with given value to set up author fields and tag engine
								author_erased()		-	erases author field
								author_brdeduce()	-	calls tagengine breakdeduce() method
								author_added(Tag)	-	puts new tag value to the list
								next_pressed()		-	stores result and loads new image from Conveyor
								del_pressed()		-	deletes current image and loads new image
								tags_added(QList<Tag>)-	adds tag sequence to tag engine
								tags_erased(QList<Tag>)-deletes tag sequence from tag engine
								tags_clear()		-	drops tag sequence in tag engine
								image_click_received()-	changes images using Conveyor steps
								changenum()			-	changes number of the page in comic
						signals:	save_time()		-	notifies that new tags are appeared and save required
								mainWindow			-	child of QObject, control class used to switch windows and save/loads
						slots:		
								onLDConfirm()		-	constructs tagger widget using options from settings window
								save_called()		-	dumps all program data into file with SAVE_FILENAME path
				 */		

static const LaunchInfo::options DEFAULTS({			//	This defaults are used when no saves exists
			"C:\\", "C:\\tagged", LaunchInfo::standard,	true, false, 
			true, true, INTERFACE_FONT_SIZE, TAGS_FONT_SIZE , QSize()
	});




class Tagger : public QWidget
	//	Main gui class
{
	Q_OBJECT
private:
	LaunchInfo::options options;		//	Holds options for operating the conveyor
	abs_conveyor * conveyor;			//	pointer to conveyor
	TagEngine tagengine;				//	tagengine 
	QList<Tag> all_authors;				//	holds author tags
	All_tag_storage& all_tags;		//	links all tags to their names
	QPair<QPixmap, QString> current_workset;	//	holds pair of <Image to display : its filename>
	QHBoxLayout * mainLayout;			//	Gui objects
	LabelClickable * imageSlot;
	QVBoxLayout * leftPanel;
	QHBoxLayout * buttons;
	QHBoxLayout * namepanel;
	QLineEdit * authorField;
	QLineEdit * fnameField;
	QSpinBox * numField;
	QLabel * extensionField;
	QTabWidget * tagSetupArea;
	AuthorTabWidget * authorTab;
	TripleListTabWidget * tripleTabs[3];
	QPushButton * delImgButton;
	QPushButton * nextButton;
	QPushButton * delComicPageButton;
	int tagcount;						//	this counter is used to control id in tags
	int authcount;						
public:
	int gltagcnt;
	Tagger(QWidget * root, LaunchInfo::options opts, QList<Tag> & auths, All_tag_storage & tags, int glc);
	void set_namepanel();				//	This method set ups tag engine and fills name bar
	All_tag_storage getAllTags();		//	returns all tags
	QList<Tag> getAuthors() { return authorTab->getAuthors(); };	//	returns all authors
	~Tagger() { delete conveyor; };
public slots:
	void author_selected(Tag t);
	void author_erased();
	void author_brdeduce();
	void author_added(Tag t);
	void next_pressed();
	void del_pressed();
	void tags_added(QList<Tag>  tags_to_add);
	void tags_erased(QList<Tag>  tags_to_erase);
	void tags_clear();
	void image_click_received(bool side);
	void pagenum_changed(int nvl);
	void delete_curr_comic_page();
	void gotoPageAndId(int id, int page);
signals:
	void save_time();
};

