// -*- c++ -*-

#ifndef KJAVAAPPLET_H
#define KJAVAAPPLET_H

#include <kurl.h>

#include <qobject.h>
#include <qmap.h>




/**
 * @short A Java applet
 *
 * @version $Id$
 * @author Richard J. Moore, rich@kde.org
 */

class KJavaAppletWidget;
class KJavaAppletContext;

class KJavaApplet : public QObject
{
Q_OBJECT

public:
    KJavaApplet( KJavaAppletWidget* _parent, KJavaAppletContext* _context = 0 );
    KJavaApplet( KJavaAppletContext* context = 0 );
    virtual ~KJavaApplet();

   //
   // Stuff to do with the applet
   //

   /**
    * Specify the name of the class file to run. For example 'Lake.class'.
    */
   void setAppletClass( const QString &clazzName );
   QString &appletClass();

   /**
    * Specify the location of the jar file containing the class.
    * (unimplemented)
    */
   void setJARFile( const QString &jar );
   QString &jarFile();

   /**
    * Specify a parameter to be passed to the applet.
    */
   void setParameter( const QString &name, const QString &value );
   QString &parameter( const QString &name );
   QMap< QString, QString >& getParams();

   /**
    * Set the URL of the document embedding the applet.
    */
   void setBaseURL( const QString &base );
   QString &baseURL();

   /**
    * Set the codebase of the applet classes.
    */
   void setCodeBase( const QString &codeBase );
   QString &codeBase();

   void setAppletName( const QString &name );
   QString &appletName();
 
   /**
    * Set the size of the applet
    */
   void setSize( QSize size );
   QSize size();

   /**
    * Interface for applets to resize themselves
    */
   void resizeAppletWidget( int width, int height );

   void create();
   bool isCreated();
 
   void show( const QString &title );

   /**
    * Run the applet.
    */
   void start();

   /**
    * Pause the applet.
    */
   void stop();

   int appletId();
   void setAppletId( int id );

private:
   // Applet info
   struct KJavaAppletPrivate *d;
   QMap<QString, QString> params;
   KJavaAppletContext *context;
   int id;
};

#endif // KJAVAAPPLET_H
