#include "LaunchInfo.h"
#include <sstream>
#include <iostream>
#include <string>
#include <istream>
namespace LaunchInfo {
	std::wstring serialize(LaunchInfo::options& opts)
		//	serializing into widestring
	{
		std::wostringstream wsout;		//	using standard string streams
		wsout << opts.ifname.toStdWString() << "\n" << opts.ofname.toStdWString() << "\n"
			<< (int)opts.mod << " " << opts.deduce_author << ' ' << opts.clear_tags
			<< ' ' << opts.backup << ' ' << opts.delete_original << ' '
			<< opts.font_interface << ' ' << opts.font_tags << " <options end>\n";
		return wsout.str();
	}
	options deserialize(std::wistream& fout)
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
