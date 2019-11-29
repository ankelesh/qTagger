#pragma once
#include <QtWidgets/QLabel>
#include <QtGui/QMouseEvent>

class LabelClickable : public QLabel
	//	Simple wrapper to override method
{
	Q_OBJECT
public:
	LabelClickable(const QString& text, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags())
		:QLabel(text, parent, f) {};
	LabelClickable(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags())
		:QLabel(parent, f) {};
	void mousePressEvent(QMouseEvent* qme)
		//	emits signal that label was clicked, determines which screen side was clicked, left = true
	{
		emit clicked((qme->x() < (size().width() / 2))); QLabel::mousePressEvent(qme);
	}
signals:
	void clicked(bool side);
};