#pragma once
#include <fstream>
#include <qstring.h>

	class Debug
	{
	private:
		std::ofstream fout;
	public:
		Debug();
		Debug(const char *);
		void in(const char *);
		void in(const char, const char);
		void in(const std::string &);
		void operator<<(std::string & st) { this->in(st); };
		void operator<<(QString & st);
		void operator<<(const char * chs);
		void operator<<(const int);
		void operator<<(const bool);
		~Debug();
	};
	static Debug debug;


