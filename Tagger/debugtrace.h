#pragma once

#include <iostream>
#include <sstream>
#include <QTextStream>
#include <QFile>
#include <QString>
#include <exception>
#include <QDebug>
#include <iostream>
#include <QVector>
#include <QFile>
#include <iomanip>
/*
	This is debugging and tracing module. You can use it for dynamic testing
	with varying verbosity of this tool. Use macroses starting with detrace_
	to automatically output your data. Alter creation of main detrace object to
	set up output mode and filter of the messages.
	use setp to set up priority for incomming input.

	TODO: add filtering by filenames to skip messages from unnecessary headers.

	Update:
		+1 macro
	Update:
		+1 macro
		replaced comments so they were captured with msvs
	Update:
		better naming of macros variables
  */
#ifdef Q_OS_ANDROID
#define FOUTPATH "/storage/emulated/0/log.txt"
#endif
#ifdef Q_OS_WIN32
#define FOUTPATH "log.txt"
#endif
#ifdef Q_OS_WINCE
#define FOUTPATH "log.txt"
#endif

  // print message about creating an object within specified method or function
#define detrace_OCREATED(OBJECT, AROUND) detrace << detr_supply::objectConstruction \
    << "][Created object " << OBJECT << " around " << AROUND << '\n'

  // print message about triggered default constructor
#define detrace_DCONSTR(OBJECT) detrace << detr_supply::defaultConstructor << \
    "\n][Created object " << OBJECT << " by his default constructor" << '\n'

// pring message about something happening in method
#define detrace_METHEXPL(HAPPENED) detrace << detr_supply::methodExplanation << HAPPENED << '\n'

// print message about something not important
#define detrace_SIMPLMSG(MESSAGE) detrace << detr_supply::notImportantMessage << MESSAGE << '\n'

// print message about called method
#define detrace_METHCALL(_METHOD_) detrace << detr_supply::methodCalled << \
    "\n\n->Method called: "  << _METHOD_ << '\n'

// print message about slot activation
#define detrace_SLOTCALL(_SLOT_, OBJECT) detrace << detr_supply::methodCalled << "\n\n-->Slot " \
    << _SLOT_ << " of object "  << OBJECT << " called" << '\n'

// print message about invocation of any method by any method of any class
#define detrace_METHINVOK(INVOCATED_METHOD,INVOCATED_OBJECT,_METHOD_ ,OBJECT) detrace << detr_supply::methodInvocation \
    << "\n\n-)Method " << INVOCATED_METHOD << " of object " << INVOCATED_OBJECT << \
	" invocated in method " << _METHOD_ << " of object " << OBJECT << '\n'

// print message about emitting signal
#define detrace_SIGNALINV(_SIGNAL_, OBJECT) detrace << detr_supply::signalInvocation\
    << "\n\n-! Signal " << _SIGNAL_ << " is emitted from " << OBJECT << '\n'

// print message containig snapshot of current data. Variables to dump must be provided using <<
#define detrace_METHDATAS(_METHOD_, _VARIABLES_, _STATE_) detrace << detr_supply::methodDataSnapshot\
    << "in method " << _METHOD_ << " state of variables "<< _VARIABLES_  << " was " _STATE_ << '\n'

// print message containing one text variable and its value in separated place
#define detrace_METHTEXTS(_METHOD_,_VARIABLE_,_TEXT_) detrace << detr_supply::methodDataSnapshot\
    << "in method " << _METHOD_ << " state of text variable " << _VARIABLE_ << " was: " << '\n'\
    << "________________________________________" << '\n' << _TEXT_ << '\n'

// print message about received arguments in method
#define detrace_METHDRECEIVE(_METHOD_, _ARGUMENTS_ , _VALUES_) detrace << detr_supply::methodDataSnapshot\
    << "method " << _METHOD_ << " received data: arguments " << _ARGUMENTS_ << " were filled with values"\
    << '\n' << "|" << _VALUES_ << "|" << '\n'

// print message about sent request
#define detrace_NETREQSENT(_METHOD_,_REQUEST_, _SUPPLIED_) detrace << detr_supply::netrequestSent << \
    "method " << _METHOD_ << " sent request " << _REQUEST_ << " supplied with " << _SUPPLIED_ << '\n'

// prints message using high priority
#define detrace_MSGIMP(_MESSAGE_) detrace << detr_supply::importantMessage << '\n' <<\
    "!" <<  '\n'<< _MESSAGE_ <<"!" << '\n' << '\n'

// prints low priority message that frequent method was invoked
#define detrace_METHFRECALL(_METHOD_) detrace << detr_supply::frequentMethodCalled <<\
    "method " << _METHOD_ << "called" << '\n'

// prints low priority message about cycle
#define detrace_CYCLEEXPL(_EXPLANATION_) detrace << detr_supply::cycleExplanation << \
    ">>cycling: " << _EXPLANATION_ << '\n'

// prints status of connecting with high priority to avoid disconnection errors
#define detrace_CONNECTSTAT(_SIGNAL_TO_SLOT_, _STATE_) detrace << detr_supply::errorPossible << \
	"after connecting " << _SIGNAL_TO_SLOT_ << " state was " << _STATE_ << '\n'

//print message about possible error during flow of method X
#define detrace_METHPERROR(_METHOD_, _CONTEXT_) detrace << detr_supply::errorPossible << \
	"Error possible in method " << _METHOD_ << "with context: " << _CONTEXT_ << '\n'

#define detrace_SUCCESS detrace << detr_supply::methodDataSnapshot << "Operation success!" << '\n'

#define detrace_FAIL detrace << detr_supply::methodDataSnapshot << "Operation failed!" << '\n'


namespace detr_supply { // Holds enums for defining output method and priorities
	enum OutputMode { toall, file, qDeb, qStr, Cons, some_united, buffConsole };
	// Defines output mode: file-> prints to log.txt, qDeb-> prints to console,
	// qStr-> prints to inner string, Cons -> prints to std::cout
	enum DebugPriority {
		all, errorPossible, importantMessage,
		objectConstruction, methodInvocation,
		signalInvocation,
		methodCalled, methodDataSnapshot, methodExplanation,
		netrequestSent, defaultConstructor, frequentMethodCalled,
		cycleExplanation, notImportantMessage
	};
	// Defines priority level: everything that is lower than detrace starting priority
	// will be printed.
}
using namespace detr_supply;
static DebugPriority setp(const int i) // converts integer to DebugPriority
{
	if (i >= all && i <= notImportantMessage)
	{
		return (DebugPriority)i;
	}
	return notImportantMessage;
}

class debugtrace
	// This class implements priority tracking and prints into various
	// output
{
private:
	QFile outfile; // log.txt
	QTextStream fout; // stream for file output
	DebugPriority priorityLvl; // main priority
	DebugPriority msgPriorityLvl; // priority of incoming message
	OutputMode omode; // mode of output, check enum
	QString outstring; // string holding output
	std::ostringstream buffer;
	QTextStream sout;  // stream for string output
	QString qDebugFullMessageHolder; // string to avoid new lines in qDebug()
	void (debugtrace::* outmethod)(const QString& str); // pointer to current
	DebugPriority* blacklist;           // blacklist of messages by priority
	int bllen;
	bool blackaswhite;
	bool nospaces;

	QVector<void(debugtrace::*)(const QString & str)> omode_united;           //  holds modes for printToSome
	// output method
	void printToFile(const QString& str); // Output methods
	void printToQDebug(const QString& str);
	void printToString(const QString& str);
	void printToAll(const QString& str);
	void printToConsole(const QString& str);
	void printToSome(const QString& str);
	void printToConsBuff(const QString& str);
	bool isValid();
	void flushQDebug();
	void flushBuffer();

public:
	debugtrace(DebugPriority priority, OutputMode mode,
		OutputMode  onlyOutputTo[], int oolen,
		DebugPriority  Blacklist[], int bllen, bool blackAsWhite, bool nospaces); // no default values
	debugtrace& operator<<(const DebugPriority); // sets priority for all next
	// messages
	debugtrace& operator<<(const int); // wrappers for normalizing the message
	debugtrace& operator<<(const long long int);
	debugtrace& operator<<(const char*);
	debugtrace& operator<<(const QString&);
	debugtrace& operator<<(const bool);
	debugtrace& operator<<(const double);
	debugtrace& operator<<(const std::exception&);
	debugtrace& operator<<(const char);
	void changeOutputMode(const OutputMode, OutputMode[], int); // changes outmethod to corresponding
	QString getCurrentString(); // returns outstring
	~debugtrace() { fout.flush(); }
};
// This is main static object of this module. You should not create more
// instances of it, instead use macroses.

// this whitelist defines which streams to use
extern debugtrace detrace;//(all,file,  onlyOutputTo, ootolen,blacklist, blackllen);

static bool debugtraceTestModule() // unit test
{
	detrace_SIMPLMSG("Test started");
	detrace_OCREATED("detrace", "detraceTest");
	detrace_METHEXPL("inmethdescr");
	detrace_DCONSTR("detrace");
	return true;
}
