/*
 * Override X11 functions and set X window properties, to provide app-starting
 * notification.
 *
 * The original functions XMapWindow() and XMapRaised() are replaced with new
 * versions. These will call the function KDE_InterceptXMapRequest() if it
 * hasn't been called before, then call the original function.
 *
 * KDE_InterceptXMapRequest() first dlopen()s libX11 and sets
 * KDE_RealXMapRaised and KDE_RealXMapWindow to the original functions, so
 * that they may be called later.
 *
 * Next we set properties on the initial window, which will be picked up by
 * the wm when it sees the window map.
 *
 * (C) 2000 Rik Hemsley <rik@kde.org>
 * (C) 2000 Simon Hausmann <hausmann@kde.org>
 * (C) 2000 Bill Soudan <soudan@kde.org>
 */

#include <malloc.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <ltdl.h>
#include <config.h>

int XMapWindow(Display *, Window);
int XMapRaised(Display *, Window);

typedef Window (*KDE_XMapRequestSignature)(Display *, Window);

KDE_XMapRequestSignature KDE_RealXMapWindow = NULL;
KDE_XMapRequestSignature KDE_RealXMapRaised = NULL;

static long KDE_initialDesktop = 0;

void KDE_InterceptXMapRequest(Display *, Window);
void KDE_SetInitialDesktop(Display *, Window);

  int
XMapWindow(Display * d, Window w)
{
  if (NULL == KDE_RealXMapWindow)
    KDE_InterceptXMapRequest(d, w);

  /* The first Window that's mapped doesn't seem to actually be the
   * first Window that KWin manages.  The first window that KWin manages
   * seems to be the first one that has more than 0 properties.
   * Is this a valid assumption? */

  /* only set the KWIN_INITIAL_DESKTOP property on the first window
   * that already has properties */
  if (KDE_initialDesktop) {
    int num;
    (void)XListProperties(d, w, &num);
    if (num>1) {
      KDE_SetInitialDesktop(d, w);
      KDE_initialDesktop=0;
    }
  }

  return KDE_RealXMapWindow(d, w);
}

  int
XMapRaised(Display * d, Window w)
{
  if (NULL == KDE_RealXMapRaised)
    KDE_InterceptXMapRequest(d, w);

  /* The first Window that's mapped doesn't seem to actually be the
   * first Window that KWin manages.  The first window that KWin manages
   * seems to be the first one that has more than 0 properties.
   * Is this a valid assumption? */

  /* only set the KWIN_INITIAL_DESKTOP property on the first window
   * that already has properties */
  if (KDE_initialDesktop) {
    int num;
    (void)XListProperties(d, w, &num);
    if (num>1) {
      KDE_SetInitialDesktop(d, w);
      KDE_initialDesktop=0;
    }
  }


  return KDE_RealXMapRaised(d, w);
}

  void
KDE_InterceptXMapRequest(Display * d, Window w)
{
  /* Vars *****************************************************************/

  Atom netMapNotify;
  XTextProperty prop;
  Status status;
  char * pidString = 0L;
  char *xInitialDesktop;

  lt_dlhandle libX11Handle;

  /* Init *****************************************************************/

  xInitialDesktop = getenv("X_INITIAL_DESKTOP");
  if (xInitialDesktop)
    KDE_initialDesktop = atoi(xInitialDesktop);
  
  putenv("LD_PRELOAD=");
  putenv("X_INITIAL_DESKTOP=");

  /* Find symbols *********************************************************/

  libX11Handle = lt_dlopen("libX11.so");

  if (NULL == libX11Handle)
    libX11Handle = lt_dlopen("libX11.so.6");

  if (NULL == libX11Handle) {
    fprintf(stderr, "KDE: Could not dlopen libX11\n");
    exit(1);
  }

  KDE_RealXMapWindow =
    (KDE_XMapRequestSignature)lt_dlsym(libX11Handle, "XMapWindow");

  if (NULL == KDE_RealXMapWindow) {
    fprintf(stderr, "KDE: Could not find symbol XMapWindow in libX11\n");
    exit(1);
  }

  KDE_RealXMapRaised =
    (KDE_XMapRequestSignature)lt_dlsym(libX11Handle, "XMapRaised");

  if (NULL == KDE_RealXMapRaised) {
    fprintf(stderr, "KDE: Could not find symbol XMapRaised in libX11\n");
    exit(1);
  }

  /* Set property on initial window ***************************************/

  netMapNotify = XInternAtom(d, "_NET_MAP_NOTIFY", False);

  pidString = (char *)malloc(32);

  snprintf(pidString, 32, "%d", getpid());

  status = XStringListToTextProperty(&pidString, 1, &prop);

  if (0 != status)
    XSetTextProperty(d, w, &prop, netMapNotify);
  else
    fprintf(stderr, "KDE: kmapnotify: Could not set text property\n");
}

  void
KDE_SetInitialDesktop(Display *d, Window w) {

  Atom a;

  // FIXME - what name to use for the atom?
  //         read by kdebase/kwin/client.cpp Client::manage
  a = XInternAtom(d, "KWIN_INITIAL_DESKTOP", False); // _NET_INITIAL_DESKTOP?

  XChangeProperty(d, w, a, a, 32, PropModeReplace, (unsigned char *)
    &KDE_initialDesktop, 1);

}


/* vim: set ts=2:sw=2:tw=78: */
