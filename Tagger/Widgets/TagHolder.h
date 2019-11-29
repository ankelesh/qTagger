#pragma once
#include <QListWidget>
#include "Engines/TagEngine.h"


class TagHolder : public QListWidget
{
	Q_OBJECT
protected:
	QList<Tag>& my_tag_list;
	int id;

	void keyPressEvent(QKeyEvent*);


	void _moveCursor(bool up);
	void _leaveThis(bool forward);
	void _switchItem();
	void _deselectAll();
	void _confirm();
	void _addTagRequest();
	void _deleteUnderSelection();
	void _erase();
	void _finishImage();
public:
	TagHolder(int id, QList<Tag>& tlist, QWidget* parent);
	void reload();
	QList<Tag> getSelectedTags();
	int getId() { return id; };
	Tag& findTag(QString tag);
	void sortingReload();
	void markCurrentItem();
private slots:
	void curritemChanged(QListWidgetItem* n, QListWidgetItem* pr);
	
public slots:
	void addTag(QString tname);
	void deleteTag(QString tname);
	void deleteSelectedTags();
signals:
	void addRequest(int id);
	void applyRequest();
	void eraseRequest();
	void goOutHere(int id, bool forward);
	void confirmThisImage();
};