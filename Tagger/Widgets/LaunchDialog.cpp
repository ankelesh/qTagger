#include "LaunchDialog.h"
#include <QtWidgets/QLabel>
#include <QtWidgets/qpushbutton.h>

LaunchDialog::LaunchDialog(LaunchInfo::options& opt, QRect& rct)
	: opts(opt), mainlayout(new QVBoxLayout(this)),
	header(new QLabel("Please check tagger settings:", this)),
	setform(new settingsForm(this, opts)),
	butlay(new QHBoxLayout(this)), ok(new QPushButton("Ok", this)),
	cancel(new QPushButton("Cancel", this)), scr(rct)
	//	This constructor is optimal 
{
	//Creating gui
	this->setLayout(mainlayout);
	mainlayout->addWidget(header);
	mainlayout->addWidget(setform);
	mainlayout->addLayout(butlay);
	butlay->addWidget(ok);
	butlay->addWidget(cancel);
	header->show();
	ok->show();
	cancel->show();
	header->show();
	setform->show();
	QObject::connect(setform, &settingsForm::done, this, &LaunchDialog::onOk);
	QObject::connect(ok, &QPushButton::clicked, this, &LaunchDialog::onOk);
	QObject::connect(cancel, &QPushButton::clicked, this, &QWidget::close);
}
void LaunchDialog::onOk()	//	this slot transmits ok-button clicks outside
{
	emit done();
}