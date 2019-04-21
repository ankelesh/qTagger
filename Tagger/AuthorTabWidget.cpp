#include "AuthorTabWidget.h"


AuthorTabWidget::AuthorTabWidget(QWidget * root, QList <Tag>  & auths, int& glcl, int interface_fnt, int tag_font)
	: QWidget(root), tags(), linker(), lastAuthor({ -1, "unknown", 0 }),
	mainLayout(new QHBoxLayout(this)),listLayout(new QVBoxLayout(this)),
	buttonHolder(new QVBoxLayout(this)),
	tagholder(new QListWidget(this)), addButton(new QPushButton("Add\nauthor", this)),
	eraseButton(new QPushButton("Erase", this)), breakDeduce(new QPushButton("Author\n to name", this))
	,unknownButton(new QPushButton("Set \nunknown", this)),deleteButton(new QPushButton("Delete\n author", this))
	, apply(new QPushButton("Apply\n tag", this)),completer(new QCompleter(tags,this)), 
	search(new QLineEdit()), global_tag_counter(glcl)
	//	Constructs and fills widgets with data
{
	//Main layout setup
	this->setLayout(mainLayout);
	mainLayout->addLayout(buttonHolder);
	mainLayout->addLayout(listLayout);
	//List layout setup
	listLayout->addWidget(search);
	listLayout->addWidget(tagholder);
	search->setCompleter(completer);
	//	Setup buttons
	QFont font_int = QFont("Times new Roman", interface_fnt);
	QFont font_tag = QFont("Arial", tag_font+ 3);
	buttonHolder->addWidget(addButton);
	QSizePolicy exp = QSizePolicy(QSizePolicy::Minimum,QSizePolicy::MinimumExpanding);
	addButton->setSizePolicy(exp);
	addButton->setFont(font_int);
	buttonHolder->addWidget(eraseButton);
	eraseButton->setSizePolicy(exp);
	eraseButton->setFont(font_int);
	buttonHolder->addWidget(breakDeduce);
	breakDeduce->setSizePolicy(exp);
	breakDeduce->setFont(font_int);
	buttonHolder->addWidget(unknownButton);
	unknownButton->setSizePolicy(exp);
	unknownButton->setFont(font_int);
	buttonHolder->addWidget(deleteButton);
	deleteButton->setSizePolicy(exp);
	deleteButton->setFont(font_int);
	buttonHolder->addWidget(apply);
	apply->setSizePolicy(exp);
	apply->setFont(font_int);
	//	Set up the linker and view of the list
	auto begin = auths.begin();
	while (begin != auths.end())
	{
		tags.push_back(begin->tag);
		linker.insert(begin->tag, *begin);
		tagholder->addItem("");
		++begin;
	}	
	tagholder->setFont(font_tag);
	sort_and_refresh();
	//	Connecting all buttons to their slots
	QObject::connect(tagholder, &QListWidget::itemDoubleClicked, this, &AuthorTabWidget::list_clicked);
	QObject::connect(addButton, &QPushButton::clicked, this, &AuthorTabWidget::add_tag_press);
	QObject::connect(eraseButton, &QPushButton::clicked, this, &AuthorTabWidget::erase_press);
	QObject::connect(breakDeduce, &QPushButton::clicked, this, &AuthorTabWidget::break_de_press);
	QObject::connect(unknownButton, &QPushButton::clicked, this, &AuthorTabWidget::unknown_press);
	QObject::connect(search, &QLineEdit::returnPressed, this, &AuthorTabWidget::search_completed);
	QObject::connect(deleteButton, &QPushButton::clicked, this, &AuthorTabWidget::delete_press);
	QObject::connect(apply, &QPushButton::clicked, this, [this](bool b) {
		emit AuthorTabWidget::list_clicked(this->getCurrItem()); });
}
void AuthorTabWidget::add_tag_press()
//	This slot ascs new tag name and creates tew tag object
{
	//Ask new name using QInputDialog
	bool ok;
	QString newAuth = QInputDialog::getText(this, "New author",
		"Please enter new author name",QLineEdit::Normal,"", &ok);
	if (ok & newAuth != "")	//	If there IS new author name
	{
		if (!tags.contains(newAuth)) //	If this author IS new
		{
			tags.push_back(newAuth);	//	new author is added to the view
			linker.insert(newAuth, Tag({ ++global_tag_counter, newAuth, 1 }));
			//	linker connects view with real tag object, which is created with value 1 
			tagholder->addItem(tags.last());	//	refresh view
		}
	}
	delete completer;
	completer = new QCompleter(tags, this);
	search->setCompleter(completer);
	emit author_add(linker[newAuth]);
}
void AuthorTabWidget::erase_press()
//	This slot just passes signal further
{
	emit author_erase();
}
void AuthorTabWidget::break_de_press()
//	pass signal further
{
	emit break_deduction(lastAuthor.tag);
}
void AuthorTabWidget::list_clicked(QListWidgetItem * qlwi)
//	This slot updates list and emits author signal
{
	if (qlwi->text() != "")	//	If the view was correct
	{
		++linker[qlwi->text()].weight;	//	update weight
		emit got_author(linker[qlwi->text()]);
		sort_and_refresh();
	}
}
void AuthorTabWidget::unknown_press()
//	This slot emits got_author with dummy values
{
	emit got_author(Tag{ 0,"unknown_author", 0 });
}
void AuthorTabWidget::search_completed()
//	This slot responds for search field operations
{
	QString line = search->text();
	if (tags.contains(line))	//	if there is author like this
	{
		emit got_author(linker[line]);
	}
	else
	{	//	Ask for the new one
		bool ok;
		QString newAuth = QInputDialog::getText(this, "New author",
			"Please enter new author name", QLineEdit::Normal, line, &ok);
		if (ok & newAuth != "")	//	Similar with add_tag_press call
		{
			if (!tags.contains(newAuth)) {
				tags.push_back(newAuth);
				linker.insert(newAuth, Tag({ ++global_tag_counter, newAuth, 1 }));
				tagholder->addItem(tags.last());
				delete completer;
				completer = new QCompleter(tags, this);
				search->setCompleter(completer);
				emit got_author(linker[newAuth]);
			}
		}
	}
}
void AuthorTabWidget::delete_press()
//	This slot responds for deleting the selected items
{
	if (tagholder->currentItem()->text() != "")
	{
		if (tags.contains(tagholder->currentItem()->text()))
		{
			//Remove tag from everywhere - from widget, linker and view
			linker.remove(tagholder->currentItem()->text());
			tagholder->takeItem(tagholder->count()-1);
			sort_and_refresh();
		}
	}
}
void AuthorTabWidget::sort_and_refresh()
//	This method updates view and sorts authors by popularity
{
	QVector<Tag> temp;			//	Temp vector for the faster sort
	temp.resize(linker.count());
	std::copy(linker.begin(), linker.end(), temp.begin());	
	std::sort(temp.begin(), temp.end(), [=](Tag & t1, Tag & t2) {return t1.weight > t2.weight; });
	//After sorting vector, starts copy actions
	auto beg = temp.begin();
	tags.clear();
	while (beg != temp.end())
	{	//	copy from vector back to tags
		tags.push_back(beg->tag);
		++beg;
	}
	for (int i = 0; i < tagholder->count(); ++i)
	{
		// refresh list view
		tagholder->item(i)->setText(tags.at(i));
	}
}
void AuthorTabWidget::setLAuthor(QString & qs)
{
	if (tags.contains(qs))
	{
		lastAuthor = linker[qs];
	}
}
QList<Tag> AuthorTabWidget::getAuthors()
//	Turns the linker map back to list
{
	QList<Tag> auths;
	auths.reserve(linker.count());
	auto beg = linker.begin();
	while (beg != linker.end())
	{
		auths.push_back(*beg);
		++beg;
	}
	return auths;
}
