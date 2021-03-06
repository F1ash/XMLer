// -*- C++ -*-
/* ------------------------------------ */
/* C++ class header (QThread)           */
/* Author: Alexei Panov <me@elemc.name> */
/* ------------------------------------ */
/* Name: XMLerLoadFileThread
   Description:
*/
#ifndef XMLERLOADFILETHREAD_H
#define XMLERLOADFILETHREAD_H

#include <QXmlSimpleReader>
#include <QXmlParseException>
#include <QXmlInputSource>
#include <QXmlStreamReader>

#include "xmlerfileoperthread.h"

#include "xmlerhandler.h"
#include "xmlerexception.h"
#include "xmlerinputsource.h"

class XMLerLoadFileThread : public XMLerFileOperThread
{
  Q_OBJECT
public:
  explicit XMLerLoadFileThread(QObject *parent = 0);
  ~XMLerLoadFileThread();

  void run ();
  
private:
  QMap<QString, QString> _info;
  XMLerHandler *handler;

  QMap<QString, QString> getInformationFromFile ();
  void checkExceptionInHandler ();
  QString progressMessage () const;

  /* CLEANIT
signals:
    void parseException ( int mainType, XMLerExceptionList exceptions ); */

};

#endif
