// -*- C++ -*-
/* ------------------------------------ */
/* C++ class source (QAbstractItemModel)*/
/* Author: Alexei Panov <me@elemc.name> */
/* ------------------------------------ */
/* Name: XMLerModel
   Description:
*/
#include "xmlermodel.h"

XMLerModel::XMLerModel (QObject *parent):
  QAbstractItemModel(parent)
{
  _document = 0;
  _rootItem = 0;
  _modified = false;

  _loader = new XMLerLoadFileThread ( this );
  connect ( _loader, SIGNAL(done(DocumentXMLNode*)), this, SLOT(on_loaderDone(DocumentXMLNode*)) );

  _saver = new XMLerSaveFileThread ( this );
  connect ( _saver, SIGNAL(done()), this, SLOT(on_saverDone()) );

  _finder = new XMLerFindThread ( this );
  connect ( _finder, SIGNAL(done()), this, SLOT(on_finderDone()) );

  bookmark_current_position = -1;
}
XMLerModel::~XMLerModel ()
{
  foundedNodes.clear();
  bookmarkNodes.clear();

  if ( _document )
    delete _document;

  /* clean loader */
  delete _loader;
  _loader = 0;

  /* clean saver */
  delete _saver;
  _saver = 0;

  /* clean finder */
  delete _finder;
  _finder = 0;
}

/* self */
bool XMLerModel::loadXMLFile( const QString &fileName )
{
  _loader->setFileName ( fileName );
  _loader->start();
  return true;
}
bool XMLerModel::saveXMLFile( const QString &fileName )
{
  bool result = true;

  _saver->setFileName ( fileName );
  _saver->setDocument ( _document );
  _saver->start();

  return result;
}
bool XMLerModel::isNewModel () const
{
  if ( isEmptyModel () )
    return true;

  return document()->fileName().isEmpty(); // if fileName is not set, then it is a new model
}
bool XMLerModel::isEmptyModel () const
{
  return ( _document == 0 );
}
QString XMLerModel::fileName () const
{
  if ( isEmptyModel () )
    return tr("untitled");

  return _document->fileName();
}
bool XMLerModel::isModified () const
{
  return _modified;
}
QString XMLerModel::titlePart () const
{
  QString fn = fileName();
  if ( isModified() )
    fn += "*";
  return fn;
}
QModelIndex XMLerModel::rootIndex () const
{
  return index ( 0, 0 );
}
XMLerLoadFileThread *XMLerModel::loader ()
{
  return _loader;
}
XMLerSaveFileThread *XMLerModel::saver ()
{
  return _saver;
}
XMLerFindThread *XMLerModel::finder ()
{
  return _finder;
}
DocumentXMLNode *XMLerModel::document () const
{
  return _document;
}
void XMLerModel::modif ()
{
  _modified = true;
  emit touchModel();
}
QByteArray XMLerModel::indexToBuffer ( const QModelIndex &index )
{
  if ( !index.isValid() )
    return QByteArray();

  if ( BaseXMLNode *item = static_cast<BaseXMLNode *>(index.internalPointer()) ) {
    XMLerSaveFileThread temp_saver;
    QByteArray data = temp_saver.toBuffer ( item );
    return data;
  }

  return QByteArray();
}
QModelIndex XMLerModel::indexByNode ( BaseXMLNode *node ) const
{
    return createIndex(node->row(), 0, node);
}
void XMLerModel::bookmarkToggle ( const QModelIndex &index )
{
    if ( !index.isValid() )
        return;
    BaseXMLNode *item = static_cast<BaseXMLNode *>(index.internalPointer());
    if ( !item )
        return;

    if ( bookmarkNodes.contains( item ) )
        bookmarkNodes.removeAt( bookmarkNodes.indexOf( item ) );
    else
        bookmarkNodes.append( item );

    safeUpdateBookmarkIndex();
    emit touchModel();
}

/* Virtuals */
Qt::ItemFlags XMLerModel::flags(const QModelIndex &index) const
{
  Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);
  if ( !index.isValid() )
    return defaultFlags;

  BaseXMLNode *item = static_cast<BaseXMLNode *>(index.internalPointer());
  if ( !item )
    return defaultFlags;
  Qt::ItemFlags flags;
  if ( this->index(0,0)==index or \
       this->index(0,1)==index or \
       this->index(0,2)==index or \
       this->index(0,3)==index )
    flags = (defaultFlags | Qt::ItemIsEditable | Qt::ItemIsDropEnabled);
  else
    flags = (defaultFlags | Qt::ItemIsEditable | Qt::ItemIsDropEnabled | Qt::ItemIsDragEnabled);
  return flags;
}
int XMLerModel::columnCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return 4;
}
QVariant XMLerModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if ( orientation == Qt::Horizontal ) {
    if ( role == Qt::DisplayRole ) {
      switch (section) {
      case 1:
        return QString();
        break;
      case 2:
        return tr("Local name");
        break;
      case 0:
        return tr("Qualified name");
        break;
      case 3:
        return tr("Namespace URI");
      default:
        break;
      }
    }
  }

  return QAbstractItemModel::headerData( section, orientation, role );
}
QModelIndex XMLerModel::index(int row, int column, const QModelIndex &parent) const
{
  if (!hasIndex(row, column, parent))
    return QModelIndex();

  BaseXMLNode *parentItem;
  if (!parent.isValid())
    parentItem = _document; //rootItem;
  else
    parentItem = static_cast<BaseXMLNode *>(parent.internalPointer());

  BaseXMLNode *childItem = parentItem->childItemAt(row);
  if ( childItem )
    return createIndex(row, column, childItem);

  return QModelIndex();  
}
QModelIndex XMLerModel::parent(const QModelIndex &child) const
{
  if ( !child.isValid() )
    return QModelIndex();

  BaseXMLNode *childNode = static_cast<BaseXMLNode *>(child.internalPointer());
  BaseXMLNode *parentNode = childNode->parentNode();

  if ( !parentNode || parentNode == _document )
    return QModelIndex();

  return createIndex(parentNode->row(), 0, parentNode);
}
QVariant XMLerModel::data(const QModelIndex &index, int role) const
{
  if ( !index.isValid() )
    return QVariant();

  BaseXMLNode *item = static_cast<BaseXMLNode *>(index.internalPointer());
  if ( !item )
    return QVariant();

  if ( role == Qt::DisplayRole or role == Qt::EditRole ) {
    switch( index.column() ) {
    case 2:
      return item->name();
      break;
    case 0:
      return item->qName();
      break;
    case 3:
      return item->namespaceURI();
      break;
    default:
      break;
    }
  }
  else if ( role == Qt::DecorationRole ) {
    if ( index.column() == 0 )
        return item->typeToIcon();
    else if ( index.column() == 1 )
        return stateNodeIcon( item );
  }
  /* CLEANIT: not needed more
  else if ( role == Qt::BackgroundRole ) {
    if ( foundedNodes.contains ( item ) )
      return QBrush( Qt::darkYellow );
  } */

  return QVariant();
}
int XMLerModel::rowCount(const QModelIndex &parent) const
{
  if ( parent.column() > 0 )
    return 0;

  BaseXMLNode *item;
  if ( !parent.isValid() )
    item = _document; //rootItem;
  else
    item = static_cast<BaseXMLNode*>(parent.internalPointer());

  if ( item )
    return item->childCount();

  return 0;
}
bool XMLerModel::setData( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole )
{
  if ( !index.isValid() )
  {
    return false;
  };

  BaseXMLNode *item = static_cast<BaseXMLNode *>(index.internalPointer());
  if ( !item )
  {
    return false;
  };

  if ( role == Qt::EditRole ){
    switch( index.column() ) {
    case 0:
      item->setQName ( QString().append(value.toString()) );
      break;
    case 2:
      item->setLocalName ( QString().append(value.toString()) );
      break;
    case 3:
      item->setNamespaceURI ( QString().append(value.toString()) );
      break;
    default:
      break;
    }
  };
  
  emit dataChanged(index.parent(), index.child(1,1));
  return true;
}
/* Self private */
QIcon XMLerModel::stateNodeIcon ( BaseXMLNode *node ) const
{
    if ( !node )
        return QIcon();
    int state = 0;
    if ( foundedNodes.contains( node ) )
        state += 1;
    if ( bookmarkNodes.contains( node ) )
        state += 2;

    QIcon result;

    switch ( state ) {
       case 1:
            result = QIcon::fromTheme( "flag-yellow" );
            break;
        case 2:
            result = QIcon::fromTheme( "flag-red" );
            break;
        case 3:
            result = QIcon::fromTheme( "flag-green" );
            break;
        default:
            result = QIcon(); 
            break;
     };

     return result;
}
void XMLerModel::safeUpdateBookmarkIndex ()
{
    int list_size = bookmarkNodes.size();
    if ( list_size == 0 ) {
        bookmark_current_position = -1;
        return;
    }
    if ( bookmark_current_position >= list_size)
        bookmark_current_position = 0; //FIXME: maybe set last item
}
/* Slots */
void XMLerModel::on_loaderDone ( DocumentXMLNode *doc )
{
  if ( !doc )
    return;

  beginResetModel();
  _document = doc;
  endResetModel();
  _modified = false;
  emit touchModel();
}
void XMLerModel::on_saverDone ()
{
  _modified = false;
  emit touchModel();
}
void XMLerModel::on_finderDone ()
{
  foundedNodes.clear();
  foundedNodes = _finder->foundedNodes();
  emit touchModel();
}


/* public slots */
void XMLerModel::findNodes( const QString &findText )
{
  foundedNodes.clear();
  if ( !findText.isEmpty() ) {
    _finder->setDocument ( _document );
    _finder->setText ( findText );
    _finder->start();
  }
  else {
   emit touchModel(); 
  }
}
void XMLerModel::bookmarkNext () 
{
    if ( bookmarkNodes.size() == 0 )
        return;

    bookmark_current_position += 1;

    if ( bookmark_current_position >= bookmarkNodes.size() )
        bookmark_current_position = 0;

    emit gotoBookmark ( bookmarkNodes.at( bookmark_current_position ) );
}
void XMLerModel::bookmarkPrev ()
{
    if ( bookmarkNodes.size() == 0 )
        return;

    bookmark_current_position -= 1;

    if ( bookmark_current_position < 0 )
        bookmark_current_position = bookmarkNodes.size() - 1;

    emit gotoBookmark ( bookmarkNodes.at( bookmark_current_position ) );
}

/* edit tree */
bool XMLerModel::insertRow(int row, const QModelIndex &parent)
{
    BaseXMLNode *parentItem;
    if ( parent.isValid() )
        parentItem = static_cast<BaseXMLNode *>(parent.internalPointer());
    else
        parentItem = _rootItem;
    if ( !parentItem )
        parentItem = _rootItem;

    if (row == -1) row = parentItem->size()+1;
    beginInsertRows(parent, row, row);

    BaseXMLNode *newItem = new BaseXMLNode(parentItem);

    parentItem->insertChild(row, newItem);

    endInsertRows();
    emit dataChanged(parent, parent.child(0, parentItem->childCount()));
    return true;
}
bool XMLerModel::removeRow(int row, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row);
    BaseXMLNode *pitem;

    if ( !parent.isValid() )
        pitem = _rootItem;
    else
        pitem = static_cast<BaseXMLNode *>(parent.internalPointer());

    if ( !pitem )
        pitem = _rootItem;

    pitem->removeChildAt(row);
    endRemoveRows();
    emit dataChanged(parent, parent.child(0, pitem->childCount()));

    return true;
}
bool XMLerModel::removeRows(int row, int count, const QModelIndex &parent)
{
    bool result = true;
    for ( int i = row; i < row+count; i++ )
        result &= removeRow(i, parent);

    return result;
}

// drag & drop
Qt::DropActions XMLerModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}
QStringList XMLerModel::mimeTypes() const
{
    QStringList types;
    types << DRAG_AND_DROP_MIME;
    return types;
}
QDomElement XMLerModel::convertItemToDomElement(BaseXMLNode *item, QDomDocument &doc) const
{
    QDomElement itemElem;
    itemElem = doc.createElement("item");
    if ( item->childCount() ) {
        for ( int i = 0; i < item->childCount(); i++ ) {
            QDomElement childElem = convertItemToDomElement(item->childItemAt(i), doc);
            itemElem.appendChild(childElem);
        }
    }
    else {
        itemElem.setAttribute("QName", item->qName());
        itemElem.setAttribute("LocalName", item->name());
        itemElem.setAttribute("URI", item->namespaceURI());
    }

    return itemElem;
}
QByteArray XMLerModel::indexesToXML(const QModelIndexList &indexes) const
{
    QDomDocument doc;
    QDomElement	root = doc.createElement( "new_item" );

    doc.appendChild( root );

    foreach(QModelIndex idx, indexes) {
        if ( !idx.isValid() )
            continue;
        if ( idx.column() != 0 )
            continue;

        BaseXMLNode *item = static_cast<BaseXMLNode *>(idx.internalPointer());
        if ( !item )
            continue;

        QDomElement child = convertItemToDomElement(item, doc);
        root.appendChild(child);
    }

    QByteArray clearBuffer;
    QBuffer clearFile ( &clearBuffer );
    if ( !clearFile.open( QIODevice::WriteOnly | QIODevice::Text ) )
        return QByteArray();

    QTextStream clearStream(&clearFile);
    doc.save(clearStream, 4);
    clearFile.close();

    return clearBuffer;
}
QMimeData *XMLerModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData = indexesToXML(indexes);

    mimeData->setData(DRAG_AND_DROP_MIME, encodedData);
    return mimeData;
}
bool XMLerModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
                               int row, int column, const QModelIndex &parent)
{
    Q_UNUSED(column)
    if ( action == Qt::IgnoreAction )
        return true;

    if ( !data->hasFormat(DRAG_AND_DROP_MIME) )
        return false;


    BaseXMLNode *parentItem;
    int beginRow = row;
    if ( row == -1 && parent.isValid() )
        beginRow = rowCount(parent);
    else if ( row == -1 )
        beginRow = rowCount();

    if ( !parent.isValid() )
        parentItem = _rootItem;
    else {
        parentItem = static_cast<BaseXMLNode *>(parent.internalPointer());
        if ( !parentItem )
            parentItem = _rootItem;
    }

    QByteArray buf = data->data(DRAG_AND_DROP_MIME);
    QDomDocument doc;
    if ( !doc.setContent(buf) )
        return false;


    QDomElement rootElement = doc.documentElement();
    if ( rootElement.tagName() != "new_item" )
        return false;

    for(QDomNode n = rootElement.firstChild(); !n.isNull(); n = n.nextSibling())
    {
        if ( n.isElement() )
            //parseElementForDND( beginRow, parent, n.toElement() );
            true;
    }

    emit dataChanged(parent, parent);
    return true;
}
