#include "AuthorTabWidget.h"


AuthorTabWidget::AuthorTabWidget(QWidget * root, QList <Tag>  & auths, int& glcl, int interface_fnt, int tag_font)
	: QWidget(root), allauthors(&auths), tags(),  lastAuthor("unknown", 0 ),
	mainLayout(new QHBoxLayout(this)),listLayout(new QVBoxLayout(this)),
	buttonHolder(new QVBoxLayout(this)),
	tagholder(new TagHolder(0, auths, this)), addButton(new QPushButton("Add\nauthor", this)),
	eraseButton(new QPushButton("Erase", this)), breakDeduce(new QPushButton("Author\n to name", this))
	,unknownButton(new QPushButton("Set \nunknown", this)),deleteButton(new QPushButton("Delete\n author", this))
	, apply(new QPushButton("Apply\n tag", this)),completer(new QCompleter(tags,this)), 
	search(new QLineEdit()), global_tag_counter(glcl)
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
	tagholder->setFont(font_tag);
	sort_and_refresh();
	//	Connecting all buttons to their slots
	QObject::connect(tagholder, &QListWidget::itemDoubleClicked, this, &AuthorTabWidget::list_clicked);
	QObject::connect(tagholder, &TagHolder::addRequest, this, &AuthorTabWidget::add_tag_press);
	QObject::connect(tagholder, &TagHolder::eraseRequest, this, &AuthorTabWidget::erase_press);
	QObject::connect(tagholder, &TagHolder::applyRequest, this, &AuthorTabWidget::apply_press);
	QObject::connect(tagholder, &TagHolder::goOutHere, this, &AuthorTabWidget::leaveThis);
	QObject::connect(tagholder, &TagHolder::confirmThisImage, this, &AuthorTabWidget::taggingThisImageDone);
	QObject::connect(addButton, &QPushButton::clicked, this, &AuthorTabWidget::add_tag_press);
	QObject::connect(eraseButton, &QPushButton::clicked, this, &AuthorTabWidget::erase_press);
	QObject::connect(breakDeduce, &QPushButton::clicked, this, &AuthorTabWidget::break_de_press);
	QObject::connect(unknownButton, &QPushButton::clicked, this, &AuthorTabWidget::unknown_press);
	QObject::connect(search, &QLineEdit::returnPressed, this, &AuthorTabWidget::search_completed);
	QObject::connect(deleteButton, &QPushButton::clicked, tagholder, &TagHolder::deleteSelectedTags);
	QObject::connect(apply, &QPushButton::clicked, this, &AuthorTabWidget::apply_press);
}
void AuthorTabWidget::add_tag_press(int)
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
			tagholder->addTag(newAuth);
			//	linker connects view with real tag object, which is created with value 1 
		}
	}
	tagholder->item(tagholder->count()-1)->setSelected(true);
	emit author_add(allauthors->last());
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
	auto tlist = tagholder->getSelectedTags();
	if (tlist.count() == 1)
	{
		emit got_author(tlist.first());
	}
}
void AuthorTabWidget::unknown_press()
//	This slot emits got_author with dummy values
{
	emit got_author(Tag("unknown_author", 0));
}
void AuthorTabWidget::search_completed()
//	This slot responds for search field operations
{
	QString line = search->text();
	if (tags.contains(line))	//	if there is author like this
	{
		emit got_author(tagholder->findTag(line));
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
				tagholder->addTag(newAuth);
				delete completer;
				completer = new QCompleter(tags, this);
				search->setCompleter(completer);
				emit got_author(tagholder->findTag(newAuth));
			}
		}
	}
}

void AuthorTabWidget::apply_press()
{
	auto list = tagholder->getSelectedTags();
	if (list.isEmpty())
	{
		return;
	}
	emit got_author(list.first());
}
void AuthorTabWidget::leaveThis(int id, bool forward)
{
	if (id == 0)
	{
		if (forward)
		{
			emit goOutThis( tagholder->getId()+1, -1);
		}
		else
		{
			emit goOutThis(-1, -2);
		}
	}
}
void AuthorTabWidget::sort_and_refresh()
//	This method updates view and sorts authors by popularity
{
	std::sort(allauthors->begin(), allauthors->end(), [=](Tag & t1, Tag & t2) {return t1.weight > t2.weight; });
	tagholder->reload();
}
void AuthorTabWidget::setLAuthor(QString & qs)
{
	if (tags.contains(qs))
	{
		lastAuthor = tagholder->findTag(qs);
	}
}
QList<Tag> AuthorTabWidget::getAuthors()
//	Turns the linker map back to list
{
	return *allauthors;
}

void AuthorTabWidget::prepare()
{
	tagholder->setFocus();
}

void AuthorTabWidget::applyCurrent()
{
	apply_press();
}
