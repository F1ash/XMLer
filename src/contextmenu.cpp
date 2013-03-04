#include "contextmenu.h"

ContextMenu::ContextMenu (QWidget *parent = 0, \
	QModelIndex index = QModelIndex(), \
	XMLerModel *model = new XMLerModel()) :
	QMenu(parent)
{
	currIdx = index;
	currModel = model;
	BaseXMLNode *item = static_cast<BaseXMLNode *>(index.internalPointer());
	QAction *add = this->addAction( QIcon().fromTheme("list-add"), \
		QString(tr("Add item")) );
	QAction *remove = this->addAction( QIcon().fromTheme("list-remove"), \
		QString(tr("Remove item")) );
	QAction *insert = this->addAction( QIcon().fromTheme("distribute-vertical-y"), \
		QString(tr("Insert item")) );
	connect(   add, SIGNAL(triggered()), this, SLOT(   addItem()));
	connect(remove, SIGNAL(triggered()), this, SLOT(removeItem()));
	connect(insert, SIGNAL(triggered()), this, SLOT(insertItem()));
}
ContextMenu::~ContextMenu()
{
	this->deleteLater();
}
void ContextMenu::addItem()
{
	qDebug("Add");
	if (currIdx.parent().isValid())
		currModel->insertRow(-1, currIdx.parent());
	else qDebug("Append is impossible.");
}
void ContextMenu::removeItem()
{
	qDebug("Rem");
	if ( currIdx != currModel->index(0,0) and \
		 currIdx != currModel->index(0,1) and \
		 currIdx != currModel->index(0,2) and \
		 currIdx != currModel->index(0,3) )
		currModel->removeRow(currIdx.row(), currIdx.parent());
	else qDebug("Remove rootIndex is impossible.");
}
void ContextMenu::insertItem()
{
	qDebug("Ins");
	if (currIdx.parent().isValid())
		currModel->insertRow(currIdx.row()+1, currIdx.parent());
	else qDebug("Isert is impossible.");
}
