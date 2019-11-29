#pragma once
#include <QString>
#include <QSize>

namespace LaunchInfo
{
	enum ConveyorMode { standard, comic };		//	defines which conveyor must be constructed
	struct options								//	holds info used for Tagger constructor
	{
		QString ifname;							//	path of input directory
		QString ofname;							//	path of output directory
		ConveyorMode mod;						//	which conveyor will be constructed
		bool deduce_author;						//	tagengine setting 
		bool clear_tags;						//	conveyor settings
		bool backup;
		bool delete_original;
		int font_interface;
		int font_tags;
		QSize winsize;							//	size of the available space for displaying image
	};
	std::wstring serialize(LaunchInfo::options& opt);	//	serialization and deserializations functs
	LaunchInfo::options deserialize(std::wistream& fout);
}