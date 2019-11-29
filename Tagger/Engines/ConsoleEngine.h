#pragma once
#include <QObject>
#include "Widgets/Tagger.h"
#include <QMap>


class ConsoleEngine;
typedef bool (ConsoleEngine::*handlerFunction)(const QString & cmd);
class ConsoleEngine : public QObject
{
private:
	Tagger* tagger;
	

	QMap<QString, handlerFunction> handlers;



	bool handleNext(const QString& cmd);
	bool handlePlus(const QString& cmd);

public:
	ConsoleEngine(QObject* parent, Tagger* tagger);
	ConsoleEngine(Tagger* parent);




public slots:
	void listenInput(const QString& newText);



};



