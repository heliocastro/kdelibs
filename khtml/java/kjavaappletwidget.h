// -*- c++ -*-

#ifndef KJAVAAPPLETWIDGET_H
#define KJAVAAPPLETWIDGET_H

#include "kjavaappletcontext.h"
#include "kjavaapplet.h"
#include <javaembed.h>
#include <kwinmodule.h>

/**
 * @short A widget for displaying Java applets
 *
 * KJavaAppletWidget provides support for the inclusion of Java applets
 * in Qt and KDE applications. To create an applet, you must first create
 * a context object in which it will run. There can be several applets and
 * contexts in operation at a given time, for example in a web browser there
 * would be one context object for each web page. Applets in the same context
 * can communicate with each other, applets in different contexts cannot. (Well
 * actually, they can, but only via some very evil tricks). Once you have
 * created a KJavaAppletContext, you can create as many applets in it as you
 * want.
 *
 * Once you have created the applet widget, you should call the various setXXX
 * methods to configure it, they pretty much correspond to the HTML tags used
 * to embed applets in a web page. Once the applet is configured call the
 * create() method to set things in motion. The applet is running when it
 * first appears, but you can start or stop it when you like (for example
 * if it scrolls off the screen).
 *
 * This widget works by firing off a Java server process with which it
 * communicates using the KDE Java Applet Server (KJAS) protocol via a pipe.
 * The applet windows are swallowed and attached to the QWidget, but they are
 * actually running in a different process. This has the advantage of robustness
 * and reusability. The details of the communication are hidden from the user
 * in the KJASClient class. Normally only a single server process is used for
 * all of the applets in a given application, this is all sorted automatically.
 * The KJAS server is 100% pure Java, and should also prove useful for people
 * wishing to add java support to other systems (for example a perl/Tk binding
 * is perfectly feasible). All you need to do is implement the protocol and
 * (optionally) swallow the applet windows.
 *
 * Note that the KJAS protocol is not yet stable - it will certainly change for
 * a while before settling down. This will not affect you unless you use the
 * KJAS protocol directly.
 *
 * @author Richard J. Moore, rich@kde.org
 */

class KJavaAppletWidgetPrivate;

class KJavaAppletWidget : public KJavaEmbed
{
    Q_OBJECT
public:
    KJavaAppletWidget( KJavaAppletContext* context,
                       QWidget* parent=0, const char* name=0 );

   ~KJavaAppletWidget();

    KJavaApplet* applet() { return m_applet; }

    void showApplet();

    QSize sizeHint();
    void resize( int, int );

protected slots:
    void setWindow( WId w );

protected:
    void init();

private:
    KJavaAppletWidgetPrivate* d;

    KJavaApplet* m_applet;
    KWinModule*  m_kwm;
    QString      m_swallowTitle;

};

#endif // KJAVAAPPLETWIDGET_H

