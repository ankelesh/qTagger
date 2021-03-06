#pragma once
#include "CharacterTabWidget.h"
#include "Widgets/TagHolder.h"
/*
		This file contains specialized widget for managing author tags
			AuthorTabWidget		-	QWidget child, contains QListWidget and lot of buttons
				provides signals:
					author_add(Tag) 		-	new author was added to the list
					author_erase()			-	request for deleting author
					break_deduction(QString)-	request for moving author back to filename
					got_author(Tag)			-	new author was chosen by user
*/

class AuthorTabWidget : public QWidget
	//	This class is specialized for author operations 
{
	Q_OBJECT
private:
	taglist* allauthors;
	QStringList tags;
	Tag lastAuthor;				//	Holds last author choose
	QHBoxLayout * mainLayout;	//	Holds all elements
	QVBoxLayout * listLayout;	//	Holds list widget and search string
	QVBoxLayout * buttonHolder;	//	Holds all buttons
	TagHolder * tagholder;	//	Holds author names
	QPushButton * addButton;	
	QPushButton * eraseButton;
	QPushButton *breakDeduce;
	QPushButton * unknownButton;
	QPushButton * deleteButton;
	QPushButton * apply;
	QCompleter * completer;		//	Provides completing to search widget
	QLineEdit * search;
	int & global_tag_counter;	//	Link to the global counter, which provides unique values for tag id's
public:
	AuthorTabWidget(QWidget * root, QList <Tag> & auths, int & glcl, int fntszb = 20, int fntszn = 20);
	//This function moves most popular tags to the top
	void sort_and_refresh();
	//getters and setters
	QString getLAuthor() { return lastAuthor.tag; };
	void setLAuthor(Tag & t) { lastAuthor = t; };
	void setLAuthor(QString & qs);
	int getAuthsCount() const { return allauthors->count(); }
	QListWidgetItem * getCurrItem() { return tagholder->currentItem(); };
	QList<Tag> getAuthors();
	void prepare();
	void applyCurrent();
public slots:
	void add_tag_press(int id = 0);
	void erase_press();
	void break_de_press();
	void list_clicked(QListWidgetItem * qlwi);
	void unknown_press();
	void search_completed();
	void apply_press();
	void leaveThis(int id, bool forward);
signals:
	void author_add(Tag t);
	void author_erase();
	void break_deduction(QString qs);
	void got_author(Tag t);
	void goOutThis(int id, int page);
	void taggingThisImageDone();
};


