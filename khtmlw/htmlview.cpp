//----------------------------------------------------------------------------
//
// KDE HTML Widget
//
// Copyright (c) 1997 The KDE Project
//

#include "htmlview.h"

QList<KHTMLView> *KHTMLView::viewList = NULL;

KHTMLView::KHTMLView( QWidget *_parent, const char *_name, int _flags, KHTMLView *_parent_view ) 
    : QWidget( _parent, _name, _flags )
{    
    parentView = _parent_view;
    
    // Allow resizing of frame
    bAllowResize = TRUE;
    // Set scrolling to auto
    scrolling = -1;
    frameBorder = -1;
    
    // debugT("Constructed KHTML View\n");
    
    if ( viewList == NULL )
	viewList = new QList<KHTMLView>;
    viewList->setAutoDelete( FALSE );
    viewList->append( this );
    
    frameName = _name;
    
    displayVScroll = FALSE;
    displayHScroll = FALSE;

    initGUI();
}

KHTMLView::~KHTMLView()
{
    viewList->removeRef( this );
    
    // debugT("Deleted View\n");
}

KHTMLView* KHTMLView::newView( QWidget *_parent, const char *_name, int _flags )
{
    return new KHTMLView( _parent, _name, _flags, this );

    /*
    connect( view, SIGNAL( documentStarted( KHTMLView * ) ),
	     this, SLOT( slotDocumentStarted( KHTMLView * ) ) );
    connect( view, SIGNAL( documentDone( KHTMLView * ) ),
	     this, SLOT( slotDocumentDone( KHTMLView * ) ) );
    connect( view, SIGNAL( imageRequest( KHTMLView *, const char * ) ),
	     this, SLOT( slotImageRequest( KHTMLView *, const char * ) ) );
    connect( view, SIGNAL( URLSelected( KHTMLView *, const char*, int, const char* ) ),
	     this, SLOT( slotURLSelected( KHTMLView *, const char *, int, const char* ) ) );    
    connect( view, SIGNAL( onURL( KHTMLView *, const char* ) ),
	     this, SLOT( slotOnURL( KHTMLView *, const char * ) ) );
    connect( view, SIGNAL( popupMenu( KHTMLView *, const char*, const QPoint & ) ),
	     this, SLOT( slotPopupMenu( KHTMLView *, const char *, const QPoint & ) ) );
    connect( view, SIGNAL( cancelImageRequest( KHTMLView *, const char* ) ),
	     this, SLOT( slotCancelImageRequest( KHTMLView *, const char * ) ) );
    connect( view, SIGNAL( formSubmitted( KHTMLView *, const char *, const char* ) ),
	     this, SLOT( slotFormSubmitted( KHTMLView *, const char *, const char* ) ) ); */
}

KHTMLView* KHTMLView::findView( const char *_name )
{
    KHTMLView *v;
    for ( v = viewList->first(); v != 0L; v = viewList->next() )
    {
	if ( v->getFrameName() )
	{
	    // debugT("Comparing '%s' '%s'\n", _name, v->getFrameName() );
	    if ( strcmp( v->getFrameName(), _name ) == 0 )
		return v;
	}
    }
    
    return 0L;
}

void KHTMLView::begin( const char *_url, int _dx, int _dy )
{
    if ( displayHScroll )
	view->setGeometry( 0, 0, width(), height() );
    displayVScroll = FALSE;
    displayHScroll = FALSE;
    vert->hide();
    horz->hide();

    if ( _url )
	url = _url;
    
    view->begin( _url, _dx, _dy );
}

void KHTMLView::write( const char *_text )
{
    view->write( _text );
}

void KHTMLView::end()
{
    view->end();
}

void KHTMLView::parse()
{
    view->parse();
}

void KHTMLView::print()
{
    view->print();
}

void KHTMLView::initGUI()
{
    horz = new QScrollBar( 0, 0, 12, width(), 0, QScrollBar::Horizontal,
	    this, "horz" );
    horz->hide();
    vert = new QScrollBar( 0, 0, 12, height(), 0, QScrollBar::Vertical,
	    this, "vert" );
    vert->hide();
    
    horz->setMinimumSize( 16, 16 );
    vert->setMinimumSize( 16, 16 );

    view = new KHTMLWidget( this, "" );
    CHECK_PTR( view );
    view->setView( this );
    
    connect( view, SIGNAL( scrollVert( int ) ), SLOT( slotScrollVert( int ) ) );
    connect( view, SIGNAL( scrollHorz( int ) ), SLOT( slotScrollHorz( int ) ) );

    connect( vert, SIGNAL(valueChanged(int)), view, SLOT(slotScrollVert(int)) );
    connect( horz, SIGNAL(valueChanged(int)), view, SLOT(slotScrollHorz(int)) );

    connect( view, SIGNAL( documentChanged() ), SLOT( slotDocumentChanged() ) );
    connect( view, SIGNAL( setTitle( const char* ) ),
	     this, SLOT( slotSetTitle( const char * ) ) );
    connect( view, SIGNAL( URLSelected( const char*, int, const char* ) ),
	     this, SLOT( slotURLSelected( const char *, int, const char* ) ) );    
    connect( view, SIGNAL( onURL( const char* ) ),
	     this, SLOT( slotOnURL( const char * ) ) );
    connect( view, SIGNAL( textSelected( bool ) ),
	     this, SLOT( slotTextSelected( bool ) ) );
    connect( view, SIGNAL( popupMenu( const char*, const QPoint & ) ),
	     this, SLOT( slotPopupMenu( const char *, const QPoint & ) ) );
    connect( view, SIGNAL( fileRequest( const char* ) ),
	     this, SLOT( slotImageRequest( const char * ) ) );
    connect( view, SIGNAL( cancelFileRequest( const char* ) ),
	     this, SLOT( slotCancelImageRequest( const char * ) ) );
    connect( view, SIGNAL( formSubmitted( const char *, const char* ) ),
	     this, SLOT( slotFormSubmitted( const char *, const char* ) ) );
    connect( view, SIGNAL( documentStarted() ),
	     this, SLOT( slotDocumentStarted() ) );
    connect( view, SIGNAL( documentDone() ),
	     this, SLOT( slotDocumentDone() ) );
    
    view->setURLCursor( upArrowCursor );
    view->raise();
}

void KHTMLView::resizeEvent( QResizeEvent * )
{
    // resize the view - it will handle object size/pos calc
    view->setGeometry( 0, 0, width(), height() );

    if ( url.isEmpty() )
	return;

    // place/hide scrollbars
    calcScrollBars();

    // if we need a horzontal scrollbar then resize view again.
    // This does not cause object size/pos to be recalculated as
    // the width is not changed.
    if ( displayHScroll )
	view->setGeometry( 0, 0, width(), height() - 16 );

    vert->setSteps( 12, view->height() );
    horz->setSteps( 12, view->width() );
}
    
void KHTMLView::closeEvent( QCloseEvent *e )
{
    // debugT("Closing\n");
    e->accept();

    delete this;
}
    
void KHTMLView::slotScrollVert( int _y )
{
    vert->setValue( _y );
}

void KHTMLView::slotScrollHorz( int _x )
{
    horz->setValue( _x );
}

void KHTMLView::slotDocumentChanged()
{    
    if ( url.isNull() )
	return;

    bool oldh = displayHScroll;

    calcScrollBars();    

    if ( displayHScroll && !oldh )
	view->setGeometry( 0, 0, width(), height() - 16 );
    else if ( !displayHScroll && oldh )
	view->setGeometry( 0, 0, width(), height() );
}

void KHTMLView::calcScrollBars()
{
    if ( view->docWidth() > view->width() && !isFrameSet() )
	displayHScroll = TRUE;
    else
        displayHScroll = FALSE;
    
    if ( view->docHeight() > view->height() && !isFrameSet() )
	displayVScroll = TRUE;
    else
        displayVScroll = FALSE;

    if ( displayVScroll && displayHScroll )
    {
	horz->setRange( 0, view->docWidth() + 16 - view->width() );
	vert->setRange( 0, view->docHeight() - height() + 16 );
    }
    else if ( displayHScroll )
    {
	horz->setRange( 0, view->docWidth() - view->width() );
    }
    else if ( displayVScroll )
    {
	vert->setRange( 0, view->docHeight() - height() );
    }    

    int right = 0;
    if ( displayVScroll )
	right = 16;
  
    int bottom = 0;
    if ( !displayHScroll )
	horz->hide();
    else
    {
	bottom = 16;
	// debugT("Showing HScrollBar\n");
	horz->setGeometry( 0, height() - 16, width() - right, 16 );
	horz->show();
	horz->raise();
    }

    if ( !displayVScroll )
    {
	vert->hide();
	view->slotScrollVert( 0 );
    }
    else
    {
	// debugT("Showing VScrollBar\n");
	vert->setGeometry( width() - 16, 0, 16, height() - bottom );
	vert->show();
	vert->raise();
    }
}

void KHTMLView::slotDocumentStarted( KHTMLView *_view )
{
    emit documentStarted( _view );
}

void KHTMLView::slotDocumentStarted()
{
    emit documentStarted( this );
}

void KHTMLView::slotDocumentDone( KHTMLView *_view )
{
    emit documentDone( _view );
}

void KHTMLView::slotDocumentDone()
{
    emit documentDone( this );
}

void KHTMLView::slotDocumentRequest( KHTMLView *_view, const char *_url )
{
    emit documentRequest( _view, _url );
}

void KHTMLView::slotCancelDocumentRequest( KHTMLView *_view, const char *_url )
{
    emit cancelDocumentRequest( _view, _url );
}

void KHTMLView::slotSetTitle( const char *_text )
{
    emit setTitle( _text );
}

void KHTMLView::slotURLSelected( KHTMLView *_view, const char *_url,
    int _button, const char *_target )
{
    // debugT("URL selected '%s'\n",_url );
    emit URLSelected( _view, _url, _button, _target );
}

void KHTMLView::slotURLSelected( const char *_url, int _button,
    const char *_target )
{
    // debugT("URL 2 selected '%s'\n",_url );
    emit URLSelected( this, _url, _button, _target );
}

void KHTMLView::slotOnURL( KHTMLView *_view, const char *_url )
{
    // debugT("On URL '%s'\n",_url );
    emit onURL( _view, _url );
}

void KHTMLView::slotOnURL( const char *_url )
{
    // debugT("On URL 2 '%s'\n",_url);
    emit onURL( this, _url );
}

void KHTMLView::slotTextSelected( bool _selected )
{
    emit textSelected( this, _selected );
}

void KHTMLView::slotPopupMenu( KHTMLView *_view, const char *_url,
    const QPoint &_point )
{
    emit popupMenu( _view, _url, _point );
}

void KHTMLView::slotPopupMenu( const char *_url, const QPoint &_point )
{
    emit popupMenu( this, _url, _point );
}

void KHTMLView::slotImageRequest( KHTMLView *_view, const char *_url )
{
    emit imageRequest( _view, _url );
}

void KHTMLView::slotImageRequest( const char *_url )
{
    emit imageRequest( this, _url );
    emit imageRequest( _url );
}

void KHTMLView::slotCancelImageRequest( KHTMLView *_view, const char *_url )
{
    emit cancelImageRequest( _view, _url );
}

void KHTMLView::slotCancelImageRequest( const char *_url )
{
    emit cancelImageRequest( this, _url );
    emit cancelImageRequest( _url );
}

void KHTMLView::slotImageLoaded( const char *_url, const char *_filename )
{
    if ( view )
	view->slotFileLoaded( _url, _filename );
}

void KHTMLView::slotFormSubmitted( KHTMLView *_view, const char *_method, const char *_url )
{
    emit formSubmitted( _view, _url, _method );
}

void KHTMLView::slotFormSubmitted( const char *_method, const char *_url )
{
    // debugT("Form submitted '%s'\n",_url);
    
    emit formSubmitted( this, _url, _method );
}

int KHTMLView::xOffset()
{
    return view->xOffset();
}

int KHTMLView::yOffset()
{
    return view->xOffset();
}

const char* KHTMLView::getURL( QPoint & p )
{
    return view->getURL( p );
}

void KHTMLView::select( QRegExp& _pattern, bool _select )
{
    view->select( 0L, _pattern, _select );
}

void KHTMLView::openURL( const char *_url )
{
    emit documentRequest( this, _url );
}

bool KHTMLView::isFrameSet()
{
    return view->isFrameSet();
}

void KHTMLView::setIsFrameSet( bool _frameset )
{
    /*  if ( _frameset )
    {
	displayVScroll = FALSE;
	displayHScroll = FALSE;
    } */

    view->setIsFrameSet( _frameset ); 
}

bool KHTMLView::isFrame()
{
    return view->isFrame();
}

void KHTMLView::setIsFrame( bool _frame )
{
    // debugT(">>>>>>>>>>>>>>>> I am a frame %x <<<<<<<<<<<<<<<<<<\n",(int)this);
  
    /*  if ( _frame )
    {
	displayVScroll = FALSE;
	displayHScroll = FALSE;
    } */

    view->setIsFrame( _frame ); 
}

void KHTMLView::setSelected( bool _selected )
{
    view->setSelected( _selected ); 
    if ( _selected )
	emit frameSelected( this );
}

bool KHTMLView::isSelected()
{
    return view->isSelected();
}

KHTMLView* KHTMLView::getSelectedView()
{
    // debugT(">>>>>>>>>>>>>>>>>>>>>>>>>> HTMLView 1 <<<<<<<<<<<<<<<<<<<<<<<<\n");
    if ( isFrame() && isSelected() )
	return this;

    // debugT(">>>>>>>>>>>>>>>>>>>>>>>>>> HTMLView 2 <<<<<<<<<<<<<<<<<<<<<<<<\n");
    if ( isFrameSet() )
	return view->getSelectedFrame();

    // debugT(">>>>>>>>>>>>>>>>>>>>>>>>>> HTMLView 3 <<<<<<<<<<<<<<<<<<<<<<<<\n");
    return 0L;
}

void KHTMLView::slotVertSubtractLine()
{
    if ( vert->isVisible() )
	vert->subtractLine ();
}

void KHTMLView::slotVertAddLine()
{
    if ( vert->isVisible() )
	vert->addLine ();
}

void KHTMLView::slotVertSubtractPage()
{
    if ( vert->isVisible() )
	vert->subtractPage ();
}

void KHTMLView::slotVertAddPage()
{
    if ( vert->isVisible() )
	vert->addPage ();
}                       

bool KHTMLView::mouseMoveHook( QMouseEvent * )
{
    return FALSE;
}

bool KHTMLView::mouseReleaseHook( QMouseEvent * )
{
    return FALSE;
}

bool KHTMLView::mousePressedHook( const char*, const char*, QMouseEvent*, bool )
{
    return FALSE;
}

bool KHTMLView::dndHook( const char *, QPoint & )
{
    return FALSE;
}

void KHTMLView::select( QPainter * _painter, QRect &_rect )
{
    view->select( _painter, _rect );
}

void KHTMLView::select( QPainter * _painter, bool _select )
{
    view->select( _painter, _select );
}

void KHTMLView::selectByURL( QPainter *_painter,const char *_url,bool _select )
{
    view->selectByURL( _painter, _url, _select );
}

void KHTMLView::getSelected( QStrList &_list )
{
    view->getSelected( _list );
}

void KHTMLView::getSelectedText( QString &_str )
{
    view->getSelectedText( _str );
}

bool KHTMLView::isTextSelected() const
{
    return view->isTextSelected();
}

void KHTMLView::setMarginWidth( int _w )
{
    view->setMarginWidth( _w );
}

void KHTMLView::setMarginHeight( int _h )
{
    view->setMarginHeight( _h );
}

bool KHTMLView::gotoAnchor(const char* anchor)
{
    return view->gotoAnchor(anchor);
}

#include "htmlview.moc"

