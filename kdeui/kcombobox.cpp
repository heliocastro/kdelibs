/* This file is part of the KDE libraries

   Copyright (c) 2000 Dawit Alemayehu <adawit@kde.org>
                 2000,2001 Carsten Pfeiffer <pfeiffer@kde.org>
                 2000 Stefan Schimanski <1Stein@gmx.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License (LGPL) as published by the Free Software Foundation; either
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

#include <qclipboard.h>
#include <qlistbox.h>
#include <qpopupmenu.h>

#include <kcursor.h>
#include <klocale.h>
#include <kpixmapprovider.h>
#include <kstdaccel.h>
#include <kicontheme.h>
#include <kdebug.h>
#include <kcompletionbox.h>
#include <kurl.h>
#include <kurldrag.h>
#include <knotifyclient.h>
#include <kiconloader.h>

#include "kcombobox.h"
#include "kcombobox.moc"


class KComboBox::KComboBoxPrivate
{
public:
    KComboBoxPrivate() {
      hasReference= false;
      handleURLDrops= true;
      completionBox= 0;
      popupMenu= 0;
      subMenu= 0;
    }
    bool handleURLDrops;
    bool hasReference;
    KCompletionBox *completionBox;
    QPopupMenu* popupMenu;
    QPopupMenu* subMenu;
    QString origText;
};

KComboBox::KComboBox( QWidget *parent, const char *name )
          :QComboBox( parent, name )
{
    m_trapReturnKey = false;
    m_pEdit = lineEdit();
    init();
}

KComboBox::KComboBox( bool rw, QWidget *parent, const char *name )
          :QComboBox( rw, parent, name )
{
    m_trapReturnKey = false;
    m_pEdit = lineEdit();

    if ( rw ) {
        KCursor::setAutoHideCursor( lineEdit(), true, true );
        m_pEdit->installEventFilter( this );
    }
    init();
}

KComboBox::~KComboBox()
{
    delete d->popupMenu;
    delete d->completionBox;
    delete d;
}

void KComboBox::init()
{
    d = new KComboBoxPrivate;

    // Permanently set some parameters in the parent object.
    QComboBox::setAutoCompletion( false );

    // Initialize enable popup menu to false.
    // Below it will be enabled if the widget
    // is editable.
    m_bEnableMenu = false;

    // Enable context menu by default if widget
    // is editable.
    setContextMenuEnabled( true );
}


bool KComboBox::contains( const QString& _text ) const
{
    if ( _text.isEmpty() )
        return false;

    for (int i = 0; i < count(); i++ ) {
        if ( text(i) == _text )
            return true;
    }
    return false;
}

void KComboBox::setAutoCompletion( bool autocomplete )
{
    setCompletionMode( autocomplete ? KGlobalSettings::CompletionAuto:
                                      KGlobalSettings::completionMode() );
}

void KComboBox::setCompletionMode( KGlobalSettings::Completion mode )
{
    KGlobalSettings::Completion oldMode = completionMode();
    if ( oldMode != mode && oldMode == KGlobalSettings::CompletionPopup &&
         d->completionBox )
        d->completionBox->hide();

    KCompletionBase::setCompletionMode( mode );
}

void KComboBox::setContextMenuEnabled( bool showMenu )
{
    if( m_pEdit )
        m_bEnableMenu = showMenu;
}


void KComboBox::setURLDropsEnabled(bool enable)
{
    d->handleURLDrops=enable;
}

bool KComboBox::isURLDropsEnabled() const
{
    return d->handleURLDrops;
}


void KComboBox::setCompletedText( const QString& text, bool marked )
{
    if ( m_pEdit && marked )
        m_pEdit->validateAndSet( text, currentText().length(),
                                 currentText().length(), text.length() );
    else
    {
        if ( text != currentText() ) // no need to flicker
            setEditText( text );
    }
}

void KComboBox::setCompletedText( const QString& text )
{
    KGlobalSettings::Completion mode = completionMode();
    bool marked = ( mode == KGlobalSettings::CompletionAuto ||
                    mode == KGlobalSettings::CompletionMan ||
                    mode == KGlobalSettings::CompletionPopup );
    setCompletedText( text, marked );
}

void KComboBox::makeCompletion( const QString& text )
{
    if( m_pEdit )
    {
        KCompletion *comp = compObj();
        if( !comp )
            return; // No Completion object or empty completion text allowed!

        KGlobalSettings::Completion mode = completionMode();
        bool compPopup = (mode == KGlobalSettings::CompletionPopup);
        if ( compPopup && !d->completionBox )
            makeCompletionBox();

        QString match = comp->makeCompletion( text );
        if ( compPopup )
        {
            if ( match.isNull() )
            {
                d->completionBox->hide();
                d->completionBox->clear();
            }
            else
                setCompletedItems( comp->allMatches() );
        }
        else
        {
            // all other completion modes
            // If no match or the same text, simply return without completing.
            if( match.isNull() || match == text )
                return;
            bool marked = ( mode == KGlobalSettings::CompletionAuto ||
                            mode == KGlobalSettings::CompletionMan );

            setCompletedText( match, marked );
        }
    }
    else
    {
        if( text.isNull() || !listBox() )
            return;

        int index = listBox()->index( listBox()->findItem( text ) );
        if( index >= 0 ) {
            setCurrentItem( index );
        }
    }
}

void KComboBox::rotateText( KCompletionBase::KeyBindingType type )
{
    if( m_pEdit )
    {
        // Support for rotating through highlighted text!  This means
        // that if there are multiple matches for the portion of text
        // entered into the combo box,  one can simply rotate through
        // all the other possible matches using the previous/next match
        // keys.  BTW this feature only works if there is a completion
        // object available to begin with.
        KCompletion* comp = compObj();
        if( comp &&
            ( type == KCompletionBase::PrevCompletionMatch ||
              type == KCompletionBase::NextCompletionMatch ) )
        {
            QString input = ( type == KCompletionBase::PrevCompletionMatch ) ? comp->previousMatch() : comp->nextMatch();
           // Ignore rotating to the same text
           if( input.isNull() || input == currentText() )
               return;

            bool marked = m_pEdit->hasMarkedText(); // See if it had marked text
            setCompletedText( input, marked );
        }
    }
}

void KComboBox::keyPressEvent ( QKeyEvent * e )
{
    if ( m_pEdit && m_pEdit->hasFocus() )
    {
        // ### this will most probably need fixing when Qt3 is used. QCombBox
        // in Qt2 is buggy, so here are some hacks that might break with Qt3.
        
        KGlobalSettings::Completion mode = completionMode();
        KeyBindingMap keys = getKeyBindings();

        if ( mode == KGlobalSettings::CompletionAuto ||
             mode == KGlobalSettings::CompletionMan )
        {
            QString keycode = e->text();
            if ( !keycode.isNull() && keycode.unicode()->isPrint() )
            {
                QComboBox::keyPressEvent ( e );
                QString txt = currentText();
                int len = txt.length();
                if (!m_pEdit->hasMarkedText() && len && cursorPosition() ==len)
                {
                    if ( emitSignals() )
                        emit completion( txt ); // emit when requested...
                    if ( handleSignals() )
                        makeCompletion( txt );  // handle when requested...
                }
                return;
            }
        }

        else if ( mode == KGlobalSettings::CompletionPopup )
        {
            QComboBox::keyPressEvent( e );
            QString txt = currentText();
            int key = e->key();
            if ( !e->text().isNull() && key != Key_Escape && 
                 key != Key_Return && key != Key_Enter )
            {
                if ( emitSignals() )
                    emit completion( txt ); // emit when requested...
                if ( handleSignals() )
                    makeCompletion( txt );  // handle when requested...
            }
            return;
        }

        else if ( mode == KGlobalSettings::CompletionShell )
        {
            int key = ( keys[TextCompletion] == 0 ) ? KStdAccel::key(KStdAccel::TextCompletion) : keys[TextCompletion];
            if ( KStdAccel::isEqual( e, key ) )
            {
                // Emit completion if there is a completion object present,
                // the current text is not the same as the previous and the
                // cursor is at the end of the string.
                QString txt = currentText();
                if ( m_pEdit->cursorPosition() == (int) txt.length() )
                {
                    if ( emitSignals() )
                        emit completion( txt ); // emit when requested...
                    if ( handleSignals() )
                        makeCompletion( txt );  // handle when requested...
                    return;
               }
            }
            else if ( d->completionBox )
                d->completionBox->hide();
        }

        // handle rotation
        if ( mode != KGlobalSettings::CompletionNone )
        {
            // Handles previousMatch.
            int key = ( keys[PrevCompletionMatch] == 0 ) ? KStdAccel::key(KStdAccel::PrevCompletion) : keys[PrevCompletionMatch];
            if ( KStdAccel::isEqual( e, key ) )
            {
                if ( emitSignals() )
                    emit textRotation( KCompletionBase::PrevCompletionMatch );
                if ( handleSignals() )
                    rotateText( KCompletionBase::PrevCompletionMatch );
                return;
            }

            // Handles nextMatch.
            key = ( keys[NextCompletionMatch] == 0 ) ? KStdAccel::key(KStdAccel::NextCompletion) : keys[NextCompletionMatch];
            if ( KStdAccel::isEqual( e, key ) )
            {
                if ( emitSignals() )
                    emit textRotation( KCompletionBase::NextCompletionMatch );
                if ( handleSignals() )
                    rotateText( KCompletionBase::NextCompletionMatch );
                return;
            }
        }
    }

    // read-only combobox
    else if ( !m_pEdit )
    {
        QString keycode = e->text();
        if ( !keycode.isNull() && keycode.unicode()->isPrint() )
        {
            emit completion ( keycode );
            if ( handleSignals() )
                makeCompletion( keycode );
            return;
        }
    }
    QComboBox::keyPressEvent( e );
}


bool KComboBox::eventFilter( QObject* o, QEvent* ev )
{
    if ( o == m_pEdit )
    {
        KCursor::autoHideEventFilter( m_pEdit, ev );

        int type = ev->type();
        if ( type == QEvent::KeyPress )
        {
            QKeyEvent *e = static_cast<QKeyEvent *>( ev );
            if ( e->key() == Key_Return || e->key() == Key_Enter)
            {
                // On Return pressed event, emit both
                // returnPressed(const QString&) and returnPressed() signals
                emit returnPressed();
                emit returnPressed( currentText() );

		if ( d->completionBox && d->completionBox->isVisible() )
                    d->completionBox->hide();

                return m_trapReturnKey;
            }
        }

        else if ( type == QEvent::MouseButtonPress )
        {
            QMouseEvent* e = static_cast<QMouseEvent*>( ev );

            if ( e->button() == RightButton )
            {
                if( !m_bEnableMenu )
                    return true;

                if ( !d->popupMenu )
                {
                    d->hasReference = false;
                    d->popupMenu = contextMenuInternal();
                }

                initPopup();
                
                KGlobalSettings::Completion oldMode = completionMode();
                int result = d->popupMenu->exec( e->globalPos() );

                if ( !d->hasReference ) {
                    delete d->popupMenu;
                    d->popupMenu = 0L;
		}

                if ( result == Cut )
                    m_pEdit->cut();
                else if ( result == Copy )
                    m_pEdit->copy();
                else if ( result == Paste )
                    m_pEdit->paste();
                else if ( result == Clear )
                    m_pEdit->clear();
                else if ( result == Unselect )
                    m_pEdit->deselect();
                else if ( result == SelectAll )
                    m_pEdit->selectAll();
                else if ( result == Default )
                    setCompletionMode( KGlobalSettings::completionMode() );
                else if ( result == NoCompletion )
                    setCompletionMode( KGlobalSettings::CompletionNone );
                else if ( result == AutoCompletion )
                    setCompletionMode( KGlobalSettings::CompletionAuto );
                else if ( result == SemiAutoCompletion )
                    setCompletionMode( KGlobalSettings::CompletionMan );
                else if ( result == ShellCompletion )
                    setCompletionMode( KGlobalSettings::CompletionShell );
                else if ( result == PopupCompletion )
                    setCompletionMode( KGlobalSettings::CompletionPopup );

                if ( oldMode != completionMode() )
                {
                    if ( oldMode == KGlobalSettings::CompletionPopup &&
                         d->completionBox )
                        d->completionBox->hide();
                    emit completionModeChanged( completionMode() );
                }
                return true;
            }
        }
        else if(type == QEvent::Drop)
        {
            QDropEvent *e = static_cast<QDropEvent *>( ev );
            KURL::List urlList;
            if(d->handleURLDrops && KURLDrag::decode( e, urlList ))
            {
                QString dropText;
                KURL::List::ConstIterator it;
                for( it = urlList.begin() ; it != urlList.end() ; ++it)
                {
                    if(!dropText.isEmpty())
                        dropText+=' ';

                    dropText += (*it).prettyURL();
                }

                if(!m_pEdit->text().isEmpty())
                    dropText=' '+dropText;

                m_pEdit->end(false);
                m_pEdit->insert(dropText);

                return true;
            }
        }
    }
    return QComboBox::eventFilter( o, ev );
}

void KComboBox::initPopup()
{
    if( compObj() ) 
    {
        d->subMenu->clear();
    
        d->subMenu->insertItem( i18n("None"), NoCompletion );
        d->subMenu->insertItem( i18n("Manual"), ShellCompletion );
        d->subMenu->insertItem( i18n("Popup"), PopupCompletion );
        d->subMenu->insertItem( i18n("Automatic"), AutoCompletion );
        d->subMenu->insertItem( i18n("Short Automatic"), SemiAutoCompletion );
    
        KGlobalSettings::Completion mode = completionMode();
        d->subMenu->setItemChecked( NoCompletion, 
                                    mode == KGlobalSettings::CompletionNone );
        d->subMenu->setItemChecked( ShellCompletion, 
                                    mode == KGlobalSettings::CompletionShell );
        d->subMenu->setItemChecked( PopupCompletion, 
                                    mode == KGlobalSettings::CompletionPopup );
        d->subMenu->setItemChecked( AutoCompletion, 
                                    mode == KGlobalSettings::CompletionAuto );
        d->subMenu->setItemChecked( SemiAutoCompletion, 
                                    mode == KGlobalSettings::CompletionMan );
        if ( mode != KGlobalSettings::completionMode() )
        {
            d->subMenu->insertSeparator();
            d->subMenu->insertItem( i18n("Default"), Default );
        }
    }

    bool flag = ( m_pEdit->echoMode() == QLineEdit::Normal && 
                  !m_pEdit->isReadOnly() );
    bool allMarked =(m_pEdit->markedText().length() == currentText().length());
    d->popupMenu->setItemEnabled( Cut, flag && m_pEdit->hasMarkedText() );
    d->popupMenu->setItemEnabled( Copy, flag && m_pEdit->hasMarkedText() );
    d->popupMenu->setItemEnabled( Paste, flag && 
                                 !QApplication::clipboard()->text().isEmpty());
    d->popupMenu->setItemEnabled( Clear, flag && !currentText().isEmpty() );
    d->popupMenu->setItemEnabled( Unselect, m_pEdit->hasMarkedText() );
    d->popupMenu->setItemEnabled( SelectAll, flag && !allMarked );
}

void KComboBox::setTrapReturnKey( bool grab )
{
    m_trapReturnKey = grab;
}

bool KComboBox::trapReturnKey() const
{
    return m_trapReturnKey;
}


void KComboBox::setEditURL( const KURL& url )
{
    QComboBox::setEditText( url.prettyURL() );
}

void KComboBox::insertURL( const KURL& url, int index )
{
    QComboBox::insertItem( url.prettyURL(), index );
}

void KComboBox::insertURL( const QPixmap& pixmap, const KURL& url, int index )
{
    QComboBox::insertItem( pixmap, url.prettyURL(), index );
}

void KComboBox::changeURL( const KURL& url, int index )
{
    QComboBox::changeItem( url.prettyURL(), index );
}

void KComboBox::changeURL( const QPixmap& pixmap, const KURL& url, int index )
{
    QComboBox::changeItem( pixmap, url.prettyURL(), index );
}

void KComboBox::makeCompletionBox()
{
    if ( d->completionBox )
        return;

    d->completionBox = new KCompletionBox( lineEdit(), "completion box" );
    connect( d->completionBox, SIGNAL( highlighted( const QString& )),
             SLOT( setEditText( const QString& )));
    connect( d->completionBox, SIGNAL( activated( const QString& )),
             SIGNAL( activated( const QString & )));
/*
    connect( d->completionBox, SIGNAL( aboutToShow() ),
             SLOT( slotAboutToShow() ) );
    connect( d->completionBox, SIGNAL( cancelled() ),
             SLOT( slotCancelled() ) );
*/
}

void KComboBox::slotAboutToShow()
{
    d->origText = currentText();
}

void KComboBox::slotCancelled()
{
    setEditText( d->origText );
    d->origText = QString::null;
}

// FIXME: make pure virtual in KCompletionBase!
void KComboBox::setCompletedItems( const QStringList& items )
{
    if ( completionMode() == KGlobalSettings::CompletionPopup ||
         completionMode() == KGlobalSettings::CompletionShell )
    {
        if ( !d->completionBox )
            makeCompletionBox();

        d->completionBox->clear();

        if ( !items.isEmpty() )
        {
            d->completionBox->insertStringList( items );
            d->completionBox->popup();
            setFocus(); // let the user keep on typing
        }
        else
            d->completionBox->hide();
    }
    else
    {
        if ( !items.isEmpty() ) // fallback
            setCompletedText( items.first() );
    }
}

// ### merge these two for 3.0
KCompletionBox * KComboBox::completionBox()
{
    makeCompletionBox();
    return d->completionBox;
}

KCompletionBox * KComboBox::completionBox( bool create )
{
    if ( create )
	makeCompletionBox();

    return d->completionBox;
}

void KComboBox::setCompletionObject( KCompletion* comp, bool hsig )
{
    KCompletion *oldComp = completionObject( false, false ); // don't create!

    if ( oldComp && handleSignals() )
      disconnect( oldComp, SIGNAL( matches( const QStringList& )),
                  this, SLOT( setCompletedItems( const QStringList& )));

    if ( comp && hsig )
      connect( comp, SIGNAL( matches( const QStringList& )),
               this, SLOT( setCompletedItems( const QStringList& )));

    KCompletionBase::setCompletionObject( comp, hsig );
}

QPopupMenu* KComboBox::contextMenu()
{
    d->hasReference = true;
    return contextMenuInternal();
}

QPopupMenu* KComboBox::contextMenuInternal()
{
    if( !d->popupMenu )
    {
        d->popupMenu = new QPopupMenu( this );
        d->popupMenu->insertItem( SmallIconSet("editcut"), 
                                  i18n( "Cut" ), Cut );
        d->popupMenu->insertItem( SmallIconSet("editcopy"), 
                                  i18n( "Copy" ), Copy );
        d->popupMenu->insertItem( SmallIconSet("editpaste"), 
                                  i18n( "Paste" ), Paste );
        d->popupMenu->insertItem( SmallIconSet("editclear"), 
                                  i18n( "Clear" ), Clear );
        // Create and insert the completion sub-menu if a completion object
        // is present.
        if ( compObj() )
        {
            d->subMenu = new QPopupMenu( d->popupMenu );
            d->popupMenu->insertSeparator();
            d->popupMenu->insertItem( SmallIconSet("completion"), 
                                      i18n("Completion"), d->subMenu );
        }
        d->popupMenu->insertSeparator();
        d->popupMenu->insertItem( i18n( "Unselect" ), Unselect );
        d->popupMenu->insertItem( i18n( "Select All" ), SelectAll );
    }
    return d->popupMenu;
}

// QWidget::create() turns off mouse-Tracking which would break auto-hiding
void KComboBox::create( WId id, bool initializeWindow, bool destroyOldWindow )
{
    QComboBox::create( id, initializeWindow, destroyOldWindow );
    KCursor::setAutoHideCursor( lineEdit(), true, true );
}


// *********************************************************************
// *********************************************************************


// we are always read-write
KHistoryCombo::KHistoryCombo( QWidget *parent, const char *name )
              :KComboBox( true, parent, name )
{
    init( true ); // using completion
}

// we are always read-write
KHistoryCombo::KHistoryCombo( bool useCompletion, QWidget *parent, const char *name )
              :KComboBox( true, parent, name )
{
    init( useCompletion );
}

void KHistoryCombo::init( bool useCompletion )
{
    if ( useCompletion )
	completionObject()->setOrder( KCompletion::Weighted );

    setInsertionPolicy( NoInsertion );
    myIterateIndex = -1;
    myRotated = false;
    myPixProvider = 0L;

    contextMenu()->insertSeparator();
    contextMenu()->insertItem( i18n("Empty contents"), this,
			       SLOT( slotClear()));

    connect( this, SIGNAL( activated(int) ), SLOT( slotReset() ));
    connect( this, SIGNAL( returnPressed(const QString&) ), SLOT(slotReset()));
}

KHistoryCombo::~KHistoryCombo()
{
    delete myPixProvider;
}

void KHistoryCombo::setHistoryItems( QStringList items,
                                     bool setCompletionList )
{
    KComboBox::clear();

    // limit to maxCount()
    while ( (int) items.count() > maxCount() && !items.isEmpty() )
        items.remove( items.begin() );

    insertItems( items );

    if ( setCompletionList && useCompletion() ) {
        // we don't have any weighting information here ;(
        KCompletion *comp = completionObject();
        comp->setOrder( KCompletion::Insertion );
        comp->setItems( items );
        comp->setOrder( KCompletion::Weighted );
    }

    clearEdit();
}

QStringList KHistoryCombo::historyItems() const
{
    QStringList list;
    for ( int i = 0; i < count(); i++ )
        list.append( text( i ) );

    return list;
}

void KHistoryCombo::clearHistory()
{
    KComboBox::clear();
    if ( useCompletion() )
	completionObject()->clear();
}

void KHistoryCombo::addToHistory( const QString& item )
{
    if ( item.isEmpty() || (count() > 0 && item == text(0) ))
        return;

    // remove all existing items before adding
    if ( !duplicatesEnabled() ) {
        for ( int i = 0; i < count(); i++ ) {
            if ( text( i ) == item )
                removeItem( i );
        }
    }

    // now add the item
    if ( myPixProvider )
        insertItem( myPixProvider->pixmapFor(item, KIcon::SizeSmall), item, 0);
    else
        insertItem( item, 0 );

    int last;
    QString rmItem;

    bool useComp = useCompletion();
    while ( count() > maxCount() && count() > 0 ) {
        // remove the last item, as long as we are longer than maxCount()
        // remove the removed item from the completionObject if it isn't
        // anymore available at all in the combobox.
        last = count() - 1;
        rmItem = text( last );
        removeItem( last );
        if ( useComp && !contains( rmItem ) )
            completionObject()->removeItem( rmItem );
    }

    if ( useComp )
	completionObject()->addItem( item );
}

bool KHistoryCombo::removeFromHistory( const QString& item )
{
    if ( item.isEmpty() )
        return false;

    bool removed = false;
    QString temp = currentText();
    for ( int i = 0; i < count(); i++ ) {
        while ( item == text( i ) ) {
            removed = true;
            removeItem( i );
        }
    }

    if ( removed && useCompletion() )
        completionObject()->removeItem( item );

    setEditText( temp );
    return removed;
}

void KHistoryCombo::keyPressEvent( QKeyEvent *e )
{
    // save the current text in the lineedit
    if ( myIterateIndex == -1 )
        myText = currentText();

    // going up in the history, rotating when reaching QListBox::count()
    if ( KStdAccel::isEqual( e, KStdAccel::rotateUp() ) ) {
        myIterateIndex++;
	
	// skip duplicates/empty items
	while ( myIterateIndex < count()-1 &&
		(currentText() == text( myIterateIndex ) ||
		 text( myIterateIndex ).isEmpty()) )
	    myIterateIndex++;
	
        if ( myIterateIndex >= count() ) {
            myRotated = true;
            myIterateIndex = -1;

            // if the typed text is the same as the first item, skip the first
            if ( myText == text(0) )
                myIterateIndex = 0;

            setEditText( myText );
        }
        else
            setEditText( text( myIterateIndex ));
    }


    // going down in the history, no rotation possible. Last item will be
    // the text that was in the lineedit before Up was called.
    else if ( KStdAccel::isEqual( e, KStdAccel::rotateDown() ) ) {
        myIterateIndex--;

	// skip duplicates/empty items
	while ( myIterateIndex >= 0 &&
		(currentText() == text( myIterateIndex ) ||
		 text( myIterateIndex ).isEmpty()) )
	    myIterateIndex--;
	
	
        if ( myIterateIndex < 0 ) {
            if ( myRotated && myIterateIndex == -2 ) {
                myRotated = false;
                myIterateIndex = count() - 1;
                setEditText( text(myIterateIndex) );
            }
            else { // bottom of history
                if ( myIterateIndex == -2 ) {
                    KNotifyClient::event( KNotifyClient::notification,
                                      i18n("No further item in the history."));
                }

                myIterateIndex = -1;
                if ( currentText() != myText )
                    setEditText( myText );
            }
        }
        else
            setEditText( text( myIterateIndex ));
    }

    else
        KComboBox::keyPressEvent( e );
}

void KHistoryCombo::slotReset()
{
    myIterateIndex = -1;
    myRotated = false;
}


void KHistoryCombo::setPixmapProvider( KPixmapProvider *prov )
{
    if ( myPixProvider == prov )
        return;

    myPixProvider = prov;

    // re-insert all the items with/without pixmap
    // I would prefer to use changeItem(), but that doesn't honour the pixmap
    // when using an editable combobox (what we do)
    if ( count() > 0 ) {
        QStringList items( historyItems() );
        clear();
        insertItems( items );
    }
}

void KHistoryCombo::insertItems( const QStringList& items )
{
    QStringList::ConstIterator it = items.begin();
    QString item;
    while ( it != items.end() ) {
        item = *it;
        if ( !item.isEmpty() ) { // only insert non-empty items
            if ( myPixProvider )
                insertItem( myPixProvider->pixmapFor(item, KIcon::SizeSmall),
                            item );
            else
                insertItem( item );
        }
        ++it;
    }
}

void KHistoryCombo::slotClear()
{
    clearHistory();
    emit cleared();
}
