#include "Tagger.h"

namespace LaunchInfo {
	std::wstring serialize(LaunchInfo::options & opts)
		//	serializing into widestring
	{
		std::wostringstream wsout;		//	using standard string streams
		wsout << opts.ifname.toStdWString() << "\n" << opts.ofname.toStdWString() << "\n"
			<< (int)opts.mod << " " << opts.deduce_author << ' ' << opts.clear_tags
			<< ' ' << opts.backup << ' ' << opts.delete_original << ' '
			<< opts.font_interface << ' ' << opts.font_tags << " <options end>\n";
		return wsout.str();
	}
	options deserialize(std::wfstream & fout)
		//	deserializing
	{
		options nw;
		std::wstring buffer;
		std::getline(fout, buffer);			//	first line - input directory 
		nw.ifname = QString::fromStdWString(buffer);
		std::getline(fout, buffer);			//	second line - output directory
		nw.ofname = QString::fromStdWString(buffer);
		int temp;	//	last line - boolean variables
		fout >> temp >> nw.deduce_author >> nw.clear_tags >> nw.backup >> nw.delete_original >> nw.font_interface >> nw.font_tags;
		nw.mod = (LaunchInfo::ConveyorMode) temp;
		std::getline(fout, buffer);
		return nw;
	}
}

settingsForm::settingsForm(QWidget * qw, LaunchInfo::options & opts)
	: QWidget(qw), formlayout(new QFormLayout(this)),
	rpath(new QLineEdit(this)), opath(new QLineEdit(this)), rb(new QHBoxLayout(this)), comicmode(new QRadioButton("Comic", this)),
	stdmode(new QRadioButton("Standard", this)),
	d_auth(new QCheckBox(this)), cl_tg(new QCheckBox(this)),
	bck(new QCheckBox(this)), delor(new QCheckBox(this))
	, font_int(new QSpinBox(this)), font_tag(new QSpinBox(this))
	// Constructs form using preloaded options
{
	this->setLayout(formlayout);
	rb->addWidget(stdmode);
	rb->addWidget(comicmode);
	formlayout->addRow("Directory: ", rpath);
	formlayout->addRow("Output: ", opath);
	formlayout->addRow("Mode: ", rb);
	formlayout->addRow("Deduce author ", d_auth);
	formlayout->addRow("Clear tags", cl_tg);
	formlayout->addRow("Make backup images", bck);
	formlayout->addRow("Delete originals", delor);
	formlayout->addRow("Size of interface font", font_int);
	formlayout->addRow("Size of tags font", font_tag);
	rpath->setText(opts.ifname);
	opath->setText(opts.ofname);
	stdmode->setChecked(opts.mod == LaunchInfo::standard);
	comicmode->setChecked(opts.mod == LaunchInfo::comic);
	d_auth->setChecked(opts.deduce_author);
	cl_tg->setChecked(opts.clear_tags);
	bck->setChecked(opts.backup);
	delor->setChecked(opts.delete_original);
	font_int->setValue(opts.font_interface);
	font_int->setMaximum(30);
	font_int->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum));
	font_tag->setValue(opts.font_tags);
	font_tag->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum));
	font_tag->setMaximum(30);
	QObject::connect(opath, &QLineEdit::returnPressed, this, &settingsForm::onEnter);
	QObject::connect(rpath, &QLineEdit::returnPressed, this, &settingsForm::onEnter);
	QObject::connect(rpath, &QLineEdit::textChanged, this, &settingsForm::onChange);
}
void settingsForm::onChange()	//	This slot refreshes opath for easier setup
{
	opath->setText(rpath->text() + "tagged");
}
LaunchInfo::options settingsForm::submit()	//	Constructs and returns options object
{
	return LaunchInfo::options({ rpath->text(), opath->text()
		, (comicmode->isChecked()) ? LaunchInfo::comic : LaunchInfo::standard,
		d_auth->isChecked(), cl_tg->isChecked(), bck->isChecked(), delor->isChecked(),
		font_int->value(), font_tag->value(), QSize() });
}
void settingsForm::onEnter()
{
	emit done();
}
LaunchDialog::LaunchDialog( LaunchInfo::options & opt, QRect & rct) 
	: opts(opt), mainlayout(new QVBoxLayout(this)),
	header(new QLabel("Please check tagger settings:", this)),
	setform(new settingsForm(this, opts)),
	butlay(new QHBoxLayout(this)), ok(new QPushButton("Ok", this)),
	cancel(new QPushButton("Cancel", this)), scr(rct)
	//	This constructor is optimal 
{
	//Creating gui
	this->setLayout(mainlayout);
	mainlayout->addWidget(header);
	mainlayout->addWidget(setform);
	mainlayout->addLayout(butlay);
	butlay->addWidget(ok);
	butlay->addWidget(cancel);
	header->show();
	ok->show();
	cancel->show();
	header->show();
	setform->show();
	QObject::connect(setform, &settingsForm::done, this, &LaunchDialog::onOk);
	QObject::connect(ok, &QPushButton::clicked, this, &LaunchDialog::onOk);
	QObject::connect(cancel, &QPushButton::clicked, this, &QWidget::close);
}
void LaunchDialog::onOk()	//	this slot transmits ok-button clicks outside
{
	emit done();
}
Tagger::Tagger(QWidget * root, LaunchInfo::options opts, QList<Tag> & auths, All_tag_storage & tags, int glc)
	: QWidget(root), options(opts), conveyor(nullptr),
	tagengine(), all_authors(auths), all_tags(tags.getAll()), current_workset(),
	mainLayout(new QHBoxLayout(this)), imageSlot(new LabelClickable(this)),
	leftPanel(new QVBoxLayout(this)), namepanel(new QHBoxLayout(this)),
	buttons(new QHBoxLayout(this)), 
	authorField(new QLineEdit(this)), fnameField(new QLineEdit(this)),
	numField(nullptr), extensionField(new QLabel(this)),
	tagSetupArea(new QTabWidget(this)), 
	authorTab(new AuthorTabWidget(tagSetupArea, all_authors, gltagcnt, opts.font_interface, opts.font_tags)),
	characterTab(new TripleListTabWidget(tagSetupArea, all_tags, tags.fandom, tags.name, tags.chtype,
		QStringList({ "fandom", "name", "character" }), gltagcnt, opts.font_interface, opts.font_tags))
	, actionsTab(new TripleListTabWidget(tagSetupArea, all_tags, tags.action, tags.actype, tags.quantity,
		QStringList({ "action", "type", "quantity" }), gltagcnt, opts.font_interface, opts.font_tags)),
	miscTab(new TripleListTabWidget(tagSetupArea, all_tags, tags.objects, tags.specials, tags.miscs,
		QStringList({ "objects", "specials", "miscs" }), gltagcnt, opts.font_interface, opts.font_tags))
	,delImgButton(new QPushButton("Delete this", this)), nextButton(new QPushButton("Next!", this))
	,delComicPageButton(nullptr)
	,tagcount(all_tags.count()), authcount(all_authors.count())
	//	Constructor creates conveyor and loads first image
{
	gltagcnt = glc;		//	Setup global tag counter
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
	tagSetupArea->addTab(characterTab, "Fandom and character");
	tagSetupArea->addTab(actionsTab, "Actions and quantity");
	tagSetupArea->addTab(miscTab, "Specials and miscellaneous");
	// Connecting everything
	QObject::connect(authorTab, &AuthorTabWidget::author_add, this, &Tagger::author_added);
	QObject::connect(authorTab, &AuthorTabWidget::author_erase, this, &Tagger::author_erased);
	QObject::connect(authorTab, &AuthorTabWidget::break_deduction, this, &Tagger::author_brdeduce);
	QObject::connect(authorTab, &AuthorTabWidget::got_author, this, &Tagger::author_selected);
	QObject::connect(nextButton, &QPushButton::clicked, this, &Tagger::next_pressed);
	QObject::connect(characterTab, &TripleListTabWidget::got_tags, this, &Tagger::tags_added);
	QObject::connect(characterTab, &TripleListTabWidget::tags_erase, this, &Tagger::tags_erased);
	QObject::connect(characterTab, &TripleListTabWidget::tags_clear, this, &Tagger::tags_clear);
	QObject::connect(actionsTab, &TripleListTabWidget::got_tags, this, &Tagger::tags_added);
	QObject::connect(actionsTab, &TripleListTabWidget::tags_erase, this, &Tagger::tags_erased);
	QObject::connect(actionsTab, &TripleListTabWidget::tags_clear, this, &Tagger::tags_clear);
	QObject::connect(miscTab, &TripleListTabWidget::got_tags, this, &Tagger::tags_added);
	QObject::connect(miscTab, &TripleListTabWidget::tags_erase, this, &Tagger::tags_erased);
	QObject::connect(miscTab, &TripleListTabWidget::tags_clear, this, &Tagger::tags_clear);
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
	All_tag_storage alts;
	auto tagsets = characterTab->get_tag_keys() +	//	get all lists from tabs
		actionsTab->get_tag_keys() + miscTab->get_tag_keys();
	QList<QList<Tag> *> altbegs({					//	this array provides pointers for shorter cycle
				&alts.fandom, &alts.name, &alts.chtype,
				&alts.action, &alts.actype, &alts.quantity,
				&alts.objects, &alts.specials, &alts.miscs
		});
	//	This cycle copies to the storage tags from linker one-by-one
	auto start =  tagsets.begin();
	auto altsstart = altbegs.begin();
	while (start != tagsets.end())							//	while we have any tag in list 
	{
		auto begin = (*start)->begin();						//	this is beginnings of inner tab list division
		while (begin != (*start)->end())					//	while there is any tag in inner list
		{
			(*altsstart)->push_back(all_tags[*begin]);		//	link and push into storage
			++begin;
		}
		++start;											//	iterating
		++altsstart;
	}
	return alts;
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
	all_authors.push_back(t);
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
			auto stepres = reinterpret_cast<ComicConveyor *>(conveyor)->stepBack();
			imageSlot->setPixmap(stepres.first);
			numField->setValue(stepres.second);
		}
		else
		{						//	Else - go forward
			auto stepres = reinterpret_cast<ComicConveyor *>(conveyor)->stepForward();
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
			reinterpret_cast<ComicConveyor *>(conveyor)->setnum(nvl);
		}
	}
}
void Tagger::delete_curr_comic_page()
{
	if (options.mod == LaunchInfo::comic && conveyor->ready())
	{
		reinterpret_cast<ComicConveyor *>(conveyor)->utilizeCurrentImage(options.backup);
		current_workset = reinterpret_cast<ComicConveyor * > (conveyor)->stepForward();
		imageSlot->setPixmap(current_workset.first);
	}
}
void mainWindow::onLDconfirm()	//	Creates new tagger, saves settings
{
	t = new Tagger(&root, ld->submit(), start_set.authors, start_set.tags, glc);
	dumpAll(SAVE_FILENAME);
	start_set.erase();		//	clearing start set to save some memory
	ld->hide();
	t->show();
	QObject::connect(t, &Tagger::save_time, this, &mainWindow::save_called);
	root.showMaximized();	//	go maximized
}
mainWindow::full_start_set mainWindow::loadAll(QString fname)	//	Deserialization of all data
{
	full_start_set loaded;
	loaded.opts = DEFAULTS;	//	Fill options with defaults 
	std::wfstream wfin;
	wfin.open(fname.toStdString(), std::ios_base::in);
	if (wfin.is_open())	//	If we have file
	{
		float version;
		wfin >> version >> glc;
		wfin.get();
		if (version < LATEST_SUPPORTED_SAVE)	//	If version can not support save - just return defaults
			return loaded;
		loaded.opts = LaunchInfo::deserialize(wfin);	//	get options
		std::wstring buffer;
		std::getline(wfin, buffer);			//	get authors
		while (buffer != (wchar_t *)"%")
		{
			loaded.authors.push_back(getTag(buffer));
			std::getline(wfin, buffer);
		}
		loaded.tags = load_storage(wfin, true);	//	load full storage
	}
	return loaded;
}
void mainWindow::dumpAll(QString fname)	//	Serialize all
{
	if (t == nullptr)	//	If no tagger exists, no need for saving
		return;
	std::wfstream wfout;
	wfout.open(fname.toStdString(), std::ios_base::out);
	if (wfout.is_open())	//	if we can open the file
	{
		wfout << VERSION << ' ' << t->gltagcnt
			<< " " << LaunchInfo::serialize(ld->submit());	//	Put version, counter and options inside
		for each (auto tg in t->getAuthors())
		{
			wfout << putTag(tg);	//	authors inside
		}
		wfout << "%\n";
		wfout << dump_storage(t->getAllTags());	//	tags inside	
	}
}