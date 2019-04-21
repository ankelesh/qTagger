#include "debug.h"


	Debug::Debug()
	{
		fout.open("log.txt");
	}
	Debug::Debug(const char * filename)
	{
		fout.open(filename);
	}

	void Debug::in(const char * st)
	{
		fout << st;
		fout.flush();
	}
	void Debug::in(const char ch, const char bord = ' ')
	{
		fout << bord << ch << bord;
		fout.flush();
	}
	void Debug::in(const std::string & s)
	{
		fout << s.c_str() << '\n';
		fout.flush();
	}
	void Debug::operator<<(QString & st)
	{
		this->in(st.toStdString());
	}
	void Debug::operator<<(const char * chs)
	{
		fout << chs << '\n';
		fout.flush();
	}
	void Debug::operator<<(const int num)
	{
		fout << ' ' << num << ' ';
		fout.flush();
	}
	void Debug::operator<<(const bool val)
	{
		fout << ' ' << ((val) ? "true" : "false") << ' ';
		fout.flush();
	}
	Debug::~Debug()
	{
		fout.close();
	}
