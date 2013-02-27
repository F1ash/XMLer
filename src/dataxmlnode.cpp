// -*- C++ -*-
/* ------------------------------------ */
/* C++ class source (BaseXMLNode)       */
/* Author: Alexei Panov <me@elemc.name> */
/* ------------------------------------ */
/* Name: DataXMLNode
   Description:
*/
#include "dataxmlnode.h"

DataXMLNode::DataXMLNode (BaseXMLNode *parent):
  BaseXMLNode(parent)
{
  setNodeType(BaseXMLNode::Data);
  _data.clear();
  _qName = QString();
  _localName = QString();
  _namespaceURI = QString();
}

DataXMLNode::~DataXMLNode ()
{
}

void DataXMLNode::setNamespaceURI( const QString &uri )
{
  _namespaceURI = uri;
}
void DataXMLNode::setLocalName( const QString &ln )
{
  _localName = ln;
}
void DataXMLNode::setQName( const QString &qn )
{
  _qName = qn;
}

void DataXMLNode::setData ( const QString &d )
{
  _data = d;
}
QString DataXMLNode::data () const
{
  return _data;
}

/* Virtuals */
quint32 DataXMLNode::childCount() const
{
  return 0; // No childrens
}
XMLNodePtrList DataXMLNode::childs() const
{
  return XMLNodePtrList();
}
void DataXMLNode::appendChild(BaseXMLNode *child)
{
  // Do nothing
}
QString DataXMLNode::name () const
{
  return _localName;
}
QString DataXMLNode::qName () const
{
  return _qName;
}
QString DataXMLNode::namespaceURI () const
{
  return _namespaceURI;
}
