// -*- C++ -*-
/* ------------------------------------ */
/* C++ source main file                 */
/* Author: Alexei Panov <me@elemc.name> */
/* ------------------------------------ */
/* Name: main.cpp
   Description: This is main file for XMLer project
*/

#include "config.h"

#include "xmlerapplication.h"
#include <QtCore/QTextCodec>
#include <QtCore/QStringList>
#include <QtCore/QTranslator>
#include <QtCore/QLibraryInfo>
#include "mainwindow.h"

QStringList XMLer_languages ()
{
    QStringList sl;
    sl.append( QString("ru_ru") );

    return sl;
}

int main ( int argc, char **argv )
{
  XMLerApplication app( argc, argv );

#ifdef QT4_BUILD
  /* set utf-8 codec for tr */
  QTextCodec *utfCodec = QTextCodec::codecForName("UTF-8");
  QTextCodec::setCodecForTr(utfCodec);
#endif

  /* translator */
  QTranslator trans;
  QTranslator qt_trans;
  QLocale locale = QLocale::system();
  if ( XMLer_languages().contains ( locale.name().toLower() ) ) {
    QString trans_name = QString( "XMLer_%1" ).arg ( locale.name() );  
    QString qt_name = QString( "qt_%1").arg ( locale.name() );
    #ifdef Q_OS_DARWIN
        QString trans_dir = app.applicationDirPath() + "/../Resources/translations/";
        trans.load ( trans_name, trans_dir);
        qt_trans.load( qt_name, trans_dir);
    #elif defined(Q_OS_WIN32)
        QString trans_dir = app.applicationDirPath() + "/translations/";
        trans.load ( trans_name, trans_dir);
        qt_trans.load( qt_name, trans_dir);
    #else
        trans.load( trans_name, QString( XMLER_TRANSLATIONS_DIR ) );
        qt_trans.load( qt_name, QLibraryInfo::location( QLibraryInfo::TranslationsPath ) );
    #endif
      }
  app.installTranslator( &qt_trans );
  app.installTranslator( &trans );

  app.openFiles();

  int result = app.exec();
  return result;
}
