#pragma once
#include "Conveyor.h"
#include "AuthorTabWidget.h"
#include <qformlayout.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <Qt3DInput/qmouseevent.h>


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

namespace LaunchInfo 
{
	enum ConveyorMode { standard, comic };		//	defines which conveyor must be constructed
	struct options								//	holds info used for Tagger constructor
	{
		QString ifname;							//	path of input directory
		QString ofname;							//	path of output directory
		ConveyorMode mod;						//	which conveyor will be constructed
		bool deduce_author;						//	tagengine setting 
		bool clear_tags;						//	conveyor settings
		bool backup;
		bool delete_original;
		int font_interface;
		int font_tags;
		QSize winsize;							//	size of the available space for displaying image
	};
	std::wstring serialize(LaunchInfo::options & opt);	//	serialization and deserializations functs
	LaunchInfo::options deserialize(std::wfstream & fout);
}
static const LaunchInfo::options DEFAULTS({			//	This defaults are used when no saves exists
			"C:\\", "C:\\tagged", LaunchInfo::standard,	true, false, 
			true, true, INTERFACE_FONT_SIZE, TAGS_FONT_SIZE , QSize()
	});

class settingsForm : public QWidget
	//	This class coonstructs form for user input
{
	Q_OBJECT
private:
	QFormLayout * formlayout;
	QLineEdit * rpath;		//	input dir
	QLineEdit * opath;		//	output dir
	QHBoxLayout * rb;		
	QRadioButton * comicmode;	//	radiobuttons to determine which mode will be used
	QRadioButton * stdmode;
	QCheckBox * d_auth;			//	checkboxes	for bool variables in options
	QCheckBox * cl_tg;
	QCheckBox * bck;
	QCheckBox * delor;
	QSpinBox * font_int;
	QSpinBox * font_tag;
public:
	settingsForm(QWidget * qw, LaunchInfo::options & opts);
	LaunchInfo::options submit();	//	collects info from form into options structure
public slots:
	void onChange();			//		changes opath to be synced with rpath
	void onEnter();				//		sends request to continue
signals:
	void done();				//		is emitted when user finishes filling the form
};
class LaunchDialog : public QWidget
	//	This class creates window with form to asc user about settings
{
	Q_OBJECT
private:
	LaunchInfo::options opts;		//	holds options
	QVBoxLayout * mainlayout;		//	layouts and buttons
	QLabel * header;
	settingsForm * setform;
	QHBoxLayout * butlay;
	QPushButton * ok;
	QPushButton * cancel;
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
	LaunchDialog(LaunchInfo::options & opt, QRect & rct);
public slots:
	void onOk();
signals:
	void done();
};
class LabelClickable : public QLabel
	//	Simple wrapper to override method
{
	Q_OBJECT
public:
	LabelClickable(const QString &text, QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags())
		:QLabel(text, parent, f) {};
	LabelClickable(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags())
		:QLabel(parent, f) {};
	void mousePressEvent(QMouseEvent * qme)
		//	emits signal that label was clicked, determines which screen side was clicked, left = true
	{
		emit clicked((qme->x() < (size().width() / 2))); QLabel::mousePressEvent(qme);
	}
signals:
	void clicked(bool side);
};
class Tagger : public QWidget
	//	Main gui class
{
	Q_OBJECT
private:
	LaunchInfo::options options;		//	Holds options for operating the conveyor
	abs_conveyor * conveyor;			//	pointer to conveyor
	TagEngine tagengine;				//	tagengine 
	QList<Tag> all_authors;				//	holds author tags
	QMap<QString, Tag> all_tags;		//	links all tags to their names
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
	TripleListTabWidget * characterTab;
	TripleListTabWidget * actionsTab;
	TripleListTabWidget * miscTab;
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
signals:
	void save_time();
};
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
	LaunchDialog * ld;		//	launch dialog and tagger windows
	Tagger * t;
	int glc;				//	loaded value of global tag counter
public:
	full_start_set loadAll(QString fname);	//	Loads from SAVE_FILENAME program data
	mainWindow(QRect & geom) :root()
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
