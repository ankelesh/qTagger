#include "TagHolder.h"
#include <QKeyEvent>
#include <algorithm>
QBrush staticBrush(QColor("#e0d1f0"));
QColor bgcolor("#e0d1f0");
Tag NOTATAG("", -1);
void TagHolder::keyPressEvent(QKeyEvent* kev)
{
	switch (kev->key())
	{
	case Qt::Key_Up:
	case Qt::Key_W:
		_moveCursor(true);
		return;
	case Qt::Key_Down:
	case Qt::Key_S:
		_moveCursor(false);
		return;
	case Qt::Key_Right:
	case Qt::Key_D:
		_leaveThis(true);
		return;
	case Qt::Key_Left:
	case Qt::Key_A:
		_leaveThis(false);
		return;
	case Qt::Key_E:
		_switchItem();
		return;
	case Qt::Key_Q:
	case Qt::Key_Escape:
		_deselectAll();
		return;
	case Qt::Key_T:
		_erase();
		return;
	case Qt::Key_V:
		_addTagRequest();
		return;
	case Qt::Key_G:
		_deleteUnderSelection();
		return;
	case Qt::Key_R:
		_confirm();
		return;
	case Qt::Key_N:
	case Qt::Key_Space:
		_finishImage();
		return;
	default:
		return;
	}
}

void TagHolder::_moveCursor(bool up)
{
	if (currentRow() == -1)
	{
		setCurrentRow(0, QItemSelectionModel::Current);
		update();
		return;
	}
	if (up)
	{
		setCurrentRow(
			((currentRow() - 1 >= 0) ? currentRow() - 1 : 0)
		, QItemSelectionModel::Current);
	}
	else
	{
		setCurrentRow((
			(currentRow() + 1 < my_tag_list.count()) ? currentRow() + 1 : my_tag_list.count()-1
		), QItemSelectionModel::Current);
	}
	update();
}

void TagHolder::_leaveThis(bool forward)
{
	if (currentItem() != nullptr)
		currentItem()->setBackground(palette().base());
	emit goOutHere(id, forward);
}

void TagHolder::_switchItem()
{
	if (currentItem() != nullptr)
	currentItem()->setSelected(!currentItem()->isSelected());
}

void TagHolder::_deselectAll()
{
	clearSelection();
}

void TagHolder::_confirm()
{
	emit applyRequest();
}

void TagHolder::_addTagRequest()
{
	emit addRequest(id);
}

void TagHolder::_deleteUnderSelection()
{
	if (currentItem() != nullptr)
	{
		my_tag_list.takeAt(currentRow());
		takeItem(currentRow());
	}
}

void TagHolder::_erase()
{
	emit eraseRequest();
}

void TagHolder::_finishImage()
{
	emit confirmThisImage();
}

TagHolder::TagHolder(int my_id, QList<Tag>& tlist, QWidget* parent)
	: QListWidget(parent), my_tag_list(tlist), id(my_id)
{
	QList<Tag>::iterator begin = my_tag_list.begin();
	while (begin != my_tag_list.end())
	{
		addItem(begin++->tag);
	}
	setFocusPolicy(Qt::FocusPolicy::StrongFocus);
	QObject::connect(this, &QListWidget::currentItemChanged, this, &TagHolder::curritemChanged);
}

void TagHolder::reload()
{
	clear();
	QList<Tag>::iterator begin = my_tag_list.begin();
	while (begin != my_tag_list.end())
	{
		addItem(begin++->tag);
	}
}

QList<Tag> TagHolder::getSelectedTags()
{
	QList<Tag> temp;
	auto itSelected = selectedItems();
	auto begin = itSelected.begin();
	while (begin != itSelected.end())
	{
		temp.push_back(my_tag_list.at(indexFromItem((*begin++)).row()));
	}
	return temp;
}
Tag& TagHolder::findTag(QString tag)
{
	auto result = std::find(my_tag_list.begin(), my_tag_list.end(), Tag(tag));
	if (result != my_tag_list.end())
	{
		return *result;
	}
	return NOTATAG;
}
void TagHolder::sortingReload()
{
	std::sort(my_tag_list.begin(), my_tag_list.end());
	reload();
}
void TagHolder::markCurrentItem()
{
	if (currentItem() != nullptr)
		currentItem()->setBackgroundColor(bgcolor);
}
void TagHolder::addTag(QString tname)
{
	if (!my_tag_list.contains(tname))
	{
		my_tag_list.push_back(Tag(tname, 1));
		addItem(tname);
	}
}
void TagHolder::deleteTag(QString tname)
{
	auto result = std::find(my_tag_list.begin(), my_tag_list.end(), Tag(tname));
	if (result != my_tag_list.end())
	{
		int index = result - my_tag_list.begin();
		takeItem(index);
		my_tag_list.takeAt(index);
	}
}
void TagHolder::deleteSelectedTags()
{
	auto itSelected = selectedItems();
	auto begin = itSelected.begin();
	while (begin != itSelected.end())
	{
		takeItem(indexFromItem(*begin).row());
		my_tag_list.removeAt(indexFromItem(*begin).row());
		++begin;
	}
}
void TagHolder::curritemChanged(QListWidgetItem* n, QListWidgetItem* pr)
{
	if (pr != nullptr)
		pr->setBackground(palette().base());
	if (n != nullptr)
		n->setBackgroundColor(bgcolor);
}
