/* This file is part of the KDE libraries

   Copyright (C) 1997 Sven Radej (sven.radej@iname.com)
   Copyright (c) 1999 Patrick Ward <PAT_WARD@HP-USA-om5.om.hp.com>
   Copyright (c) 1999 Preston Brown <pbrown@kde.org>
   Copyright (c) 2000 Dawit Alemayehu <adawit@earthlink.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qobjcoll.h>

#include <klocale.h>
#include <kstdaccel.h>

#include "klineedit.h"
#include "klineedit.moc"


KLineEdit::KLineEdit( const QString &string, QWidget *parent, const char *name )
          : QLineEdit( string, parent, name )
{
    init();
}

KLineEdit::KLineEdit( QWidget *parent, const char *name )
          : QLineEdit( parent, name )
{
    init();
}

KLineEdit::~KLineEdit ()
{
    m_pContextMenu = 0; // Reset the pointer to NULL;
}

void KLineEdit::init()
{
    // Hack and recover the built-in popup-menu to add
    // your own item to it.  What a piece of work :))
    QObjectList *list = queryList( "QPopupMenu" );
    QObjectListIt it ( *list );
    m_pContextMenu = (QPopupMenu*) it.current();
    delete list;

    m_bEnableMenu = false;
    setEnableContextMenu( true ); // Enable the context menu by default
}

void KLineEdit::setEnableContextMenu( bool showMenu )
{
    if( !m_bEnableMenu && showMenu )
    {
        connect ( m_pContextMenu, SIGNAL( aboutToShow() ), this, SLOT( aboutToShowMenu() ) );
        showModeChanger();
    }
    else if( m_bEnableMenu && !showMenu )
    {
        disconnect ( m_pContextMenu, SIGNAL( aboutToShow() ), this, SLOT( aboutToShowMenu() ) );
        hideModeChanger();
    }
    m_bEnableMenu = showMenu;
}

void KLineEdit::setCompletionMode( KGlobalSettings::Completion mode )
{
    // If the widgets echo mode is not Normal, no completion
    // feature will be enabled even if one is requested.
    if ( echoMode() != QLineEdit::Normal )
        mode = KGlobalSettings::CompletionNone; // override the request.

    KCompletionBase::setCompletionMode( mode );
}

void KLineEdit::rotateText( KCompletionBase::KeyBindingType type )
{
   	KCompletion* comp = completionObject();
	if( comp &&
		(type == KCompletionBase::PrevCompletionMatch ||
		 type == KCompletionBase::NextCompletionMatch ) )
	{
		QString input = (type == KCompletionBase::PrevCompletionMatch) ? comp->previousMatch() : comp->nextMatch();
		// Skip rotation if previous/next match is null or the same text.
        if( input.isNull() || input == text() ) return;
        int pos = cursorPosition();
        if( hasMarkedText() )
			validateAndSet( input, pos, pos, input.length() );
		else
			setText( input );
    }

/*  else
    {

		QStringList list = comp->items();
        int cnt = list.count();
        if( cnt == 0 ) return;
        int index = list.findIndex( text() );
        if( index == -1 )
        {
        	if( dir == KCompletionBase::UpKeyEvent )
        		index = cnt -1;
        }
        else
        	index += ( dir == KCompletionBase::UpKeyEvent ) ? -1 : 1;
        	
        if ( index > -1 && index < cnt )
        	setText( list[index] );
    } */
}

void KLineEdit::makeCompletion( const QString& text )
{
	QString match;
   	int pos = cursorPosition();
	KCompletion *comp = completionObject();
   	KGlobalSettings::Completion mode = completionMode();
    	
/*  if( mode == KGlobalSettings::CompletionShell &&
	   	comp->hasMultipleMatches() && text != comp->lastMatch() )
    	match = comp->nextMatch();
    else */
    match = comp->makeCompletion( text );
    	
    // If no match or the same match, simply return
    // without completing.
    if( match.length() == 0 || match == text )
        return;

    setText( match );
	if( mode == KGlobalSettings::CompletionAuto ||
		mode == KGlobalSettings::CompletionMan )
	{
		setSelection( pos, match.length() );
		setCursorPosition( pos );
	}
}

void KLineEdit::connectSignals( bool handle ) const
{
    if( handle && !handleSignals() )
    {
        connect( this, SIGNAL( completion( const QString& ) ), this, SLOT( makeCompletion( const QString& ) ) );
        connect( this, SIGNAL( previousMatch( KeyBindingType ) ), this, SLOT( rotateText( KeyBindingType ) ) );
        connect( this, SIGNAL( nextMatch( KeyBindingType ) ), this, SLOT( rotateText( KeyBindingType ) ) );
    }
    else if( !handle && handleSignals() )
    {
        disconnect( this, SIGNAL( completion( const QString& ) ), this, SLOT( makeCompletion( const QString& ) ) );
        disconnect( this, SIGNAL( previousMatch( KeyBindingType ) ), this, SLOT( rotateText( KeyBindingType ) ) );
        disconnect( this, SIGNAL( nextMatch( KeyBindingType ) ), this, SLOT( rotateText( KeyBindingType ) ) );
    }
}

void KLineEdit::selectedItem( int id )
{
    if( id == 0 )
    {
    	id = KGlobalSettings::completionMode();
    }
    setCompletionMode( (KGlobalSettings::Completion)id );
}

void KLineEdit::keyPressEvent( QKeyEvent *e )
{
    // Trap RETURN/ENTER events.  Let people connect to
    // returnPressed() and returnPressed( const QString& )
    // as needed if they were relying on this event being
    // propagated up-stream.  This is also consistent with
    // KComboBox.
    if( e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter )
    {
        emit returnPressed( displayText() );
    }
    // Filter key-events if EchoMode is normal & completion mode is not set to CompletionNone
    else if( echoMode() == QLineEdit::Normal && completionMode() != KGlobalSettings::CompletionNone )
    {
        bool fireSignals = emitSignals();
        KGlobalSettings::Completion mode = completionMode();
        if( mode == KGlobalSettings::CompletionAuto )
        {
            QString keycode = e->text();
            if( !keycode.isNull() && keycode.unicode()->isPrint() && fireSignals )
            {
                QLineEdit::keyPressEvent ( e );
                emit completion( text() );
                return;
            }
        }
        // Handles completion.
        KCompletion* comp = completionObject();
        KeyBindingMap keys = getKeyBindings();
        int key = ( keys[TextCompletion] == 0 ) ? KStdAccel::key(KStdAccel::TextCompletion) : keys[TextCompletion];
        if( KStdAccel::isEqual( e, key ) && fireSignals )
        {
            // Emit completion if the completion mode is NOT
            // CompletionAuto and if the mode is CompletionShell,
            // the cursor is at the end of the string.
            int len = text().length();
            if( (mode == KGlobalSettings::CompletionMan && (comp != 0 && comp->lastMatch() != displayText() ) ) ||
                (mode == KGlobalSettings::CompletionShell && cursorPosition() == len && len != 0 ) )
            {
                emit completion( text() );
                return;
            }
        }
        // Handles previous match
    	key = ( keys[PrevCompletionMatch] == 0 ) ? KStdAccel::key(KStdAccel::PrevCompletion) : keys[PrevCompletionMatch];
        if( KStdAccel::isEqual( e, key ) && fireSignals )
        {
            emit previousMatch( KCompletionBase::PrevCompletionMatch );
            return;
        }
        // Handles next match
	    key = ( keys[NextCompletionMatch] == 0 ) ? KStdAccel::key(KStdAccel::NextCompletion) : keys[NextCompletionMatch];
        if( KStdAccel::isEqual( e, key ) && fireSignals)
        {
            emit nextMatch( KCompletionBase::NextCompletionMatch );
            return;
        }
    }
    // Let QLineEdit handle any other keys events.
    QLineEdit::keyPressEvent ( e );
}

void KLineEdit::mousePressEvent( QMouseEvent* e )
{
    if( e->button() == Qt::RightButton )
    {
        if( !m_bEnableMenu )
            return;
    }
    QLineEdit::mousePressEvent( e );
}
