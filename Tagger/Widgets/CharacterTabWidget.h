#pragma once
#include <qboxlayout.h>
#include <qlistwidget.h>
#include <qpushbutton.h>
#include <QCompleter>
#include "Engines/TagEngine.h"
#include <qlineedit.h>
#include <qinputdialog.h>
#include <qsizepolicy.h>
#include "Widgets/Tagholder.h"
/*
		Despite its name, this file contains unspecialized widget, holding 3 list views and tag operation buttons
			TripleListWidget	-	Children of QWidget, holds 3 list views, search string and a lot of buttons
				provides signals:
						tags_erase(QList<Tag>)	-	provides a list of selected tags to erase it
						got_tags(QList<Tag>)	-	provides a list of selected tags to add them
						tags_clear()			-	provides request to drop tag sequence
*/

class TripleListTabWidget : public QWidget
	//	This widget creates 3 list views with provided names and tags
{
	Q_OBJECT
private:
	QStringList taglist[3];
	QString list_names[3];
	int idOffsets[3];
	QHBoxLayout * mainLayout;		//	Layouts
	QVBoxLayout * listLayout;
	QHBoxLayout * tagLayout;
	QVBoxLayout * buttonHolder;
	TagHolder * tag_hld[3];
	QPushButton * addButton_1;		//	Buttons for adding new tags
	QPushButton * addButton_2;
	QPushButton * addButton_3;
	QPushButton * eraseButton;		//	Other buttons
	QPushButton * deleteButton;
	QPushButton * apply;
	QPushButton * selclear;
	QPushButton * tagclear;
	QCompleter * completer;			//	Search	through the tags
	QLineEdit * search; 
public:
	enum add_destination { list1, list2, list3, all };		//	This enum provides destination to sort functions
	//Constructor
	TripleListTabWidget(QWidget *parent, 
		QList<Tag> & list_1, QList<Tag> & list_2, QList<Tag> & list_3, QStringList list_names, int& idStart, int nfsz = 15, int bfsz = 20 );
	void sort_and_refresh(add_destination dest);			//Sorter
	QList<QStringList *> get_tag_keys() {					//	Getter of tag names list
		return QList<QStringList *>({ &taglist[0], &taglist[1], &taglist[2] });
	}
	void setup_completer();	// This setups search
	void prepare(int id);
	void applyCurrent();
public slots:
	void add_tag_press(int dest, QString text = "");
	void add_tag_call(int dest);
	void erase_press();
	void list_clicked(QListWidgetItem * qlwi);
	void search_completed();
	void delete_press();
	void apply_press();
	void clear_selection();
	void clear_tags();
	void goOutOfHolder(int id, bool forward);
signals:
	void tags_erase(QList<Tag> tlist);
	void got_tags(QList<Tag> tlist);
	void tags_clear();
	void goOutThis(int id, int page);
	void taggingThisImageDone();
};

