#ifndef _KPART_H
#define _KPART_H

#include <qstring.h>
#include <qdom.h>
#include <qguardedptr.h>
#include <kurl.h>

#include <kparts/xmlgui.h>

class KInstance;
class QWidget;
class QAction;
class QActionCollection;
class QEvent;

namespace KParts
{

class PartManager;
class Plugin;
class PartPrivate;
class PartActivateEvent;
class PartSelectEvent;
class GUIActivateEvent;

/**
 * Base class for parts.
 *
 * A "part" is a GUI component, featuring
 * @li a widget embeddedable in any application
 * @li GUI elements that will be merged in the "host" user interface
 * (menubars, toolbars... )
 *
 * About the widget :
 * Note that @ref KParts::Part does not inherit @ref QWidget.
 * This is due to the fact that the "visual representation"
 * will probably not be a mere @ref QWidget, but an elaborate one.
 * That's why when implementing your @ref KParts::Part (or derived)
 * you should call @ref KParts::Part::setWidget() in your constructor.
 *
 * About the GUI elements :
 * Those elements trigger actions, defined by the part (@ref action()).
 * The layout of the actions in the GUI is defined by an XML file (@ref setXMLFile()).
 *
 * This class is an abstract interface that you need to inherit from.
 * See also @ref PartReadOnly and @ref PartReadWrite, which define the
 * framework for a "viewer" part and for an "editor"-like part.
 * Use Part directly only if your part doesn't fit into those.
 */
class Part : public QObject, public XMLGUIServant
{
  Q_OBJECT
public:
    Part( QObject *parent = 0, const char* name = 0 );
    virtual ~Part();

    QAction* action( const char* name );
    QActionCollection* actionCollection();

    virtual Plugin* plugin( const char* libname );

    virtual QAction *action( const QDomElement &element );

    /**
     * Embed this part into a host widget.
     *
     * You don't need to do this if you created the widget with the
     * correct parent widget - this is just a @ref QWidget::reparent().
     * Note that the @ref Part is still the holder
     * of the @ref QWidget, meaning that if you delete the @ref Part,
     * then the widget gets destroyed as well, and vice-versa.
     * This method is deprecated since creating the widget with the correct
     * parent is simpler anyway.
     * @deprecated
     */
    virtual void embed( QWidget * parentWidget );

    /**
     * @return The widget defined by this part, set by @ref setWidget().
     */
    virtual QWidget *widget();

    /**
     * @return The instance (@ref KInstance) for this part.
     */
    virtual KInstance *instance();

    // Only called by PartManager - should be protected and using friend ?
    virtual void setManager( PartManager * manager );
    /**
     * @return The part manager handling this part, if any (0L otherwise).
     */
    PartManager * manager();

    /**
     * @return The parsed XML in a @ref QDomDocument, set by @ref setXMLFile() or @ref setXML()
     */
    virtual QDomDocument document() const;

    /**
     * @internal
     */
    virtual Part *hitTest( QWidget *widget, const QPoint &globalPos );

    virtual void setSelectable( bool selectable );
    virtual bool isSelectable() const;

protected:
    /**
     * Set the instance (@ref KInstance) for this part.
     *
     * Call this first in the inherited class constructor.
     * (At least before @ref setXMLFile().)
     */
    virtual void setInstance( KInstance *instance, bool loadPlugins = true );

    /**
     * Set the main widget.
     *
     * Call this in the Part-inherited class constructor.
     */
    virtual void setWidget( QWidget * widget );

    /**
     * Set the name of the rc file containing the XML for the part.
     *
     * Call this in the Part-inherited class constructor.
     *
     * @param file Either an absolute path for the file, or simply the filename,
     *             which will then be assumed to be installed in the "data" resource,
     *             under a directory named like the instance.
     **/
    virtual void setXMLFile( QString file );

    //TODO: return bool, to make it possible for the part to check whether its xml is invalid (Simon)
    /**
     * Set the XML for the part.
     *
     * Call this in the Part-inherited class constructor if you
     *  don't call @ref setXMLFile().
     **/
    virtual void setXML( const QString &document );

    virtual bool event( QEvent *event );

    virtual void partActivateEvent( PartActivateEvent *event );
    virtual void partSelectEvent( PartSelectEvent *event );
    virtual void guiActivateEvent( GUIActivateEvent *event );

private slots:
    void slotWidgetDestroyed();

private:
    QGuardedPtr<QWidget> m_widget;
    KInstance * m_instance;
    /**
     * Holds the contents of the config file
     */
    QString m_config;
    QActionCollection m_collection;
    PartManager * m_manager;

    PartPrivate *d;
};

class ReadOnlyPartPrivate;

/**
 * Base class for any "viewer" part.
 *
 * This class takes care of network transparency for you,
 * in the simplest way (synchronously).
 * To use the built-in network transparency, you only need to implement
 * @ref openFile(), not @ref openURL().
 * To prevent network transparency, or to implement it another way
 * (e.g. asynchronously), override openURL().
 *
 * KParts Application can use the signals to show feedback while the URL is being loaded.
 */
class ReadOnlyPart : public Part
{
  Q_OBJECT
public:
  /**
   * Constructor
   * See also @ref Part for the setXXX methods to call.
   */
  ReadOnlyPart( QObject *parent = 0, const char *name = 0 );
  /**
   * Destructor
   */
  virtual ~ReadOnlyPart();

  /**
   * Only reimplement openURL if you don't want synchronous network transparency
   * Otherwise, reimplement @ref openFile() only .
   **/
  virtual bool openURL( const KURL &url );
  virtual const KURL & url() const { return m_url; }

  /**
   * Called when closing the current url (e.g. document), for instance
   * when switching to another url (note that @ref openURL calls it
   * automatically in this case).
   * If the current URL is not fully loaded yet, aborts loading.
   * Deletes the temporary file used when the url is remote.
   * @return always true, but the return value exists for reimplementations
   */
  virtual bool closeURL();

signals:
  /**
   * The part emits this when starting data.
   * If using a KIOJob, it sets the jobId in the signal, so that
   * progress information can be shown. Otherwise, jobId is 0.
   **/
  void started( int jobId );
  /**
   * Emit this when you have completed loading data.
   * Hosting apps will want to know when the process of loading the data
   *  is finished, so that they can access the data when everything is loaded.
   **/
  void completed();
  /**
   * Emit this if loading is canceled by the user or by an error.
   **/
  void canceled( const QString &errMsg );

protected slots:
  void slotJobFinished( int _id );
  void slotJobError( int, int, const char * );

protected:
  /**
   * If the part uses the standard implementation of @ref openURL,
   * it must reimplement this, to open @p m_file.
   * Otherwise simply define it to { return false; }
   */
  virtual bool openFile() = 0;

  /**
   * @internal
   */
  void abortLoad();

  /**
   * Remote (or local) url - the one displayed to the user.
   */
  KURL m_url;
  /**
   * Local file - the only one the part implementation should deal with.
   */
  QString m_file;
  /**
   * If @p true, @p m_file is a temporary file that needs to be deleted later.
   */
  bool m_bTemp;

private:
  ReadOnlyPartPrivate *d;
};

/**
 * Base class for an "editor" part.
 *
 * This class handles network transparency for you.
 * Anything that can open a URL, allow modifications, and save
 * (to the same URL or a different one).
 *
 * The part acts read-only by default, because a ReadWritePart could
 * be used as a ReadOnlyPart without knowing it.
 * Applications should call @ref setReadWrite to enable the part's
 * read-write mode.
 *
 * Part writers :
 * You HAVE to call setReadWrite( false ) in the constructor.
 * And any part inheriting ReadWritePart should check @ref isReadWrite
 * before allowing any action that modifies the part.
 * The part probably wants to reimplement @ref setReadWrite, disable those
 * actions. Don't forget to call the parent @ref setReadWrite.
 */
class ReadWritePart : public ReadOnlyPart
{
  Q_OBJECT
public:
  /**
   * Constructor
   * In the inherited constructor, call setReadWrite( false ).
   * See also @ref Part for the setXXX methods to call.
   */
  ReadWritePart( QObject *parent = 0, const char *name = 0 );
  /**
   * Destructor
   * Applications using a ReadWritePart should make sure, before
   * destroying it, to call closeURL().
   * In queryClose, for instance, they should allow closing only if
   * the return value of closeURL was true. This allows to cancel.
   */
  virtual ~ReadWritePart();

  /**
   * @return true if the part is in read-write mode
   */
  virtual bool isReadWrite() { return m_bReadWrite; }

  /**
   * Changes the behaviour of this part to readonly or readwrite.
   * Note that the initial behaviour of a ReadWritePart is _readonly_.
   * This allows embedding a ReadWritePart as a ReadOnlyPart.
   * @param readwrite set to true to enable readwrite mode
   */
  virtual void setReadWrite ( bool readwrite = true );

  /**
   * @return true if the document has been modified.
   */
  virtual bool isModified() const { return m_bModified; }

  /**
   * Called when closing the current url (e.g. document), for instance
   * when switching to another url (note that @ref openURL calls it
   * automatically in this case).
   * If the current URL is not fully loaded yet, aborts loading.
   * Reimplemented from ReadOnlyPart, to handle modified parts
   * (and suggest saving in this case, with yes/no/cancel).
   * @return false on cancel
   */
  virtual bool closeURL();

  /**
   * Save the file to a new location.
   *
   * Calls @ref save(), no need to reimplement
   */
  virtual bool saveAs( const KURL &url );

  virtual void setModified( bool modified );

public slots:
  /**
   * Call @ref setModified() whenever the contents get modified.
   * This is a slot for convenience, so that you can connect it
   * to a signal, like textChanged().
   */
  virtual void setModified();

  /**
   * Save the file in the location from which it was opened.
   * You can connect this to the "save" action.
   * Calls @ref saveFile and @ref saveToURL, no need to reimplement.
   */
  virtual bool save();

protected:
  /**
   * Save to a local file.
   * You need to implement it, to save to @p m_file.
   * The framework takes care of re-uploading afterwards.
   */
  virtual bool saveFile() = 0;

  /**
   * Save the file.
   *
   * Uploads the file, if @p m_url is remote.
   * This will emit @ref started(), and either @ref completed() or @ref canceled(),
   * in case you want to provide feedback.
   */
  virtual bool saveToURL();

protected slots:
  void slotUploadFinished( int _id );
  void slotUploadError( int, int, const char * );

private:
  bool m_bModified;
  bool m_bReadWrite;
  bool m_bClosing;
};

};

#endif
