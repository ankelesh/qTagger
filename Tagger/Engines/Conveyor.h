#pragma once
#include <qstring.h>
#include <qdir.h>
#include <qsize.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qpair.h>
#include <qset.h>

/*
	This file contains instruments for cycling through the folder holding files to tag
		abs_conveyor	-	abstract interface class for polymorthism
		Conveyor		-	cycles through images
		ComicConveyor	-	cycles through subfoldes holding comics
		EndOCol			-	xpm icon for showing the end of image collection
*/

class abs_conveyor		//	abstract interface
{
protected:
	QString rpath;		//	holds directory path to cycle
	QDir root;			//	directory object to make paths
	QString opath;		//	output directory path
	QDir outdir;	
	QSize resolution;	//	resolution to scale images
	QSet<QString> not_needed;	//	images or comics, which already was cycled
	bool works;			//	this flag shows if conveyor was initialized and has images to show
	bool no_erase;		//	optimisation flag, saves some time if there is no need to check	notneeded-set
public:
	abs_conveyor() {};
	virtual bool initialize(QString rootpath, QString outputpath, QSize res = QSize(500, 1000)) = 0;
	//	non-constructor initializer method
	virtual bool store(QString filename = "plot.jpg", bool backup = false, bool no_erase = false) = 0;
	//	this method must copy, backup and erase image or comic from root to output directory
	virtual void setResolution(QSize qs) = 0;
	//	setter
	virtual QPair<QPixmap, QString> next() = 0;
	//	this method gives next image or comic from root folder
	virtual void utilizeCurrent(bool backup) = 0;
	//	deletes current item
	bool ready() { return works; }
};
class Conveyor : public abs_conveyor
	//	Image-cycling conveyor
{
private:
	QString current;	//	holds name of currently opened image
public:
	Conveyor() { works = false; };
	Conveyor(QString rootpath, QString outputpath, QSize res = QSize(500,1000));
	bool initialize(QString rootpath, QString outputpath, QSize res = QSize(500, 1000));
	//	Constructors
	QPair<QPixmap, QString> next();
	bool store(QString filename = "plot.jpg", bool backup = false, bool no_erase = false);
	void setResolution(QSize qs) { resolution = qs; };
	bool ready() { return works; }
	void utilizeCurrent(bool backup);
	//Overrides
};
class ComicConveyor : public abs_conveyor
	//	Cycles through comic subfoldes
{
private:
	QDir current;	//	current subdirectory
	QString name;	//	name of the comic
	int pos;		//	position of current image
	QStringList comic;	//	list of image names for cycling needs
	QList<int> numbers;	//	holds numbers of each page for changing order puproces
public:
	ComicConveyor() { works = false; }
	ComicConveyor(QString rootpath, QString outputpath, QSize res = QSize(500, 1000));
	bool initialize(QString rootpath, QString outputpath, QSize res = QSize(500, 1000));
	// Constructors
	QPair<QPixmap, QString> next();
	bool ready() { return works; }
	void utilizeCurrent(bool backup);
	bool store(QString filename = "comic", bool backup = false, bool no_erase = false);
	void setResolution(QSize qs) { resolution = qs; };
	//Overrides
	QPair <QPixmap, int> stepForward();		//	cycling through comic pages
	QPair<QPixmap, int> stepBack();
	bool utilizeCurrentImage(bool backup);	//	Deletes image at position pos
	int position() const;					//	getters
	int totalPages() const;
	void setnum(int num) { numbers[pos] = num; }	//	setter
	int getnum(int ps) { return (ps > 0 && ps < numbers.count()) ? numbers[ps] : numbers[pos]; }	
	int getnum() { if (works && numbers.count() > 0) return numbers.at(pos); else return 0; }
	//This returns number of the page
};

/* XPM */
static char *EndOCol[] = {
	/* columns rows colors chars-per-pixel */
	"44 20 24 1 ",
	"  c black",
	". c #2B0000",
	"X c #00002B",
	"o c #2B002B",
	"O c gray17",
	"+ c #550000",
	"@ c #000055",
	"# c #802B00",
	"$ c #AA5500",
	"% c #80802B",
	"& c #D4802B",
	"* c #FFAA55",
	"= c #002B80",
	"- c #0055AA",
	"; c #2B80AA",
	": c #2B80D4",
	"> c #55AAFF",
	", c #FFD480",
	"< c #FFFFAA",
	"1 c #80D4FF",
	"2 c #AAFFFF",
	"3 c #FFFFD4",
	"4 c #D4FFFF",
	"5 c white",
	/* pixels */
	"55555555555555555555555555555555555555555555",
	"55555555555555555555555555555555555555555555",
	"55555555555555555555555555555555555555555555",
	"55555555555555555555555555555555555555555555",
	"55555555555555555555555555555555555555555555",
	"555555555553#    :5$ :555&-53#    >555555555",
	"555555555553#>55555$O@255&-53#:55* :55555555",
	"555555555553#>55555$;#-55&-53#:555*@25555555",
	"555555555553#>55555$:,o15&-53#:555<+>5555555",
	"555555555553#   @25$:5$=4&-53#:555<+>5555555",
	"555555555553#>55555$:5<+:&-53#:555<+>5555555",
	"555555555553#>55555$:55*@%-53#:555*@25555555",
	"555555555553#>55555$:553# -53#:55,.:55555555",
	"555555555553#    :5$:555,.-53#   X1555555555",
	"55555555555555555555555555555555555555555555",
	"55555555555555555555555555555555555555555555",
	"55555555555555555555555555555555555555555555",
	"55555555555555555555555555555555555555555555",
	"55555555555555555555555555555555555555555555",
	"55555555555555555555555555555555555555555555"
};
