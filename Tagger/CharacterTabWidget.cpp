#include "CharacterTabWidget.h"


TripleListTabWidget::TripleListTabWidget(QWidget *parent, QMap<QString, Tag> & lnk,
	QList<Tag> & list_1, QList<Tag> & list_2, QList<Tag> & list_3, QStringList list_names, int & glcl, int interface_size, int tag_size)
	: QWidget(parent), taglist_1(), taglist_2(), taglist_3(), list_name_1(),
	list_name_2(), list_name_3(), linker(lnk),
	mainLayout(new QHBoxLayout(this)), listLayout(new QVBoxLayout(this)),
	tagLayout(new QHBoxLayout(this)), buttonHolder(new QVBoxLayout(this)),
	tag_hld_1(new QListWidget(this)), tag_hld_2(new QListWidget(this))
	, tag_hld_3(new QListWidget(this)), addButton_1(new QPushButton( this)),
	addButton_2(new QPushButton( this)), addButton_3(new QPushButton(this))
	, eraseButton(new QPushButton("Erase\ntags", this)), deleteButton(new QPushButton("Delete\ntags", this)),
	apply(new QPushButton("Apply\ntags", this)), selclear(new QPushButton("Deselect", this)),
	tagclear(new QPushButton("Clear\ntags", this)),
	completer(new QCompleter(this)), search(new QLineEdit(this)), global_tag_counter(glcl)
{
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
	tagLayout->addWidget(tag_hld_1);
	tagLayout->addWidget(tag_hld_2);
	tagLayout->addWidget(tag_hld_3);
	//	Naming button
	if (list_names.count() > 2)
	{
		list_name_1 = list_names.at(0);
		list_name_2 = list_names.at(1);
		list_name_3 = list_names.at(2);
	}
	QFont font_int = QFont("Times new Roman", interface_size);
	QFont font_tag = QFont("Arial", tag_size);
	addButton_1->setText("Add\n " + list_name_1 + "\ntag");
	addButton_1->setFont(font_int);
	addButton_2->setText("Add\n " + list_name_2 + "\ntag");
	addButton_2->setFont(font_int);
	addButton_3->setText("Add \n" + list_name_3 + "\ntag");
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
	tag_hld_1->setSelectionMode(QAbstractItemView::SelectionMode::MultiSelection);
	tag_hld_1->setFont(font_tag);
	tag_hld_2->setSelectionMode(QAbstractItemView::SelectionMode::MultiSelection);
	tag_hld_2->setFont(font_tag);
	tag_hld_3->setSelectionMode(QAbstractItemView::SelectionMode::MultiSelection);
	tag_hld_3->setFont(font_tag);
	//	Making lists
	auto begin = list_1.begin();
	while (begin != list_1.end())
	{
		taglist_1.push_back(begin->tag);
		tag_hld_1->addItem("");
		++begin;
	}
	begin = list_2.begin();
	while (begin != list_2.end())
	{
		taglist_2.push_back(begin->tag);
		tag_hld_2->addItem("");
		++begin;
	}
	begin = list_3.begin();
	while (begin != list_3.end())
	{
		taglist_3.push_back(begin->tag);
		tag_hld_3->addItem("");
		++begin;
	}
	//	Sorting and filling completer
	sort_and_refresh(all);
	setup_completer();
	search->setFont(font_int);
	//Connecting slots
	QObject::connect(addButton_1, &QPushButton::clicked, this, [this]() {emit add_tag_press(list1); });
	QObject::connect(addButton_2, &QPushButton::clicked, this, [this]() {emit add_tag_press(list2); });
	QObject::connect(addButton_3, &QPushButton::clicked, this, [this]() {emit add_tag_press(list3); });
	QObject::connect(eraseButton, &QPushButton::clicked, this, &TripleListTabWidget::erase_press);
	QObject::connect(deleteButton, &QPushButton::clicked, this, &TripleListTabWidget::delete_press);
	QObject::connect(apply, &QPushButton::clicked, this, &TripleListTabWidget::apply_press);
	QObject::connect(search, &QLineEdit::returnPressed, this, &TripleListTabWidget::search_completed);
	QObject::connect(tag_hld_1, &QListWidget::itemDoubleClicked, this, &TripleListTabWidget::list_clicked);
	QObject::connect(tag_hld_2, &QListWidget::itemDoubleClicked, this, &TripleListTabWidget::list_clicked);
	QObject::connect(tag_hld_3, &QListWidget::itemDoubleClicked, this, &TripleListTabWidget::list_clicked);
	QObject::connect(selclear, &QPushButton::clicked, this, &TripleListTabWidget::clear_selection);
	QObject::connect(tagclear, &QPushButton::clicked, this, &TripleListTabWidget::clear_tags);
}
void TripleListTabWidget::sort_and_refresh(add_destination dest)
{
	//set up iterators and temp vector for quicker sorting
	QVector<Tag> temp;
	QList<QString>::iterator begin = taglist_1.begin();
	QVector<Tag> ::iterator beg = temp.begin();
	switch (dest)
	{
		//This switch selects list to sort, all list - cases are the same
	case all:
	case list1:
		// Filling temp vector
		temp.reserve(taglist_1.count());
		begin = taglist_1.begin();
		while (begin != taglist_1.end())
			{	temp.push_back(linker[*begin]); ++begin;}
		//	sorting
		std::sort(temp.begin(), temp.end());
		// Filling taglists back
		beg = temp.begin();
		taglist_1.clear();
		while (beg != temp.end())
			{	taglist_1.push_back(beg->tag); ++beg;}
		//Fill list widget
		for (int i = 0; i < tag_hld_1->count(); ++i)
		{
			tag_hld_1->item(i)->setText(taglist_1.at(i));
		}
		if (dest == list1) break;
	case list2:
		temp.clear();
		temp.reserve(taglist_2.count());
		begin = taglist_2.begin();
		while (begin != taglist_2.end())
		{
			temp.push_back(linker[*begin]); ++begin;
		}
		std::sort(temp.begin(), temp.end());
		 beg = temp.begin();
		taglist_2.clear();
		while (beg != temp.end())
		{
			taglist_2.push_back(beg->tag); ++beg;
		}
		for (int i = 0; i < tag_hld_2->count(); ++i)
		{
			tag_hld_2->item(i)->setText(taglist_2.at(i));
		}
		if (dest == list2) break;
	case list3:
		temp.clear();
		temp.reserve(taglist_3.count());
		begin = taglist_3.begin();
		while (begin != taglist_3.end())
		{
			temp.push_back(linker[*begin]); ++begin;
		}
		std::sort(temp.begin(), temp.end());
		beg = temp.begin();
		taglist_3.clear();
		while (beg != temp.end())
		{
			taglist_3.push_back(beg->tag); ++beg;
		}
		for (int i = 0; i < tag_hld_3->count(); ++i)
		{
			tag_hld_3->item(i)->setText(taglist_3.at(i));
		}
		break;
	}
}
void TripleListTabWidget::setup_completer()
//	Sets up the completer with refreshed values
{
	QStringList forcomp = taglist_1 + taglist_2 + taglist_3;
	delete completer;
	completer = new QCompleter(forcomp, this);
	search->setCompleter(completer);
}
void TripleListTabWidget::add_tag_press(add_destination dest, QString text)
//	Calls dialog to get new tag
{
	QString add;
	bool ok;
	switch (dest)
	{
	case list1:
		add = QInputDialog::getText(this, "Add tag", "Enter new " + list_name_1 + " tag", QLineEdit::Normal, text, &ok);
		if (ok & add != "")	//	If there IS a new name
		{
			if (!taglist_1.contains(add) & !linker.contains(add))
			{
				//	Adds new tag to linker, list and view
				taglist_1.push_back(add);
				linker.insert(add, Tag({ ++global_tag_counter, add, 1 }));
				tag_hld_1->addItem(add);
			}
		}
		break;
		//	All cases are similar to first
	case list2:
		add = QInputDialog::getText(this, "Add tag", "Enter new " + list_name_2 + " tag", QLineEdit::Normal, text, &ok);
		if (ok & add != "")
		{
			if (!taglist_2.contains(add) & !linker.contains(add))
			{
				taglist_2.push_back(add);
				linker.insert(add, Tag({++global_tag_counter, add, 1 }));
				tag_hld_2->addItem(add);
			}
		}
		break;
	case list3:
		add = QInputDialog::getText(this, "Add tag", "Enter new " + list_name_3 + " tag", QLineEdit::Normal, text, &ok); 
		if (ok & add != "")
		{
			if (!taglist_3.contains(add) & !linker.contains(add))
			{
				taglist_3.push_back(add);
				linker.insert(add, Tag({++global_tag_counter, add, 1 }));
				tag_hld_3->addItem(add);
			}
		}
		break;
	case all:
		break;
	}
	setup_completer();
}
void TripleListTabWidget::erase_press()
//	Sends request to erase selected tags
{
	QStringList tags_to_delete;	//	temp list for collecting tags
	if (tag_hld_1->selectedItems().count() > 0)
	{
		for each (auto item in tag_hld_1->selectedItems())
		{
			//	Collect tag names from all three lists
			tags_to_delete.push_back(item->text());
		}
	}
	if (tag_hld_2->selectedItems().count() > 0)
	{
		for each (auto item in tag_hld_2->selectedItems())
		{
			tags_to_delete.push_back(item->text());
		}
	}
	if (tag_hld_3->selectedItems().count() > 0)
	{
		for each (auto item in tag_hld_3->selectedItems())
		{
			tags_to_delete.push_back(item->text());
		}
	}
	QList<Tag> fin;	//	Form a tag list using linker and names 
	auto beg = tags_to_delete.begin();
	while (beg != tags_to_delete.end())
	{
		fin.push_back(linker[*beg]);
		++beg;
	}
	emit tags_erase(fin);
}
void TripleListTabWidget::list_clicked(QListWidgetItem * qlwi)
// This slot sends request to apply tag which was double-clicked
{
	QList<Tag> ql;
	ql.push_back(linker[qlwi->text()]);
	emit got_tags(ql);	//	send a list with one tag
	++linker[qlwi->text()].weight;	//	update weight
	sort_and_refresh(all);	//	sort all, maybe its better to optimise by deducing list number, but difference in performance is little
}
void TripleListTabWidget::search_completed()
// This slot sends a request to apply one tag or asks for a new tag
{
	QString line = search->text();
	if (taglist_1.contains(line) | taglist_2.contains(line) | taglist_3.contains(line))
	{	//	if there IS tag like this - apply
		emit got_tags(QList<Tag>({ linker[line] }));
	}
	else
	{	//	ask for the new one
		bool ok;
		QString curr = "";
		curr = QInputDialog::getItem(this, "Which column?", "Please select column",
			QStringList({ list_name_1, list_name_2, list_name_3 }), 1, false, &ok);
		if (ok)	//	if the column WAS selected
		{
			if (curr == list_name_1)
			{	//	unoptimized emitting add signal
				emit add_tag_press(list1, line);
			}
			else if (curr == list_name_2)
			{
				emit add_tag_press(list2, line);
			}
			else if (curr == list_name_3)
			{
				emit add_tag_press(list3, line);
			}
		}
	}
}
void TripleListTabWidget::delete_press()
//	Similar to erase slot, but instead of emitting signal - just deletes tags
{
	if (tag_hld_1->selectedItems().count() > 0)
	{
		for each (auto item in tag_hld_1->selectedItems())
		{
			linker.remove(item->text());	//	delete from linker
			taglist_1.removeOne(item->text());		//	list
			tag_hld_1->takeItem(tag_hld_1->count() - 1);	//	and view
		}
	}
	if (tag_hld_2->selectedItems().count() > 0)
	{
		for each (auto item in tag_hld_2->selectedItems())
		{
			linker.remove(item->text());
			taglist_2.removeOne(item->text());
			tag_hld_2->takeItem(tag_hld_2->count() - 1);
		}
	}
	if (tag_hld_3->selectedItems().count() > 0)
	{
		for each (auto item in tag_hld_3->selectedItems())
		{
			linker.remove(item->text());
			taglist_3.removeOne(item->text());
			tag_hld_3->takeItem(tag_hld_3->count() - 1);
		}
	}
	sort_and_refresh(all);
	setup_completer();
}
void TripleListTabWidget::apply_press()
//	This slot is ABSOLUTELY similar with erase slot, just calls other signal
{
	QStringList tags_to_apply;
	if (tag_hld_1->selectedItems().count() > 0)
	{
		for each (auto item in tag_hld_1->selectedItems())
		{
			tags_to_apply.push_back(item->text());
		}
	}
	if (tag_hld_2->selectedItems().count() > 0)
	{
		for each (auto item in tag_hld_2->selectedItems())
		{
			tags_to_apply.push_back(item->text());
		}
	}
	if (tag_hld_3->selectedItems().count() > 0)
	{
		for each (auto item in tag_hld_3->selectedItems())
		{
			tags_to_apply.push_back(item->text());
		}
	}
	QList<Tag> fin;
	auto beg = tags_to_apply.begin();
	while (beg != tags_to_apply.end())
	{
		fin.push_back(linker[*beg]);
		++linker[*beg].weight;
		++beg;
	}
	emit got_tags(fin);
	sort_and_refresh(all);
}
void TripleListTabWidget::clear_selection()
//	This slot clears selections of all three lists
{
	tag_hld_1->clearSelection();
	tag_hld_2->clearSelection();
	tag_hld_3->clearSelection();
}
void TripleListTabWidget::clear_tags()
//	Passes signal outside
{
	emit tags_clear();
}
