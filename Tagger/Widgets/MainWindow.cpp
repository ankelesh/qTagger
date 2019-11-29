#include "MainWindow.h"

void mainWindow::onLDconfirm()	//	Creates new tagger, saves settings
{
	t = new Tagger(&root, ld->submit(), start_set.authors, start_set.tags, glc);
	dumpAll(SAVE_FILENAME);//	clearing start set to save some memory
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
		while (buffer != L"%")
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
		wfout << dump_storage(start_set.tags);	//	tags inside	
	}
}