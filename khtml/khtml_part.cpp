/* This file is part of the KDE project
 *
 * Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
 *                     1999 Lars Knoll <knoll@kde.org>
 *                     1999 Antti Koivisto <koivisto@kde.org>
 *		       2000 Simon Hausmann <hausmann@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "khtml_part.h"
#include "khtml_factory.h"
#include "khtml_run.h"
#include "khtml_events.h"
#include "khtml_find.h"

#include "dom/html_document.h"
#include "dom/dom_node.h"
#include "dom/dom_element.h"
#include "html/html_documentimpl.h"
#include "misc/khtmldata.h"
#include "html/html_miscimpl.h"
#include "html/html_inlineimpl.h"
#include "html/html_formimpl.h"
#include "rendering/render_frames.h"
#include "misc/htmlhashes.h"
#include "misc/loader.h"
#include "xml/dom_textimpl.h"

#include "khtmlview.h"
#include "decoder.h"
#include "kjs.h"
#include "khtml_settings.h"

#include <assert.h>

#include <kglobal.h>
#include <kstddirs.h>
#include <kio/job.h>
#include <kmimetype.h>
#include <kdebug.h>
#include <klocale.h>
#include <kcharsets.h>
#include <kmessagebox.h>
#include <kaction.h>
#include <kstdaction.h>
#include <kfiledialog.h>
#include <klibloader.h>
#include <ktrader.h>
#include <kparts/partmanager.h>
#include <kcharsets.h>
#include <kxmlgui.h>
#include <kcursor.h>

#include <qtextcodec.h>

#include <unistd.h>
#include <qstring.h>
#include <qfont.h>
#include <qfile.h>
#include <qfontinfo.h>
#include <qobject.h>
#include <qregexp.h>
#include <qtimer.h>
#include <qwidget.h>
#include <qclipboard.h>
#include <qapplication.h>
#include <qdragobject.h>

namespace khtml
{
  struct ChildFrame
  {
    ChildFrame() { m_bCompleted = false; m_frame = 0L; m_bPreloaded = false; m_bFrame = true; }

    RenderPart *m_frame;
    QGuardedPtr<KParts::ReadOnlyPart> m_part;
    QGuardedPtr<KParts::BrowserExtension> m_extension;
    QString m_serviceName;
    QString m_serviceType;
    QStringList m_services;
    bool m_bCompleted;
    QString m_name;
    KParts::URLArgs m_args;
    QGuardedPtr<KHTMLRun> m_run;
    bool m_bPreloaded;
    KURL m_workingURL;
    bool m_bFrame;
    QStringList m_params;
  };

};

typedef QMap<QString,khtml::ChildFrame>::ConstIterator ConstFrameIt;
typedef QMap<QString,khtml::ChildFrame>::Iterator FrameIt;

class KHTMLPartPrivate
{
public:
  KHTMLPartPrivate()
  {
    m_doc = 0L;
    m_decoder = 0L;
    m_jscript = 0L;
    m_job = 0L;
    m_bComplete = true;
    m_bParsing = false;
    m_manager = 0L;
    m_settings = new khtml::Settings();
    m_bClearing = false;
    m_bCleared = false;
    m_userSheet = QString::null;
    m_fontBase = 0;
    m_bDnd = true;
    m_startOffset = m_endOffset = 0;
    m_startBeforeEnd = true;
    m_linkCursor = KCursor::handCursor();
  }
  ~KHTMLPartPrivate()
  {
    //no need to delete m_view here! kparts does it for us (: (Simon)
    if ( m_extension )
      delete m_extension;
    delete m_settings;
    delete m_jscript;
  }

  QMap<QString,khtml::ChildFrame> m_frames;
  QValueList<khtml::ChildFrame> m_objects;

  QGuardedPtr<KHTMLView> m_view;
  KHTMLPartBrowserExtension *m_extension;
  KHTMLPartBrowserHostExtension *m_hostExtension;
  DOM::HTMLDocumentImpl *m_doc;
  khtml::Decoder *m_decoder;
  QString m_encoding;
  KJSProxy *m_jscript;
  bool m_bJScriptEnabled;
  bool m_bJavaEnabled;

  khtml::Settings *m_settings;

  KIO::TransferJob * m_job;

  bool m_bComplete;
  bool m_bParsing;

  KURL m_workingURL;
  KURL m_baseURL;
  QString m_baseTarget;

  int m_delayRedirect;
  KURL m_redirectURL;

  KAction *m_paViewDocument;
  KAction *m_paViewFrame;
  KAction *m_paSaveBackground;
  KAction *m_paSaveDocument;
  KAction *m_paSaveFrame;
  KSelectAction *m_paSetEncoding;
  KAction *m_paIncFontSizes;
  KAction *m_paDecFontSizes;
  KAction *m_paLoadImages;
  KAction *m_paFind;

  KParts::PartManager *m_manager;

  bool m_bClearing;
  bool m_bCleared;

  DOM::DOMString m_userSheet;
  DOM::DOMString m_userSheetUrl;

  QString m_popupMenuXML;

  int m_fontBase;

  int m_findPos;
  DOM::NodeImpl *m_findNode;

  QString m_strSelectedURL;

  bool m_bMousePressed;

  DOM::Node m_selectionStart;
  long m_startOffset;
  DOM::Node m_selectionEnd;
  long m_endOffset;
  bool m_startBeforeEnd;
  QString m_overURL;
  bool m_bDnd;

  QPoint m_dragStartPos;

  QCursor m_linkCursor;
};

namespace khtml {
    class PartStyleSheetLoader : public CachedObjectClient
    {
    public:
	PartStyleSheetLoader(KHTMLPartPrivate *part, DOM::DOMString url)
	{
	    m_part = part;
	    Cache::requestStyleSheet(url, DOMString());
	}

	virtual void setStyleSheet(const DOM::DOMString &url, const DOM::DOMString &sheet)
	{
	    m_part->m_userSheet = sheet;
	    m_part->m_userSheetUrl = url;
	    delete this;
	}
	KHTMLPartPrivate *m_part;
    };
};

KHTMLPart::KHTMLPart( QWidget *parentWidget, const char *widgetname, QObject *parent, const char *name )
: KParts::ReadOnlyPart( parent ? parent : parentWidget, name ? name : widgetname )
{
  setInstance( KHTMLFactory::instance() ); // doesn't work inside init() for derived classes
  init( new KHTMLView( this, parentWidget, widgetname ) );
}

KHTMLPart::KHTMLPart( KHTMLView *view, QObject *parent, const char *name )
: KParts::ReadOnlyPart( parent, name )
{
  setInstance( KHTMLFactory::instance() );
  assert( view );
  init( view );
}

void KHTMLPart::init( KHTMLView *view )
{
  khtml::Cache::ref();

  setXMLFile( "khtml.rc" );

  d = new KHTMLPartPrivate;

  d->m_view = view;
  setWidget( d->m_view );

  d->m_extension = new KHTMLPartBrowserExtension( this );
  d->m_hostExtension = new KHTMLPartBrowserHostExtension( this );

  d->m_paLoadImages = 0;

  d->m_bJScriptEnabled = KHTMLFactory::defaultHTMLSettings()->enableJavaScript();
  d->m_bJavaEnabled = KHTMLFactory::defaultHTMLSettings()->enableJava();

  autoloadImages( KHTMLFactory::defaultHTMLSettings()->autoLoadImages() );

  d->m_paViewDocument = new KAction( i18n( "View Document Source" ), 0, this, SLOT( slotViewDocumentSource() ), actionCollection(), "viewDocumentSource" );
  d->m_paViewFrame = new KAction( i18n( "View Frame Source" ), 0, this, SLOT( slotViewFrameSource() ), actionCollection(), "viewFrameSource" );
  d->m_paSaveBackground = new KAction( i18n( "Save &Background Image As.." ), 0, this, SLOT( slotSaveBackground() ), actionCollection(), "saveBackground" );
  d->m_paSaveDocument = new KAction( i18n( "&Save As.." ), 0, this, SLOT( slotSaveDocument() ), actionCollection(), "saveDocument" );
  d->m_paSaveFrame = new KAction( i18n( "Save &Frame As.." ), 0, this, SLOT( slotSaveFrame() ), actionCollection(), "saveFrame" );

  d->m_paSetEncoding = new KSelectAction( i18n( "Set &Encoding.." ), 0, this, SLOT( slotSetEncoding() ), actionCollection(), "setEncoding" );
  QStringList encodings = KGlobal::charsets()->availableCharsetNames();
  encodings.prepend( i18n( "Auto" ) );
  d->m_paSetEncoding->setItems( encodings );
  d->m_paSetEncoding->setCurrentItem(0);

  d->m_paIncFontSizes = new KAction( i18n( "Increase Font Sizes" ), "viewmag+", 0, this, SLOT( slotIncFontSizes() ), actionCollection(), "incFontSizes" );
  d->m_paDecFontSizes = new KAction( i18n( "Decrease Font Sizes" ), "viewmag-", 0, this, SLOT( slotDecFontSizes() ), actionCollection(), "decFontSizes" );

  d->m_paFind = KStdAction::find( this, SLOT( slotFind() ), actionCollection(), "find" );
  
  /*
    if ( !autoloadImages() )
      d->m_paLoadImages = new KAction( i18n( "Display Images on Page" ), "image", 0, this, SLOT( slotLoadImages() ), actionCollection(), "loadImages" );
  */

  connect( this, SIGNAL( completed() ),
	   this, SLOT( updateActions() ) );
  connect( this, SIGNAL( started( KIO::Job * ) ),
	   this, SLOT( updateActions() ) );

  d->m_popupMenuXML = KXMLGUIFactory::readConfigFile( locate( "data", "khtml/khtml_popupmenu.rc", KHTMLFactory::instance() ) );

  connect( khtml::Cache::loader(), SIGNAL( requestDone() ),
	   this, SLOT( checkCompleted() ) );

  findTextBegin(); //reset find variables
}

KHTMLPart::~KHTMLPart()
{
  disconnect( khtml::Cache::loader(), SIGNAL( requestDone() ),
	      this, SLOT( checkCompleted() ) );
  if ( d->m_view )
  {
    d->m_view->hide();
    d->m_view->viewport()->hide();
    d->m_view->m_part = 0;
  }
  closeURL();

  clear();

  delete d;
  khtml::Cache::deref();
}

bool KHTMLPart::openURL( const KURL &url )
{
  kdDebug( 6050 ) << "KHTMLPart::openURL" << url.url() << endl;
  static QString http_protocol = QString::fromLatin1( "http" );

  KParts::URLArgs args( d->m_extension->urlArgs() );
  if ( d->m_frames.count() == 0 && urlcmp( url.url(), m_url.url(), true, true ) && args.postData.size() == 0 && !args.reload )
  {
    kdDebug( 6050 ) << "KHTMLPart::openURL now m_url = " << url.url() << endl;
    m_url = url;
    emit started( 0L );

    if ( !url.htmlRef().isEmpty() )
      gotoAnchor( url.htmlRef() );
    else
      d->m_view->setContentsPos( 0, 0 );

    d->m_bComplete = true;
    d->m_bParsing = false;

    kdDebug( 6050 ) << "completed..." << endl;
    emit completed();
    return true;
  }

  kdDebug( 6050 ) << "closing old URL" << endl;
  if ( !closeURL() )
    return false;

  if ( args.postData.size() > 0 && url.protocol() == http_protocol )
      d->m_job = KIO::http_post( url, args.postData, false );
  else
      d->m_job = KIO::get( url, args.reload, false );

  connect( d->m_job, SIGNAL( result( KIO::Job * ) ),
           SLOT( slotFinished( KIO::Job * ) ) );
  connect( d->m_job, SIGNAL( data( KIO::Job*, const QByteArray &)),
           SLOT( slotData( KIO::Job*, const QByteArray &)));

  connect( d->m_job, SIGNAL(redirection(KIO::Job*, const KURL&) ),
           SLOT( slotRedirection(KIO::Job*,const KURL&) ) );

  d->m_bComplete = false;

  d->m_workingURL = url;

  // initializing m_url to the new url breaks relative links when opening such a link after this call and _before_ begin() is called (when the first
  // data arrives) (Simon)
  //  m_url = url;
  kdDebug( 6050 ) << "KHTMLPart::openURL now (before started) m_url = " << m_url.url() << endl;

  emit started( d->m_job );

  return true;
}

bool KHTMLPart::closeURL()
{
  if ( d->m_job )
  {
    d->m_job->kill();
    d->m_job = 0;
  }

  d->m_bComplete = true; // to avoid emitting completed() in end() (David)

  if ( d->m_bParsing )
  {
    kdDebug( 6050 ) << " was still parsing... calling end " << endl;
    end();
    d->m_bParsing = false;
  }

  d->m_workingURL = KURL();

  khtml::Cache::loader()->cancelRequests( m_url.url() );

  return true;
}

DOM::HTMLDocument KHTMLPart::htmlDocument() const
{
  return d->m_doc;
}

KHTMLPartBrowserExtension *KHTMLPart::browserExtension() const
{
  return d->m_extension;
}

KHTMLView *KHTMLPart::view() const
{
  return d->m_view;
}

void KHTMLPart::enableJScript( bool enable )
{
  d->m_bJScriptEnabled = enable;
}

bool KHTMLPart::jScriptEnabled() const
{
  return d->m_bJScriptEnabled;
}

KJSProxy *KHTMLPart::jScript()
{
  if ( !d->m_bJScriptEnabled )
    return 0;

  if ( !d->m_jscript )
  {
    KLibrary *lib = KLibLoader::self()->library("kjs_html");
    if ( !lib )
      return 0;
    // look for plain C init function
    void *sym = lib->symbol("kjs_html_init");
    if ( !sym )
      return 0;
    typedef KJSProxy* (*initFunction)(KHTMLPart *);
    initFunction initSym = (initFunction) sym;
    d->m_jscript = (*initSym)(this);
  }

  return d->m_jscript;
}

void KHTMLPart::executeScript( const QString &script )
{
  if ( !d->m_bJScriptEnabled )
    return;

  jScript()->evaluate( script.unicode(), script.length() );
}

void KHTMLPart::enableJava( bool enable )
{
  d->m_bJavaEnabled = enable;
}

bool KHTMLPart::javaEnabled() const
{
  return d->m_bJavaEnabled;
}

void KHTMLPart::autoloadImages( bool enable )
{
  if ( enable == khtml::Cache::autoloadImages() )
    return;

  khtml::Cache::autoloadImages( enable );

  KXMLGUIFactory *guiFactory = factory();
  if ( guiFactory )
    guiFactory->removeClient( this );

  if ( enable )
  {
    if ( d->m_paLoadImages )
      delete d->m_paLoadImages;
    d->m_paLoadImages = 0;
  }
  else if ( !d->m_paLoadImages )
    d->m_paLoadImages = new KAction( i18n( "Display Images on Page" ), "image", 0, this, SLOT( slotLoadImages() ), actionCollection(), "loadImages" );

  if ( guiFactory )
    guiFactory->addClient( this );

}

bool KHTMLPart::autoloadImages() const
{
  return khtml::Cache::autoloadImages();
}

void KHTMLPart::clear()
{
    kdDebug( 6090 ) << "KHTMLPart::clear()" << endl;
  if ( d->m_bCleared )
    return;
  d->m_bCleared = true;

  d->m_bClearing = true;
  if ( d->m_doc )
  {
    kdDebug( 6090 ) << "KHTMLPart::clear(): dereferencing the document" << endl;
    d->m_doc->detach();
    d->m_doc->deref();
  }
  d->m_doc = 0;

  if ( d->m_decoder )
    delete d->m_decoder;

  d->m_decoder = 0;

  if ( d->m_jscript )
    d->m_jscript->clear();

  if ( d->m_view )
    d->m_view->clear();
  /*
  QMap<QString,khtml::ChildFrame>::ConstIterator it = d->m_frames.begin();
  QMap<QString,khtml::ChildFrame>::ConstIterator end = d->m_frames.end();
  for(; it != end; ++it )
    if ( it.data().m_part )
    {
      partManager()->removePart( it.data().m_part );
      delete (KParts::ReadOnlyPart *)it.data().m_part;
    }
  */
  d->m_frames.clear();
  d->m_objects.clear();

  d->m_baseURL = KURL();
  d->m_baseTarget = QString::null;
  d->m_delayRedirect = 0;
  d->m_redirectURL = KURL();
  d->m_bClearing = false;

  d->m_bMousePressed = false;

  d->m_selectionStart = DOM::Node();
  d->m_selectionEnd = DOM::Node();
  d->m_startOffset = 0;
  d->m_endOffset = 0;
}

bool KHTMLPart::openFile()
{
  return true;
}

DOM::HTMLDocumentImpl *KHTMLPart::docImpl() const
{
  return d->m_doc;
}

void KHTMLPart::slotData( KIO::Job*, const QByteArray &data )
{
  kdDebug( 6050 ) << "slotData: " << data.size() << endl;
  // The first data ?
  if ( !d->m_workingURL.isEmpty() )
  {
    kdDebug( 6050 ) << "begin!" << endl;
    d->m_bParsing = true;

    begin( d->m_workingURL, d->m_extension->urlArgs().xOffset, d->m_extension->urlArgs().yOffset );

    d->m_workingURL = KURL();
  }

  write( data.data(), data.size() );
}

void KHTMLPart::slotFinished( KIO::Job * job )
{
  if (job->error())
  {
    job->showErrorDialog();
    d->m_job = 0L;
    emit canceled( job->errorString() );
    // TODO: what else ?
    return;
  }
  kdDebug( 6050 ) << "slotFinished" << endl;

  d->m_workingURL = KURL();

  d->m_job = 0L;

  if ( d->m_bParsing )
  {
    end(); //will emit completed()
  }
}

void KHTMLPart::begin( const KURL &url, int xOffset, int yOffset )
{
  clear();
  d->m_bCleared = false;

  // ###
  //stopParser();

  KParts::URLArgs args( d->m_extension->urlArgs() );
  args.xOffset = xOffset;
  args.yOffset = yOffset;
  d->m_extension->setURLArgs( args );

  m_url = url;
  if ( !m_url.isEmpty() )
  {
    KURL::List lst = KURL::split( m_url );
    KURL baseurl;
    if ( !lst.isEmpty() )
      baseurl = *lst.begin();

    KURL title( baseurl );
    title.setRef( QString::null );
    title.setQuery( QString::null );
    emit setWindowCaption( title.url() );
  }
  else
    emit setWindowCaption( i18n( "* Unknown *" ) );

  d->m_doc = new HTMLDocumentImpl( d->m_view );
  d->m_doc->ref();
  d->m_doc->attach( d->m_view );
  d->m_doc->setURL( m_url.url() );
  d->m_doc->setRestoreState(args.docState());
  d->m_doc->open();
  // clear widget
  d->m_view->resizeContents( 0, 0 );

  d->m_bParsing = true;
}

void KHTMLPart::write( const char *str, int len )
{
  if ( !d->m_decoder )
  {
    d->m_decoder = new khtml::Decoder();
    if(d->m_encoding != QString::null)
	d->m_decoder->setEncoding(d->m_encoding.latin1());
  }
  if ( len == 0 )
    return;

  if ( len == -1 )
    len = strlen( str );

  QString decoded = d->m_decoder->decode( str, len );
  if(d->m_decoder->visuallyOrdered()) d->m_doc->setVisuallyOrdered();
  const QTextCodec *c = d->m_decoder->codec();
  if(!d->m_settings->charset == QFont::Unicode)
      setCharset(c->name());
  d->m_doc->write( decoded );
}

void KHTMLPart::write( const QString &str )
{
  if ( str.isNull() )
    return;

  d->m_doc->write( str );
}

void KHTMLPart::end()
{
  d->m_bParsing = false;
  d->m_doc->close();

  if ( !m_url.htmlRef().isEmpty() )
    gotoAnchor( m_url.htmlRef() );
  else if (d->m_view->contentsY()==0) // check that the view has not been moved by the use
  {
    d->m_view->setContentsPos( d->m_extension->urlArgs().xOffset, d->m_extension->urlArgs().yOffset );
  }

  if ( !d->m_redirectURL.isEmpty() )
  {
    QTimer::singleShot( 1000 * d->m_delayRedirect, this, SLOT( slotRedirect() ) );
    return;
  }

  checkCompleted();
}

void KHTMLPart::checkCompleted()
{
  //kdDebug( 6050 ) << "KHTMLPart::checkCompleted() parsing: " << d->m_bParsing
  //          << " complete: " << d->m_bComplete << endl;
  int requests = 0;

  if ( d->m_bParsing || d->m_bComplete )
    return;

  QMap<QString,khtml::ChildFrame>::ConstIterator it = d->m_frames.begin();
  QMap<QString,khtml::ChildFrame>::ConstIterator end = d->m_frames.end();
  for (; it != end; ++it )
    if ( !it.data().m_bCompleted )
      return;

  requests = khtml::Cache::loader()->numRequests( m_url.url() );
  kdDebug( 6060 ) << "number of loader requests: " << requests << endl;
  if ( requests > 0 )
    return;

  d->m_bComplete = true;

  emit completed();
}

const khtml::Settings *KHTMLPart::settings() const
{
  return d->m_settings;
}

void KHTMLPart::setBaseURL( const KURL &url )
{
  d->m_baseURL = url;
}

KURL KHTMLPart::baseURL() const
{
  return d->m_baseURL;
}

void KHTMLPart::setBaseTarget( const QString &target )
{
  d->m_baseTarget = target;
}

QString KHTMLPart::baseTarget() const
{
  return d->m_baseTarget;
}

KURL KHTMLPart::completeURL( const QString &url, const QString &/*target*/ )
{
  // WABA: The following check is necassery to fix forms which don't set
  // an action URL in the believe that it default to the same URL as
  // the current page which contains the form.
  if (url.isEmpty())
  {
    return m_url;
  }

/* ###  KURL orig;
  if(url[0] == '#' && !target.isEmpty() && findFrame(target))
  {
    orig = KURL(findFrame(target)->url());
  }
  else */

  // Changed the code below so that we do not do
  // unecessary double parsing in KURL.  Also made
  // sure we do not take short-cuts when rebuilding
  // a complete URL from a base URL and a relative
  // url.  Calling setEncodedPathAndQuery is a big
  // mistake when rebuilding such URLs because the
  // query and fragment ( reference ) separators might
  // get encoded as well!! Always use the relative URL
  // constructor when re-building relative URLs. (DA)
  if( d->m_baseURL.isEmpty())
  {
    KURL u( m_url, url );
    return u;
  }
  else
  {
    KURL u ( d->m_baseURL, url );
    return u;
  }

/*
  if(url[0] != '/')
  {
    KURL u( orig, url );
    return cURL;
  }	
  orig.setEncodedPathAndQuery( url );
  return orig;
*/

}

void KHTMLPart::scheduleRedirection( int delay, const KURL &url )
{
  d->m_delayRedirect = delay;
  d->m_redirectURL = url;
}

void KHTMLPart::slotRedirect()
{
  kdDebug( 6050 ) << "KHTMLPart::slotRedirect()" << endl;

  KURL u = d->m_redirectURL;
  d->m_delayRedirect = 0;
  d->m_redirectURL = KURL();
  urlSelected( u.url() );
}

void KHTMLPart::slotRedirection(KIO::Job*, const KURL& url)
{
  // the slave told us that we got redirected
  kdDebug( 6050 ) << "redirection by KIO to " << url.url() << endl;

  emit d->m_extension->setLocationBarURL( url.prettyURL() );

  d->m_workingURL = url;
}

// ####
bool KHTMLPart::setCharset( const QString &name, bool /*override*/ )
{
  // ### hack: FIXME, use QFontDatabase!!!!!
  KCharsets *c = KGlobal::charsets();
  if(!c->isAvailable(name))
  {
    return false;
  }

  QFont f(settings()->families()[0]);
  c->setQFont(f, name);

  QFontInfo fi(f);

  d->m_settings->charset = f.charSet();
  return true;
}

bool KHTMLPart::setEncoding( const QString &name, bool /*override*/ )
{
    d->m_encoding = name;

    // ### hack!!!!
    if(!d->m_settings->charset == QFont::Unicode)
	d->m_settings->charset = KGlobal::charsets()->nameToID(name);

    // reload document
    closeURL();
    KURL url = m_url;
    m_url = 0;
    openURL(url);

    return true;
}

void KHTMLPart::setUserStyleSheet(const KURL &url)
{
    d->m_userSheetUrl = DOMString();
    d->m_userSheet = DOMString();
    new khtml::PartStyleSheetLoader(d, url.url());
}

void KHTMLPart::setUserStyleSheet(const QString &styleSheet)
{
    d->m_userSheet = styleSheet;
    d->m_userSheetUrl = DOMString();
}


bool KHTMLPart::gotoAnchor( const QString &name )
{
  HTMLCollectionImpl *anchors =
      new HTMLCollectionImpl( d->m_doc, HTMLCollectionImpl::DOC_ANCHORS);
  anchors->ref();
  NodeImpl *n = anchors->namedItem(name);
  anchors->deref();

  if(!n)
  {
    n = d->m_doc->getElementById(name);
  }
	
  if(!n) return false;

  int x = 0, y = 0;
  HTMLAnchorElementImpl *a = static_cast<HTMLAnchorElementImpl *>(n);
  a->getAnchorPosition(x, y);
  d->m_view->setContentsPos(x-50, y-50);
  return true;
}

void KHTMLPart::setFontSizes( const QValueList<int> &newFontSizes )
{
  d->m_settings->setFontSizes( newFontSizes );
}

QValueList<int> KHTMLPart::fontSizes() const
{
  return d->m_settings->fontSizes();
}

void KHTMLPart::resetFontSizes()
{
  d->m_settings->resetFontSizes();
}

void KHTMLPart::setStandardFont( const QString &name )
{
    d->m_settings->setDefaultFamily(name);
}

void KHTMLPart::setFixedFont( const QString &name )
{
    d->m_settings->setMonoSpaceFamily(name);
}

void KHTMLPart::setURLCursor( const QCursor &c )
{
  d->m_linkCursor = c;
}

const QCursor &KHTMLPart::urlCursor() const
{
  return d->m_linkCursor;
}

void KHTMLPart::findTextBegin()
{
  d->m_findPos = -1;
  d->m_findNode = 0;
}

bool KHTMLPart::findTextNext( const QRegExp &exp )
{
    if(!d->m_findNode) d->m_findNode = d->m_doc->body();

    if ( !d->m_findNode ||
	 d->m_findNode->id() == ID_FRAMESET )
      return false;

    while(1)
    {
	if(d->m_findNode->id() == ID_TEXT)
	{
	    DOMStringImpl *t = (static_cast<TextImpl *>(d->m_findNode))->string();
	    QConstString s(t->s, t->l);
	    d->m_findPos = s.string().find(exp, d->m_findPos+1);
	    if(d->m_findPos != -1)
	    {
		int x = 0, y = 0;
		d->m_findNode->renderer()->absolutePosition(x, y);
		d->m_view->setContentsPos(x-50, y-50);
		return true;
	    }
	}
	d->m_findPos = -1;
	NodeImpl *next = d->m_findNode->firstChild();
	if(!next) next = d->m_findNode->nextSibling();
	while(d->m_findNode && !next) {
	    d->m_findNode = d->m_findNode->parentNode();
	    if( d->m_findNode ) {
		next = d->m_findNode->nextSibling();
            }
	}
	d->m_findNode = next;
	if(!d->m_findNode) return false;
    }
}

bool KHTMLPart::findTextNext( const QString &str, bool forward, bool caseSensitive )
{
    if(!d->m_findNode) d->m_findNode = d->m_doc->body();

    if ( !d->m_findNode ||
	 d->m_findNode->id() == ID_FRAMESET )
      return false;

    while(1)
    {
	if(d->m_findNode->id() == ID_TEXT)
	{
	    DOMStringImpl *t = (static_cast<TextImpl *>(d->m_findNode))->string();
	    QConstString s(t->s, t->l);
	    d->m_findPos = s.string().find(str, d->m_findPos+1, caseSensitive);
	    if(d->m_findPos != -1)
	    {
		int x = 0, y = 0;
		d->m_findNode->renderer()->absolutePosition(x, y);
		d->m_view->setContentsPos(x-50, y-50);
		d->m_doc->setSelection( d->m_findNode, d->m_findPos,
					d->m_findNode, d->m_findPos+str.length() );
		return true;
	    }
	}
	d->m_findPos = -1;

	NodeImpl *next;
	
	if ( forward )
	{
     	  next = d->m_findNode->firstChild();
	
	  if(!next) next = d->m_findNode->nextSibling();
	  while(d->m_findNode && !next) {
	      d->m_findNode = d->m_findNode->parentNode();
	      if( d->m_findNode ) {
		  next = d->m_findNode->nextSibling();
              }
	  }
	}
	else
	{
     	  next = d->m_findNode->lastChild();
	
	  if (!next ) next = d->m_findNode->previousSibling();
	  while ( d->m_findNode && !next )
	  {
	    d->m_findNode = d->m_findNode->parentNode();
	    if( d->m_findNode )
	    {
	      next = d->m_findNode->previousSibling();
	    }
	  }
	}
	
	d->m_findNode = next;
	if(!d->m_findNode) return false;
    }
}

QString KHTMLPart::selectedText() const
{
  QString text;
  DOM::Node n = d->m_selectionStart;
  while(!n.isNull()) {
      if(n.nodeType() == DOM::Node::TEXT_NODE) {
        QString str = static_cast<TextImpl *>(n.handle())->data().string();
	if(n == d->m_selectionStart && n == d->m_selectionEnd)
	  text = str.mid(d->m_startOffset, d->m_endOffset - d->m_startOffset);
	else if(n == d->m_selectionStart)
	  text = str.mid(d->m_startOffset);
	else if(n == d->m_selectionEnd)
	  text += str.left(d->m_endOffset);
	else
	 text += str;
      }
      else if(n.elementId() == ID_BR)
	    text += "\n";
	else if(n.elementId() == ID_P || n.elementId() == ID_TD)
	    text += "\n\n";
	if(n == d->m_selectionEnd) break;
	DOM::Node next = n.firstChild();
	if(next.isNull()) next = n.nextSibling();
	while( next.isNull() && !n.parentNode().isNull() ) {
	    n = n.parentNode();
	    next = n.nextSibling();
	}

	n = next;
    }
    return text;
}

bool KHTMLPart::hasSelection() const
{
  return ( !d->m_selectionStart.isNull() &&
	   !d->m_selectionEnd.isNull() );
}

DOM::Range KHTMLPart::selection() const
{
    // ###
    return DOM::Range();
}


void KHTMLPart::overURL( const QString &url )
{
  emit onURL( url );

  if ( url.isEmpty() )
  {
    emit setStatusBarText( url );
    return;
  }

  KURL u( m_url, url );
  QString com;

  KMimeType::Ptr typ = KMimeType::findByURL( u );

  if ( typ )
    com = typ->comment( u, false );

  if ( u.isMalformed() )
  {
    emit setStatusBarText( u.prettyURL() );
    return;
  }

  if ( u.isLocalFile() )
  {
    // TODO : use KIO::stat() and create a KFileItem out of its result,
   // to use KFileItem::statusBarText()
    QCString path = QFile::encodeName( u.path() );
	
    struct stat buff;
    stat( path.data(), &buff );

    struct stat lbuff;
    lstat( path.data(), &lbuff );

    QString text = u.url();
    QString text2 = text;

    if (S_ISLNK( lbuff.st_mode ) )
    {
      QString tmp;
      if ( com.isNull() )
	tmp = i18n( "Symbolic Link");
      else
	tmp = i18n("%1 (Link)").arg(com);
      char buff_two[1024];
      text += " -> ";
      int n = readlink ( path.data(), buff_two, 1022);
      if (n == -1)
      {
        text2 += "  ";
        text2 += tmp;
	emit setStatusBarText( text2 );
	return;
      }
      buff_two[n] = 0;

      text += buff_two;
      text += "  ";
      text += tmp;
    }
    else if ( S_ISREG( buff.st_mode ) )
    {
      if (buff.st_size < 1024)
	text = QString("%1 (%2 %3)").arg(text2).arg((long) buff.st_size).arg(i18n("bytes"));
      else
      {
	float d = (float) buff.st_size/1024.0;
	text = QString("%1 (%2 K)").arg(text2).arg(d, 0, 'f', 2); // was %.2f
      }
      text += "  ";
      text += com;
    }
    else if ( S_ISDIR( buff.st_mode ) )
    {
      text += "  ";
      text += com;
    }
    else
    {
      text += "  ";
      text += com;
    }
    emit setStatusBarText( text );
  }
  else
    emit setStatusBarText( u.prettyURL() );

}

void KHTMLPart::urlSelected( const QString &url, int button, int state, const QString &_target )
{
  bool hasTarget = false;

   QString target = _target;
  if ( target.isEmpty() )
    target = d->m_baseTarget;
  else
      hasTarget = true;

  KURL cURL = completeURL( url, target );

  if ( button == LeftButton && ( state & ShiftButton ) && !cURL.isMalformed() )
  {
    KHTMLPopupGUIClient::saveURL( d->m_view, i18n( "Save As .." ), cURL );
    return;
  }

  if ( url.isEmpty() )
    return;

  // Security
  KURL u( url );
  if ( !u.protocol().isEmpty() && !m_url.protocol().isEmpty() &&
       ::strcasecmp( u.protocol().latin1(), "cgi" ) == 0 &&
       ::strcasecmp( m_url.protocol().latin1(), "file" ) != 0 &&
       ::strcasecmp( m_url.protocol().latin1(), "cgi" ) != 0 )
  {
    KMessageBox::error( 0,
			i18n( "This page is untrusted\nbut it contains a link to your local file system."),
			i18n( "Security Alert" ));
    return;
  }


  KParts::URLArgs args;
  args.frameName = target;

  if ( hasTarget )
  {
    // unknown frame names should open in a new window.
    khtml::ChildFrame *frame = recursiveFrameRequest( cURL, args, false );
    if ( frame )
    {
      requestObject( frame, cURL, args );
      return;
    }
  }

  if ( !d->m_bComplete && !hasTarget )
    closeURL();

  emit d->m_extension->openURLRequest( cURL, args );
}

void KHTMLPart::slotViewDocumentSource()
{
  emit d->m_extension->openURLRequest( m_url, KParts::URLArgs( false, 0, 0, QString::fromLatin1( "text/plain" ) ) );
}

void KHTMLPart::slotViewFrameSource()
{
  // ### frames
  emit d->m_extension->openURLRequest( ((KParts::ReadOnlyPart *)partManager()->activePart())->url(), KParts::URLArgs( false, 0, 0, QString::fromLatin1( "text/plain" ) ) );
}

void KHTMLPart::slotSaveBackground()
{
  QString relURL = d->m_doc->body()->getAttribute( ATTR_BACKGROUND ).string();

  KURL backgroundURL( m_url, relURL );

  KFileDialog *dlg = new KFileDialog( QString::null, "*",
					d->m_view , "filedialog", true );
  dlg->setCaption(i18n("Save background image as"));

  dlg->setSelection( backgroundURL.filename() );
  if ( dlg->exec() )
  {
    KURL destURL( dlg->selectedURL());
    if ( !destURL.isMalformed() )
    {
      /*KIO::Job *job = */KIO::file_copy( backgroundURL, destURL );
      // TODO connect job result, to display errors
    }
  }

  delete dlg;
}

void KHTMLPart::slotSaveDocument()
{
  // ### frames

  KURL srcURL( m_url );

  if ( srcURL.filename(false).isEmpty() )
    srcURL.setFileName( "index.html" );

  KFileDialog *dlg = new KFileDialog( QString::null, i18n("HTML files|* *.html *.htm"),
				      d->m_view , "filedialog", true );
  dlg->setCaption(i18n("Save as"));

  dlg->setSelection( srcURL.filename() );
  if ( dlg->exec() )
  {
     KURL destURL( dlg->selectedURL() );
      if ( !destURL.isMalformed() )
      {
        /*KIO::Job *job = */ KIO::file_copy( url(), destURL );
        // TODO connect job result, to display errors
      }
  }

  delete dlg;
}

void KHTMLPart::slotSaveFrame()
{
  // ### frames
}

void KHTMLPart::slotSetEncoding()
{
    // first Item is always auto
    if(d->m_paSetEncoding->currentItem() == 0)
	setEncoding(QString::null);
    else
	setEncoding(d->m_paSetEncoding->currentText());
}

void KHTMLPart::updateActions()
{
  bool frames = d->m_frames.count() > 0;
  d->m_paViewFrame->setEnabled( frames );
  d->m_paSaveFrame->setEnabled( frames );

  QString bgURL;

  // ### frames

  if ( d->m_doc && d->m_doc->body() && !d->m_bClearing )
    bgURL = d->m_doc->body()->getAttribute( ATTR_BACKGROUND ).string();

  d->m_paSaveBackground->setEnabled( !bgURL.isEmpty() );
}

void KHTMLPart::requestFrame( khtml::RenderPart *frame, const QString &url, const QString &frameName,
			      const QStringList &params )
{
  kdDebug( 6050 ) << "childRequest( ..., " << url << ", " << frameName << " )" << endl;
  QMap<QString,khtml::ChildFrame>::Iterator it = d->m_frames.find( frameName );

  if ( it == d->m_frames.end() )
  {
    khtml::ChildFrame child;
    kdDebug( 6050 ) << "inserting new frame into frame map" << endl;
    child.m_name = frameName;
    it = d->m_frames.insert( frameName, child );
  }

  it.data().m_frame = frame;
  it.data().m_params = params;

  requestObject( &it.data(), completeURL( url ) );
}

void KHTMLPart::requestObject( khtml::RenderPart *frame, const QString &url, const QString &serviceType,
			       const QStringList &params )
{
  khtml::ChildFrame child;
  QValueList<khtml::ChildFrame>::Iterator it = d->m_objects.append( child );
  (*it).m_frame = frame;
  (*it).m_bFrame = false;
  (*it).m_params = params;

  KParts::URLArgs args;
  args.serviceType = serviceType;
  requestObject( &(*it), completeURL( url ), args );
}

void KHTMLPart::requestObject( khtml::ChildFrame *child, const KURL &url, const KParts::URLArgs &_args )
{
  if ( child->m_bPreloaded )
  {
    if ( child->m_frame && child->m_part )
      child->m_frame->setWidget( child->m_part->widget() );

    child->m_bPreloaded = false;
    return;
  }

  KParts::URLArgs args( _args );

  if ( child->m_run )
    delete (KHTMLRun *)child->m_run;

  if ( child->m_part && !args.reload && urlcmp( child->m_part->url().url(), url.url(), true, true ) )
    args.serviceType = child->m_serviceType;

  child->m_args = args;
  child->m_serviceName = QString::null;

  if ( args.serviceType.isEmpty() )
    child->m_run = new KHTMLRun( this, child, url );
  else
    processObjectRequest( child, url, args.serviceType );
}

void KHTMLPart::processObjectRequest( khtml::ChildFrame *child, const KURL &url, const QString &mimetype )
{
  kdDebug( 6050 ) << "trying to create part for " << mimetype << endl;

  if ( !child->m_services.contains( mimetype ) )
  {
    KParts::ReadOnlyPart *part = createPart( d->m_view->viewport(), child->m_name.ascii(), this, child->m_name.ascii(), mimetype, child->m_serviceName, child->m_services, child->m_params );

    if ( !part )
      return;

    child->m_serviceType = mimetype;
    if ( child->m_frame )
      child->m_frame->setWidget( part->widget() );

    //CRITICAL STUFF
    if ( child->m_part )
    {
      partManager()->removePart( (KParts::ReadOnlyPart *)child->m_part );
      delete (KParts::ReadOnlyPart *)child->m_part;
    }

    if ( child->m_bFrame )
      partManager()->addPart( part );

    child->m_part = part;

    if ( child->m_bFrame )
    {
      connect( part, SIGNAL( started( KIO::Job *) ),
  	       this, SLOT( slotChildStarted( KIO::Job *) ) );
      connect( part, SIGNAL( completed() ),
	       this, SLOT( slotChildCompleted() ) );
    }

    child->m_extension = (KParts::BrowserExtension *)part->child( 0L, "KParts::BrowserExtension" );

    if ( child->m_extension )
    {
      connect( child->m_extension, SIGNAL( openURLNotify() ),
	       d->m_extension, SIGNAL( openURLNotify() ) );

      connect( child->m_extension, SIGNAL( openURLRequest( const KURL &, const KParts::URLArgs & ) ),
	       this, SLOT( slotChildURLRequest( const KURL &, const KParts::URLArgs & ) ) );

      connect( child->m_extension, SIGNAL( createNewWindow( const KURL &, const KParts::URLArgs & ) ),
	       d->m_extension, SIGNAL( createNewWindow( const KURL &, const KParts::URLArgs & ) ) );

      connect( child->m_extension, SIGNAL( popupMenu( const QPoint &, const KFileItemList & ) ),
	       d->m_extension, SIGNAL( popupMenu( const QPoint &, const KFileItemList & ) ) );
      connect( child->m_extension, SIGNAL( popupMenu( KXMLGUIClient *, const QPoint &, const KFileItemList & ) ),
	       d->m_extension, SIGNAL( popupMenu( KXMLGUIClient *, const QPoint &, const KFileItemList & ) ) );
      connect( child->m_extension, SIGNAL( popupMenu( const QPoint &, const KURL &, const QString &, mode_t ) ),
	       d->m_extension, SIGNAL( popupMenu( const QPoint &, const KURL &, const QString &, mode_t ) ) );
      connect( child->m_extension, SIGNAL( popupMenu( KXMLGUIClient *, const QPoint &, const KURL &, const QString &, mode_t ) ),
	       d->m_extension, SIGNAL( popupMenu( KXMLGUIClient *, const QPoint &, const KURL &, const QString &, mode_t ) ) );

    }

  }

  if ( child->m_bPreloaded )
  {
    if ( child->m_frame && child->m_part )
      child->m_frame->setWidget( child->m_part->widget() );

    child->m_bPreloaded = false;
    return;
  }

  child->m_bCompleted = false;
  if ( child->m_extension )
    child->m_extension->setURLArgs( child->m_args );

  kdDebug( 6050 ) << "opening " << url.url() << " in frame" << endl;
  child->m_part->openURL( url );
}

KParts::ReadOnlyPart *KHTMLPart::createPart( QWidget *parentWidget, const char *widgetName,
					     QObject *parent, const char *name, const QString &mimetype,
					     QString &serviceName, QStringList &serviceTypes,
					     const QStringList &params )
{
  QString constr = QString::fromLatin1( "('KParts/ReadOnlyPart' in ServiceTypes)" );

  if ( !serviceName.isEmpty() )
    constr.append( QString::fromLatin1( "and ( Name == '%1' )" ).arg( serviceName ) );

  KTrader::OfferList offers = KTrader::self()->query( mimetype, constr );

  //  assert( offers.count() >= 1 );
  if ( offers.count() == 0 )
    return 0L;

  KService::Ptr service = *offers.begin();

  KLibFactory *factory = KLibLoader::self()->factory( service->library().latin1() );

  if ( !factory )
    return 0L;

  KParts::ReadOnlyPart *res = 0L;

  const char *className = "KParts::ReadOnlyPart";
  if ( service->serviceTypes().contains( "Browser/View" ) )
    className = "Browser/View";

  if ( factory->inherits( "KParts::Factory" ) )
    res = static_cast<KParts::ReadOnlyPart *>(static_cast<KParts::Factory *>( factory )->createPart( parentWidget, widgetName, parent, name, className, params ));
  else
  res = static_cast<KParts::ReadOnlyPart *>(factory->create( parentWidget, widgetName, className ));

  if ( !res )
    return res;

  serviceTypes = service->serviceTypes();
  serviceName = service->name();

  return res;
}

KParts::PartManager *KHTMLPart::partManager()
{
  if ( !d->m_manager )
  {
    d->m_manager = new KParts::PartManager( d->m_view );
    connect( d->m_manager, SIGNAL( activePartChanged( KParts::Part * ) ),
	     this, SLOT( updateActions() ) );
  }

  return d->m_manager;
}

void KHTMLPart::submitForm( const char *action, const QString &url, const QCString &formData, const QString &_target )
{
  QString target = _target;
  if ( target.isEmpty() )
    target = d->m_baseTarget;

  KURL u = completeURL( url, target );

  if ( u.isMalformed() )
  {
    // ### ERROR HANDLING!
    return;
  }

  if ( strcmp( action, "get" ) == 0 )
  {
    QString query = u.query();
    if ( !query.isEmpty() )
      query = query + "&";

    query.append( QString::fromLatin1( formData ) ); // HTMLFormElementImpl calls formData.latin1(), so I assume that this
                                                     // conversion is ok

    u.setQuery( query );

    KParts::URLArgs args;
    args.frameName = target;
    emit d->m_extension->openURLRequest( u, args );
  }
  else
  {
    QByteArray pdata( formData.length() );
    memcpy( pdata.data(), formData.data(), formData.length() );

    KParts::URLArgs args;
    args.postData = pdata;
    args.frameName = target;

    emit d->m_extension->openURLRequest( u, args );
  }
}

void KHTMLPart::popupMenu( const QString &url )
{
  KURL u( m_url );
  if ( !url.isEmpty() )
    u = KURL( m_url, url );
  /*
  mode_t mode = 0;
  if ( !u.isLocalFile() )
  {
    QString cURL = u.url( 1 );
    int i = cURL.length();
    // A url ending with '/' is always a directory
    if ( i >= 1 && cURL[ i - 1 ] == '/' )
      mode = S_IFDIR;
  }
  */
  mode_t mode = S_IFDIR; // treat all html documents as "DIR" in order to have the back/fwd/reload
                         // buttons in the popupmenu

  KXMLGUIClient *client = new KHTMLPopupGUIClient( this, d->m_popupMenuXML, url.isEmpty() ? KURL() : u );

  emit d->m_extension->popupMenu( client, QCursor::pos(), u, QString::fromLatin1( "text/html" ), mode );

  delete client;

  emit popupMenu(url, QCursor::pos());
}

void KHTMLPart::slotChildStarted( KIO::Job *job )
{
  khtml::ChildFrame *child = frame( sender() );

  assert( child );

  child->m_bCompleted = false;

  if ( d->m_bComplete )
  {
    if ( !parentPart() ) // "toplevel" html document? if yes, then notify the hosting browser about the document (url) changes
      emit d->m_extension->openURLNotify();
    emit started( job );
  }
}

void KHTMLPart::slotChildCompleted()
{
  khtml::ChildFrame *child = frame( sender() );

  assert( child );

  child->m_bCompleted = true;
  child->m_args = KParts::URLArgs();

  checkCompleted();
}

void KHTMLPart::slotChildURLRequest( const KURL &url, const KParts::URLArgs &args )
{
  khtml::ChildFrame *child = frame( sender()->parent() );

  static QString _top = QString::fromLatin1( "_top" );
  static QString _self = QString::fromLatin1( "_self" );
  static QString _parent = QString::fromLatin1( "_parent" );
  static QString _blank = QString::fromLatin1( "_blank" );

  QString frameName = args.frameName.lower();

  if ( !frameName.isEmpty() )
  {
    if ( frameName == _top )
    {
      emit d->m_extension->openURLRequest( url, args );
      return;
    }
    else if ( frameName == _blank )
    {
      emit d->m_extension->createNewWindow( url, args );
      return;
    }
    else if ( frameName == _parent )
    {
      KParts::URLArgs newArgs( args );
      newArgs.frameName = QString::null;

      emit d->m_extension->openURLRequest( url, newArgs );
      return;
    }
    else if ( frameName != _self )
    {
      khtml::ChildFrame *_frame = recursiveFrameRequest( url, args );

      if ( !_frame )
      {
        emit d->m_extension->openURLRequest( url, args );
        return;
      }

      child = _frame;
    }
  }

  if ( child )
    requestObject( child, url, args );
  else if ( frameName == _self ) // this is for embedded objects (via <object>) which want to replace the current document
  {
    KParts::URLArgs newArgs( args );
    newArgs.frameName = QString::null;
    emit d->m_extension->openURLRequest( url, newArgs );
  }
}

khtml::ChildFrame *KHTMLPart::frame( const QObject *obj )
{
  assert( obj->inherits( "KParts::ReadOnlyPart" ) );
  const KParts::ReadOnlyPart *part = (KParts::ReadOnlyPart *)obj;

  QMap<QString,khtml::ChildFrame>::Iterator it = d->m_frames.begin();
  QMap<QString,khtml::ChildFrame>::Iterator end = d->m_frames.end();
  for (; it != end; ++it )
    if ( (KParts::ReadOnlyPart *)it.data().m_part == part )
      return &it.data();

  return 0L;
}

KHTMLPart *KHTMLPart::parentPart()
{
  if ( !parent() || !parent()->inherits( "KHTMLPart" ) )
    return 0L;

  return (KHTMLPart *)parent();
}

khtml::ChildFrame *KHTMLPart::recursiveFrameRequest( const KURL &url, const KParts::URLArgs &args,
						     bool callParent )
{
  QMap<QString,khtml::ChildFrame>::Iterator it = d->m_frames.find( args.frameName );

  if ( it != d->m_frames.end() )
    return &it.data();

  it = d->m_frames.begin();
  QMap<QString,khtml::ChildFrame>::Iterator end = d->m_frames.end();
  for (; it != end; ++it )
    if ( it.data().m_part && it.data().m_part->inherits( "KHTMLPart" ) )
    {
      KHTMLPart *childPart = (KHTMLPart *)(KParts::ReadOnlyPart *)it.data().m_part;

      khtml::ChildFrame *res = childPart->recursiveFrameRequest( url, args, false );
      if ( !res )
        continue;

      childPart->requestObject( res, url, args );
      return 0L;
    }

  if ( parentPart() && callParent )
  {
    khtml::ChildFrame *res = parentPart()->recursiveFrameRequest( url, args );

    if ( res )
      parentPart()->requestObject( res, url, args );

    return 0L;
  }

  return 0L;
}

void KHTMLPart::saveState( QDataStream &stream )
{
  kdDebug( 6050 ) << "KHTMLPart::saveState saving URL " << m_url.url() << endl;

  stream << m_url << (Q_INT32)d->m_view->contentsX() << (Q_INT32)d->m_view->contentsY();

  // Save the state of the document (Most notably the state of any forms)
  QStringList docState;
  if (d->m_doc)
  {
     docState = d->m_doc->state();
  }
  stream << docState;

  // Save font data
  stream << fontSizes() << d->m_fontBase;

  // Save frame data
  stream << (Q_UINT32)d->m_frames.count();

  QStringList frameNameLst, frameServiceTypeLst, frameServiceNameLst;
  KURL::List frameURLLst;
  QValueList<QByteArray> frameStateBufferLst;

  ConstFrameIt it = d->m_frames.begin();
  ConstFrameIt end = d->m_frames.end();
  for (; it != end; ++it )
  {
    frameNameLst << (*it).m_name;
    frameServiceTypeLst << (*it).m_serviceType;
    frameServiceNameLst << (*it).m_serviceName;
    if ( (*it).m_part )
      frameURLLst << (*it).m_part->url();
    else
      frameURLLst << KURL();

    QByteArray state;
    QDataStream frameStream( state, IO_WriteOnly );

    if ( (*it).m_part && (*it).m_extension )
      (*it).m_extension->saveState( frameStream );

    frameStateBufferLst << state;
  }

  stream << frameNameLst << frameServiceTypeLst << frameServiceNameLst << frameURLLst << frameStateBufferLst;
}

void KHTMLPart::restoreState( QDataStream &stream )
{
  KURL u;
  Q_INT32 xOffset;
  Q_INT32 yOffset;
  Q_UINT32 frameCount;
  QStringList frameNames, frameServiceTypes, docState, frameServiceNames;
  KURL::List frameURLs;
  QValueList<QByteArray> frameStateBuffers;
  QValueList<int> fSizes;

  stream >> u >> xOffset >> yOffset;
  stream >> docState;

  stream >> fSizes >> d->m_fontBase;
  setFontSizes( fSizes );

  stream >> frameCount >> frameNames >> frameServiceTypes >> frameServiceNames
	 >> frameURLs >> frameStateBuffers;

  d->m_bComplete = false;

  kdDebug( 6050 ) << "restoreState() docState.count() = " << docState.count() << endl;
  kdDebug( 6050 ) << "m_url " << m_url.url() << " <-> " << u.url() << endl;
  kdDebug( 6050 ) << "m_frames.count() " << d->m_frames.count() << " <-> " << frameCount << endl;

  if ( u == m_url && frameCount >= 1 && frameCount == d->m_frames.count() )
  {
    kdDebug( 6050 ) << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!! partial restoring !!!!!!!!!!!!!!!!!!!!!" << endl;
    emit started( 0L );

    FrameIt fIt = d->m_frames.begin();
    FrameIt fEnd = d->m_frames.end();

    QStringList::ConstIterator fNameIt = frameNames.begin();
    QStringList::ConstIterator fServiceTypeIt = frameServiceTypes.begin();
    QStringList::ConstIterator fServiceNameIt = frameServiceNames.begin();
    KURL::List::ConstIterator fURLIt = frameURLs.begin();
    QValueList<QByteArray>::ConstIterator fBufferIt = frameStateBuffers.begin();

    for (; fIt != fEnd; ++fIt, ++fNameIt, ++fServiceTypeIt, ++fServiceNameIt, ++fURLIt, ++fBufferIt )
    {
      khtml::ChildFrame *child = &(*fIt);

      kdDebug( 6050 ) <<  *fNameIt  << " ---- " <<  *fServiceTypeIt << endl;

      if ( child->m_name != *fNameIt || child->m_serviceType != *fServiceTypeIt )
      {
        child->m_bPreloaded = true;
	child->m_name = *fNameIt;
	child->m_serviceName = *fServiceNameIt;
	processObjectRequest( child, *fURLIt, *fServiceTypeIt );
      }

      if ( child->m_part )
      {
        if ( child->m_extension )
	{
	  QDataStream frameStream( *fBufferIt, IO_ReadOnly );
	  child->m_extension->restoreState( frameStream );
	}
	else
	  child->m_part->openURL( *fURLIt );
      }
    }

    //    emit completed();
  }
  else
  {
    if ( !urlcmp( u.url(), m_url.url(), true, true ) )
      clear();

    QStringList::ConstIterator fNameIt = frameNames.begin();
    QStringList::ConstIterator fNameEnd = frameNames.end();

    QStringList::ConstIterator fServiceTypeIt = frameServiceTypes.begin();
    QStringList::ConstIterator fServiceNameIt = frameServiceNames.begin();
    KURL::List::ConstIterator fURLIt = frameURLs.begin();
    QValueList<QByteArray>::ConstIterator fBufferIt = frameStateBuffers.begin();

    for (; fNameIt != fNameEnd; ++fNameIt, ++fServiceTypeIt, ++fServiceNameIt, ++fURLIt, ++fBufferIt )
    {
      khtml::ChildFrame newChild;
      newChild.m_bPreloaded = true;
      newChild.m_name = *fNameIt;
      newChild.m_serviceName = *fServiceNameIt;

      kdDebug( 6050 ) << *fNameIt << " ---- " << *fServiceTypeIt << endl;

      FrameIt childFrame = d->m_frames.insert( *fNameIt, newChild );

      processObjectRequest( &childFrame.data(), *fURLIt, *fServiceTypeIt );

      childFrame.data().m_bPreloaded = true;

      if ( childFrame.data().m_part )
      {
        if ( childFrame.data().m_extension )
	{
	  QDataStream frameStream( *fBufferIt, IO_ReadOnly );
	  childFrame.data().m_extension->restoreState( frameStream );
	}
	else
	  childFrame.data().m_part->openURL( *fURLIt );
      }
    }

    KParts::URLArgs args( d->m_extension->urlArgs() );
    args.xOffset = xOffset;
    args.yOffset = yOffset;
    args.setDocState(docState);
    d->m_extension->setURLArgs( args );
    kdDebug( 6050 ) << "in restoreState : calling openURL for " << u.url() << endl;
    openURL( u );
  }

}

void KHTMLPart::show()
{
  if ( d->m_view )
    d->m_view->show();
}

void KHTMLPart::hide()
{
  if ( d->m_view )
    d->m_view->hide();
}

DOM::Node KHTMLPart::nodeUnderMouse() const
{
    return d->m_view->nodeUnderMouse();
}

void KHTMLPart::emitSelectionChanged()
{
  emit d->m_extension->enableAction( "copy", hasSelection() );
  emit d->m_extension->selectionInfo( selectedText() );
  emit selectionChanged();
}

void KHTMLPart::slotIncFontSizes()
{
  updateFontSize( ++d->m_fontBase );
  if ( !d->m_paDecFontSizes->isEnabled() )
    d->m_paDecFontSizes->setEnabled( true );
}

void KHTMLPart::slotDecFontSizes()
{
  if ( d->m_fontBase >= 1 )
    updateFontSize( --d->m_fontBase );

  if ( d->m_fontBase == 0 )
    d->m_paDecFontSizes->setEnabled( false );
}

void KHTMLPart::updateFontSize( int add )
{
  resetFontSizes();
  QValueList<int> sizes = fontSizes();

  QValueList<int>::Iterator it = sizes.begin();
  QValueList<int>::Iterator end = sizes.end();
  for (; it != end; ++it )
    (*it) += add;

  setFontSizes( sizes );

  if(d->m_doc) d->m_doc->applyChanges();
}

void KHTMLPart::slotLoadImages()
{
  autoloadImages( false );
  autoloadImages( true );
}

void KHTMLPart::reparseConfiguration()
{
  KHTMLSettings *settings = KHTMLFactory::defaultHTMLSettings();
  settings->init();

  autoloadImages( settings->autoLoadImages() );

  d->m_bJScriptEnabled = settings->enableJavaScript();
  d->m_bJavaEnabled = settings->enableJava();
}

QStringList KHTMLPart::frameNames() const
{
  QStringList res;

  QMap<QString,khtml::ChildFrame>::ConstIterator it = d->m_frames.begin();
  QMap<QString,khtml::ChildFrame>::ConstIterator end = d->m_frames.end();
  for (; it != end; ++it )
    res += it.key();

  return res;
}

const QList<KParts::ReadOnlyPart> KHTMLPart::frames() const
{
  QList<KParts::ReadOnlyPart> res;

  QMap<QString,khtml::ChildFrame>::ConstIterator it = d->m_frames.begin();
  QMap<QString,khtml::ChildFrame>::ConstIterator end = d->m_frames.end();
  for (; it != end; ++it )
     res.append( it.data().m_part );

  return res;
}

bool KHTMLPart::openURLInFrame( const KURL &url, const KParts::URLArgs &urlArgs )
{
  QMap<QString,khtml::ChildFrame>::Iterator it = d->m_frames.find( urlArgs.frameName );

  if ( it == d->m_frames.end() )
    return false;

  requestObject( &it.data(), url, urlArgs );

  return true;
}

void KHTMLPart::setDNDEnabled( bool b )
{
  d->m_bDnd = b;
}

bool KHTMLPart::dndEnabled() const
{
  return d->m_bDnd;
}

bool KHTMLPart::event( QEvent *event )
{
  if ( KParts::ReadOnlyPart::event( event ) )
   return true;

  if ( khtml::MousePressEvent::test( event ) )
  {
    khtmlMousePressEvent( static_cast<khtml::MousePressEvent *>( event ) );
    return true;
  }

  if ( khtml::MouseDoubleClickEvent::test( event ) )
  {
    khtmlMouseDoubleClickEvent( static_cast<khtml::MouseDoubleClickEvent *>( event ) );
    return true;
  }

  if ( khtml::MouseMoveEvent::test( event ) )
  {
    khtmlMouseMoveEvent( static_cast<khtml::MouseMoveEvent *>( event ) );
    return true;
  }

  if ( khtml::MouseReleaseEvent::test( event ) )
  {
    khtmlMouseReleaseEvent( static_cast<khtml::MouseReleaseEvent *>( event ) );
    return true;
  }

  if ( khtml::DrawContentsEvent::test( event ) )
  {
    khtmlDrawContentsEvent( static_cast<khtml::DrawContentsEvent *>( event ) );
    return true;
  }

  return false;
}

void KHTMLPart::khtmlMousePressEvent( khtml::MousePressEvent *event )
{
  DOM::DOMString url = event->url();
  QMouseEvent *_mouse = event->qmouseEvent();
  DOM::Node innerNode = event->innerNode();

   d->m_dragStartPos = _mouse->pos();

  if ( event->url() != 0 )
    d->m_strSelectedURL = event->url().string();
  else
    d->m_strSelectedURL = QString::null;

  if ( _mouse->button() == LeftButton ||
       _mouse->button() == MidButton )
  {
    d->m_bMousePressed = true;

    if ( _mouse->button() == LeftButton )
    {
      if ( !innerNode.isNull() )
      {
        d->m_selectionStart = innerNode;
	d->m_startOffset = event->offset();
	d->m_selectionEnd = innerNode;
	d->m_endOffset = d->m_startOffset;
	d->m_doc->clearSelection();
      }
      else
      {
        d->m_selectionStart = DOM::Node();
	d->m_selectionEnd = DOM::Node();
      }
      emitSelectionChanged();
    }
  }

  if ( _mouse->button() == RightButton )
    popupMenu( d->m_strSelectedURL );
  else if ( _mouse->button() == MidButton && !d->m_strSelectedURL.isEmpty() )
  {
    KURL u( m_url, d->m_strSelectedURL );
    if ( !u.isMalformed() )
      emit d->m_extension->createNewWindow( u );

    d->m_strSelectedURL = QString::null;
  }
}

void KHTMLPart::khtmlMouseDoubleClickEvent( khtml::MouseDoubleClickEvent * )
{
}

void KHTMLPart::khtmlMouseMoveEvent( khtml::MouseMoveEvent *event )
{
  QMouseEvent *_mouse = event->qmouseEvent();
  DOM::DOMString url = event->url();
  DOM::Node innerNode = event->innerNode();

  if(d->m_bMousePressed && !d->m_strSelectedURL.isEmpty() &&
    ( d->m_dragStartPos - _mouse->pos() ).manhattanLength() > KGlobalSettings::dndEventDelay() &&
       d->m_bDnd )
  {
    QStringList uris;
    KURL u( completeURL( d->m_strSelectedURL) );
    uris.append( u.url() );
    QUriDrag *drag = new QUriDrag( d->m_view->viewport() );
    drag->setUnicodeUris( uris );
	
    QPixmap p = KMimeType::pixmapForURL(u, 0, KIcon::SizeMedium);
	
    if ( !p.isNull() )
      drag->setPixmap(p);
    else
      kdDebug( 6000 ) << "null pixmap" << endl;
 	
    drag->drag();

    // when we finish our drag, we need to undo our mouse press
    d->m_bMousePressed = false;
    d->m_strSelectedURL = "";
    return;
  }

  if ( !d->m_bMousePressed && url.length() )
  {
    QString surl = url.string();
    if ( d->m_overURL.isEmpty() )
    {
      d->m_view->setCursor( d->m_linkCursor );
      d->m_overURL = surl;
      overURL( d->m_overURL );
    }
    else if ( d->m_overURL != surl )
    {
      d->m_overURL = surl;
      overURL( d->m_overURL );
    }
    return;
  }
  else if( d->m_overURL.length() && !url.length() )
  {
    d->m_view->setCursor( arrowCursor );
    overURL( QString::null );
    d->m_overURL = "";
  }

  // selection stuff
  if( d->m_bMousePressed && !innerNode.isNull() && innerNode.nodeType() == DOM::Node::TEXT_NODE ) {
	d->m_selectionEnd = innerNode;
	d->m_endOffset = event->offset();
	//	kdDebug( 6000 ) << "setting end of selection to " << innerNode << "/" << offset << endl;

	// we have to get to know if end is before start or not...
	DOM::Node n = d->m_selectionStart;
	d->m_startBeforeEnd = false;
	while(!n.isNull()) {
	    if(n == d->m_selectionEnd) {
		d->m_startBeforeEnd = true;
		break;
	    }
	    DOM::Node next = n.firstChild();
	    if(next.isNull()) next = n.nextSibling();
	    while( next.isNull() && !n.parentNode().isNull() ) {
	        n = n.parentNode();
		next = n.nextSibling();
	    }
	    n = next;
	    //viewport()->repaint(false);
	}
	
	if ( !d->m_selectionStart.isNull() && !d->m_selectionEnd.isNull() )
	{
  	  if (d->m_selectionEnd == d->m_selectionStart && d->m_endOffset < d->m_startOffset)
	       d->m_doc
	    	  ->setSelection(d->m_selectionStart.handle(),d->m_endOffset,
			         d->m_selectionEnd.handle(),d->m_startOffset);
	  else if (d->m_startBeforeEnd)
	      d->m_doc
	    	  ->setSelection(d->m_selectionStart.handle(),d->m_startOffset,
			         d->m_selectionEnd.handle(),d->m_endOffset);
	  else
	      d->m_doc
	    	  ->setSelection(d->m_selectionEnd.handle(),d->m_endOffset,
			         d->m_selectionStart.handle(),d->m_startOffset);
	}
	
    }
}

void KHTMLPart::khtmlMouseReleaseEvent( khtml::MouseReleaseEvent *event )
{
  QMouseEvent *_mouse = event->qmouseEvent();
  DOM::Node innerNode = event->innerNode();

  if ( d->m_bMousePressed )
  {
    // in case we started an autoscroll in MouseMove event
    // ###
    //stopAutoScrollY();
    //disconnect( this, SLOT( slotUpdateSelectText(int) ) );
  }

  // Used to prevent mouseMoveEvent from initiating a drag before
  // the mouse is pressed again.
  d->m_bMousePressed = false;

  if ( !d->m_strSelectedURL.isEmpty() && _mouse->button() != RightButton )
  {
    KURL u(d->m_strSelectedURL);
    QString pressedTarget;
     if(u.protocol() == "target")
     {
       d->m_strSelectedURL = u.ref();
       pressedTarget = u.host();
     }	
     kdDebug( 6000 ) << "m_strSelectedURL='" << d->m_strSelectedURL << "' target=" << pressedTarget << endl;

     urlSelected( d->m_strSelectedURL, _mouse->button(), _mouse->state(), pressedTarget );
   }

  if(!innerNode.isNull() && innerNode.nodeType() == DOM::Node::TEXT_NODE) {
  //	kdDebug( 6000 ) << "final range of selection to " << d->selectionStart << "/" << d->startOffset << " --> " << innerNode << "/" << offset << endl;
	d->m_selectionEnd = innerNode;
	d->m_endOffset = event->offset();
    }

    // delete selection in case start and end position are at the same point
    if(d->m_selectionStart == d->m_selectionEnd && d->m_startOffset == d->m_endOffset) {
	d->m_selectionStart = 0;
	d->m_selectionEnd = 0;
	d->m_startOffset = 0;
	d->m_endOffset = 0;
        emitSelectionChanged();
    } else {
	// we have to get to know if end is before start or not...
	DOM::Node n = d->m_selectionStart;
	d->m_startBeforeEnd = false;
	while(!n.isNull()) {
	    if(n == d->m_selectionEnd) {
		d->m_startBeforeEnd = true;
		break;
	    }
	    DOM::Node next = n.firstChild();
	    if(next.isNull()) next = n.nextSibling();
	    while( next.isNull() && !n.parentNode().isNull() ) {
	        n = n.parentNode();
		next = n.nextSibling();
	    }	
	    n = next;
	}
	if(!d->m_startBeforeEnd)
	{
	    DOM::Node tmpNode = d->m_selectionStart;
	    int tmpOffset = d->m_startOffset;
	    d->m_selectionStart = d->m_selectionEnd;
	    d->m_startOffset = d->m_endOffset;
	    d->m_selectionEnd = tmpNode;
	    d->m_endOffset = tmpOffset;
	    d->m_startBeforeEnd = true;
	}
	// get selected text and paste to the clipboard
	QString text = selectedText();
	QClipboard *cb = QApplication::clipboard();
	cb->setText(text);
	//kdDebug( 6000 ) << "selectedText = " << text << endl;
        emitSelectionChanged();
    }
}

void KHTMLPart::khtmlDrawContentsEvent( khtml::DrawContentsEvent * )
{
}

void KHTMLPart::slotFind()
{
  KHTMLFind *findDlg = new KHTMLFind( this, "khtmlfind" );
  
  findDlg->exec();
  
  delete findDlg;
} 

KHTMLPartBrowserExtension::KHTMLPartBrowserExtension( KHTMLPart *parent, const char *name )
: KParts::BrowserExtension( parent, name )
{
  m_part = parent;
}

int KHTMLPartBrowserExtension::xOffset()
{
  return m_part->view()->contentsX();
}

int KHTMLPartBrowserExtension::yOffset()
{
  return m_part->view()->contentsY();
}

void KHTMLPartBrowserExtension::saveState( QDataStream &stream )
{
  kdDebug( 6050 ) << "saveState!" << endl;
  m_part->saveState( stream );
}

void KHTMLPartBrowserExtension::restoreState( QDataStream &stream )
{
  kdDebug( 6050 ) << "restoreState!" << endl;
  m_part->restoreState( stream );
}

void KHTMLPartBrowserExtension::copy()
{
  // get selected text and paste to the clipboard
  QString text = m_part->selectedText();
  QClipboard *cb = QApplication::clipboard();
  cb->setText(text);
}

void KHTMLPartBrowserExtension::reparseConfiguration()
{
  m_part->reparseConfiguration();
}

void KHTMLPartBrowserExtension::print()
{
  m_part->view()->print();
}

class KHTMLPopupGUIClient::KHTMLPopupGUIClientPrivate
{
public:
  KHTMLPart *m_khtml;
  KURL m_url;
  KURL m_imageURL;
  KAction *m_paSaveLinkAs;
  KAction *m_paSaveImageAs;
  KAction *m_paCopyLinkLocation;
  KAction *m_paReloadFrame;
};


KHTMLPopupGUIClient::KHTMLPopupGUIClient( KHTMLPart *khtml, const QString &doc, const KURL &url )
{
  d = new KHTMLPopupGUIClientPrivate;
  d->m_khtml = khtml;
  d->m_url = url;

  setInstance( khtml->instance() );

  // frameset? -> add "Reload Frame"
  if ( khtml->parentPart() )
  {
    d->m_paReloadFrame = new KAction( i18n( "Reload Frame" ), 0, this, SLOT( slotReloadFrame() ),
				      actionCollection(), "reloadframe" );
  }

  if ( !url.isEmpty() )
  {
    d->m_paSaveLinkAs = new KAction( i18n( "&Save Link As ..." ), 0, this, SLOT( slotSaveLinkAs() ),
 				     actionCollection(), "savelinkas" );
    d->m_paCopyLinkLocation = new KAction( i18n( "Copy Link Location" ), 0, this, SLOT( slotCopyLinkLocation() ),
					   actionCollection(), "copylinklocation" );
  }

  DOM::Element e;
  e = khtml->nodeUnderMouse();

  if ( !e.isNull() && e.elementId() == ID_IMG )
  {
    d->m_imageURL = KURL( d->m_khtml->url(), e.getAttribute( "src" ).string() );
    d->m_paSaveImageAs = new KAction( i18n( "Save Image As ..." ), 0, this, SLOT( slotSaveImageAs() ),
				      actionCollection(), "saveimageas" );
  }

  setXML( doc );
  setDOMDocument( QDomDocument(), true ); // ### HACK

  QDomElement menu = domDocument().documentElement().namedItem( "Menu" ).toElement();

  if ( actionCollection()->count() > 0 )
    menu.insertBefore( domDocument().createElement( "separator" ), menu.firstChild() );
}

KHTMLPopupGUIClient::~KHTMLPopupGUIClient()
{
  delete d;
}

void KHTMLPopupGUIClient::slotSaveLinkAs()
{
  if ( d->m_url.filename( false ).isEmpty() )
    d->m_url.setFileName( "index.html" );

  saveURL( d->m_khtml->widget(), i18n( "Save Link As" ), d->m_url );
}

void KHTMLPopupGUIClient::slotSaveImageAs()
{
  saveURL( d->m_khtml->widget(), i18n( "Save Image As" ), d->m_imageURL );
}

void KHTMLPopupGUIClient::slotCopyLinkLocation()
{
  QApplication::clipboard()->setText( d->m_url.url() );
}

void KHTMLPopupGUIClient::slotReloadFrame()
{
  KParts::URLArgs args( d->m_khtml->browserExtension()->urlArgs() );
  args.reload = true;
  // reload document
  d->m_khtml->closeURL();
  d->m_khtml->browserExtension()->setURLArgs( args );
  d->m_khtml->openURL( d->m_khtml->url() );
}

void KHTMLPopupGUIClient::saveURL( QWidget *parent, const QString &caption, const KURL &url )
{
  KFileDialog *dlg = new KFileDialog( QString::null, QString::null, parent, "filedia", true );

  dlg->setCaption( caption );

  dlg->setSelection( url.filename() );

  if ( dlg->exec() )
  {
    KURL destURL( dlg->selectedURL() );
    if ( !destURL.isMalformed() )
    {
      /*KIO::Job *job = */ KIO::copy( url, destURL );
      // TODO connect job result, to display errors
    }
  }

  delete dlg;
}

KHTMLPartBrowserHostExtension::KHTMLPartBrowserHostExtension( KHTMLPart *part )
: KParts::BrowserHostExtension( part )
{
  m_part = part;
}

KHTMLPartBrowserHostExtension::~KHTMLPartBrowserHostExtension()
{
}

QStringList KHTMLPartBrowserHostExtension::frameNames() const
{
  return m_part->frameNames();
}

const QList<KParts::ReadOnlyPart> KHTMLPartBrowserHostExtension::frames() const
{
  return m_part->frames();
}

bool KHTMLPartBrowserHostExtension::openURLInFrame( const KURL &url, const KParts::URLArgs &urlArgs )
{
  return m_part->openURLInFrame( url, urlArgs );
}

#include "khtml_part.moc"
