#include "Conveyor.h"
#define DEBUG
#ifdef DEBUG
#include "debugtrace.h"
#endif


QPixmap getImg(QDir & from, const QString & fname, QSize & resolution)
//	This function loads image and scales it to given size
{
	QPixmap qp(from.absoluteFilePath(fname));
	detrace_METHEXPL("path was: " << from.absoluteFilePath(fname));

	QSize qps = qp.size();
	int qpsmax = (((float)qps.width() / (float)resolution.width()) >
		((float)qps.height() / (float)resolution.height())) ?		//	Scaling by the largest size
		resolution.width() : resolution.height();
	qp = (qpsmax == resolution.width()) ? 
		qp.scaledToWidth(qpsmax, Qt::TransformationMode::SmoothTransformation) 
		: qp.scaledToHeight(qpsmax, Qt::TransformationMode::SmoothTransformation);
	return qp;
}
bool cutAndPaste(QDir & from, const QString & ffname, QDir & dest, QString & dfname, bool backup, bool no_erase)
//	This function responds for safe copy with backup of one image, replacing its name. EXTENSION IS AFFECTED
{
	QFile output(dest.absoluteFilePath(dfname));
	if (!output.open(QIODevice::WriteOnly))
	{
		return false;
	}
	QFile input(from.absoluteFilePath(ffname));
	if (!input.open(QIODevice::ReadOnly))
	{
		return false;
	}
	//	When both files are opened, copiing is executed by simple loading file into memory. Not optimised, but simple
	output.write(input.readAll());
	if (backup)
	{
		// If backup is needed, backup directory is created and same copy procedure repeats
		if (!from.exists(from.absoluteFilePath("backup")))
		{
			from.mkdir("backup");
		}
		QDir temp(from.absoluteFilePath("backup"));
		QFile backfile(temp.absoluteFilePath(temp.absoluteFilePath(ffname)));
		if (!backfile.open(QIODevice::WriteOnly))
		{
			return false;
		}
		input.seek(0);	//	set filestream to start
		backfile.write(input.readAll());
		backfile.close();
	}
	output.close();
	input.close();
	if (!no_erase)
	{	
		//delete original
		from.remove(ffname);
		from.refresh();
	}
	return true;
}
bool deleteImage(QDir & from,const QString & ffname, bool backup)
//	This function deletes image, providing backup
{
	if (backup)
	{
		QFile input(from.absoluteFilePath(ffname));
		if (!input.open(QIODevice::ReadOnly))
		{
			return false;
		}
		if (!from.exists(from.absoluteFilePath("backup")))
		{
			from.mkdir("backup");
		}
		QDir temp(from.absoluteFilePath("backup"));
		QFile backfile(temp.absoluteFilePath(temp.absoluteFilePath(ffname)));
		if (!backfile.open(QIODevice::WriteOnly))
		{
			return false;
		}
		backfile.write(input.readAll());
		backfile.close();
	}
	from.remove(ffname);
	from.refresh();
	return true;
}
Conveyor::Conveyor(QString rootpath, QString outputpath, QSize res) 
//	Default constructor
{
	rpath = rootpath; root = QDir(rootpath);
	opath = outputpath; outdir = QDir(outputpath);
	current = ""; resolution = res;
	//	Filter setup for only image formats
	root.setNameFilters(QStringList({ "*.jpg", "*.png", "*.jpeg", "*.gif", "*.bmp" }));
	no_erase = true;
	if (!outdir.exists())
	{
		//	Create output directory
		outdir.mkdir(opath);
	}
	works = (root.exists()) ? true : false;
}
QPair<QPixmap, QString> Conveyor::next()
//	Provides pair of <Image : name> from directory
{
	if (works) {
		//	If there is any image
		if (no_erase) {
			//	If we are NOT clearing directory by deliting originals
			auto list = root.entryList().toSet();
			QStringList remains = list.subtract(not_needed).toList();
			//	Than we need to check if there is any image left
			if (remains.count() > 0)
			{
				current = remains.first();
				not_needed << current;
				return QPair<QPixmap, QString> {getImg(root, current, resolution), current};
			}
			else
				works = false;	//	No unchecked images? Stop conveyor
		}
		else
		{
			if (root.entryList().isEmpty())
				return QPair<QPixmap, QString> {getImg(root, current, resolution), current};

			current = root.entryList().first();
			return QPair<QPixmap, QString> {getImg(root, current, resolution), current};
		}
	}
		//	This result is provided if nothing to return
		return QPair<QPixmap, QString>{QPixmap(EndOCol), "None"};
}
bool Conveyor::store(QString filename, bool backup, bool noerase)
//	This function renames and copies images 
{
	if (!works)		//	Only if there is something to copy
		return false;
	no_erase = noerase;	//	Refreshing variable for optimising next() method
	return cutAndPaste(root, current, outdir, filename, backup, no_erase);
}
void Conveyor::utilizeCurrent(bool backup)
{
	deleteImage(root, current, backup);
	root.refresh();
}
bool Conveyor::initialize(QString rootpath, QString outputpath, QSize res )
//	Similar with constructor
{
	root = QDir(rootpath);
	rpath = rootpath;
	outdir = QDir(outputpath);
	opath = outputpath;
	resolution = res;
	root.setNameFilters(QStringList({ "*.jpg", "*.png", "*.jpeg", "*.gif", "*.bmp" }));
	if (!outdir.exists())
	{
		outdir.mkdir(opath);
	}
	works = (root.exists()) ? true : false;
	return works;
}
ComicConveyor::ComicConveyor(QString rootpath, QString outputpath, QSize res)
//	This constructor does not sets filters
{
	rpath = rootpath; root = QDir(rootpath);
	opath = outputpath; outdir = QDir(outputpath);
	 resolution = res;
	if (!outdir.exists())
	{
		outdir.mkdir(opath);
	}
	works = (root.exists()) ? true : false;
}
bool ComicConveyor::initialize(QString rootpath, QString outputpath, QSize res)
{
	rpath = rootpath;
	root = QDir(rootpath);
	opath = outputpath;
	outdir = QDir(outputpath);
	resolution = res;
	if (!outdir.exists())
	{
		outdir.mkdir(opath);
	}
	works = root.exists();
	works = root.entryList(QDir::NoDotAndDotDot).count() > 0;
	return works;
}
QPair<QPixmap, QString> ComicConveyor::next()
//	This function loads comic information into memory, except only image files for optimizing memory usage
{
	if (works) //	if there is comics to tag
	{
		auto list = root.entryList(QDir::Dirs | QDir::NoDotAndDotDot).toSet();
		pos = 0;
		if (no_erase)	//	if we leave originals, we must find out which were already touched
		{
			QStringList remains = list.subtract(not_needed).toList();
			if (remains.count() > 0)	//	If there is something 
			{
				name = remains.first();
				not_needed.insert(name);
				current = QDir(root.absoluteFilePath(name));
				current.setNameFilters(QStringList({ "*.jpg", "*.png", "*.jpeg", "*.gif", "*.bmp" }));
				comic.clear();
				comic.append(current.entryList());	//	Loading all image files names
				for (int i = 0; i < comic.count(); ++i) { numbers.push_back(i); }	//	Every page is assigned number
				pos = 0;	//	Set to comic start
				if (comic.count() > 0)	//	If comic has any image
				{
					return QPair<QPixmap, QString> {getImg(current, comic.at(pos), resolution), name };
				}
			}
			else {
				works = false;
				return QPair<QPixmap, QString>{QPixmap(EndOCol), "None"};
			}
		}
		else
		{
			name = *list.begin();
			//This version is faster
			current = QDir(root.absoluteFilePath(name));
			current.setNameFilters(QStringList({ "*.jpg", "*.png", "*.jpeg", "*.gif", "*.bmp" }));
			comic.clear();
			comic.append(current.entryList());	//	Loading all image files names
			for (int i = 0; i < comic.count(); ++i) { numbers.push_back(i); }	//	Every page is assigned number
			pos = 0;	//	Set to comic start
			if (comic.count() > 0)	//	If comic has any image
			{
				return QPair<QPixmap, QString> {getImg(current, comic.at(pos), resolution), name };
			}
			else
				return QPair<QPixmap, QString>{QPixmap(EndOCol), "None"};
		}
	}
	//	We return pair of <First image in comic : name of the comic>
	return QPair<QPixmap, QString>{QPixmap(EndOCol), "None"};
}
void ComicConveyor::utilizeCurrent(bool backup)
//	This method deletes entire comic folder with all contents
{
	if (!works) return;
	int max = comic.count();
	for (int i = 0; i < max; ++i)
	{
		deleteImage(current, comic.at(i), backup);
	}
	root.rmdir(name);
	root.refresh();
}
QPair <QPixmap, int> ComicConveyor::stepForward()
//	This function loads next image in comic
{
	if (works & comic.count() > 0 & pos < comic.count()-1)	//	if there is any image
	{
		++pos;		//	return image, selector to next filename
		return QPair<QPixmap, int> {getImg(current, comic.at(pos), resolution), numbers.at(pos)};
	}
	else if (works &comic.count() > 0 & pos == comic.count()-1)	//	else just return last image
	{
		//	Note that this isnt optimised, its better to check position number in the calling class
		return QPair<QPixmap, int> {getImg(current, comic.at(pos), resolution), numbers[pos]};
	}
	//	Returns pair of		<next Image	: next position>
	return QPair<QPixmap, int>{QPixmap(EndOCol), 0};
}
QPair<QPixmap, int> ComicConveyor::stepBack()
// Reflection of StepForward
{
	if (works & comic.count() > 0 & pos > 0)
	{
		--pos;				//	This and equations are only changes
		return QPair<QPixmap,int> {getImg(current, comic[pos], resolution), numbers[pos]};
	}
	else if (works &comic.count() > 0 & pos == 0)
	{
		return QPair<QPixmap, int> {getImg(current, comic[0], resolution), numbers[pos]};
	}
	return QPair<QPixmap, int>{QPixmap(EndOCol),0};
}
bool ComicConveyor::utilizeCurrentImage(bool backup)
{
	if (!works)
		return false;
	deleteImage(current, comic.at(pos), backup);
	comic.removeAt(pos);
	numbers.removeAt(pos);
	pos = (pos == 0) ? 0 : pos - 1;
	current.refresh();
	return true;
}
int ComicConveyor::position() const
{
	return pos;
}
int ComicConveyor::totalPages() const
{
	return comic.count();
}
bool ComicConveyor::store(QString filename, bool backup, bool no_erase)
//	Copy-paste entire comic under the new name
{
	if (!works)		//	If we really have comic to copy
		return false;
	outdir.mkdir(filename);		//	create folder under new name
	QDir temp(outdir.absoluteFilePath(filename));
	int max = comic.count();
	for (int i = 0; i < max; ++i)
	{
		// names of each file consists of the comic name and its number in numbers vector, not its position in conveyor
		QString iname = filename +" " +  QString::number(numbers[i]) + "." + comic[i].split('.').last();
		if (!cutAndPaste(current, comic[i], temp, iname, backup, no_erase))
		{
			return false;
		}
	}
	return true;
}
