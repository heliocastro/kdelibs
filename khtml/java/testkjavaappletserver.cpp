#include <kapp.h>
#include <kwmmapp.h>
#include <kjavaappletserver.h>
#include <kjavaapplet.h>
#include <kjavaappletwidget.h>
#include <qstring.h>
#include <stdio.h>
#include <unistd.h>

int oldmain(int argc, char **argv)
{
  KJavaAppletServer *kjas = KJavaAppletServer::getDefaultServer();

  kjas->createContext( 0 );

  QString a,b,c,d,e,f;

  a = "fred";
  b = "Lake.class";
  c = "file:/dos/pcplus/java/lake/";
  kjas->createApplet( 0, 0, a, b, c );

  d = "image";
  e = "arch.jpg";
  kjas->setParameter( 0, 0, d, e );

  f = "another_title";
  kjas->showApplet( 0, 0, f );

  KApplication app( argc, argv );
  app.exec();
}

int bettermain(int argc, char **argv)
{
  QString a,b,c,d,e,f;

  a = "fred";
  b = "Lake.class";
  c = "file:/dos/pcplus/java/lake/";

  KJavaApplet *applet = new KJavaApplet();
  applet->setAppletName( a );
  applet->setAppletClass( b );
  applet->setBaseURL( c );

  applet->create();

  d = "image";
  e = "arch.jpg";
  applet->setParameter( d, e );

  f = "another_title";
  applet->show( f );

  KApplication app( argc, argv );
  app.exec();
}

int main(int argc, char **argv)
{
  KWMModuleApplication app( argc, argv );
  QString a,b,c,d,e,f;

  a = "fred";
  b = "BBGun_Brick.class";
  c = "file:/build/kde/kdelibs/khtml/java/BBGun_Brick.class";

  KJavaAppletWidget *applet = new KJavaAppletWidget();
  CHECK_PTR( applet );
  applet->setAppletName( a );
  applet->setAppletClass( b );
  applet->setBaseURL( c );

  applet->create();

  app.connectToKWM();

  //  d = "image";
  //  e = "arch.jpg";
  //  applet->setParameter( d, e );

  applet->show();

  ////

  app.exec();
}
