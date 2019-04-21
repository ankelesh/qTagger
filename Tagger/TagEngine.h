#pragma once
#include <qstring.h>
#include <qlist.h>
#include <qmap.h>
#include <sstream>
#include <fstream>
#include <algorithm>


/*

This file contains everything related to tagging. It is independent and can be connected to any Qt app
or, with refactor to std::containers and strings, to everything.

	Tag					 - core structure
	All_tag_storage		 - modifiable structure, holds some lists for filling widgets. 
	TagEngine			 - Core tagging class, holds tag sequence and manages future filename
*/



struct Tag
{
	int id;			//	Unique id, can be used for fast search or sorts, can be key in maps
	QString tag;	//	Name. Can hold any characters
	int weight = 1; //	Weight. Usable for determining the most popular tag, or to sort, etc
};

static bool operator==(const Tag & t1, const Tag & t2) { return t1.id == t2.id; };
static bool operator<(const Tag & t1, const Tag & t2) { return t1.weight < t2.weight; };

Tag getTag(std::wfstream & wfout);	//	Use this function to stream deserialization
Tag getTag(std::wstring & wstr);	//	Constructs tag from wstring
std::wstring putTag(Tag & t);		//	Serializes tag to widestring

struct All_tag_storage
{
	QList<Tag> fandom;				//	This lists will be used in tab Widgets initializations
	QList<Tag> name;
	QList<Tag> chtype;
	QList<Tag> action;
	QList<Tag> actype;
	QList<Tag> quantity;
	QList<Tag> objects;
	QList<Tag> specials;
	QList<Tag> miscs;
	QMap<QString, Tag> getAll();	//	This functions constructs global linker for tags
};

All_tag_storage load_storage(std::wfstream & wfout, bool yn = true);	//	Deserialization from stream
std::wstring dump_storage(All_tag_storage & tags);						//	Serialization to widestring

class TagEngine			
	//	Class wraps the filename for tagging puproses
{
private:
	QString originalfname;	//	Holds immutable part of original filename
	QString author;			//	Holds author name - it will be first word in final
	QString extension;		//	Holds extension
	QList <Tag> tagSequence;	//	Holds tags
public:
	//	Constructor and initializer
	TagEngine(QString fn = "no_filename.nf",bool deduce_author = false, bool comic=false, QList<Tag> &  taglib = QList<Tag>());
	void setfname(QString fn = "no_filename.nf", bool deduce_author = false, bool comic = false, QList<Tag> &  taglib = QList<Tag>());
	//	Tag operations
	void addtag(Tag t);
	void deltag(Tag t);
	//	Author changing
	void setauth(QString a);
	//	Getters 
	QString getauth() const { return author; }
	QString getext() const { return extension; }
	QString getfn() const { return originalfname; }
	QString getfntags()const;
	//	Droppers
	void dropauth();
	void droptags();
	//	Search
	bool hastag(Tag t) const;
	//	Main method - returns fully constructed string
	QString getFinal() const;
	//	Moves author name to the filename
	void breakDeduce(QString & la);
	//	Overrides filename with string. Does not affects author.
	void overridefn(QString & qs);
};

QString testEngine();			//Some tests for debugging puproces