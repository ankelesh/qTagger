#include "TagEngine.h"

TagEngine::TagEngine(QString fn, bool deduce_author, bool comic, QList<Tag> &  taglib)
//	Constructs tag engine from string fn, deleting tags if they provided
{	
	//	Comics does not have extensions
	if (!comic)
		extension = "." + fn.split('.').last();
	else
		extension = "";
	//	Erase extension
	fn = fn.remove(extension);
	//	Deduce author function - tries to insert first word
	if (deduce_author)
	{
		auto space_split = fn.split(' ');		//	Splitting filename 
		if (space_split.count() > 1)			//	If there is more than 1 word
		{
			QString possible_author = space_split.first();	//	then we have candidate
			if (possible_author.contains(QRegExp("[a-z]")) | possible_author.contains(QRegExp("[à-ÿ]")))
			{
				//	It must contain at least one alphabetic character
				author = possible_author;
				fn = fn.remove(possible_author);
			}
		}
	}
	//	Checking for tags to erase
	if (taglib.count() > 0)
	{
		auto begin = taglib.begin();
		while (begin != taglib.end())
		{
			if (fn.contains(" " + begin->tag))
			{
				//	For each tag in list - delete it 
				fn = fn.remove(" " + begin->tag);
			}
			++begin;
		}
	}
	originalfname = fn;
}
void TagEngine::setfname(QString fn, bool deduce_author, bool comic, QList<Tag> & taglib)
//	Absolutely similar with constructor
{
	if (!comic)
		extension = "." + fn.split('.').last();
	else
		extension = "";
	fn = fn.remove(extension);
	if (deduce_author)
	{
		auto space_split = fn.split(' ');
		if (space_split.count() > 1)
		{
			QString possible_author = space_split.first();
			if (possible_author.contains(QRegExp("[a-z]")) | possible_author.contains(QRegExp("[à-ÿ]")))
			{
				author = possible_author;
				fn = fn.remove(possible_author);
			}
		}
	}
	if (taglib.count() > 0)
	{
		auto begin = taglib.begin();
		while (begin != taglib.end())
		{
			if (fn.contains(" " + begin->tag))
			{
				fn = fn.remove(" " + begin->tag);
			}
			++begin;
		}
	}
	originalfname = fn;
}
void TagEngine::addtag(Tag t)
// Adds tag 
{
	if (!tagSequence.contains(t))
	{
		tagSequence.push_back(t);
	}
}
void TagEngine::deltag(Tag t)
//	Deletes tag
{
	if (tagSequence.contains(t))
	{
		tagSequence.removeOne(t);
	}
}
void TagEngine::setauth(QString a)
{
	author = a;
}
void TagEngine::dropauth()
{
	author = "";
}
void TagEngine::droptags()
{
	tagSequence.clear();
}
bool TagEngine::hastag(Tag t) const
{
	return tagSequence.contains(t);
}
QString TagEngine::getFinal() const
//	Constructs final filename by summarizing all parts
{
	QString fin = author;
	fin +=" " +  originalfname;
	auto begin = tagSequence.begin();
	while (begin != tagSequence.end())
	{
		fin += ' ' + begin->tag;
		++begin;
	}
	fin += extension;
	return fin;
}
void TagEngine::breakDeduce(QString & la)
{
	originalfname = author + originalfname;
	author = la;
}
void TagEngine::overridefn(QString & nfn)
//	Drops tag sequence and rewrites filename. WARNING - all tags will expire and you WILL be able to add duplicates
{
	tagSequence.clear();
	originalfname = nfn;
}

QString TagEngine::getfntags()const
//	Constructs main substring - holding tags and name
{
	QString fn = originalfname;
	auto begin = tagSequence.begin();
	while (begin != tagSequence.end())
	{
		fn += " " + begin->tag; ++begin;
	}
	return fn;
}
QString testEngine()
{
	QString testOut("Test 1:\n");
	TagEngine mtagger("Aflux Bath.jpg", true, false);
	testOut += mtagger.getauth() + '\n' + "ext: " + mtagger.getext() + "\nrest: " + mtagger.getfn();
	testOut += "\nfinal >" + mtagger.getFinal() + "<" + "\nTest2:\n";
	mtagger = TagEngine("0101010 Aflux Bath.jpg", true, false);
	testOut += mtagger.getauth() + '\n' + "ext: " + mtagger.getext() + "\nrest: " + mtagger.getfn();
	mtagger = TagEngine("Aflux Bath.jpg", true, false);
	mtagger.addtag(Tag({ 1, "cat", 1 }));
	mtagger.addtag(Tag({ 2, "bed", 1 }));
	mtagger.addtag(Tag({ 3, "tdl", 1 }));
	mtagger.deltag(Tag({ 3, "tdl", 1 }));
	testOut += "\nTest 3:\n>" + mtagger.getFinal();
	QList<Tag> must_not_be{ Tag({1, "cat", 2}), Tag({2,"dog", 2}) };
	mtagger = TagEngine("Aflux Bath cat.jpg", true, false, must_not_be);
	testOut += "\nTest 4:\n>" + mtagger.getFinal();
	return testOut;
}
QMap<QString, Tag> All_tag_storage::getAll()
// Simple function, constructs map holding pairs <Tag name : Tag>
{
	QMap <QString, Tag> all;
	QList<Tag>::iterator begins [9]{ 
		fandom.begin(), name.begin(), chtype.begin(),
		action.begin(), actype.begin(), quantity.begin(),
		objects.begin(),specials.begin(), miscs.begin()};
	QList<Tag>::iterator ends[9]{ 
		fandom.end(), name.end(), chtype.end(),
		action.end(), actype.end(), quantity.end(), 
		objects.end(), specials.end(), miscs.end()};
	QList<Tag>::iterator begin;
	for (int i = 0; i < 9; ++i) {
		begin = begins[i];
		while (begin != ends[i])
		{
			if (!all.contains(begin->tag))
				all.insert(begin->tag, *begin);
			++begin;
		}
	}
	return all;
}
All_tag_storage load_storage(std::wfstream & wfout, bool yn)
//	Deserializator, dont forget to update it while updating storage
{
	if (yn)
	{
		All_tag_storage storage;
		//	This array holds begins of all lists for shorting cycle code
		QList<Tag> * storages[] = { 
			&storage.fandom, &storage.name, &storage.chtype 
			,&storage.action, &storage.actype, &storage.quantity,
			&storage.objects, &storage.specials, &storage.miscs};
		for (int i = 0; i < 9; ++i)
			//	Takes one list by one to fill it
		{
			while (!(wfout.peek() == '<'))
			{
				//	Stop sym is < at the string start
				storages[i]->push_back(getTag(wfout));
			}
			std::wstring tmp;
			std::getline(wfout, tmp);
			//	Erases the stop sequence
		}
		return storage;
	}
	return All_tag_storage();
}
std::wstring dump_storage(All_tag_storage & tags)
{
	QList<std::wstring> out;
	std::wstringstream wsout;
	QList<Tag>::iterator begins[9]{
		tags.fandom.begin(), tags.name.begin(), tags.chtype.begin(),
		tags.action.begin(), tags.actype.begin(), tags.quantity.begin(),
		tags.objects.begin(),tags.specials.begin(), tags.miscs.begin() };
	QList<Tag>::iterator ends[9]{
		tags.fandom.end(), tags.name.end(), tags.chtype.end(),
		tags.action.end(), tags.actype.end(), tags.quantity.end(),
		tags.objects.end(), tags.specials.end(), tags.miscs.end() };
	auto begin = begins[0];
	for (int i = 0; i < 9; ++i)
	{
		begin = begins[i];
		while (begin != ends[i])
		{
			wsout <<  putTag(*begin);
			++begin;
		}
		wsout << "<end_storage>\n";
	}
	return wsout.str();
}
Tag getTag(std::wfstream & wfout)
//	Tag serialization
{
		Tag tag;
		wfout.get();	//	Deleting | symbol, which is added for better reading 
		wfout >> tag.id >> tag.weight;
			//	Gets id and weight
		std::wstring temp;
		wfout.get();
		std::getline(wfout, temp);	//	rest of the line is tag name
		tag.tag = QString::fromStdWString(temp);	//	conversion from wide string
		return tag;
}
Tag getTag(std::wstring & wstr)
// Similar with stream version
{
	std::wstringstream wfout(wstr);
	Tag tag;
	wfout.get();
	wfout >> tag.id >> tag.weight;
	std::wstring temp;
	wfout.get();
	std::getline(wfout, temp);
	tag.tag = QString::fromStdWString(temp);
	return tag;
}
std::wstring putTag(Tag & t)
// Simply drops all members to line
{
	std::wstringstream wsout;
	wsout << "| " << t.id << ' ' << t.weight << " " << t.tag.toStdWString() << '\n';
	return wsout.str();
}
