#include "settingsForm.h"
#include <QtWidgets/QLineEdit>


settingsForm::settingsForm(QWidget* qw, LaunchInfo::options& opts)
	: QWidget(qw), formlayout(new QFormLayout(this)),
	rpath(new QLineEdit(this)), opath(new QLineEdit(this)), rb(new QHBoxLayout(this)), comicmode(new QRadioButton("Comic", this)),
	stdmode(new QRadioButton("Standard", this)),
	d_auth(new QCheckBox(this)), cl_tg(new QCheckBox(this)),
	bck(new QCheckBox(this)), delor(new QCheckBox(this))
	, font_int(new QSpinBox(this)), font_tag(new QSpinBox(this))
	// Constructs form using preloaded options
{
	this->setLayout(formlayout);
	rb->addWidget(stdmode);
	rb->addWidget(comicmode);
	formlayout->addRow("Directory: ", rpath);
	formlayout->addRow("Output: ", opath);
	formlayout->addRow("Mode: ", rb);
	formlayout->addRow("Deduce author ", d_auth);
	formlayout->addRow("Clear tags", cl_tg);
	formlayout->addRow("Make backup images", bck);
	formlayout->addRow("Delete originals", delor);
	formlayout->addRow("Size of interface font", font_int);
	formlayout->addRow("Size of tags font", font_tag);
	rpath->setText(opts.ifname);
	opath->setText(opts.ofname);
	stdmode->setChecked(opts.mod == LaunchInfo::standard);
	comicmode->setChecked(opts.mod == LaunchInfo::comic);
	d_auth->setChecked(opts.deduce_author);
	cl_tg->setChecked(opts.clear_tags);
	bck->setChecked(opts.backup);
	delor->setChecked(opts.delete_original);
	font_int->setValue(opts.font_interface);
	font_int->setMaximum(30);
	font_int->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum));
	font_tag->setValue(opts.font_tags);
	font_tag->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum));
	font_tag->setMaximum(30);
	QObject::connect(opath, &QLineEdit::returnPressed, this, &settingsForm::onEnter);
	QObject::connect(rpath, &QLineEdit::returnPressed, this, &settingsForm::onEnter);
	QObject::connect(rpath, &QLineEdit::textChanged, this, &settingsForm::onChange);
}
void settingsForm::onChange()	//	This slot refreshes opath for easier setup
{
	opath->setText(rpath->text() + "tagged");
}
LaunchInfo::options settingsForm::submit()	//	Constructs and returns options object
{
	return LaunchInfo::options({ rpath->text(), opath->text()
		, (comicmode->isChecked()) ? LaunchInfo::comic : LaunchInfo::standard,
		d_auth->isChecked(), cl_tg->isChecked(), bck->isChecked(), delor->isChecked(),
		font_int->value(), font_tag->value(), QSize() });
}
void settingsForm::onEnter()
{
	emit done();
}
