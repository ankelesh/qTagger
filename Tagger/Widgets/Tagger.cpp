#include "Tagger.h"

enum tabs{characterTab, actionsTab, miscTab};

Tagger::Tagger(QWidget * root, LaunchInfo::options opts, QList<Tag> & auths, All_tag_storage & tags, int glc)
	: QWidget(root), options(opts), conveyor(nullptr),
	tagengine(), all_authors(auths), all_tags(tags), current_workset(),
	mainLayout(new QHBoxLayout(this)), imageSlot(new LabelClickable(this)),
	leftPanel(new QVBoxLayout(this)), namepanel(new QHBoxLayout(this)),
	buttons(new QHBoxLayout(this)), 
	authorField(new QLineEdit(this)), fnameField(new QLineEdit(this)),
	numField(nullptr), extensionField(new QLabel(this)),
	tagSetupArea(new QTabWidget(this)), 
	authorTab(new AuthorTabWidget(tagSetupArea, all_authors, gltagcnt, opts.font_interface, opts.font_tags)),
	tripleTabs()
	,delImgButton(new QPushButton("Delete this", this)), nextButton(new QPushButton("Next!", this))
	,delComicPageButton(nullptr)
	,tagcount(tags.count()), authcount(all_authors.count())
	//	Constructor creates conveyor and loads first image
{
	switch (options.mod)	//	Construct suitable conveyor
	{
	case (LaunchInfo::standard):
		conveyor = new Conveyor(options.ifname, options.ofname, options.winsize);
		break;
	case (LaunchInfo::comic):
		conveyor = new ComicConveyor(options.ifname, options.ofname, options.winsize);
		delComicPageButton = new QPushButton("Delete page", this);
		break;
	}
	int pcounter = 1;
	tripleTabs[0] = new TripleListTabWidget(tagSetupArea, tags.fandom, tags.name, tags.chtype,
		QStringList({ "fandom", "name", "character" }), pcounter, opts.font_interface, opts.font_tags);
		tripleTabs[1] = new TripleListTabWidget(tagSetupArea,  tags.action, tags.actype, tags.quantity,
			QStringList({ "action", "type", "quantity" }), pcounter, opts.font_interface, opts.font_tags);
		tripleTabs[2] = new TripleListTabWidget(tagSetupArea, tags.objects, tags.specials, tags.miscs,
			QStringList({ "objects", "specials", "miscs" }), pcounter, opts.font_interface, opts.font_tags);
	//	Creating gui
	root->setLayout(mainLayout);
	mainLayout->addLayout(leftPanel);
	mainLayout->addWidget(imageSlot);
	leftPanel->addLayout(namepanel);
	leftPanel->addWidget(tagSetupArea);
	leftPanel->addLayout(buttons);
	buttons->addWidget(delImgButton);
	buttons->addWidget(nextButton);
	if (options.mod == LaunchInfo::comic)
		buttons->addWidget(delComicPageButton);
	//Set size policies and fonts
	delImgButton->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum));
	delImgButton->setFont(QFont("Times new Roman", opts.font_interface+5, 20));
	if (options.mod == LaunchInfo::comic)
	{
		delComicPageButton->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum));
		delComicPageButton->setFont(QFont("Times new Roman", opts.font_interface+5, 20));
	}
	nextButton->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
	nextButton->setFont(QFont("Times new Roman", opts.font_interface + 10, 20));
	namepanel->addWidget(authorField);
	authorField->setMinimumWidth(150);
	authorField->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum));
	namepanel->addWidget(fnameField);
	if (options.mod == LaunchInfo::comic)	//	If we have need in numering images - create field
	{
		numField = new QSpinBox(this);
		namepanel->addWidget(numField);
	}
	namepanel->addWidget(extensionField);
	//	Load first image
	current_workset = conveyor->next();
	imageSlot->setPixmap(current_workset.first);
	imageSlot->setMinimumSize(options.winsize);
	set_namepanel();	//	Set all fields
	tagSetupArea->addTab(authorTab, "Author");
	tagSetupArea->setFont(QFont("Times new roman", opts.font_interface));
	tagSetupArea->addTab(tripleTabs[characterTab], "Fandom and character");
	tagSetupArea->addTab(tripleTabs[actionsTab], "Actions and quantity");
	tagSetupArea->addTab(tripleTabs[miscTab], "Specials and miscellaneous");
	// Connecting everything
	QObject::connect(authorTab, &AuthorTabWidget::author_add, this, &Tagger::author_added);
	QObject::connect(authorTab, &AuthorTabWidget::author_erase, this, &Tagger::author_erased);
	QObject::connect(authorTab, &AuthorTabWidget::break_deduction, this, &Tagger::author_brdeduce);
	QObject::connect(authorTab, &AuthorTabWidget::got_author, this, &Tagger::author_selected);
	QObject::connect(authorTab, &AuthorTabWidget::goOutThis, this, &Tagger::gotoPageAndId);
	QObject::connect(authorTab, &AuthorTabWidget::taggingThisImageDone, this, &Tagger::next_pressed);
	QObject::connect(nextButton, &QPushButton::clicked, this, &Tagger::next_pressed);
	for (int i = 0; i < 3; ++i)
	{
		QObject::connect(tripleTabs[i], &TripleListTabWidget::got_tags, this, &Tagger::tags_added);
		QObject::connect(tripleTabs[i], &TripleListTabWidget::tags_erase, this, &Tagger::tags_erased);
		QObject::connect(tripleTabs[i], &TripleListTabWidget::tags_clear, this, &Tagger::tags_clear);
		QObject::connect(tripleTabs[i], &TripleListTabWidget::goOutThis, this, &Tagger::gotoPageAndId);
		QObject::connect(tripleTabs[i], &TripleListTabWidget::taggingThisImageDone, this, &Tagger::next_pressed);
	}
	QObject::connect(delImgButton, &QPushButton::clicked, this, &Tagger::del_pressed);
	QObject::connect(imageSlot, &LabelClickable::clicked, this, &Tagger::image_click_received);
	QObject::connect(numField, QOverload<int>::of(&QSpinBox::valueChanged), this, &Tagger::pagenum_changed);
	if (options.mod == LaunchInfo::comic)
		QObject::connect(delComicPageButton, &QPushButton::clicked, this, &Tagger::delete_curr_comic_page);
}
void Tagger::set_namepanel()	//	This method primes tag engine with current image's name
{
	tagengine.setfname(current_workset.second, options.deduce_author, 
		options.mod == LaunchInfo::comic);
	authorField->setText(tagengine.getauth());
	fnameField->setText(tagengine.getfn());
	if (options.mod == LaunchInfo::comic)	
	{
		numField->setValue(reinterpret_cast<ComicConveyor *>(conveyor)->
			getnum());
	}
	extensionField->setText(tagengine.getext());
}
All_tag_storage Tagger::getAllTags()	//	This method creates storage holding every tag available on tabs
{
	return all_tags;
}
void Tagger::author_selected(Tag t)	//	set author
{
	tagengine.setauth(t.tag);
	authorField->setText(tagengine.getauth());
	tagSetupArea->setCurrentIndex(1);	//	if we set up author, we must procede to next tab
}
void Tagger::author_erased()	//	Simply erases author
{
	tagengine.setauth("");
	authorField->clear();
}
void Tagger::author_brdeduce()	//	Calling breakDeduce in tagengine
{
	tagengine.breakDeduce(authorTab->getLAuthor());
	authorField->setText(tagengine.getauth());
	fnameField->setText(tagengine.getfn());
}
void Tagger::author_added(Tag t)
{
}
void Tagger::next_pressed()	//	get new pair from conveyor and store previous one
{
	if (tagengine.getfntags() != fnameField->text())	//	If mismatch between view and tag engine - override 
	{
		tagengine.overridefn(fnameField->text());
	}
	if (tagengine.getauth() != authorField->text())
	{
		tagengine.setauth(authorField->text());
	}//	refreshes the last author
	authorTab->setLAuthor(tagengine.getauth());
	// Call to conveyor to store last image
	conveyor->store(tagengine.getFinal(), options.backup,!options.delete_original);
	// Call to obtain new <image : name>
	current_workset = conveyor->next();
	// Set up namepanel, image and tagengine
	set_namepanel();
	imageSlot->setPixmap(current_workset.first);
	tagengine = TagEngine(current_workset.second, options.deduce_author,
		options.mod == LaunchInfo::comic);
	//	If we have mismatch between counts - we must refresh saved data
	if (all_tags.count() != tagcount | authorTab->getAuthsCount() != authcount)
	{
		tagcount = all_tags.count(); authcount = authorTab->getAuthsCount();
		emit save_time();
	}
	//	Go to first tab
	tagSetupArea->setCurrentIndex(0);
}
void Tagger::tags_added(QList<Tag>  tags_to_add)
{
	//	Simply adds tags and goes to next tab
	for each (Tag tag in tags_to_add)
	{
		tagengine.addtag(tag);
	}
	fnameField->setText(tagengine.getfntags());
	tagSetupArea->setCurrentIndex((
		tagSetupArea->currentIndex() < tagSetupArea->count() - 1) ? tagSetupArea->currentIndex() + 1 : 0);
}
void Tagger::tags_erased(QList<Tag>  tags_to_erase)	//	Deletes tags and refreshes filename line
{
	for each (Tag tag in tags_to_erase)
	{
		tagengine.deltag(tag);
	}
	fnameField->setText(tagengine.getfntags());
}
void Tagger::tags_clear()	//	clears and refreshes view
{
	tagengine.droptags();
	fnameField->setText(tagengine.getfntags());
}
void Tagger::del_pressed()	//	Deletes current image and procedes to next one
{
	if (conveyor->ready()) {
		conveyor->utilizeCurrent(options.backup);
		current_workset = conveyor->next();	//	This method is simplier than next_pressed because its highly likely that you had no changes
		set_namepanel();
		imageSlot->setPixmap(current_workset.first);
	}
}
void Tagger::image_click_received(bool side)	//	If image was clicked
{
	if (options.mod == LaunchInfo::comic)	//	If we have a comic
	{
		if (side)							//	if left - go back
		{
			auto stepres = dynamic_cast<ComicConveyor *>(conveyor)->stepBack();
			imageSlot->setPixmap(stepres.first);
			numField->setValue(stepres.second);
		}
		else
		{						//	Else - go forward
			auto stepres = dynamic_cast<ComicConveyor *>(conveyor)->stepForward();
			imageSlot->setPixmap(stepres.first);
			numField->setValue(stepres.second);
		}
	}
}
void Tagger::pagenum_changed(int nvl)	//	Simply changes number of page to given by user
{
	if (options.mod == LaunchInfo::comic && conveyor->ready())
	{
		if (conveyor->ready())
		{
			dynamic_cast<ComicConveyor *>(conveyor)->setnum(nvl);
		}
	}
}
void Tagger::delete_curr_comic_page()
{
	if (options.mod == LaunchInfo::comic && conveyor->ready())
	{
		dynamic_cast<ComicConveyor *>(conveyor)->utilizeCurrentImage(options.backup);
		current_workset = dynamic_cast<ComicConveyor * > (conveyor)->stepForward();
		imageSlot->setPixmap(current_workset.first);
	}
}

void Tagger::gotoPageAndId(int id, int page)
{
	int ind = tagSetupArea->currentIndex();
	switch (page)
	{
	case -1:
		if (ind + 1 >= tagSetupArea->count())
		{
			ind = 0;
		}
		else
		{
			++ind;
		}
		tagSetupArea->setCurrentIndex(ind);
		break;
	case -2:

		if (ind - 1 < 0)
		{
			ind = tagSetupArea->count()-1;
		}
		else
		{
			
			--ind;
		}
		tagSetupArea->setCurrentIndex(ind);
		break;
	default:
		if (page < tagSetupArea->count() && page >= 0)
		{
		
			tagSetupArea->setCurrentIndex(page);
		}
		break;
	}
	switch (id)
	{
	case -1:
		tripleTabs[miscTab]->prepare(9);
		break;
	case 0:
		authorTab->prepare();
		break;
	case 1:
	case 2:
	case 3:
		tripleTabs[characterTab]->prepare(id);
		break;
	case 4:
	case 5:
	case 6:
		tripleTabs[actionsTab]->prepare(id);
		break;
	case 7:
	case 8:
	case 9:
		tripleTabs[miscTab]->prepare(id);
		break;
	default:
		break;
	}

}
