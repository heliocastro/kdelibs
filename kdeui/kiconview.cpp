#include <qtimer.h>

#include "kiconview.h"
#include <kglobalsettings.h>
#include <kapp.h>
#include <kcursor.h>

#include <X11/Xlib.h>

KIconView::KIconView( QWidget *parent, const char *name, WFlags f )
    : QIconView( parent, name, f )
{
    oldCursor = viewport()->cursor();
    connect( this, SIGNAL( onViewport() ),
	     this, SLOT( slotOnViewport() ) );
    connect( this, SIGNAL( onItem( QIconViewItem * ) ),
             this, SLOT( slotOnItem( QIconViewItem * ) ) );
    slotSettingsChanged();
    connect( kapp, SIGNAL( settingsChanged() ), SLOT( slotSettingsChanged() ) );
    m_pCurrentItem = 0L;

    m_pAutoSelect = new QTimer( this );
    connect( m_pAutoSelect, SIGNAL( timeout() ),
    	     this, SLOT( slotAutoSelect() ) );

    connect( this, SIGNAL( selectionChanged() ),
	     this, SLOT( selch() ) );
}

void KIconView::slotSettingsChanged()
{
    m_bUseSingle = KGlobalSettings::singleClick();
    if( m_bUseSingle )
    {
        connect( this, SIGNAL( clicked( QIconViewItem * ) ),
                 this, SLOT( slotExecute( QIconViewItem * ) ) );
        disconnect( this, SIGNAL( doubleClicked( QIconViewItem * ) ),
                    this, SLOT( slotExecute( QIconViewItem * ) ) );
    }
    else
    {
        connect( this, SIGNAL( doubleClicked( QIconViewItem * ) ),
                 this, SLOT( slotExecute( QIconViewItem * ) ) );
        disconnect( this, SIGNAL( clicked( QIconViewItem * ) ),
                    this, SLOT( slotExecute( QIconViewItem * ) ) );
    }

    m_bChangeCursorOverItem = KGlobalSettings::changeCursorOverIcon();
    m_autoSelectDelay = KGlobalSettings::autoSelectDelay();

    if( !m_bUseSingle || !m_bChangeCursorOverItem )
        viewport()->setCursor( oldCursor );
}

void KIconView::slotOnItem( QIconViewItem *item )
{
    if ( item && m_bChangeCursorOverItem && m_bUseSingle )
        viewport()->setCursor( KCursor().handCursor() );

    if ( item && (m_autoSelectDelay > -1) && m_bUseSingle ) {
      m_pAutoSelect->start( m_autoSelectDelay, true );
      m_pCurrentItem = item;
    }
}

void KIconView::slotOnViewport()
{
    if ( m_bChangeCursorOverItem )
        viewport()->setCursor( oldCursor );

    m_pAutoSelect->stop();
    m_pCurrentItem = 0L;
}

void KIconView::slotAutoSelect()
{
  //Give this widget the keyboard focus.
  if( !hasFocus() )
    setFocus();

  Window root;
  Window child;
  int root_x, root_y, win_x, win_y;
  uint keybstate;
  XQueryPointer( qt_xdisplay(), qt_xrootwin(), &root, &child,
		 &root_x, &root_y, &win_x, &win_y, &keybstate );

  QIconViewItem* previousItem = currentItem();
  setCurrentItem( m_pCurrentItem );

  if( m_pCurrentItem ) {
    //Shift pressed?
    if( (keybstate & ShiftMask) ) {
      //Temporary implementaion of the selection until QIconView supports it
      bool block = signalsBlocked();
      blockSignals( true );

      //No Ctrl? Then clear before!
      if( !(keybstate & ControlMask) )  
	clearSelection(); 

      bool select = !m_pCurrentItem->isSelected();
      bool update = viewport()->isUpdatesEnabled();
      viewport()->setUpdatesEnabled( false );

      //Calculate the smallest rectangle that contains the current Item
      //and the one that got the autoselect event
      QRect r;
      QRect redraw;
      if ( previousItem )
	r = QRect( QMIN( previousItem->x(), m_pCurrentItem->x() ),
		   QMIN( previousItem->y(), m_pCurrentItem->y() ),
		   0, 0 );
      else
	r = QRect( 0, 0, 0, 0 );
      if ( previousItem->x() < m_pCurrentItem->x() )
	r.setWidth( m_pCurrentItem->x() - previousItem->x() + m_pCurrentItem->width() );
      else
	r.setWidth( previousItem->x() - m_pCurrentItem->x() + previousItem->width() );
      if ( previousItem->y() < m_pCurrentItem->y() )
	r.setHeight( m_pCurrentItem->y() - previousItem->y() + m_pCurrentItem->height() );
      else
	r.setHeight( previousItem->y() - m_pCurrentItem->y() + previousItem->height() );
      r = r.normalize();

      //Check for each item whether it is within the rectangle.
      //If yes, select it
      for( QIconViewItem* i = firstItem(); i; i = i->nextItem() ) {
	if( i->intersects( r ) ) {
	  redraw = redraw.unite( i->rect() );
	  setSelected( i, select, true );
	}
      }

      blockSignals( block );
      viewport()->setUpdatesEnabled( update );
      repaintContents( redraw, false );

      emit selectionChanged();

      if( selectionMode() == QIconView::Single )
	emit selectionChanged( m_pCurrentItem );

      //setSelected( m_pCurrentItem, true, (keybstate & ControlMask), (keybstate & ShiftMask) );
    }
    else if( (keybstate & ControlMask) )
      setSelected( m_pCurrentItem, !m_pCurrentItem->isSelected(), true );
    else
      setSelected( m_pCurrentItem, true );
  }
  else
    kdDebug() << "That�s not supposed to happen!!!!" << endl;
}

void KIconView::slotExecute( QIconViewItem *item )
{
  Window root;
  Window child;
  int root_x, root_y, win_x, win_y;
  uint keybstate;
  XQueryPointer( qt_xdisplay(), qt_xrootwin(), &root, &child,
		 &root_x, &root_y, &win_x, &win_y, &keybstate );

  m_pAutoSelect->stop();

  //Don�t emit executed if in SC mode and Shift or Ctrl are pressed
  if( !( m_bUseSingle && ((keybstate & ShiftMask) || (keybstate & ControlMask)) ) )
    emit executed( item );
}

void KIconView::focusOutEvent( QFocusEvent *fe )
{
  m_pAutoSelect->stop();

  QIconView::focusOutEvent( fe );
}
