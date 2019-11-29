#include "CharacterTabWidget.h"


TripleListTabWidget::TripleListTabWidget(QWidget *parent, 
	QList<Tag> & list_1, QList<Tag> & list_2, QList<Tag> & list_3, QStringList List_names, int& idstart, int interface_size, int tag_size)
	: QWidget(parent), 
	mainLayout(new QHBoxLayout(this)), listLayout(new QVBoxLayout(this)),
	tagLayout(new QHBoxLayout(this)), buttonHolder(new QVBoxLayout(this)),
	tag_hld
{
	new TagHolder(idstart, list_1,  this), 
	new TagHolder(idstart+1, list_2,  this), 
	new TagHolder(idstart+2, list_3,  this)
}
, addButton_1(new QPushButton(this)),
	addButton_2(new QPushButton( this)), addButton_3(new QPushButton(this))
	, eraseButton(new QPushButton("Erase\ntags", this)), deleteButton(new QPushButton("Delete\ntags", this)),
	apply(new QPushButton("Apply\ntags", this)), selclear(new QPushButton("Deselect", this)),
	tagclear(new QPushButton("Clear\ntags", this)),
	completer(new QCompleter(this)), search(new QLineEdit(this))
{
	for (int i = 0; i < 3; ++i)
	{
		idOffsets[i] = idstart++;
	}
	//  Layouts initialization
	this->setLayout(mainLayout);
	mainLayout->addLayout(buttonHolder);
	buttonHolder->addWidget(addButton_1);
	buttonHolder->addWidget(addButton_2);
	buttonHolder->addWidget(addButton_3);
	buttonHolder->addWidget(eraseButton);
	buttonHolder->addWidget(deleteButton);
	buttonHolder->addWidget(apply);
	buttonHolder->addWidget(selclear);
	buttonHolder->addWidget(tagclear);
	mainLayout->addLayout(listLayout);
	listLayout->addWidget(search);
	listLayout->addLayout(tagLayout);
	tagLayout->addWidget(tag_hld[0]);
	tagLayout->addWidget(tag_hld[1]);
	tagLayout->addWidget(tag_hld[2]);
	//	Naming button
	if (List_names.count() == 3)
	{
		std::copy(List_names.begin(), List_names.end(), list_names);
	}
	QFont font_int = QFont("Times new Roman", interface_size);
	QFont font_tag = QFont("Arial", tag_size);
	addButton_1->setText("Add\n " + list_names[0] + "\ntag");
	addButton_1->setFont(font_int);
	addButton_2->setText("Add\n " + list_names[1] + "\ntag");
	addButton_2->setFont(font_int);
	addButton_3->setText("Add \n" + list_names[2] + "\ntag");
	addButton_3->setFont(font_int);
	//	Buttons size setup
	QSizePolicy exp = QSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
	addButton_1->setSizePolicy(exp);
	addButton_2->setSizePolicy(exp);
	addButton_3->setSizePolicy(exp);
	eraseButton->setSizePolicy(exp);
	eraseButton->setFont(font_int);
	deleteButton->setSizePolicy(exp);
	deleteButton->setFont(font_int);
	apply->setSizePolicy(exp);
	apply->setFont(font_int);
	selclear->setSizePolicy(exp);
	selclear->setFont(font_int);
	tagclear->setSizePolicy(exp);
	tagclear->setFont(font_int);
	//	Set selection mode
	for (int i = 0; i < 3; ++i)
	{
		tag_hld[i]->setSelectionMode(QAbstractItemView::SelectionMode::MultiSelection);
		tag_hld[i]->setFont(font_tag);
		tag_hld[i]->reload();

		QObject::connect(tag_hld[i], &TagHolder::confirmThisImage, this, &TripleListTabWidget::taggingThisImageDone);
		QObject::connect(tag_hld[i], &QListWidget::itemDoubleClicked, this, &TripleListTabWidget::list_clicked);
		QObject::connect(tag_hld[i], &TagHolder::addRequest, this, &TripleListTabWidget::add_tag_call);
		QObject::connect(tag_hld[i], &TagHolder::eraseRequest, this, &TripleListTabWidget::erase_press);
		QObject::connect(tag_hld[i], &TagHolder::goOutHere, this, &TripleListTabWidget::goOutOfHolder);
		QObject::connect(tag_hld[i], &TagHolder::applyRequest, this, &TripleListTabWidget::apply_press);
	}
	
	//	Sorting and filling completer
	sort_and_refresh(all);
	setup_completer();
	search->setFont(font_int);
	//Connecting slots
	QObject::connect(addButton_1, &QPushButton::clicked, this, [this]() { add_tag_press(list1); });
	QObject::connect(addButton_2, &QPushButton::clicked, this, [this]() { add_tag_press(list2); });
	QObject::connect(addButton_3, &QPushButton::clicked, this, [this]() { add_tag_press(list3); });
	QObject::connect(eraseButton, &QPushButton::clicked, this, &TripleListTabWidget::erase_press);
	QObject::connect(deleteButton, &QPushButton::clicked, this, &TripleListTabWidget::delete_press);
	QObject::connect(apply, &QPushButton::clicked, this, &TripleListTabWidget::apply_press);
	QObject::connect(search, &QLineEdit::returnPressed, this, &TripleListTabWidget::search_completed);
	QObject::connect(selclear, &QPushButton::clicked, this, &TripleListTabWidget::clear_selection);
	QObject::connect(tagclear, &QPushButton::clicked, this, &TripleListTabWidget::clear_tags);
}
void TripleListTabWidget::sort_and_refresh(add_destination dest)
{
	if (dest > 2)
	{
		for (int i = 0; i < 3; ++i)
		{
			tag_hld[i]->sortingReload();
		}
	}
	else
	{
		tag_hld[dest]->sortingReload();
	}
}
void TripleListTabWidget::setup_completer()
//	Sets up the completer with refreshed values
{
	QStringList forcomp = taglist[0] + taglist[1] + taglist[2];
	delete completer;
	completer = new QCompleter(forcomp, this);
	search->setCompleter(completer);
}
void TripleListTabWidget::prepare(int id)
{
	int ind = (id - 1) % 3;
	tag_hld[ind]->setFocus();
	if (tag_hld[ind]->currentItem() == nullptr)
	{
		tag_hld[ind]->setCurrentRow(0);
	}
	else
	{
		tag_hld[ind]->markCurrentItem();
	}
}
void TripleListTabWidget::applyCurrent()
{
	apply_press();
}
void TripleListTabWidget::add_tag_press(int dest, QString text)
//	Calls dialog to get new tag
{
	QString add;
	bool ok;
	switch (dest)
	{
	case all:
		return;
	default:
		add = QInputDialog::getText(this, "Add tag", "Enter new " + list_names[dest] + " tag", QLineEdit::Normal, text, &ok);
		tag_hld[dest]->addTag(add);
	}
	setup_completer();
}
void TripleListTabWidget::add_tag_call(int dest)
{
	QString add;
	bool ok;
	switch ((dest-1)%3)
	{
	case all:
		return;
	default:
		add = QInputDialog::getText(this, "Add tag", "Enter new " + list_names[(dest-1)%3] + " tag", QLineEdit::Normal, "", &ok);
		tag_hld[(dest-1)%3]->addTag(add);
	}
	setup_completer();
}
void TripleListTabWidget::erase_press()
//	Sends request to erase selected tags
{
	QList<Tag> toDelete;
	for (int t = 0; t < 3; ++t)
	{
		toDelete << tag_hld[t]->getSelectedTags();
	}
	emit tags_erase(toDelete);
}
void TripleListTabWidget::list_clicked(QListWidgetItem * qlwi)
// This slot sends request to apply tag which was double-clicked
{
	QList<Tag> ql;
	ql.push_back(Tag(qlwi->text()));
	emit got_tags(ql);	//	send a list with one tag
	sort_and_refresh(all);	//	sort all, maybe its better to optimise by deducing list number, but difference in performance is little
}
void TripleListTabWidget::search_completed()
// This slot sends a request to apply one tag or asks for a new tag
{
	QString line = search->text();
	for (int i = 0; i < 3; ++i)
	{
		Tag & foundtag = tag_hld[i]->findTag(line);
		if (foundtag.weight != -1)
		{
			emit got_tags(QList<Tag>({ foundtag }));
			++foundtag.weight;
			return;
		}
	}
		bool ok;
		QString curr = "";
		curr = QInputDialog::getItem(this, "Which column?", "Please select column",
			QStringList({ list_names[0], list_names[1], list_names[2] }), 1, false, &ok);
		if (ok)	//	if the column WAS selected
		{
			for (int i = 0; i < 3; ++i)
			{
				if (curr == list_names[i])
				{
					add_tag_press((TripleListTabWidget::add_destination)i, curr);
					break;
				}
		}
	}
}
void TripleListTabWidget::delete_press()
//	Similar to erase slot, but instead of emitting signal - just deletes tags
{
	for (int i = 0; i < 3; ++i)
	{
		tag_hld[i]->deleteSelectedTags();
	}
	setup_completer();
}
void TripleListTabWidget::apply_press()
//	This slot is ABSOLUTELY similar with erase slot, just calls other signal
{
	QList<Tag> toApply;
	for (int t = 0; t < 3; ++t)
	{
		toApply << tag_hld[t]->getSelectedTags();
	}
	emit got_tags(toApply);
	sort_and_refresh(all);
}
void TripleListTabWidget::clear_selection()
//	This slot clears selections of all three lists
{
	for (int i = 0; i < 3; ++i)
	{
		tag_hld[i]->clearSelection();
	}
}
void TripleListTabWidget::clear_tags()
//	Passes signal outside
{
	emit tags_clear();
}

void TripleListTabWidget::goOutOfHolder(int id, bool forward)
{
	if (forward)
	{
		if (id == idOffsets[2])
		{
			emit goOutThis(id + 1, -1);
			return;
		}
		else
		{
			prepare(id + 1);
			return;
		}
	}
	else
	{
		if (id == idOffsets[0])
		{
			emit goOutThis(id - 1, -2);
		}
		else
		{
			prepare(id - 1);
		}
	}
}
