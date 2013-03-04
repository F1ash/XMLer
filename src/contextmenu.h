#include <QPoint>
#include <QMenu>
#include <QModelIndex>
#include "xmlermodel.h"

class ContextMenu : public QMenu, QModelIndex
{
  Q_OBJECT;
public:
  explicit ContextMenu(QWidget *parent, QModelIndex index, XMLerModel *model);
  ~ContextMenu();

private:
  QModelIndex currIdx;
  XMLerModel *currModel;
  BaseXMLNode *item;
  QAction *add;
  QAction *remove;
  QAction *insert;

private slots:
  void addItem();
  void removeItem();
  void insertItem();
};
