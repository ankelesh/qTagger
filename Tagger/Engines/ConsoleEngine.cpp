#include "ConsoleEngine.h"

bool ConsoleEngine::handleNext(const QString& cmd)
{
	tagger->next_pressed();
	return true;
}

bool ConsoleEngine::handlePlus(const QString& cmd)
{
	return true;
}

ConsoleEngine::ConsoleEngine(QObject* parent, Tagger* tagger)
{
}

ConsoleEngine::ConsoleEngine(Tagger* parent)
{
}
