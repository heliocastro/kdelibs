/* This file is part of the KDE libraries
    Copyright (C) 1997 Matthias Kalle Dalheimer (kalle@kde.org)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

// $Id$

#ifndef _KAPP_H
#define _KAPP_H

// Version macros. Never put this further down.
#define KDE_VERSION_STRING "Almost 2.0 Alpha"
#define KDE_VERSION_MAJOR 1
#define KDE_VERSION_MINOR 9
#define KDE_VERSION_RELEASE 1

class KConfig;
class KCharsets;
class KStyle;
class QTDispatcher;

typedef unsigned long Atom;

#include <qapplication.h>
#include <qpixmap.h>
class QPopupMenu;
class QStrList;
class KSessionManaged;
#define kapp KApplication::getKApplication()

class KApplicationPrivate;

/**
* Controls and provides information to all KDE applications.
*
* Only one object of this class can be instantiated in a single app.
* This instance is always accessible via the @ref getKApplication method.
*
* This class provides the following services to all KDE applications.
*
* @li It controls the event queue (see @ref QApplication ).
* @li It provides the application with KDE resources such as
* accelerators, common menu entries, a @ref KConfig object. session
* management events, help invocation etc.
* @li Installs a signal handler for the SIGCHLD signal in order to
* avoid zombie children. If you want to catch this signal yourself or
* don't want it to be caught at all, you have set a new signal handler
* (or SIG_IGN) after KApplication's constructor has run.
* @li	Provides a debugging system for all KDE GUI apps. This can be
*	invoked via the key combo Ctrl-Shift-F12 in any KDE application.
*
* @short Controls and provides information to all KDE applications.
* @author Matthias Kalle Dalheimer <kalle@kde.org>
* @version $Id$
*/
class KApplication : public QApplication
{
  friend QTDispatcher;

  Q_OBJECT
public:
  enum CaptionLayout { CaptionAppLast=1, CaptionAppFirst, CaptionNoApp };

  /**
   * Constructor. Parses command-line arguments.
   *
   */
  KApplication( int& argc, char** argv,
		const QString& rAppName = QString::null);

  /** Destructor */
  virtual ~KApplication();

  /** Returns the number of command line arguments, i. e. the length
   *  of the array KApplication::argv(). In this array, the arguments
   *  handled by KApplication itself (i. e. '-icon') are already removed
   */
  int argc() const { return pArgc; }

  /** A global event filter for KApplication.
	* Filters out Ctrl-Shift-F12 for KDebug.
	*/
  virtual bool eventFilter( QObject*, QEvent* );

  /**
	* Return the current application object.

	* This is similar to the global QApplication pointer qApp. It
	* allows access to the single global KApplication object, since
	* more than one cannot be created in the same application. It
	* saves the trouble of having to pass the pointer to it explicitly
	* to every function that may require it.
	*/
  static KApplication* getKApplication() { return KApp; }

  /**
   * this function is obsolete. Use KGlobal::config() instead.
   * getConfig will just call this function
   * @see KConfig
   */
  KConfig* getConfig() const;

  /**
   * Retrieve the application session config object.
   *
   * @return A pointer to the application's instance specific
   * 	KConfig object.
   * @see KConfig
   */
  KConfig* getSessionConfig();

  /**
	* Is the application restored from the session manager?
	*
	* @return If true, this application was restored by the session manager.
	*	Note that this may mean the config object returned by
	*	getSessionConfig() contains data saved by a session closedown.
	* @see #getSessionConfig
	*/
  bool isRestored() const { return QApplication::isSessionRestored(); }

  /**
   * Disables session management for this application.
   * Useful in case  your application is started by the inital "startkde" script.
   */
  void disableSessionManagement();


    /*
      Reimplemented for internal purposes, mainly the highlevel
      handling of session management with KSessionManaged.
     */
  void commitData( QSessionManager& sm );
    /*
      Reimplemented for internal purposes, mainly the highlevel
      handling of session management with KSessionManaged.
     */
  void saveState( QSessionManager& sm );


  /**
   * Get the standard help menu.
   *
   * @param bAboutQtMenu If true, there is a menu entry for About Qt
   * @return a standard help menu
   */
  QPopupMenu* getHelpMenu( bool bAboutQtMenu,
			   const QString& appAboutText );
							
  /**
	* Get the application icon.
	* @return a QPixmap with the icon.
	* @see QPixmap
	*/
  QPixmap getIcon() const;

  /**
	* Get the mini-icon for the application.
	* @return a QPixmap with the icon.
	* @see QPixmap
	*/
  QPixmap getMiniIcon() const;

  /** 
      Makes @param topWidget a top widget of the application.
      
      This means bascially applying the right window caption and
      icon. An application may have several top widgets. You don't
      need to call this function manually when using KTMainWindow 
      
      @see #getIcon, #getCaption
    */
  void setTopWidget( QWidget *topWidget );

  /**
   * Invoke the kdehelp HTML help viewer.
   *
   * @param aFilename	The filename that is to be loaded. Its
   *			location is computed automatically
   *			according to the KFSSTND.  If aFilename
   *			is empty, the logical appname with .html
   *			appended to it is used.
   * @param aTopic		This allows context-sensitive help. Its
   *			value will be appended to the filename,
   *			prefixed with a "#" (hash) character.
   */
  void invokeHTMLHelp( QString aFilename, QString aTopic ) const;

  /**
   * Returns a KDE style object if a plugin is in use, or else NULL
   * if a Qt internal style is being used.
   *
   */
  KStyle* kstyle() const {return pKStyle;}

  /**
   * Get the KDE font list.
   *
   * This method allows you to get the KDE font list which was
   * composed by the user with kfontmanager. Usually you should
   * work only with those fonts in your kapplication.
   *
   * @param fontlist a reference to a list to fill with fonts.
   * @return true on success.
   */
  bool getKDEFonts(QStringList &fontlist) const;

  // QString appName () const { return name(); }

  /**
   * Return a text for the window caption.
   *
   * This would be set either by
   * "-caption", otherwise it will be equivalent to the name of the
   * executable.
   */
  QString getCaption() const;

  /**
   * Builds a caption that contains the application name along with the 
   * userCaption using a standard layout. To make a compliant caption
   * for your window, simply do: setCaption(kapp->makeStdCaption(yourCaption));
   *
   * @param userCaption is the caption string you want to display in the 
   * window caption area. Do not include the application name!
   * @param withAppName indicates that the method shall include or ignore
   * the application name when making the caption string. You are not
   * compliant if you set this to false.
   */
  QString makeStdCaption( const QString &userCaption, 
			  bool withAppName=true ) const;

  /**
   * Get a file name in order to make a temporary copy of your document.
   *
   * @param pFilename The full path to the current file of your
   * document.
   * @return A new filename for auto-saving.
   */
  QString tempSaveName( const QString& pFilename ) const;

  /**
   * Check if there is an auto-save file for the document you want to
   * open.
   *
   * @param pFilename The full path to the document you want to open.
   * @param bRecover  This gets set to true if there was a recover
   * file.
   * @return The full path of the file to open.
   */
  QString checkRecoverFile( const QString& pFilename, bool& bRecover ) const;

  /**
   * Get the X11 display
   */
  Display *getDisplay() { return display; }

protected:
  /**
   * Used to catch X11 events
   */
  bool x11EventFilter( XEvent * );


  Display *display;

  Atom KDEChangePalette;
  Atom KDEChangeGeneral;
  Atom KDEChangeStyle;

  /// Current application object.
  static KApplication *KApp;
  int pArgc;

public slots:
  void aboutKDE();
  void aboutApp();
  void aboutQt();

protected slots:
  void appHelpActivated();

private:
  KApplicationPrivate* pAppData;
  KConfig* pSessionConfig; //instance specific application config object
  QString aCaption; // the name for the window title
  QString aAppAboutString; // The text for the about box
  QString aDummyString2; // do not touch
  QString aDummyString3; // do not touch
  QString aDummyString4; // do not touch
  bool bSessionManagement;
  QPixmap aIconPixmap;
  QPixmap aMiniIconPixmap;
  KStyle *pKStyle; // A KDE style object if available (mosfet)
  void* styleHandle; // A KDE style dlopen handle, if used
  QWidget *smw;

  void init( );
  void parseCommandLine( int&, char** ); // search for special KDE arguments

  virtual void kdisplaySetPalette();
  virtual void kdisplaySetStyle();
  virtual void kdisplaySetFont();
  virtual void kdisplaySetStyleAndFont();
  virtual void readSettings(bool reparse = true);
  void resizeAll();
  virtual void applyGUIStyle(GUIStyle);

  QColor inactiveTitleColor_;
  QColor inactiveTextColor_;
  QColor activeTitleColor_;
  QColor activeTextColor_;
  int contrast_;
    GUIStyle applicationStyle_; // ###### pointless, to be removed
  int captionLayout;

public:

  QColor inactiveTitleColor() const;
  QColor inactiveTextColor() const;
  QColor activeTitleColor() const;
  QColor activeTextColor() const;
  int contrast() const;
  GUIStyle applicationStyle() const;


  /**
      @internal
    */
  int xioErrhandler();

  signals:
  /**
	* KApplication has changed its Palette due to a KDisplay request.
	*
	* Normally, widgets will update their palettes automatically, but you
	* should connect to this to program special behaviour.
	*/
  void kdisplayPaletteChanged();

  /**
	* KApplication has changed its GUI Style due to a KDisplay request.
	*
	* Normally, widgets will update their styles automatically (as they would
	* respond to an explicit setGUIStyle() call), but you should connect to
	* this to program special behaviour.
	*/
  void kdisplayStyleChanged();

  /**
	* KApplication has changed its Font due to a KDisplay request.
	*
	* Normally widgets will update their fonts automatically, but you should
	* connect to this to monitor global font changes, especially if you are
	* using explicit fonts.
	*/
  void kdisplayFontChanged();

  /**
	* KApplication has changed either its GUI style, its font or its palette
	* due to a kdisplay request. Normally, widgets will update their styles
	* automatically, but you should connect to this to program special
	* behavior. */
  void appearanceChanged();

  /** Session management asks you to save the state of your application.
	*
	* Connect to this signal in order to save your data. Do NOT
	* manipulate the UI in that slot, it is blocked by the session manager.
	*
	* Use the @ref ::getSessionConfig KConfig object to store all
	* your instance specific datas.
	*
	* Do not do any closing at this point! The user may still
	* select "cancel" and then he wants to continue working with
	* your application. Cleanups could be done after shutDown()
	* (see below)
	*
	* Note: You should not use this if you are using the KTopLevelWidget.
	*       Overload @ref KTopLevelWidget::saveProperties and
	*	@ref KTopLevelWidget::readProperties in that case.
	*	This allows you to simply handle applications with multiple
	* toplevel windows.  */
  void saveYourself();

  /** Your application is killed. Either by your program itself, kwm's
      killwindow function, xkill or (the usual case) by KDE's logout.

      The signal is particularly useful if your application has to do some
      last-second cleanups. Note that no user interaction is possible at
      this state.
   */
  void shutDown();


private:

  KApplication(const KApplication&);
  KApplication& operator=(const KApplication&);
};

  /** Check, if a file may be accessed in a given mode.
        * This is a wrapper around the access() system call.
        * checkAccess() calls access() with the given parameters.
        * If this is OK, checkAccess() returns true. If not, and W_OK
        * is part of mode, it is checked if there is write access to
        * the directory. If yes, checkAccess() returns true.
        * In all other cases checkAccess() returns false.
        *
        * Other than access() this function EXPLICITELY ignores non-existant
        * files if checking for write access.
        *
        * @param pathname The full path of the file you want to test
        * @param mode     The access mode, as in the access() system call.
        * @return Whether the access is allowed, true = Access allowed
        */
  bool checkAccess(const QString& pathname, int mode);



/**
*  Provides highlevel aceess to session management on a per-object base.
*
*  You don't need to do anything with this class when using KTMainWindow
*
* @short Highlevel access to session management.
* @author Matthias Ettrich <ettrich@kde.org>
*/
class KSessionManaged 
{
public:
    KSessionManaged();
    ~KSessionManaged();
    virtual bool saveState( QSessionManager& sm );
    virtual bool commitData( QSessionManager& sm );
};



#endif

// $Log$
// Revision 1.101  1999/09/20 13:11:26  ettrich
//    ported to new session management
//    usable private data class (use pAppData)
//
// Revision 1.100  1999/08/06 18:58:45  gehrmab
//
// * KApplication::argc()
// * Fixed documentation of tempSaveName()
//
// Revision 1.99  1999/08/03 22:56:28  ettrich
// removed some debug output (it was impossible to work with this),
// some smaller cleanups, reintroduced support for local colos/fonts
// in addition to styles. Will soon be property based.
//
// Revision 1.98  1999/08/03 20:21:32  kulow
// removing the last kde_ function and kdedir() with it  ;)
//
// Revision 1.97  1999/07/23 17:07:15  steffen
// be QTDispatchers friend
//
// Revision 1.96  1999/07/22 10:01:25  kulow
// if drag.h is obsolete, kde_datadir is for long :)
//
// Revision 1.95  1999/07/20 00:47:45  pbrown
// removed kdnd stuff from kapp.
//
// Revision 1.94  1999/06/23 19:13:39  knoll
// removed xxxFont() methods. Use KGlobal::xxxFont() instead.
//
// Revision 1.93  1999/06/20 09:41:48  kulow
// big patch:
// removed const QString as return values
// marked some const functions const
// made FALSE and TRUE consistent with the rest (besides that I still
//   believe that passing TRUE is an implicit cast - it may be optimized
//   away, but it's still there somehow ;)
// removing some more functions from kapp.h (registerTopWidget, etc)
//
// BTW: What applications use kclipboard?
//
// Revision 1.92  1999/06/19 21:16:26  kulow
// found the reason why KConfig parses all files twice at startup.
// KApplication does reparseConfig right after creating the config
// object (through KGlobal). Fixed in adding a bool reparse parameter
// to readSettings
//
